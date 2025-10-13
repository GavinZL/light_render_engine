#include "ConcurrencyThreadPoolEnhanced.h"
#include "WorkThreadConcurrency.h"
#include "log/Log.h"
#include <algorithm>
#include <cmath>

namespace task
{

ConcurrencyThreadPoolEnhanced::ConcurrencyThreadPoolEnhanced()
    : mLoadAnalyzer(std::make_unique<EnhancedLoadAnalyzer>())
    , mThresholdManager(std::make_unique<AdaptiveThresholdManager>())
    , mLastThreadCreationTime(std::chrono::steady_clock::now())
    , mConfig(ThreadPoolConfigManager::getInstance().getConfig())
{
    LOGE("[TASK] ConcurrencyThreadPoolEnhanced initialized");
}

ConcurrencyThreadPoolEnhanced::~ConcurrencyThreadPoolEnhanced() {
    LOGE("[TASK] ConcurrencyThreadPoolEnhanced destroyed");
}

void ConcurrencyThreadPoolEnhanced::execute(const TaskOperatorPtr& task, TaskQueuePriority priority) {
    // 优先级判断
    if (priority < TaskQueuePriority::TQP_Low || priority >= TaskQueuePriority::TQP_Count) {
        LOGE("[TASK]ConcurrencyThreadPoolEnhanced::execute, invalid priority: %d", priority);
        assert(false);
        return;
    }

    // 队列数量监控
    _monitorTask(priority);

    int32_t prio = static_cast<int32_t>(priority);
    const auto enqueued = mData->mTaskQueues[prio].enqueue(task);
    if (enqueued) {
        // 信号量唤醒线程
        mData->mSemaphore.release();
        mTotalTasksExecuted.fetch_add(1, std::memory_order_relaxed);
    }

    // 使用增强的调度算法
    _scheduleEnhanced(priority);
}

void ConcurrencyThreadPoolEnhanced::_scheduleEnhanced(TaskQueuePriority priority) {
    const auto idleCount = mData->mIdleThreads.load(std::memory_order_acquire);
    const auto activeCount = mData->mActiveThreads.load(std::memory_order_acquire);
    const auto queueSize = mData->mTaskQueues[static_cast<int32_t>(priority)].size_approx();
    
    // 如果有空闲线程，不需要调度
    if (activeCount > 0 && idleCount > 0) {
        mReportCnt = 0;
        return;
    }
    
    // 动态调整创建阈值
    ThreadEfficiencyMetrics threadMetrics;
    threadMetrics.activeThreads = activeCount;
    threadMetrics.idleThreads = idleCount;
    threadMetrics.avgTaskDuration = mAvgTaskDuration.load();
    
    uint32_t createThreshold = mThresholdManager->getCreateThreshold(priority, threadMetrics);
    
    // 增强的负载分析
    LoadMetrics metrics = analyzeCurrentLoad(priority);
    
    if (shouldCreateNewThread(priority)) {
        createOptimalThread(priority, metrics);
    } else if (mThresholdManager->shouldOptimizeExisting(metrics)) {
        optimizeExistingThreads(metrics);
    }
    
    // 清理过期线程
    {
        std::vector<std::shared_ptr<WorkThreadBase>> expiredThreads;
        {
            std::lock_guard<std::mutex> lock(mParallelMutex);
            expiredThreads.swap(mExpiredThreads);
        }
        // expiredThreads会在作用域结束时自动清理
    }
}

bool ConcurrencyThreadPoolEnhanced::shouldCreateNewThread(TaskQueuePriority priority) {
    // 检查冷却时间
    if (isThreadCreationCooldown()) {
        return false;
    }
    
    const auto activeCount = mData->mActiveThreads.load(std::memory_order_acquire);
    const auto maxThreads = mData->mMaxThreads.load(std::memory_order_acquire);
    const auto queueSize = mData->mTaskQueues[static_cast<int32_t>(priority)].size_approx();
    
    // 检查是否达到最大线程数
    if (activeCount >= maxThreads) {
        return false;
    }
    
    // 检查队列大小是否超过阈值
    ThreadEfficiencyMetrics threadMetrics;
    threadMetrics.activeThreads = activeCount;
    threadMetrics.idleThreads = mData->mIdleThreads.load(std::memory_order_acquire);
    threadMetrics.avgTaskDuration = mAvgTaskDuration.load();
    
    uint32_t createThreshold = mThresholdManager->getCreateThreshold(priority, threadMetrics);
    
    return queueSize > createThreshold;
}

void ConcurrencyThreadPoolEnhanced::createOptimalThread(TaskQueuePriority priority, const LoadMetrics& metrics) {
    const auto activeCount = mData->mActiveThreads.load(std::memory_order_acquire);
    const auto maxThreads = mData->mMaxThreads.load(std::memory_order_acquire);
    
    // 双重检查
    if (activeCount >= maxThreads) {
        return;
    }
    
    // 创建新线程
    auto thread = std::make_shared<WorkThreadConcurrency>(shared_from_this());
    registerWorkThread(std::static_pointer_cast<WorkThreadBase>(thread));
    mTotalThreadsCreated.fetch_add(1, std::memory_order_relaxed);
    
    updateLastThreadCreationTime();
    
    LOGE("[TASK]ConcurrencyThreadPoolEnhanced::createOptimalThread, created thread %d, "
         "idleThreads: %d, activeThreads: %d, queueSize: %zu", 
         thread->threadId(), 
         mData->mIdleThreads.load(std::memory_order_relaxed),
         activeCount,
         mData->mTaskQueues[static_cast<int32_t>(priority)].size_approx());
}

void ConcurrencyThreadPoolEnhanced::optimizeExistingThreads(const LoadMetrics& metrics) {
    // 检查阻塞线程并清理
    std::lock_guard<std::mutex> lock(mParallelMutex);
    
    bool foundBlockedThread = false;
    for (auto it = mParallelThreads.begin(); it != mParallelThreads.end();) {
        if (it->second->isBlocked()) {
            TaskQueueReporter::GetInstance().notifyReport(
                TaskQueueReporterType::TQRT_TaskDurationExceedThreshold,
                it->second->blockedInfo());
            
            it = mParallelThreads.erase(it);
            mData->mActiveThreads.fetch_sub(1, std::memory_order_release);
            mTotalThreadsDestroyed.fetch_add(1, std::memory_order_relaxed);
            foundBlockedThread = true;
        } else {
            ++it;
        }
    }
    
    if (foundBlockedThread) {
        LOGE("[TASK]ConcurrencyThreadPoolEnhanced::optimizeExistingThreads, "
             "cleaned up blocked threads");
    }
}

LoadMetrics ConcurrencyThreadPoolEnhanced::analyzeCurrentLoad(TaskQueuePriority priority) {
    const auto activeCount = mData->mActiveThreads.load(std::memory_order_acquire);
    const auto idleCount = mData->mIdleThreads.load(std::memory_order_acquire);
    const auto queueSize = mData->mTaskQueues[static_cast<int32_t>(priority)].size_approx();
    
    // 使用负载分析器获取指标
    return mLoadAnalyzer->getLoadMetrics(priority, queueSize, activeCount, idleCount, mAvgTaskDuration.load());
}

void ConcurrencyThreadPoolEnhanced::updateThreadPoolStats() {
    // 更新线程池统计信息
    ThreadPoolStats stats;
    stats.activeThreads = mData->mActiveThreads.load(std::memory_order_relaxed);
    stats.idleThreads = mData->mIdleThreads.load(std::memory_order_relaxed);
    stats.maxThreads = mData->mMaxThreads.load(std::memory_order_relaxed);
    stats.totalTasksExecuted = mTotalTasksExecuted.load(std::memory_order_relaxed);
    stats.totalThreadsCreated = mTotalThreadsCreated.load(std::memory_order_relaxed);
    stats.totalThreadsDestroyed = mTotalThreadsDestroyed.load(std::memory_order_relaxed);
    stats.avgTaskDuration = mAvgTaskDuration.load(std::memory_order_relaxed);
    stats.lastStatsUpdate = std::chrono::steady_clock::now();
    
    // 这里可以将统计信息发送到监控系统
}

bool ConcurrencyThreadPoolEnhanced::isThreadCreationCooldown() {
    auto now = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - mLastThreadCreationTime);
    return elapsed < mMinThreadCreationInterval;
}

void ConcurrencyThreadPoolEnhanced::updateLastThreadCreationTime() {
    mLastThreadCreationTime = std::chrono::steady_clock::now();
}

} // namespace task