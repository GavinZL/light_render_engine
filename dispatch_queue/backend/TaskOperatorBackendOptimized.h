// 优化的操作基类
#ifndef __TASK_OPERATOR_BACKEND_OPTIMIZED_H__
#define __TASK_OPERATOR_BACKEND_OPTIMIZED_H__

#include <memory>
#include <chrono>
#include <any>
#include "TaskOperator.h"
#include "TaskQueueDefine.h"
#include "LWBarrier.h"
#include "Consumable.h"
#include "../TimerManager.h"

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

// 优化后的TaskOperator基类
class TaskOperatorOptimized : public TaskOperator
{
public:
    using SimpleCallback = std::function<void()>;
    
    explicit TaskOperatorOptimized(TaskCategory category = TaskCategory::Custom, 
                                 uint32_t priority = 128);
    explicit TaskOperatorOptimized(SimpleCallback callback);
    explicit TaskOperatorOptimized(TaskCategory category, uint32_t priority, SimpleCallback callback);
    virtual ~TaskOperatorOptimized() = default;

    virtual void operator()() override;

    // 类型安全的用户数据
    template<typename T>
    void setUserData(T&& data) {
        mUserData = std::make_any<T>(std::forward<T>(data));
    }
    
    template<typename T>
    std::optional<T> getUserData() const {
        try {
            if (mUserData.has_value()) {
                return std::any_cast<T>(mUserData);
            }
        } catch (const std::bad_any_cast& e) {
            // 类型不匹配，返回空值
        }
        return std::nullopt;
    }

    // 任务控制
    void cancel() { mIsCancelled.store(true); }
    bool isCancelled() const { return mIsCancelled.load(); }
    
    // 分类和优先级控制
    void setCategory(TaskCategory category) { mCategory = category; }
    TaskCategory getCategory() const { return mCategory; }
    void setPriority(uint32_t priority) { mPriority = priority; }
    uint32_t getPriority() const { return mPriority; }

private:
    SimpleCallback mSimpleCallback;
    std::any mUserData;
    TaskCategory mCategory;
    uint32_t mPriority;
    std::atomic<bool> mIsCancelled{false};
};

// 处理同步任务 - 保持向后兼容
class TaskBarrierOperatorOptimized : public TaskOperator
{
public:
    explicit TaskBarrierOperatorOptimized(const TaskOperatorPtr& op)
        : mRealTask(op)
    {
    }
    ~TaskBarrierOperatorOptimized() = default;

    virtual void operator()() override
    {
        recordRunStart();
        if (mRealTask && !mRealTask->isCancelled()) {
            (*mRealTask)();
        }
        mBarrier.notify();
        recordRunEnd();
    }

    void wait(std::chrono::milliseconds timeout = std::chrono::milliseconds(-1))
    {
        mBarrier.wait(timeout);
    }

private:
    TaskOperatorPtr mRealTask;
    LWBarrier mBarrier;
};

// 优化的延时任务 - 不再阻塞线程
class TaskDelayOperatorOptimized : public TaskOperator
{
public:
    TaskDelayOperatorOptimized(std::chrono::milliseconds delay, const TaskOperatorPtr& op)
        : mDelay(delay)
        , mRealTask(op)
        , mTimerId(0)
    {
    }
    ~TaskDelayOperatorOptimized() {
        // 确保定时任务被取消
        if (mTimerId != 0) {
            TimerManager::getInstance().cancelTask(mTimerId);
        }
    }

    // 不再阻塞线程，而是通过定时器管理器调度
    virtual void operator()() override
    {
        if (!mRealTask) {
            return;
        }
        
        // 将真实任务提交给定时器管理器
        mTimerId = TimerManager::getInstance().scheduleTask(mRealTask, mDelay);
        
        if (mTimerId == 0) {
            // 定时器调度失败，立即执行任务
            recordRunStart();
            (*mRealTask)();
            recordRunEnd();
        }
    }
    
    // 取消延时任务
    bool cancel() {
        if (mTimerId != 0) {
            return TimerManager::getInstance().cancelTask(mTimerId);
        }
        return false;
    }

private:
    std::chrono::milliseconds mDelay;
    TaskOperatorPtr mRealTask;
    uint64_t mTimerId;
};

// consumable 操作对象 - 保持向后兼容
class ConsumableOperatorOptimized : public TaskOperator
{
public:
    ConsumableOperatorOptimized(const TaskOperatorPtr& op, const ConsumablePtr& consumable)
        : mRealTask(op)
        , mConsumable(consumable)
    {
    }
    ~ConsumableOperatorOptimized() = default;

    virtual void operator()() override
    {
        recordRunStart();
        if (mRealTask && !mRealTask->isCancelled()) {
            (*mRealTask)();
        }
        // 释放资源
        if (mConsumable) {
            mConsumable->release();
        }
        recordRunEnd();
    }

private:
    ConsumablePtr mConsumable;
    TaskOperatorPtr mRealTask;
};

} // namespace task

#endif // __TASK_OPERATOR_BACKEND_OPTIMIZED_H__