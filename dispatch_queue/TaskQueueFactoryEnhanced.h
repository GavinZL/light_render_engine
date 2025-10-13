// 增强的任务队列工厂
#ifndef __TASK_QUEUE_FACTORY_ENHANCED_H__
#define __TASK_QUEUE_FACTORY_ENHANCED_H__

#include "TaskQueueEnhanced.h"
#include "TaskQueueFactory.h"
#include "ThreadPoolConfig.h"
#include <memory>
#include <unordered_map>
#include <mutex>

namespace task
{

// 增强的任务队列工厂类
class TaskQueueFactoryEnhanced {
public:
    static TaskQueueFactoryEnhanced& getInstance();
    
    // === 增强的队列创建接口 ===
    
    // 创建串行队列
    TaskQueueEnhancedPtr createSerialQueue(const std::string& label, 
                                          WorkThreadPriority priority = WorkThreadPriority::WTP_Normal,
                                          bool isExclusive = false);
    
    // 创建并发队列
    TaskQueueEnhancedPtr createConcurrencyQueue(const std::string& label,
                                               TaskQueuePriority priority = TaskQueuePriority::TQP_Normal);
    
    // 获取全局并发队列
    TaskQueueEnhancedPtr globalConcurrencyQueue(TaskQueuePriority priority = TaskQueuePriority::TQP_Normal);
    
    // === 队列管理 ===
    
    // 获取已创建的队列
    TaskQueueEnhancedPtr getQueue(const std::string& label);
    
    // 销毁队列
    bool destroyQueue(const std::string& label);
    
    // 获取所有队列标签
    std::vector<std::string> getAllQueueLabels() const;
    
    // 获取队列统计汇总
    std::vector<std::pair<std::string, QueueStats>> getAllQueueStats() const;
    
    // === 兼容性接口 ===
    
    // 从传统TaskQueue创建增强版本
    TaskQueueEnhancedPtr wrapLegacyQueue(const TaskQueuePtr& legacyQueue);
    
    // 创建兼容传统接口的队列
    TaskQueuePtr createLegacySerialTaskQueue(const std::string& label, 
                                            WorkThreadPriority priority, 
                                            bool isExclusive);
    TaskQueuePtr createLegacyConcurrencyTaskQueue(const std::string& label,
                                                 TaskQueuePriority priority);

private:
    TaskQueueFactoryEnhanced();
    ~TaskQueueFactoryEnhanced() = default;
    
    // 禁用拷贝和赋值
    TaskQueueFactoryEnhanced(const TaskQueueFactoryEnhanced&) = delete;
    TaskQueueFactoryEnhanced& operator=(const TaskQueueFactoryEnhanced&) = delete;
    
    // 内部辅助方法
    TaskQueueImplPtr createQueueImpl(const std::string& label, 
                                    TaskQueueType type,
                                    TaskQueuePriority priority = TaskQueuePriority::TQP_Normal,
                                    WorkThreadPriority workPriority = WorkThreadPriority::WTP_Normal,
                                    bool isExclusive = false);

private:
    mutable std::mutex mMutex;
    std::unordered_map<std::string, TaskQueueEnhancedPtr> mQueues;
    std::unordered_map<TaskQueuePriority, TaskQueueEnhancedPtr> mGlobalQueues;
};

// 全局便利函数
namespace enhanced {
    // 快速创建任务
    template<typename Func>
    inline TaskOperatorEnhancedPtr task(Func&& func) {
        return makeTask(std::forward<Func>(func));
    }
    
    template<typename Func>
    inline TaskOperatorEnhancedPtr task(TaskCategory category, Func&& func) {
        return makeTask(std::forward<Func>(func), category);
    }
    
    template<typename Func>
    inline TaskOperatorEnhancedPtr task(TaskCategory category, uint32_t priority, Func&& func) {
        return makeTask(std::forward<Func>(func), category, priority);
    }
    
    // 快速获取全局队列
    inline TaskQueueEnhancedPtr globalQueue(TaskQueuePriority priority = TaskQueuePriority::TQP_Normal) {
        return TaskQueueFactoryEnhanced::getInstance().globalConcurrencyQueue(priority);
    }
    
    // 快速执行任务
    template<typename Func>
    inline void async(Func&& func, TaskQueuePriority priority = TaskQueuePriority::TQP_Normal) {
        globalQueue(priority)->async(std::forward<Func>(func));
    }
    
    template<typename Func>
    inline void sync(Func&& func, TaskQueuePriority priority = TaskQueuePriority::TQP_Normal,
                    std::chrono::milliseconds timeout = std::chrono::milliseconds(-1)) {
        globalQueue(priority)->sync(std::forward<Func>(func), timeout);
    }
    
    template<typename Func>
    inline uint64_t after(std::chrono::milliseconds delay, Func&& func, 
                         TaskQueuePriority priority = TaskQueuePriority::TQP_Normal) {
        return globalQueue(priority)->after(delay, std::forward<Func>(func));
    }
}

} // namespace task

#endif // __TASK_QUEUE_FACTORY_ENHANCED_H__