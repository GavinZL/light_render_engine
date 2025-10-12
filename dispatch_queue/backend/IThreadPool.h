#ifndef IThreadPool_H
#define IThreadPool_H

#include <memory>
#include "TaskQueueDefine.h"
#include "Semaphore.h"
#include "QueueDefine.h"
namespace task
{
class WorkThreadBase;
class IThreadPool : public std::enable_shared_from_this<IThreadPool>
{
public:
    // 全局线程池
    static const std::shared_ptr<IThreadPool>& serialThreadPool();
    static const std::shared_ptr<IThreadPool>& parallelThreadPool();

public:
    virtual ~IThreadPool() = default;

    // 线程池管理线程生命周期
    virtual void registerWorkThread(const std::shared_ptr<WorkThreadBase>& thread)   = 0;
    virtual void unregisterWorkThread(const std::shared_ptr<WorkThreadBase>& thread) = 0;

    // 独占线程接口
    virtual void    execute(const TaskOperatorPtr& task, int32_t threadId) {}
    virtual int32_t attachOneThread(const std::string& name, WorkThreadPriority priority = WorkThreadPriority::WTP_Normal)
    {
        return -1;
    }
    virtual void detachOneThread(int32_t threadID) {}

    // 全局线程池接口
    virtual void execute(const TaskOperatorPtr& task, TaskQueuePriority priority = TaskQueuePriority::TQP_Normal) {}

    // 线程池数据
    struct Data
    {
        Semaphore                                                  mSemaphore;              //信号量通知线程执行
        std::atomic<int32_t>                                       mMaxThreads{ 0 };        //线程池允许最大创建数
        std::atomic<int32_t>                                       mActiveThreads{ 0 };     //活动线程数
        std::atomic<int32_t>                                       mIdleThreads{ 0 };       //认为在wait等待的线程为idle线程
        std::array<WorkQueue, ( int )TaskQueuePriority::TQP_Count> mTaskQueues;             // 按优先级定义队列
    };
    virtual const std::shared_ptr<Data> getData() const
    {
        return nullptr;
    }
};

}  // namespace task

#endif  // IThreadPool_H