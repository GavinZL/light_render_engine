#include "TaskQueueEnhanced.h"
#include "backend/IQueueImpl.h"
#include "backend/TaskOperatorBackend.h"
#include "TimerManager.h"
#include "ThreadPoolConfig.h"
#include "log/Log.h"
#include <algorithm>

namespace task
{

TaskQueueEnhanced::TaskQueueEnhanced(const std::string& label, const TaskQueueImplPtr& impl)
    : mLabel(label)
    , mImpl(impl)
{
}

// === 基础异步任务接口实现 ===
void TaskQueueEnhanced::async(const TaskOperatorEnhancedPtr& task) {
    if (!task || mIsPaused.load()) {
        return;
    }
    
    // 转换为传统TaskOperator
    auto legacyTask = adaptToLegacy(task);
    mImpl->async(legacyTask);
    
    updateStats(task);
}

void TaskQueueEnhanced::async(const TaskOperatorPtr& task) {
    if (!task || mIsPaused.load()) {
        return;
    }
    
    mImpl->async(task);
}

void TaskQueueEnhanced::async(Func&& func) {
    auto task = makeTask(std::move(func));
    async(task);
}

void TaskQueueEnhanced::async(TaskMark mark, Func&& func) {
    auto task = makeTask(mark, std::move(func));
    async(task);
}

void TaskQueueEnhanced::async(TaskCategory category, Func&& func) {
    auto task = makeTask(std::move(func), category);
    async(task);
}

void TaskQueueEnhanced::async(TaskCategory category, uint32_t priority, Func&& func) {
    auto task = makeTask(std::move(func), category, priority);
    async(task);
}

// === 批量操作接口实现 ===
void TaskQueueEnhanced::asyncBatch(const std::vector<TaskOperatorEnhancedPtr>& tasks) {
    if (mIsPaused.load()) {
        return;
    }
    
    for (const auto& task : tasks) {
        if (task) {
            async(task);
        }
    }
}

void TaskQueueEnhanced::asyncBatch(const std::vector<Func>& functions) {
    if (mIsPaused.load()) {
        return;
    }
    
    for (const auto& func : functions) {
        async(func);
    }
}

// === 条件执行接口实现 ===
void TaskQueueEnhanced::asyncIf(ConditionFunc condition, const TaskOperatorEnhancedPtr& task) {
    if (!condition || !task || mIsPaused.load()) {
        return;
    }
    
    // 创建条件包装任务
    auto conditionalTask = makeTask([condition, task]() {
        if (condition()) {
            (*task)();
        }
    });
    
    async(conditionalTask);
}

void TaskQueueEnhanced::asyncIf(ConditionFunc condition, Func&& func) {
    if (!condition || mIsPaused.load()) {
        return;
    }
    
    auto task = makeTask([condition, func = std::move(func)]() {
        if (condition()) {
            func();
        }
    });
    
    async(task);
}

// === 重试机制接口实现 ===
void TaskQueueEnhanced::asyncWithRetry(const TaskOperatorEnhancedPtr& task, int maxRetries, 
                                      std::chrono::milliseconds retryDelay) {
    if (!task || maxRetries < 0 || mIsPaused.load()) {
        return;
    }
    
    auto retryTask = createRetryTask(task, maxRetries, retryDelay);
    async(retryTask);
}

void TaskQueueEnhanced::asyncWithRetry(Func&& func, int maxRetries,
                                      std::chrono::milliseconds retryDelay) {
    auto task = makeTask(std::move(func));
    asyncWithRetry(task, maxRetries, retryDelay);
}

// === 同步任务接口实现 ===
void TaskQueueEnhanced::sync(const TaskOperatorEnhancedPtr& task, 
                            std::chrono::milliseconds timeout) {
    if (!task) {
        return;
    }
    
    auto legacyTask = adaptToLegacy(task);
    mImpl->sync(legacyTask, timeout);
    
    updateStats(task);
}

void TaskQueueEnhanced::sync(const TaskOperatorPtr& task, 
                            std::chrono::milliseconds timeout) {
    if (!task) {
        return;
    }
    
    mImpl->sync(task, timeout);
}

void TaskQueueEnhanced::sync(Func&& func, std::chrono::milliseconds timeout) {
    auto task = makeTask(std::move(func));
    sync(task, timeout);
}

void TaskQueueEnhanced::sync(TaskMark mark, Func&& func, std::chrono::milliseconds timeout) {
    auto task = makeTask(mark, std::move(func));
    sync(task, timeout);
}

// === 延时任务接口实现 ===
uint64_t TaskQueueEnhanced::after(std::chrono::milliseconds delay, const TaskOperatorEnhancedPtr& task) {
    if (!task) {
        return 0;
    }
    
    // 如果启用了定时器优化，使用TimerManager
    if (ThreadPoolConfigManager::getInstance().isTimerOptimizationEnabled()) {
        auto legacyTask = adaptToLegacy(task);
        return TimerManager::getInstance().scheduleTask(legacyTask, delay);
    } else {
        // 使用传统方式
        auto legacyTask = adaptToLegacy(task);
        mImpl->after(delay, legacyTask);
        return 1; // 返回伪ID，表示任务已提交
    }
}

uint64_t TaskQueueEnhanced::after(std::chrono::milliseconds delay, const TaskOperatorPtr& task) {
    if (!task) {
        return 0;
    }
    
    if (ThreadPoolConfigManager::getInstance().isTimerOptimizationEnabled()) {
        return TimerManager::getInstance().scheduleTask(task, delay);
    } else {
        mImpl->after(delay, task);
        return 1;
    }
}

uint64_t TaskQueueEnhanced::after(std::chrono::milliseconds delay, Func&& func) {
    auto task = makeTask(std::move(func));
    return after(delay, task);
}

uint64_t TaskQueueEnhanced::after(TaskMark mark, std::chrono::milliseconds delay, Func&& func) {
    auto task = makeTask(mark, std::move(func));
    return after(delay, task);
}

bool TaskQueueEnhanced::cancelDelayedTask(uint64_t taskId) {
    if (taskId == 0) {
        return false;
    }
    
    if (ThreadPoolConfigManager::getInstance().isTimerOptimizationEnabled()) {
        return TimerManager::getInstance().cancelTask(taskId);
    }
    
    // 传统方式无法取消，返回false
    return false;
}

// === 队列状态查询实现 ===
QueueStats TaskQueueEnhanced::getStats() const {
    std::lock_guard<std::mutex> lock(mStatsMutex);
    return mStats;
}

bool TaskQueueEnhanced::isEmpty() const {
    // 这里需要队列实现提供更详细的状态信息
    // 暂时返回pending count是否为0
    return pendingTaskCount() == 0;
}

size_t TaskQueueEnhanced::pendingTaskCount() const {
    std::lock_guard<std::mutex> lock(mStatsMutex);
    return mStats.pendingTaskCount;
}

// === 队列控制实现 ===
void TaskQueueEnhanced::pause() {
    mIsPaused.store(true);
    LOGE("[TASK] TaskQueueEnhanced '%s' paused", mLabel.c_str());
}

void TaskQueueEnhanced::resume() {
    mIsPaused.store(false);
    LOGE("[TASK] TaskQueueEnhanced '%s' resumed", mLabel.c_str());
}

bool TaskQueueEnhanced::isPaused() const {
    return mIsPaused.load();
}

void TaskQueueEnhanced::clear() {
    // 注意：这里无法清除已经在执行中的任务
    // 只能设置标志，让新任务不再执行
    std::lock_guard<std::mutex> lock(mStatsMutex);
    mStats.cancelledTaskCount += mStats.pendingTaskCount;
    mStats.pendingTaskCount = 0;
    LOGE("[TASK] TaskQueueEnhanced '%s' cleared", mLabel.c_str());
}

// === 任务链支持实现 ===
TaskOperatorEnhancedPtr TaskQueueEnhanced::createTaskChain(const std::vector<Func>& functions) {
    if (functions.empty()) {
        return nullptr;
    }
    
    // 创建第一个任务
    auto firstTask = makeTask(functions[0]);
    auto currentTask = firstTask;
    
    // 链接后续任务
    for (size_t i = 1; i < functions.size(); ++i) {
        auto nextTask = makeTask(functions[i]);
        currentTask->setNextTask(nextTask);
        currentTask = nextTask;
    }
    
    return firstTask;
}

void TaskQueueEnhanced::asyncChain(const std::vector<Func>& functions) {
    auto chainTask = createTaskChain(functions);
    if (chainTask) {
        async(chainTask);
    }
}

// === 私有辅助方法实现 ===
void TaskQueueEnhanced::updateStats(const TaskOperatorEnhancedPtr& task) {
    if (!task) {
        return;
    }
    
    std::lock_guard<std::mutex> lock(mStatsMutex);
    mStats.pendingTaskCount++;
    
    // 这里可以添加更详细的统计逻辑
    // 例如：基于任务的执行结果更新completedTaskCount或failedTaskCount
}

TaskOperatorPtr TaskQueueEnhanced::adaptToLegacy(const TaskOperatorEnhancedPtr& task) {
    if (!task) {
        return nullptr;
    }
    
    return std::make_shared<TaskOperatorAdapter>(task);
}

TaskOperatorEnhancedPtr TaskQueueEnhanced::createRetryTask(const TaskOperatorEnhancedPtr& originalTask, 
                                                          int maxRetries, 
                                                          std::chrono::milliseconds retryDelay) {
    return makeTask([this, originalTask, maxRetries, retryDelay]() {
        int attempts = 0;
        while (attempts <= maxRetries) {
            try {
                (*originalTask)();
                return; // 成功执行，退出重试循环
            } catch (const std::exception& e) {
                LOGE("[TASK] Task execution failed (attempt %d/%d): %s", 
                     attempts + 1, maxRetries + 1, e.what());
                
                attempts++;
                if (attempts <= maxRetries) {
                    // 等待重试延时
                    std::this_thread::sleep_for(retryDelay);
                } else {
                    // 重试次数用尽，重新抛出异常
                    throw;
                }
            }
        }
    });
}

} // namespace task