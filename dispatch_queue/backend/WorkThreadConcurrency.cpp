#include "WorkThreadConcurrency.h"
#include "log/Log.h"
#include "TaskQueueConstant.h"
#include "TaskOperator.h"
#include "common/HETimerHelper.h"
#include "display/utils/LogHelper.h"
#include "common/report/HEThreadMonitor.hpp"
#include <cassert>
#include <chrono>
#include <cstdio>
namespace task
{
WorkThreadConcurrency::WorkThreadConcurrency(const std::weak_ptr<IThreadPool>& threadPool)
    : WorkThreadBase(threadPool)
{
    mThread = std::thread(&WorkThreadConcurrency::_run, this);
    mName   = "parallel_" + std::to_string(threadId());
    LOGE("[TASK]WorkThreadConcurrency::WorkThreadConcurrency, threadId: %d", threadId());
}

WorkThreadConcurrency::~WorkThreadConcurrency()
{
    LOGE("[TASK]WorkThreadConcurrency::~WorkThreadConcurrency, join before threadId: %d", threadId());
    cancel();
    if (mThread.joinable())
    {
        mThread.join();
    }
    LOGE("[TASK]WorkThreadConcurrency::~WorkThreadConcurrency, join after threadId: %d", threadId());
}

void WorkThreadConcurrency::_run()
{
    comm::HEThreadScopeMonitor monitor(getName(), comm::HEThreadMonitor::GetInstance().getCurrentThreadId());
    LOGE("[TASK]WorkThreadConcurrency::run, threadId: %d, name: %s", threadId(), mName.c_str());
    while (!mIsCancelled.load(std::memory_order_acquire))
    {
        _changeName();
        _changePriority();
        // 如果线程长时间没有任务执行，自动退出线程
        if (!_parallel())
        {
            break;
        }
    }

    // 线程结束，从线程池中移除, 结束自己
    auto pool = _getThreadPool();
    if (pool)
    {
        pool->unregisterWorkThread(shared_from_this());
    }

    LOGE("[TASK]WorkThreadConcurrency::run, threadId: %d, exit, name: %s\n", threadId(), mName.c_str());
}

bool WorkThreadConcurrency::_parallel()
{
    auto& data = _getThreadPool()->getData();
    assert(data);
    if (data == nullptr)
    {
        LOGE("[TASK][HY] WorkThreadConcurrency::_parallel, threadId: %d, threadPool is null", threadId());
        return false;
    }

    // 线程处于非执行状态，线程池空闲线程+1(所有线程同步看见 memory_order_seq_cst)
    data->mIdleThreads.fetch_add(1, std::memory_order_seq_cst);

    // 整个线程池数据，第一次尝试获取信号量【无锁】
    bool flag = data->mSemaphore.tryAcquire();
    if (!flag)
    {
        // 自旋尝试10次
        flag = data->mSemaphore.spinAcquire(TaskQueueConstant::sMaxSpinCount);
    }
    if (!flag)
    {
        // 等待信号量 或 超时
        flag = data->mSemaphore.waitAcquire(TaskQueueConstant::sMaxSleepTimeout);
        if (!flag)
        {
            // 超时了，尝试再获取一次信号量
            if (!data->mSemaphore.tryAcquire())
            {
                // 失败了，表示线程需要退出了
                return false;
            }
        }
    }

    // 从高到低优先级 进行任务执行
    TaskOperatorPtr op;
    for (int i = ( int )TaskQueuePriority::TQP_High; i >= 0; --i)
    {
        if (data->mTaskQueues[i].try_dequeue(op) && op)
        {
            break;
        }
    }

    // 线程结束等待，线程池空闲线程-1
    data->mIdleThreads.fetch_sub(1, std::memory_order_seq_cst);
    if (op)
    {
        mCurrTask     = op;
        mStartRunTime = comm::HETimerHelper::currentTimeMillis();
        mIsRunning    = true;
        (*op)();
        mIsRunning = false;

        //收集统计信息
        _updateStat(op);
    }

    return true;
}

bool WorkThreadConcurrency::isBlocked() const
{
    // 运行超过5秒，为线程卡住
    return mIsRunning && (comm::HETimerHelper::currentTimeMillis() - mStartRunTime > TaskQueueConstant::sBlockTimeoutThreshold);
}

std::string WorkThreadConcurrency::blockedInfo()
{
    auto msg = _statInfo() + " tid: " + std::to_string(threadId())
               + " blockT:" + std::to_string(comm::HETimerHelper::currentTimeMillis() - mStartRunTime);

    LOGE_D("[TASK] ", msg.c_str());
    return msg;
}

}  // namespace task
