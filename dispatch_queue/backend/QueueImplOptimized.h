// 优化的队列实现适配器
#ifndef __QUEUE_IMPL_OPTIMIZED_H__
#define __QUEUE_IMPL_OPTIMIZED_H__

#include "ConcurrencyQueueImpl.h"
#include "SerialQueueImpl.h"
#include "../TimerManager.h"
#include "TaskOperatorBackend.h"

namespace task
{

// 优化的并发队列实现
class ConcurrencyQueueImplOptimized : public ConcurrencyQueueImpl {
public:
    ConcurrencyQueueImplOptimized(TaskQueuePriority prio, const ThreadPoolPtr& threadPool)
        : ConcurrencyQueueImpl(prio, threadPool) {}
    
    virtual ~ConcurrencyQueueImplOptimized() = default;
    
    // 重写after方法使用定时器管理器
    virtual void after(std::chrono::milliseconds delay, const TaskOperatorPtr& task) override {
        assert(_threadPool());
        task->resetCallStartTime();
        
        // 使用定时器管理器而不是阻塞线程的TaskDelayOperator
        uint64_t timerId = TimerManager::getInstance().scheduleTask(task, delay);
        
        if (timerId == 0) {
            // 定时器调度失败，回退到原有方式
            LOGE("[TASK] Timer scheduling failed, falling back to original delay method");
            ConcurrencyQueueImpl::after(delay, task);
        }
    }
};

// 优化的串行队列实现
class SerialQueueImplOptimized : public SerialQueueImpl {
public:
    SerialQueueImplOptimized(const std::string& label,
                           bool isExclusive,
                           const ThreadPoolPtr& threadPool,
                           WorkThreadPriority prio)
        : SerialQueueImpl(label, isExclusive, threadPool, prio) {}
    
    virtual ~SerialQueueImplOptimized() = default;
    
    // 重写after方法使用定时器管理器
    virtual void after(std::chrono::milliseconds delay, const TaskOperatorPtr& task) override {
        assert(_threadPool());
        task->resetCallStartTime();
        
        // 创建一个包装任务，将真实任务通过串行队列调度
        auto wrappedTask = std::make_shared<TaskOperator>(task::TaskMark::TM_TaskQueue_Inner, 
            [this, task](const TaskOperatorPtr&) {
                // 将真实任务加入串行队列
                this->async(task);
            });
        
        // 使用定时器管理器调度包装任务
        uint64_t timerId = TimerManager::getInstance().scheduleTask(wrappedTask, delay);
        
        if (timerId == 0) {
            // 定时器调度失败，回退到原有方式
            LOGE("[TASK] Timer scheduling failed, falling back to original delay method");
            SerialQueueImpl::after(delay, task);
        }
    }
};

} // namespace task

#endif // __QUEUE_IMPL_OPTIMIZED_H__