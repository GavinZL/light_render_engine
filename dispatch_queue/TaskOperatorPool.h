// TaskOperator专用对象池管理器
#ifndef __TASK_OPERATOR_POOL_H__
#define __TASK_OPERATOR_POOL_H__

#include "ObjectPool.h"
#include "TaskOperator.h"
#include "TaskOperatorEnhanced.h"
#include "backend/TaskOperatorBackend.h"
#include <unordered_map>
#include <memory>
#include <mutex>

namespace task
{

// TaskOperator池管理器
class TaskOperatorPoolManager {
public:
    static TaskOperatorPoolManager& getInstance();
    
    // === TaskOperator池 ===
    std::shared_ptr<TaskOperator> acquireTaskOperator(TaskMark mark = TaskMark::TM_TaskQueue_Inner);
    std::shared_ptr<TaskOperator> acquireTaskOperator(TaskMark mark, TaskOperator::CallBack callback);
    
    // === TaskOperatorEnhanced池 ===
    std::shared_ptr<TaskOperatorEnhanced> acquireEnhancedTaskOperator(
        TaskCategory category = TaskCategory::Custom, uint32_t priority = 128);
    std::shared_ptr<TaskOperatorEnhanced> acquireEnhancedTaskOperator(
        TaskOperatorEnhanced::SimpleCallback callback);
    std::shared_ptr<TaskOperatorEnhanced> acquireEnhancedTaskOperator(
        TaskCategory category, uint32_t priority, TaskOperatorEnhanced::SimpleCallback callback);
    
    // === 后端操作器池 ===
    std::shared_ptr<TaskBarrierOperator> acquireBarrierOperator(const TaskOperatorPtr& task);
    std::shared_ptr<TaskDelayOperator> acquireDelayOperator(
        std::chrono::milliseconds delay, const TaskOperatorPtr& task);
    std::shared_ptr<ConsumableOperator> acquireConsumableOperator(
        const TaskOperatorPtr& task, const ConsumablePtr& consumable);
    
    // === 池统计和管理 ===
    struct PoolManagerStats {
        ObjectPool<TaskOperator>::PoolStats taskOperatorStats;
        ObjectPool<TaskOperatorEnhanced>::PoolStats enhancedTaskOperatorStats;
        ObjectPool<TaskBarrierOperator>::PoolStats barrierOperatorStats;
        ObjectPool<TaskDelayOperator>::PoolStats delayOperatorStats;
        ObjectPool<ConsumableOperator>::PoolStats consumableOperatorStats;
        size_t totalMemoryUsage; // 估算的内存使用量
    };
    
    PoolManagerStats getStats() const;
    void configurePool(const ObjectPoolConfig& config);
    void shrinkAllPools();
    void clearAllPools();
    
    // === 内存优化控制 ===
    void enablePooling(bool enable) { mPoolingEnabled.store(enable); }
    bool isPoolingEnabled() const { return mPoolingEnabled.load(); }
    
    // 设置内存压力阈值
    void setMemoryPressureThreshold(size_t thresholdBytes) { mMemoryThreshold = thresholdBytes; }
    size_t getMemoryPressureThreshold() const { return mMemoryThreshold; }

private:
    TaskOperatorPoolManager();
    ~TaskOperatorPoolManager() = default;
    
    // 禁用拷贝和赋值
    TaskOperatorPoolManager(const TaskOperatorPoolManager&) = delete;
    TaskOperatorPoolManager& operator=(const TaskOperatorPoolManager&) = delete;
    
    // 对象重置器
    static void resetTaskOperator(std::shared_ptr<TaskOperator>& task);
    static void resetEnhancedTaskOperator(std::shared_ptr<TaskOperatorEnhanced>& task);
    static void resetBarrierOperator(std::shared_ptr<TaskBarrierOperator>& task);
    static void resetDelayOperator(std::shared_ptr<TaskDelayOperator>& task);
    static void resetConsumableOperator(std::shared_ptr<ConsumableOperator>& task);
    
    // 内存压力检查
    bool isUnderMemoryPressure() const;
    void handleMemoryPressure();

private:
    // 各种对象池
    std::unique_ptr<ObjectPool<TaskOperator>> mTaskOperatorPool;
    std::unique_ptr<ObjectPool<TaskOperatorEnhanced>> mEnhancedTaskOperatorPool;
    std::unique_ptr<ObjectPool<TaskBarrierOperator>> mBarrierOperatorPool;
    std::unique_ptr<ObjectPool<TaskDelayOperator>> mDelayOperatorPool;
    std::unique_ptr<ObjectPool<ConsumableOperator>> mConsumableOperatorPool;
    
    // 控制开关
    std::atomic<bool> mPoolingEnabled{true};
    std::atomic<size_t> mMemoryThreshold{100 * 1024 * 1024}; // 100MB默认阈值
    
    mutable std::mutex mConfigMutex;
};

// 便利函数 - 自动使用对象池
namespace pooled {
    // 创建池化的TaskOperator
    inline std::shared_ptr<TaskOperator> makeTaskOperator(TaskMark mark = TaskMark::TM_TaskQueue_Inner) {
        return TaskOperatorPoolManager::getInstance().acquireTaskOperator(mark);
    }
    
    inline std::shared_ptr<TaskOperator> makeTaskOperator(TaskMark mark, TaskOperator::CallBack callback) {
        return TaskOperatorPoolManager::getInstance().acquireTaskOperator(mark, callback);
    }
    
    // 创建池化的TaskOperatorEnhanced
    template<typename Func>
    inline std::shared_ptr<TaskOperatorEnhanced> makeEnhancedTask(Func&& func,
                                                                  TaskCategory category = TaskCategory::Custom,
                                                                  uint32_t priority = 128) {
        auto task = TaskOperatorPoolManager::getInstance().acquireEnhancedTaskOperator(category, priority);
        // 这里需要设置回调，但当前池化对象不支持动态设置回调
        // 作为权衡，对于有自定义回调的任务，暂时不使用池化
        return std::make_shared<TaskOperatorEnhanced>(category, priority, std::forward<Func>(func));
    }
    
    // 创建池化的后端操作器
    inline std::shared_ptr<TaskBarrierOperator> makeBarrierOperator(const TaskOperatorPtr& task) {
        return TaskOperatorPoolManager::getInstance().acquireBarrierOperator(task);
    }
    
    inline std::shared_ptr<TaskDelayOperator> makeDelayOperator(
        std::chrono::milliseconds delay, const TaskOperatorPtr& task) {
        return TaskOperatorPoolManager::getInstance().acquireDelayOperator(delay, task);
    }
    
    inline std::shared_ptr<ConsumableOperator> makeConsumableOperator(
        const TaskOperatorPtr& task, const ConsumablePtr& consumable) {
        return TaskOperatorPoolManager::getInstance().acquireConsumableOperator(task, consumable);
    }
}

} // namespace task

#endif // __TASK_OPERATOR_POOL_H__