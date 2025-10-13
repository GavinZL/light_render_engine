// 优化的TaskOperator接口设计
#ifndef __TASK_OPERATOR_ENHANCED_H__
#define __TASK_OPERATOR_ENHANCED_H__

#include <functional>
#include <memory>
#include <atomic>
#include <any>
#include <optional>
#include <chrono>
#include "TaskQueueDefine.h"

namespace task
{

// 任务分类枚举
enum class TaskCategory : uint8_t {
    Camera = 0,
    Pipeline,
    Display,
    Reporter,
    System,
    Custom,
    Count
};

// 增强的TaskOperator类 - 简化的回调接口
class TaskOperatorEnhanced : public std::enable_shared_from_this<TaskOperatorEnhanced>
{
public:
    // 简化的回调函数类型 - 不再需要传递TaskOperator参数
    using SimpleCallback = std::function<void()>;
    
    // 构造函数
    explicit TaskOperatorEnhanced(TaskCategory category = TaskCategory::Custom, 
                                uint32_t priority = 128);
    explicit TaskOperatorEnhanced(SimpleCallback callback);
    explicit TaskOperatorEnhanced(TaskCategory category, uint32_t priority, SimpleCallback callback);
    explicit TaskOperatorEnhanced(TaskMark mark, SimpleCallback callback);  // 向后兼容
    
    virtual ~TaskOperatorEnhanced() = default;
    
    // 执行任务
    virtual void operator()();
    
    // 类型安全的用户数据存储
    template<typename T>
    void setUserData(T&& data) {
        mUserData = std::make_any<std::decay_t<T>>(std::forward<T>(data));
    }
    
    template<typename T>
    std::optional<T> getUserData() const {
        try {
            if (mUserData.has_value()) {
                return std::any_cast<T>(mUserData);
            }
        } catch (const std::bad_any_cast&) {
            // 类型不匹配，返回空值
        }
        return std::nullopt;
    }
    
    // 清除用户数据
    void clearUserData() { mUserData.reset(); }
    bool hasUserData() const { return mUserData.has_value(); }
    
    // 任务控制
    void cancel() { mIsCancelled.store(true, std::memory_order_release); }
    bool isCancelled() const { return mIsCancelled.load(std::memory_order_acquire); }
    
    // 分类和优先级控制
    void setCategory(TaskCategory category) { mCategory = category; }
    TaskCategory getCategory() const { return mCategory; }
    void setPriority(uint32_t priority) { mPriority = priority; }
    uint32_t getPriority() const { return mPriority; }
    
    // 任务标记 - 向后兼容
    void setTaskMark(TaskMark mark) { mTaskMark = mark; }
    TaskMark taskMark() const { return mTaskMark; }
    
    // 性能统计 - 向后兼容
    void resetCallStartTime();
    std::string taskCostInfo() const;
    uint64_t taskRunDuration() const;
    uint64_t taskWaitDuration() const;
    
    // 任务状态
    bool isExecuting() const { return mIsExecuting.load(std::memory_order_acquire); }
    
    // 任务链 - 新功能：任务完成后执行后续任务
    void setNextTask(std::shared_ptr<TaskOperatorEnhanced> nextTask) { mNextTask = nextTask; }
    std::shared_ptr<TaskOperatorEnhanced> getNextTask() const { return mNextTask; }
    
protected:
    void recordRunStart();
    void recordRunEnd();
    void executeNextTask();
    
private:
    SimpleCallback mCallback;
    std::any mUserData;
    TaskCategory mCategory;
    uint32_t mPriority;
    TaskMark mTaskMark;
    
    // 状态控制
    std::atomic<bool> mIsCancelled{false};
    std::atomic<bool> mIsExecuting{false};
    
    // 性能统计
    uint64_t mTaskCallStartTime{0};
    uint64_t mTaskRunStartTime{0}; 
    uint32_t mTaskRunDuration{0};
    
    // 任务链
    std::shared_ptr<TaskOperatorEnhanced> mNextTask;
};

// 类型别名
using TaskOperatorEnhancedPtr = std::shared_ptr<TaskOperatorEnhanced>;

// 工厂函数 - 便于创建任务
template<typename Func>
TaskOperatorEnhancedPtr makeTask(Func&& func, TaskCategory category = TaskCategory::Custom, uint32_t priority = 128) {
    return std::make_shared<TaskOperatorEnhanced>(category, priority, std::forward<Func>(func));
}

template<typename Func>
TaskOperatorEnhancedPtr makeTask(TaskMark mark, Func&& func) {
    return std::make_shared<TaskOperatorEnhanced>(mark, std::forward<Func>(func));
}

// 向后兼容的适配器类
class TaskOperatorAdapter : public TaskOperator {
public:
    explicit TaskOperatorAdapter(TaskOperatorEnhancedPtr enhancedTask)
        : TaskOperator(enhancedTask->taskMark())
        , mEnhancedTask(enhancedTask) {}
    
    virtual void operator()() override {
        if (mEnhancedTask) {
            (*mEnhancedTask)();
        }
    }
    
    virtual void cancel() override { 
        TaskOperator::cancel();
        if (mEnhancedTask) {
            mEnhancedTask->cancel();
        }
    }
    
    virtual bool isCancelled() const override {
        return TaskOperator::isCancelled() || (mEnhancedTask && mEnhancedTask->isCancelled());
    }
    
private:
    TaskOperatorEnhancedPtr mEnhancedTask;
};

} // namespace task

#endif // __TASK_OPERATOR_ENHANCED_H__