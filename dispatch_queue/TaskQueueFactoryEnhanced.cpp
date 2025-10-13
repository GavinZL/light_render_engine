#include "TaskQueueFactoryEnhanced.h"
#include "backend/TaskQueueFactoryImpl.h"
#include "backend/QueueImplOptimized.h"
#include "log/Log.h"

namespace task
{

TaskQueueFactoryEnhanced& TaskQueueFactoryEnhanced::getInstance() {
    static TaskQueueFactoryEnhanced instance;
    return instance;
}

TaskQueueFactoryEnhanced::TaskQueueFactoryEnhanced() {
    LOGE("[TASK] TaskQueueFactoryEnhanced initialized");
}

TaskQueueEnhancedPtr TaskQueueFactoryEnhanced::createSerialQueue(const std::string& label, 
                                                                WorkThreadPriority priority,
                                                                bool isExclusive) {
    std::lock_guard<std::mutex> lock(mMutex);
    
    // 检查是否已存在
    auto it = mQueues.find(label);
    if (it != mQueues.end()) {
        LOGE("[TASK] Serial queue '%s' already exists", label.c_str());
        return it->second;
    }
    
    // 创建队列实现
    auto impl = createQueueImpl(label, TaskQueueType::TQT_Serial, 
                               TaskQueuePriority::TQP_Normal, priority, isExclusive);
    
    if (!impl) {
        LOGE("[TASK] Failed to create serial queue impl for '%s'", label.c_str());
        return nullptr;
    }
    
    // 创建增强队列
    auto queue = std::make_shared<TaskQueueEnhanced>(label, impl);
    mQueues[label] = queue;
    
    LOGE("[TASK] Created enhanced serial queue '%s'", label.c_str());
    return queue;
}

TaskQueueEnhancedPtr TaskQueueFactoryEnhanced::createConcurrencyQueue(const std::string& label,
                                                                     TaskQueuePriority priority) {
    std::lock_guard<std::mutex> lock(mMutex);
    
    // 检查是否已存在
    auto it = mQueues.find(label);
    if (it != mQueues.end()) {
        LOGE("[TASK] Concurrency queue '%s' already exists", label.c_str());
        return it->second;
    }
    
    // 创建队列实现
    auto impl = createQueueImpl(label, TaskQueueType::TQT_Parallel, priority);
    
    if (!impl) {
        LOGE("[TASK] Failed to create concurrency queue impl for '%s'", label.c_str());
        return nullptr;
    }
    
    // 创建增强队列
    auto queue = std::make_shared<TaskQueueEnhanced>(label, impl);
    mQueues[label] = queue;
    
    LOGE("[TASK] Created enhanced concurrency queue '%s'", label.c_str());
    return queue;
}

TaskQueueEnhancedPtr TaskQueueFactoryEnhanced::globalConcurrencyQueue(TaskQueuePriority priority) {
    std::lock_guard<std::mutex> lock(mMutex);
    
    auto it = mGlobalQueues.find(priority);
    if (it != mGlobalQueues.end()) {
        return it->second;
    }
    
    // 创建全局队列
    std::string label = "global_concurrency_" + std::to_string(static_cast<int>(priority));
    auto impl = createQueueImpl(label, TaskQueueType::TQT_Parallel, priority);
    
    if (!impl) {
        LOGE("[TASK] Failed to create global concurrency queue impl for priority %d", 
             static_cast<int>(priority));
        return nullptr;
    }
    
    auto queue = std::make_shared<TaskQueueEnhanced>(label, impl);
    mGlobalQueues[priority] = queue;
    
    LOGE("[TASK] Created global enhanced concurrency queue for priority %d", 
         static_cast<int>(priority));
    return queue;
}

TaskQueueEnhancedPtr TaskQueueFactoryEnhanced::getQueue(const std::string& label) {
    std::lock_guard<std::mutex> lock(mMutex);
    
    auto it = mQueues.find(label);
    return (it != mQueues.end()) ? it->second : nullptr;
}

bool TaskQueueFactoryEnhanced::destroyQueue(const std::string& label) {
    std::lock_guard<std::mutex> lock(mMutex);
    
    auto it = mQueues.find(label);
    if (it != mQueues.end()) {
        LOGE("[TASK] Destroying enhanced queue '%s'", label.c_str());
        mQueues.erase(it);
        return true;
    }
    
    return false;
}

std::vector<std::string> TaskQueueFactoryEnhanced::getAllQueueLabels() const {
    std::lock_guard<std::mutex> lock(mMutex);
    
    std::vector<std::string> labels;
    labels.reserve(mQueues.size());
    
    for (const auto& pair : mQueues) {
        labels.push_back(pair.first);
    }
    
    return labels;
}

std::vector<std::pair<std::string, QueueStats>> TaskQueueFactoryEnhanced::getAllQueueStats() const {
    std::lock_guard<std::mutex> lock(mMutex);
    
    std::vector<std::pair<std::string, QueueStats>> stats;
    stats.reserve(mQueues.size());
    
    for (const auto& pair : mQueues) {
        stats.emplace_back(pair.first, pair.second->getStats());
    }
    
    return stats;
}

TaskQueueEnhancedPtr TaskQueueFactoryEnhanced::wrapLegacyQueue(const TaskQueuePtr& legacyQueue) {
    if (!legacyQueue) {
        return nullptr;
    }
    
    // 这里需要实现从传统TaskQueue到增强版本的包装
    // 暂时返回nullptr，需要更复杂的实现来访问内部的IQueueImpl
    LOGE("[TASK] Legacy queue wrapping not yet implemented");
    return nullptr;
}

TaskQueuePtr TaskQueueFactoryEnhanced::createLegacySerialTaskQueue(const std::string& label, 
                                                                  WorkThreadPriority priority, 
                                                                  bool isExclusive) {
    // 使用传统工厂创建传统队列
    return TaskQueueFactory::getInstance().createSerialTaskQueue(label, priority, isExclusive);
}

TaskQueuePtr TaskQueueFactoryEnhanced::createLegacyConcurrencyTaskQueue(const std::string& label,
                                                                       TaskQueuePriority priority) {
    // 使用传统工厂创建传统队列  
    return TaskQueueFactory::getInstance().createConcurrencyTaskQueue(label, priority);
}

TaskQueueImplPtr TaskQueueFactoryEnhanced::createQueueImpl(const std::string& label, 
                                                          TaskQueueType type,
                                                          TaskQueuePriority priority,
                                                          WorkThreadPriority workPriority,
                                                          bool isExclusive) {
    
    // 获取传统工厂实现来创建底层实现
    auto& factoryImpl = TaskQueueFactoryImpl::getInstance();
    
    switch (type) {
        case TaskQueueType::TQT_Serial: {
            auto serialImpl = factoryImpl.createSerialQueueImpl(label, isExclusive, workPriority);
            
            // 如果启用了优化实现，包装为优化版本
            if (ThreadPoolConfigManager::getInstance().isOptimizedQueueImplEnabled()) {
                return std::make_shared<SerialQueueImplOptimized>(
                    label, isExclusive, serialImpl->_threadPool(), workPriority);
            }
            
            return serialImpl;
        }
        
        case TaskQueueType::TQT_Parallel: {
            auto concurrencyImpl = factoryImpl.createConcurrencyQueueImpl(priority);
            
            // 如果启用了优化实现，包装为优化版本
            if (ThreadPoolConfigManager::getInstance().isOptimizedQueueImplEnabled()) {
                return std::make_shared<ConcurrencyQueueImplOptimized>(
                    priority, concurrencyImpl->_threadPool());
            }
            
            return concurrencyImpl;
        }
        
        default:
            LOGE("[TASK] Unknown queue type: %d", static_cast<int>(type));
            return nullptr;
    }
}

} // namespace task