#include "TaskOperatorBackendOptimized.h"
#include "log/Log.h"

namespace task
{

TaskOperatorOptimized::TaskOperatorOptimized(TaskCategory category, uint32_t priority)
    : TaskOperator()
    , mCategory(category)
    , mPriority(priority)
{
}

TaskOperatorOptimized::TaskOperatorOptimized(SimpleCallback callback)
    : TaskOperator()
    , mSimpleCallback(callback)
    , mCategory(TaskCategory::Custom)
    , mPriority(128)
{
}

TaskOperatorOptimized::TaskOperatorOptimized(TaskCategory category, uint32_t priority, SimpleCallback callback)
    : TaskOperator()
    , mSimpleCallback(callback)
    , mCategory(category)
    , mPriority(priority)
{
}

void TaskOperatorOptimized::operator()() {
    if (mIsCancelled.load()) {
        LOGE("[TASK] TaskOperatorOptimized: task was cancelled, skipping execution");
        return;
    }
    
    recordRunStart();
    
    try {
        if (mSimpleCallback) {
            mSimpleCallback();
        } else {
            // 调用原始TaskOperator的执行逻辑作为后备
            TaskOperator::operator()();
        }
    } catch (const std::exception& e) {
        LOGE("[TASK] TaskOperatorOptimized: exception during execution: %s", e.what());
    } catch (...) {
        LOGE("[TASK] TaskOperatorOptimized: unknown exception during execution");
    }
    
    recordRunEnd();
}

} // namespace task