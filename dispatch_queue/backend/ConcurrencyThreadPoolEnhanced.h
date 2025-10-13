// 优化的并发线程池实现
#ifndef __CONCURRENCY_THREAD_POOL_ENHANCED_H__
#define __CONCURRENCY_THREAD_POOL_ENHANCED_H__

#include "ConcurrencyThreadPool.h"
#include "LoadBalancer.h"
#include "ThreadPoolConfig.h"
#include "TaskQueueConstant.h"
#include <memory>
#include <atomic>
#include <chrono>

namespace task
{

// 优化的并发线程池
class ConcurrencyThreadPoolEnhanced : public ConcurrencyThreadPool {
public:
    explicit ConcurrencyThreadPoolEnhanced();
    ~ConcurrencyThreadPoolEnhanced();
    
    // 重写调度方法
    virtual void execute(const TaskOperatorPtr& task, TaskQueuePriority priority = TaskQueuePriority::TQP_Normal) override;
    
protected:
    // 增强的调度算法
    void _scheduleEnhanced(TaskQueuePriority priority);
    
    // 智能线程管理
    bool shouldCreateNewThread(TaskQueuePriority priority);
    void createOptimalThread(TaskQueuePriority priority, const LoadMetrics& metrics);
    void optimizeExistingThreads(const LoadMetrics& metrics);
    
    // 负载分析
    LoadMetrics analyzeCurrentLoad(TaskQueuePriority priority);
    
    // 线程池统计
    void updateThreadPoolStats();
    
    // 温度控制（防止频繁创建销毁）
    bool isThreadCreationCooldown();
    void updateLastThreadCreationTime();
    
private:
    // 负载分析器
    std::unique_ptr<EnhancedLoadAnalyzer> mLoadAnalyzer;
    std::unique_ptr<AdaptiveThresholdManager> mThresholdManager;
    
    // 统计信息
    std::atomic<int64_t> mTotalTasksExecuted{0};
    std::atomic<int64_t> mTotalThreadsCreated{0};
    std::atomic<int64_t> mTotalThreadsDestroyed{0};
    
    // 温度控制
    std::chrono::steady_clock::time_point mLastThreadCreationTime;
    std::chrono::milliseconds mMinThreadCreationInterval{std::chrono::milliseconds(100)};
    
    // 配置引用
    const ThreadPoolConfig& mConfig;
    
    // 任务执行时间统计
    std::atomic<double> mAvgTaskDuration{0.0};
    std::atomic<int64_t> mTaskDurationSamples{0};
};

// 线程池统计信息
struct ThreadPoolStats {
    int32_t activeThreads = 0;
    int32_t idleThreads = 0;
    int32_t maxThreads = 0;
    int64_t totalTasksExecuted = 0;
    int64_t totalThreadsCreated = 0;
    int64_t totalThreadsDestroyed = 0;
    double avgTaskDuration = 0.0;
    std::chrono::steady_clock::time_point lastStatsUpdate;
};

} // namespace task

#endif // __CONCURRENCY_THREAD_POOL_ENHANCED_H__