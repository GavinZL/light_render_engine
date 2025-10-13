// 增强的TaskQueue接口
#ifndef __TASK_QUEUE_ENHANCED_H__
#define __TASK_QUEUE_ENHANCED_H__

#include <functional>
#include <vector>
#include <memory>
#include <string>
#include <chrono>
#include <optional>
#include "TaskQueueDefine.h"
#include "TaskOperatorEnhanced.h"

namespace task
{

// 队列统计信息
struct QueueStats {
    size_t pendingTaskCount = 0;
    size_t completedTaskCount = 0;
    size_t failedTaskCount = 0;
    size_t cancelledTaskCount = 0;
    double averageWaitTime = 0.0;
    double averageExecutionTime = 0.0;
    std::chrono::milliseconds totalExecutionTime{0};
};

// 增强的任务队列类
class TaskQueueEnhanced final
{
public:
    using Func = std::function<void()>;
    using ConditionFunc = std::function<bool()>;

public:
    explicit TaskQueueEnhanced(const std::string& label, const TaskQueueImplPtr& impl);
    ~TaskQueueEnhanced() = default;

    // 队列标签
    const std::string& label() const { return mLabel; }

    // === 基础异步任务接口 ===
    void async(const TaskOperatorEnhancedPtr& task);
    void async(const TaskOperatorPtr& task);  // 向后兼容
    
    // 简化的lambda接口
    void async(Func&& func);
    void async(TaskMark mark, Func&& func);
    void async(TaskCategory category, Func&& func);
    void async(TaskCategory category, uint32_t priority, Func&& func);

    // === 批量操作接口 ===
    void asyncBatch(const std::vector<TaskOperatorEnhancedPtr>& tasks);
    void asyncBatch(const std::vector<Func>& functions);
    
    // === 条件执行接口 ===
    void asyncIf(ConditionFunc condition, const TaskOperatorEnhancedPtr& task);
    void asyncIf(ConditionFunc condition, Func&& func);
    
    // === 重试机制接口 ===
    void asyncWithRetry(const TaskOperatorEnhancedPtr& task, int maxRetries, 
                       std::chrono::milliseconds retryDelay = std::chrono::milliseconds(100));
    void asyncWithRetry(Func&& func, int maxRetries,
                       std::chrono::milliseconds retryDelay = std::chrono::milliseconds(100));

    // === 同步任务接口 ===
    void sync(const TaskOperatorEnhancedPtr& task, 
              std::chrono::milliseconds timeout = std::chrono::milliseconds(-1));
    void sync(const TaskOperatorPtr& task, 
              std::chrono::milliseconds timeout = std::chrono::milliseconds(-1));  // 向后兼容
    void sync(Func&& func, std::chrono::milliseconds timeout = std::chrono::milliseconds(-1));
    void sync(TaskMark mark, Func&& func, std::chrono::milliseconds timeout = std::chrono::milliseconds(-1));

    // === 延时任务接口 ===
    uint64_t after(std::chrono::milliseconds delay, const TaskOperatorEnhancedPtr& task);
    uint64_t after(std::chrono::milliseconds delay, const TaskOperatorPtr& task);  // 向后兼容
    uint64_t after(std::chrono::milliseconds delay, Func&& func);
    uint64_t after(TaskMark mark, std::chrono::milliseconds delay, Func&& func);
    
    // 取消延时任务
    bool cancelDelayedTask(uint64_t taskId);

    // === 队列状态查询 ===
    QueueStats getStats() const;
    bool isEmpty() const;
    size_t pendingTaskCount() const;
    
    // === 队列控制 ===
    void pause();
    void resume();
    bool isPaused() const;
    void clear();
    
    // === 任务链支持 ===
    TaskOperatorEnhancedPtr createTaskChain(const std::vector<Func>& functions);
    void asyncChain(const std::vector<Func>& functions);

private:
    std::string mLabel;
    std::shared_ptr<class IQueueImpl> mImpl;
    mutable std::mutex mStatsMutex;
    QueueStats mStats;
    std::atomic<bool> mIsPaused{false};
    
    // 内部辅助方法
    void updateStats(const TaskOperatorEnhancedPtr& task);
    TaskOperatorPtr adaptToLegacy(const TaskOperatorEnhancedPtr& task);
    TaskOperatorEnhancedPtr createRetryTask(const TaskOperatorEnhancedPtr& originalTask, 
                                           int maxRetries, std::chrono::milliseconds retryDelay);
};

// 类型别名
using TaskQueueEnhancedPtr = std::shared_ptr<TaskQueueEnhanced>;

// 便利函数
template<typename... Args>
TaskOperatorEnhancedPtr makeSimpleTask(Args&&... args) {
    return makeTask(std::forward<Args>(args)...);
}

} // namespace task

#endif // __TASK_QUEUE_ENHANCED_H__