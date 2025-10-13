#ifndef __TIMER_MANAGER_H__
#define __TIMER_MANAGER_H__

#include <chrono>
#include <memory>
#include <atomic>
#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <unordered_map>
#include "TaskQueueDefine.h"

namespace task
{

// 定时器配置
struct TimerConfig {
    std::chrono::milliseconds minDelay = std::chrono::milliseconds(1);
    std::chrono::milliseconds maxDelay = std::chrono::hours(24);
    size_t maxPendingTasks = 10000;
};

// 定时任务项
struct TimerTask {
    uint64_t timerId;
    std::chrono::steady_clock::time_point executeTime;
    TaskOperatorPtr task;
    bool cancelled = false;
    
    // 优先队列需要的比较函数
    bool operator>(const TimerTask& other) const {
        return executeTime > other.executeTime;
    }
};

// 定时器管理器 - 单例模式
class TimerManager {
public:
    static TimerManager& getInstance();
    
    // 调度延时任务
    uint64_t scheduleTask(const TaskOperatorPtr& task, std::chrono::milliseconds delay);
    
    // 取消任务
    bool cancelTask(uint64_t timerId);
    
    // 获取待处理任务数量
    size_t getPendingTaskCount() const;
    
    // 配置管理
    void setConfig(const TimerConfig& config);
    TimerConfig getConfig() const;
    
    // 启动和停止
    void start();
    void stop();
    
private:
    TimerManager();
    ~TimerManager();
    
    // 禁用拷贝和赋值
    TimerManager(const TimerManager&) = delete;
    TimerManager& operator=(const TimerManager&) = delete;
    
    // 定时器线程主循环
    void timerLoop();
    
    // 执行到期任务
    void executeExpiredTasks();
    
    // 获取下一个任务ID
    uint64_t getNextTimerId();
    
private:
    mutable std::mutex mMutex;
    std::condition_variable mCondition;
    std::thread mTimerThread;
    std::atomic<bool> mIsRunning;
    
    // 任务队列 - 使用优先队列按执行时间排序
    std::priority_queue<TimerTask, std::vector<TimerTask>, std::greater<TimerTask>> mTaskQueue;
    
    // 用于快速查找和取消任务
    std::unordered_map<uint64_t, bool> mCancelledTasks;
    
    // 配置
    TimerConfig mConfig;
    
    // 任务ID生成器
    std::atomic<uint64_t> mNextTimerId;
};

} // namespace task

#endif // __TIMER_MANAGER_H__