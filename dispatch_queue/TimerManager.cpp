#include "TimerManager.h"
#include "TaskQueueFactory.h"
#include "log/Log.h"
#include <algorithm>

namespace task
{

TimerManager& TimerManager::getInstance() {
    static TimerManager instance;
    return instance;
}

TimerManager::TimerManager()
    : mIsRunning(false)
    , mNextTimerId(1)
{
    start();
}

TimerManager::~TimerManager() {
    stop();
}

void TimerManager::start() {
    if (mIsRunning.exchange(true)) {
        return; // 已经启动
    }
    
    mTimerThread = std::thread(&TimerManager::timerLoop, this);
    LOGE("[TIMER] TimerManager started");
}

void TimerManager::stop() {
    if (!mIsRunning.exchange(false)) {
        return; // 已经停止
    }
    
    // 通知定时器线程退出
    {
        std::lock_guard<std::mutex> lock(mMutex);
        mCondition.notify_all();
    }
    
    if (mTimerThread.joinable()) {
        mTimerThread.join();
    }
    
    LOGE("[TIMER] TimerManager stopped");
}

uint64_t TimerManager::scheduleTask(const TaskOperatorPtr& task, std::chrono::milliseconds delay) {
    if (!task) {
        LOGE("[TIMER] scheduleTask: task is null");
        return 0;
    }
    
    // 验证延时范围
    if (delay < mConfig.minDelay || delay > mConfig.maxDelay) {
        LOGE("[TIMER] scheduleTask: invalid delay %lld ms", delay.count());
        return 0;
    }
    
    std::lock_guard<std::mutex> lock(mMutex);
    
    // 检查任务队列大小限制
    if (mTaskQueue.size() >= mConfig.maxPendingTasks) {
        LOGE("[TIMER] scheduleTask: too many pending tasks %zu", mTaskQueue.size());
        return 0;
    }
    
    uint64_t timerId = getNextTimerId();
    auto executeTime = std::chrono::steady_clock::now() + delay;
    
    TimerTask timerTask;
    timerTask.timerId = timerId;
    timerTask.executeTime = executeTime;
    timerTask.task = task;
    timerTask.cancelled = false;
    
    mTaskQueue.push(timerTask);
    mCondition.notify_one();
    
    LOGE("[TIMER] Task %llu scheduled for %lld ms later", timerId, delay.count());
    return timerId;
}

bool TimerManager::cancelTask(uint64_t timerId) {
    if (timerId == 0) {
        return false;
    }
    
    std::lock_guard<std::mutex> lock(mMutex);
    mCancelledTasks[timerId] = true;
    
    LOGE("[TIMER] Task %llu cancelled", timerId);
    return true;
}

size_t TimerManager::getPendingTaskCount() const {
    std::lock_guard<std::mutex> lock(mMutex);
    return mTaskQueue.size();
}

void TimerManager::setConfig(const TimerConfig& config) {
    std::lock_guard<std::mutex> lock(mMutex);
    mConfig = config;
}

TimerConfig TimerManager::getConfig() const {
    std::lock_guard<std::mutex> lock(mMutex);
    return mConfig;
}

void TimerManager::timerLoop() {
    LOGE("[TIMER] Timer loop started");
    
    while (mIsRunning.load()) {
        std::unique_lock<std::mutex> lock(mMutex);
        
        if (mTaskQueue.empty()) {
            // 没有任务，等待新任务或停止信号
            mCondition.wait(lock, [this] { 
                return !mTaskQueue.empty() || !mIsRunning.load(); 
            });
            continue;
        }
        
        // 获取最早的任务
        const auto& nextTask = mTaskQueue.top();
        auto now = std::chrono::steady_clock::now();
        
        if (nextTask.executeTime <= now) {
            // 任务到期，执行它
            executeExpiredTasks();
        } else {
            // 等待到下一个任务执行时间
            auto waitTime = nextTask.executeTime - now;
            mCondition.wait_for(lock, waitTime, [this] { 
                return !mIsRunning.load(); 
            });
        }
    }
    
    LOGE("[TIMER] Timer loop stopped");
}

void TimerManager::executeExpiredTasks() {
    auto now = std::chrono::steady_clock::now();
    std::vector<TimerTask> tasksToExecute;
    
    // 收集所有到期的任务
    while (!mTaskQueue.empty()) {
        const auto& task = mTaskQueue.top();
        if (task.executeTime > now) {
            break; // 后续任务都还未到期
        }
        
        // 检查任务是否被取消
        auto it = mCancelledTasks.find(task.timerId);
        if (it != mCancelledTasks.end()) {
            mCancelledTasks.erase(it);
            LOGE("[TIMER] Task %llu was cancelled, skipping", task.timerId);
        } else {
            tasksToExecute.push_back(task);
        }
        
        mTaskQueue.pop();
    }
    
    // 在释放锁后执行任务，避免长时间持有锁
    if (!tasksToExecute.empty()) {
        // 获取全局并发队列来执行定时任务
        auto globalQueue = TaskQueueFactory::getInstance().globalConcurrencyQueue();
        
        for (const auto& timerTask : tasksToExecute) {
            if (mIsRunning.load()) {
                globalQueue->async(timerTask.task);
                LOGE("[TIMER] Task %llu executed", timerTask.timerId);
            }
        }
    }
}

uint64_t TimerManager::getNextTimerId() {
    return mNextTimerId.fetch_add(1, std::memory_order_relaxed);
}

} // namespace task