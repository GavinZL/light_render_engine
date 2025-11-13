#ifndef __TASK_OPERATOR_H__
#define __TASK_OPERATOR_H__
#include <cstdint>
#include <memory>
#include <functional>
#include "TaskOperator.h"
#include "TaskQueueDefine.h"
namespace task
{
class TaskOperator : public std::enable_shared_from_this<TaskOperator>
{
public:
    using CallBack = std::function<void(const std::shared_ptr<TaskOperator>&)>;

public:
    explicit TaskOperator(TaskMark mark = TaskMark::TM_TaskQueue_Inner);
    explicit TaskOperator(TaskMark mark, CallBack callback);
    virtual ~TaskOperator() = default;
    virtual void operator()();

    // 用户数据
    void setUserData(const std::shared_ptr<void>& userData)
    {
        mUserData = userData;
    }

    template <typename T>
    std::shared_ptr<T> userData() const
    {
        return std::static_pointer_cast<T>(mUserData);
    }

    // 任务标记
    inline TaskMark taskMark() const
    {
        return mTaskMark;
    }

    // task push入队列时，重置时间
    void        resetCallStartTime();
    std::string taskCostInfo() const;

    uint64_t taskRunDuration() const;
    uint64_t taskWaitDuration() const;

private:
    CallBack                      mCallBack{ nullptr };
    mutable std::shared_ptr<void> mUserData{ nullptr };
    TaskMark                      mTaskMark{ TaskMark::TM_Unknown };

protected:
    uint64_t mTaskCallStartTime{ 0 };  ///调用开始时间
    uint64_t mTaskRunStartTime{ 0 };   ///任务开始时间  (mTaskRunStartTime - mTaskCallStartTime) 在队列中等待执行时间
    uint32_t mTaskRunDuration{ 0 };    ///任务执行时间

    void recordRunStart();
    void recordRunEnd();
};
}  // namespace task

#endif