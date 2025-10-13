// 优化的工作线程实现
#ifndef __WORK_THREAD_CONCURRENCY_ENHANCED_H__
#define __WORK_THREAD_CONCURRENCY_ENHANCED_H__

#include "WorkThreadConcurrency.h"
#include "LoadBalancer.h"
#include <vector>
#include <chrono>

namespace task
{

// 等待策略枚举
enum class WaitStrategy {
    AggressiveSpin,  // 积极自旋
    ModerateWait,    // 适度等待
    PatientWait      // 耐心等待
};

// 增强的并发工作线程
class WorkThreadConcurrencyEnhanced : public WorkThreadConcurrency {
public:
    explicit WorkThreadConcurrencyEnhanced(const std::weak_ptr<IThreadPool>& threadPool);
    ~WorkThreadConcurrencyEnhanced() = default;
    
protected:
    // 重写并行处理方法
    bool _parallel() override;
    
    // 批量任务获取
    bool acquireTaskBatch(const std::shared_ptr<IThreadPool::Data>& data,
                         std::vector<TaskOperatorPtr>& batch,
                         size_t maxBatchSize);
    
    // 增强的空闲状态处理
    bool handleIdleState(const std::shared_ptr<IThreadPool::Data>& data);
    
    // 智能等待策略确定
    WaitStrategy determineWaitStrategy(const std::shared_ptr<IThreadPool::Data>& data) const;
    
    // 执行任务批次
    void executeBatch(const std::vector<TaskOperatorPtr>& batch);
    
    // 系统负载检查
    bool hasHighSystemLoad() const;
    
    // 统计信息更新
    void updateThreadStats();
    
private:
    static const size_t BATCH_SIZE = 8;  // 批量处理大小
    std::chrono::steady_clock::time_point mLastTaskTime;
    int64_t mTasksExecuted = 0;
    double mAvgTaskDuration = 0.0;
};

} // namespace task

#endif // __WORK_THREAD_CONCURRENCY_ENHANCED_H__