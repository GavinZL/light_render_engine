#include "TaskOperatorEnhanced.h"
#include "common/HETimerHelper.h"
#include "log/Log.h"
#include <string>

namespace task
{

TaskOperatorEnhanced::TaskOperatorEnhanced(TaskCategory category, uint32_t priority)
    : mCategory(category)
    , mPriority(priority)
    , mTaskMark(TaskMark::TM_TaskQueue_Inner)
{
    resetCallStartTime();
}

TaskOperatorEnhanced::TaskOperatorEnhanced(SimpleCallback callback)
    : mCallback(callback)
    , mCategory(TaskCategory::Custom)
    , mPriority(128)
    , mTaskMark(TaskMark::TM_TaskQueue_Inner)
{
    resetCallStartTime();
}

TaskOperatorEnhanced::TaskOperatorEnhanced(TaskCategory category, uint32_t priority, SimpleCallback callback)
    : mCallback(callback)
    , mCategory(category)
    , mPriority(priority)
    , mTaskMark(TaskMark::TM_TaskQueue_Inner)
{
    resetCallStartTime();
}

TaskOperatorEnhanced::TaskOperatorEnhanced(TaskMark mark, SimpleCallback callback)
    : mCallback(callback)
    , mCategory(TaskCategory::Custom)
    , mPriority(128)
    , mTaskMark(mark)
{
    resetCallStartTime();
}

void TaskOperatorEnhanced::operator()() {
    if (mIsCancelled.load(std::memory_order_acquire)) {
        LOGE("[TASK] TaskOperatorEnhanced: task was cancelled, skipping execution");
        return;
    }
    
    mIsExecuting.store(true, std::memory_order_release);
    recordRunStart();
    
    try {
        if (mCallback) {
            mCallback();
        }
    } catch (const std::exception& e) {
        LOGE("[TASK] TaskOperatorEnhanced: exception during execution: %s", e.what());
    } catch (...) {
        LOGE("[TASK] TaskOperatorEnhanced: unknown exception during execution");
    }
    
    recordRunEnd();
    mIsExecuting.store(false, std::memory_order_release);
    
    // 执行后续任务
    executeNextTask();
}

void TaskOperatorEnhanced::resetCallStartTime() {
    mTaskCallStartTime = comm::HETimerHelper::currentTimeMillis();
}

std::string TaskOperatorEnhanced::taskCostInfo() const {
    return std::string("tskType:") + std::to_string(static_cast<int>(mTaskMark))
           + std::string(" tskCat:") + std::to_string(static_cast<int>(mCategory))
           + std::string(" tskPrio:") + std::to_string(mPriority)
           + std::string(" tskWait: ") + std::to_string(mTaskRunStartTime - mTaskCallStartTime)
           + std::string(" tskRun: ") + std::to_string(mTaskRunDuration);
}

uint64_t TaskOperatorEnhanced::taskRunDuration() const {
    return mTaskRunDuration;
}

uint64_t TaskOperatorEnhanced::taskWaitDuration() const {
    return mTaskRunStartTime - mTaskCallStartTime;
}

void TaskOperatorEnhanced::recordRunStart() {
    mTaskRunStartTime = comm::HETimerHelper::currentTimeMillis();
}

void TaskOperatorEnhanced::recordRunEnd() {
    mTaskRunDuration = comm::HETimerHelper::currentTimeMillis() - mTaskRunStartTime;
}

void TaskOperatorEnhanced::executeNextTask() {
    if (mNextTask && !mNextTask->isCancelled()) {
        try {
            (*mNextTask)();
        } catch (const std::exception& e) {
            LOGE("[TASK] TaskOperatorEnhanced: exception in next task: %s", e.what());
        } catch (...) {
            LOGE("[TASK] TaskOperatorEnhanced: unknown exception in next task");
        }
    }
}

} // namespace task