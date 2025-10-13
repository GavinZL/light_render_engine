#include "TaskOperatorPool.h"
#include "log/Log.h"
#include "common/SysUtils.h"
#include <algorithm>

namespace task
{

TaskOperatorPoolManager& TaskOperatorPoolManager::getInstance() {
    static TaskOperatorPoolManager instance;
    return instance;
}

TaskOperatorPoolManager::TaskOperatorPoolManager() {
    // 配置对象池
    ObjectPoolConfig config;
    config.initialSize = 20;
    config.maxSize = 200;
    config.minSize = 10;
    config.maxIdleTime = std::chrono::seconds(300);
    config.enableAutoShrink = true;
    config.shrinkInterval = std::chrono::seconds(60);
    
    // 创建TaskOperator池
    mTaskOperatorPool = std::make_unique<ObjectPool<TaskOperator>>(
        []() { return std::make_shared<TaskOperator>(); },
        resetTaskOperator,
        config
    );
    
    // 创建TaskOperatorEnhanced池
    mEnhancedTaskOperatorPool = std::make_unique<ObjectPool<TaskOperatorEnhanced>>(
        []() { return std::make_shared<TaskOperatorEnhanced>(); },
        resetEnhancedTaskOperator,
        config
    );
    
    // 配置后端操作器池（较小的池大小）
    ObjectPoolConfig backendConfig = config;
    backendConfig.initialSize = 5;
    backendConfig.maxSize = 50;
    backendConfig.minSize = 2;
    
    // 创建后端操作器池
    mBarrierOperatorPool = std::make_unique<ObjectPool<TaskBarrierOperator>>(
        []() { return std::make_shared<TaskBarrierOperator>(nullptr); },
        resetBarrierOperator,
        backendConfig
    );
    
    mDelayOperatorPool = std::make_unique<ObjectPool<TaskDelayOperator>>(
        []() { return std::make_shared<TaskDelayOperator>(std::chrono::milliseconds(0), nullptr); },
        resetDelayOperator,
        backendConfig
    );
    
    mConsumableOperatorPool = std::make_unique<ObjectPool<ConsumableOperator>>(
        []() { return std::make_shared<ConsumableOperator>(nullptr, nullptr); },
        resetConsumableOperator,
        backendConfig
    );
    
    LOGE("[TASK] TaskOperatorPoolManager initialized with pooling enabled");
}

// === TaskOperator池实现 ===
std::shared_ptr<TaskOperator> TaskOperatorPoolManager::acquireTaskOperator(TaskMark mark) {
    if (!mPoolingEnabled.load() || isUnderMemoryPressure()) {
        return std::make_shared<TaskOperator>(mark);
    }
    
    auto task = mTaskOperatorPool->acquire();
    if (task) {
        // 这里需要重新设置TaskMark，但TaskOperator没有公共的设置接口
        // 作为权衡，我们创建一个新的TaskOperator
        return std::make_shared<TaskOperator>(mark);
    }
    
    return std::make_shared<TaskOperator>(mark);
}

std::shared_ptr<TaskOperator> TaskOperatorPoolManager::acquireTaskOperator(TaskMark mark, TaskOperator::CallBack callback) {
    // 有回调的TaskOperator较难池化，直接创建新实例
    return std::make_shared<TaskOperator>(mark, callback);
}

// === TaskOperatorEnhanced池实现 ===
std::shared_ptr<TaskOperatorEnhanced> TaskOperatorPoolManager::acquireEnhancedTaskOperator(
    TaskCategory category, uint32_t priority) {
    
    if (!mPoolingEnabled.load() || isUnderMemoryPressure()) {
        return std::make_shared<TaskOperatorEnhanced>(category, priority);
    }
    
    auto task = mEnhancedTaskOperatorPool->acquire();
    if (task) {
        // 重置属性
        task->setCategory(category);
        task->setPriority(priority);
        return task;
    }
    
    return std::make_shared<TaskOperatorEnhanced>(category, priority);
}

std::shared_ptr<TaskOperatorEnhanced> TaskOperatorPoolManager::acquireEnhancedTaskOperator(
    TaskOperatorEnhanced::SimpleCallback callback) {
    
    // 有回调的任务较难池化，直接创建新实例
    return std::make_shared<TaskOperatorEnhanced>(callback);
}

std::shared_ptr<TaskOperatorEnhanced> TaskOperatorPoolManager::acquireEnhancedTaskOperator(
    TaskCategory category, uint32_t priority, TaskOperatorEnhanced::SimpleCallback callback) {
    
    // 有回调的任务较难池化，直接创建新实例
    return std::make_shared<TaskOperatorEnhanced>(category, priority, callback);
}

// === 后端操作器池实现 ===
std::shared_ptr<TaskBarrierOperator> TaskOperatorPoolManager::acquireBarrierOperator(const TaskOperatorPtr& task) {
    if (!mPoolingEnabled.load() || isUnderMemoryPressure()) {
        return std::make_shared<TaskBarrierOperator>(task);
    }
    
    // 后端操作器池化较为复杂，暂时直接创建
    return std::make_shared<TaskBarrierOperator>(task);
}

std::shared_ptr<TaskDelayOperator> TaskOperatorPoolManager::acquireDelayOperator(
    std::chrono::milliseconds delay, const TaskOperatorPtr& task) {
    
    if (!mPoolingEnabled.load() || isUnderMemoryPressure()) {
        return std::make_shared<TaskDelayOperator>(delay, task);
    }
    
    // 后端操作器池化较为复杂，暂时直接创建
    return std::make_shared<TaskDelayOperator>(delay, task);
}

std::shared_ptr<ConsumableOperator> TaskOperatorPoolManager::acquireConsumableOperator(
    const TaskOperatorPtr& task, const ConsumablePtr& consumable) {
    
    if (!mPoolingEnabled.load() || isUnderMemoryPressure()) {
        return std::make_shared<ConsumableOperator>(task, consumable);
    }
    
    // 后端操作器池化较为复杂，暂时直接创建
    return std::make_shared<ConsumableOperator>(task, consumable);
}

// === 池统计和管理实现 ===
TaskOperatorPoolManager::PoolManagerStats TaskOperatorPoolManager::getStats() const {
    PoolManagerStats stats;
    
    if (mTaskOperatorPool) {
        stats.taskOperatorStats = mTaskOperatorPool->getStats();
    }
    
    if (mEnhancedTaskOperatorPool) {
        stats.enhancedTaskOperatorStats = mEnhancedTaskOperatorPool->getStats();
    }
    
    if (mBarrierOperatorPool) {
        stats.barrierOperatorStats = mBarrierOperatorPool->getStats();
    }
    
    if (mDelayOperatorPool) {
        stats.delayOperatorStats = mDelayOperatorPool->getStats();
    }
    
    if (mConsumableOperatorPool) {
        stats.consumableOperatorStats = mConsumableOperatorPool->getStats();
    }
    
    // 估算内存使用量（简化计算）
    stats.totalMemoryUsage = 
        (stats.taskOperatorStats.poolSize * sizeof(TaskOperator)) +
        (stats.enhancedTaskOperatorStats.poolSize * sizeof(TaskOperatorEnhanced)) +
        (stats.barrierOperatorStats.poolSize * sizeof(TaskBarrierOperator)) +
        (stats.delayOperatorStats.poolSize * sizeof(TaskDelayOperator)) +
        (stats.consumableOperatorStats.poolSize * sizeof(ConsumableOperator));
    
    return stats;
}

void TaskOperatorPoolManager::configurePool(const ObjectPoolConfig& config) {
    std::lock_guard<std::mutex> lock(mConfigMutex);
    
    if (mTaskOperatorPool) {
        mTaskOperatorPool->setConfig(config);
    }
    
    if (mEnhancedTaskOperatorPool) {
        mEnhancedTaskOperatorPool->setConfig(config);
    }
    
    // 后端操作器使用缩小的配置
    ObjectPoolConfig backendConfig = config;
    backendConfig.initialSize = std::min(config.initialSize / 4, static_cast<size_t>(5));
    backendConfig.maxSize = std::min(config.maxSize / 4, static_cast<size_t>(50));
    backendConfig.minSize = std::min(config.minSize / 2, static_cast<size_t>(2));
    
    if (mBarrierOperatorPool) {
        mBarrierOperatorPool->setConfig(backendConfig);
    }
    
    if (mDelayOperatorPool) {
        mDelayOperatorPool->setConfig(backendConfig);
    }
    
    if (mConsumableOperatorPool) {
        mConsumableOperatorPool->setConfig(backendConfig);
    }
    
    LOGE("[TASK] TaskOperatorPoolManager configuration updated");
}

void TaskOperatorPoolManager::shrinkAllPools() {
    if (mTaskOperatorPool) mTaskOperatorPool->shrink();
    if (mEnhancedTaskOperatorPool) mEnhancedTaskOperatorPool->shrink();
    if (mBarrierOperatorPool) mBarrierOperatorPool->shrink();
    if (mDelayOperatorPool) mDelayOperatorPool->shrink();
    if (mConsumableOperatorPool) mConsumableOperatorPool->shrink();
    
    LOGE("[TASK] All object pools shrunk");
}

void TaskOperatorPoolManager::clearAllPools() {
    if (mTaskOperatorPool) mTaskOperatorPool->clear();
    if (mEnhancedTaskOperatorPool) mEnhancedTaskOperatorPool->clear();
    if (mBarrierOperatorPool) mBarrierOperatorPool->clear();
    if (mDelayOperatorPool) mDelayOperatorPool->clear();
    if (mConsumableOperatorPool) mConsumableOperatorPool->clear();
    
    LOGE("[TASK] All object pools cleared");
}

// === 对象重置器实现 ===
void TaskOperatorPoolManager::resetTaskOperator(std::shared_ptr<TaskOperator>& task) {
    if (task) {
        // 重置TaskOperator状态
        task->setUserData(nullptr);
        // 注意：无法重置CallBack，因为没有公共接口
        task->resetCallStartTime();
    }
}

void TaskOperatorPoolManager::resetEnhancedTaskOperator(std::shared_ptr<TaskOperatorEnhanced>& task) {
    if (task) {
        // 重置TaskOperatorEnhanced状态
        task->clearUserData();
        task->setCategory(TaskCategory::Custom);
        task->setPriority(128);
        task->setTaskMark(TaskMark::TM_TaskQueue_Inner);
        task->setNextTask(nullptr);
        task->resetCallStartTime();
        // 注意：无法重置SimpleCallback，因为它是私有成员
    }
}

void TaskOperatorPoolManager::resetBarrierOperator(std::shared_ptr<TaskBarrierOperator>& task) {
    // TaskBarrierOperator的重置较为复杂，因为它包含LWBarrier状态
    // 暂时不进行重置，依赖构造函数参数
}

void TaskOperatorPoolManager::resetDelayOperator(std::shared_ptr<TaskDelayOperator>& task) {
    // TaskDelayOperator的重置较为复杂，因为延时和真实任务都是构造参数
    // 暂时不进行重置，依赖构造函数参数
}

void TaskOperatorPoolManager::resetConsumableOperator(std::shared_ptr<ConsumableOperator>& task) {
    // ConsumableOperator的重置较为复杂，因为涉及Consumable状态
    // 暂时不进行重置，依赖构造函数参数
}

// === 内存压力管理实现 ===
bool TaskOperatorPoolManager::isUnderMemoryPressure() const {
    auto stats = getStats();
    return stats.totalMemoryUsage > mMemoryThreshold.load();
}

void TaskOperatorPoolManager::handleMemoryPressure() {
    if (isUnderMemoryPressure()) {
        LOGE("[TASK] Memory pressure detected, shrinking pools");
        shrinkAllPools();
    }
}

} // namespace task