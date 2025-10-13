#include "WorkThreadConcurrencyEnhanced.h"
#include "TaskQueueConstant.h"
#include "common/SysUtils.h"
#include "log/Log.h"
#include <algorithm>

namespace task
{

WorkThreadConcurrencyEnhanced::WorkThreadConcurrencyEnhanced(const std::weak_ptr<IThreadPool>& threadPool)
    : WorkThreadConcurrency(threadPool)
    , mLastTaskTime(std::chrono::steady_clock::now()) {
    LOGE("[TASK] WorkThreadConcurrencyEnhanced created, threadId: %d", threadId());
}

bool WorkThreadConcurrencyEnhanced::_parallel() {
    auto pool = _getThreadPool();
    if (!pool) {
        LOGE("[TASK][HY] WorkThreadConcurrencyEnhanced::_parallel, threadId: %d, threadPool is null", threadId());
        return false;
    }
    
    auto& data = pool->getData();
    if (!data) {
        LOGE("[TASK][HY] WorkThreadConcurrencyEnhanced::_parallel, threadId: %d, data is null", threadId());
        return false;
    }

    // 线程处于非执行状态，线程池空闲线程+1
    data->mIdleThreads.fetch_add(1, std::memory_order_seq_cst);

    // 批量获取任务，提高效率
    std::vector<TaskOperatorPtr> taskBatch;
    bool hasWork = acquireTaskBatch(data, taskBatch, BATCH_SIZE);
    
    if (!hasWork) {
        bool shouldContinue = handleIdleState(data);
        data->mIdleThreads.fetch_sub(1, std::memory_order_seq_cst);
        return shouldContinue;
    }

    // 线程结束等待，线程池空闲线程-1
    data->mIdleThreads.fetch_sub(1, std::memory_order_seq_cst);
    
    // 执行任务批量
    executeBatch(taskBatch);
    
    return true;
}

bool WorkThreadConcurrencyEnhanced::acquireTaskBatch(
    const std::shared_ptr<IThreadPool::Data>& data,
    std::vector<TaskOperatorPtr>& batch,
    size_t maxBatchSize) {
    
    // 从高优先级到低优先级获取任务
    for (int i = static_cast<int>(TaskQueuePriority::TQP_High); i >= 0; --i) {
        TaskOperatorPtr task;
        size_t acquired = 0;
        
        // 高优先级任务优先单个执行，低优先级可以批量执行
        size_t targetBatch = (i == static_cast<int>(TaskQueuePriority::TQP_High)) ? 1 : maxBatchSize;
        
        while (acquired < targetBatch && data->mTaskQueues[i].try_dequeue(task)) {
            if (task) {
                batch.push_back(task);
                acquired++;
            }
        }
        
        if (!batch.empty()) {
            return true;
        }
    }
    
    return false;
}

bool WorkThreadConcurrencyEnhanced::handleIdleState(
    const std::shared_ptr<IThreadPool::Data>& data) {
    
    data->mIdleThreads.fetch_add(1, std::memory_order_seq_cst);
    
    // 自适应等待策略
    WaitStrategy strategy = determineWaitStrategy(data);
    bool acquired = false;
    
    switch (strategy) {
        case WaitStrategy::AggressiveSpin:
            acquired = data->mSemaphore.spinAcquire(TaskQueueConstant::sMaxSpinCount * 2);
            break;
            
        case WaitStrategy::ModerateWait:
            acquired = data->mSemaphore.waitAcquire(TaskQueueConstant::sMaxSleepTimeout / 2);
            break;
            
        case WaitStrategy::PatientWait:
            acquired = data->mSemaphore.waitAcquire(TaskQueueConstant::sMaxSleepTimeout);
            break;
    }
    
    data->mIdleThreads.fetch_sub(1, std::memory_order_seq_cst);
    return acquired;
}

WaitStrategy WorkThreadConcurrencyEnhanced::determineWaitStrategy(
    const std::shared_ptr<IThreadPool::Data>& data) const {
    
    int32_t activeThreads = data->mActiveThreads.load();
    int32_t idleThreads = data->mIdleThreads.load();
    
    // 系统负载高：积极等待
    if (hasHighSystemLoad()) {
        return WaitStrategy::AggressiveSpin;
    }
    
    // 空闲线程较多：耐心等待，准备退出
    if (idleThreads > activeThreads / 2) {
        return WaitStrategy::PatientWait;
    }
    
    return WaitStrategy::ModerateWait;
}

void WorkThreadConcurrencyEnhanced::executeBatch(const std::vector<TaskOperatorPtr>& batch) {
    auto startTime = std::chrono::steady_clock::now();
    
    for (const auto& task : batch) {
        if (task && !task->isCancelled()) {
            mCurrTask = task;
            mStartRunTime = comm::HETimerHelper::currentTimeMillis();
            mIsRunning = true;
            
            try {
                (*task)();
            } catch (const std::exception& e) {
                LOGE("[TASK] WorkThreadConcurrencyEnhanced: exception in task execution: %s", e.what());
            } catch (...) {
                LOGE("[TASK] WorkThreadConcurrencyEnhanced: unknown exception in task execution");
            }
            
            mIsRunning = false;
            
            // 收集统计信息
            _updateStat(task);
            mTasksExecuted++;
        }
    }
    
    auto endTime = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
    mAvgTaskDuration = (mAvgTaskDuration * (mTasksExecuted - batch.size()) + duration.count()) / mTasksExecuted;
    mLastTaskTime = endTime;
}

bool WorkThreadConcurrencyEnhanced::hasHighSystemLoad() const {
    // 简化的系统负载检查
    // 在实际实现中，应该调用系统API获取真实负载信息
    return false; // 暂时返回false
}

void WorkThreadConcurrencyEnhanced::updateThreadStats() {
    // 更新线程统计信息
    // 可以将统计信息发送到监控系统
}

} // namespace task