#include "TaskOperator.h"
#include "common/HETimerHelper.h"
#include <string>
namespace task
{

TaskOperator::TaskOperator(TaskMark mark)
    : mTaskMark(mark)
{
    resetCallStartTime();
}

TaskOperator::TaskOperator(TaskMark mark, CallBack callback)
    : mTaskMark(mark)
    , mCallBack(std::move(callback))
{
    resetCallStartTime();
}

void TaskOperator::operator()()
{
    recordRunStart();
    if (mCallBack)
    {
        mCallBack(shared_from_this());
    }
    recordRunEnd();
}

void TaskOperator::resetCallStartTime()
{
    mTaskCallStartTime = comm::HETimerHelper::currentTimeMillis();
}

std::string TaskOperator::taskCostInfo() const
{
    return std::string("tskType:") + std::to_string(static_cast<int>(mTaskMark))
           + std::string(" tskWait: ") + std::to_string(mTaskRunStartTime - mTaskCallStartTime)
           + std::string(" tskRun: ") + std::to_string(mTaskRunDuration);
}

uint64_t TaskOperator::taskRunDuration() const
{
    return mTaskRunDuration;
}

uint64_t TaskOperator::taskWaitDuration() const
{
    return mTaskRunStartTime - mTaskCallStartTime;
}

void TaskOperator::recordRunStart()
{
    mTaskRunStartTime = comm::HETimerHelper::currentTimeMillis();
}
void TaskOperator::recordRunEnd()
{
    mTaskRunDuration = comm::HETimerHelper::currentTimeMillis() - mTaskRunStartTime;
}

}  // namespace task