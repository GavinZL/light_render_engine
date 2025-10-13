// 线程池配置管理
#ifndef __THREAD_POOL_CONFIG_H__
#define __THREAD_POOL_CONFIG_H__

#include <chrono>
#include <atomic>
#include <memory>

namespace task
{

// 线程池配置结构
struct ThreadPoolConfig {
    int32_t minThreads = 2;
    int32_t maxThreads = 0; // 0 表示使用CPU核数
    std::chrono::seconds threadIdleTimeout = std::chrono::seconds(60);
    bool enableWorkStealing = true;
    size_t queueCapacity = 1000;
    bool enableTimerOptimization = true;  // 启用定时器优化
    bool enableOptimizedQueueImpl = true; // 启用优化的队列实现
};

// 线程池配置管理器 - 单例模式
class ThreadPoolConfigManager {
public:
    static ThreadPoolConfigManager& getInstance();
    
    // 配置管理
    void setConfig(const ThreadPoolConfig& config);
    ThreadPoolConfig getConfig() const;
    void resetToDefault();
    
    // 运行时调整
    void setMaxThreads(int32_t maxThreads);
    void setQueueCapacity(size_t capacity);
    void enableTimerOptimization(bool enable);
    void enableOptimizedQueueImpl(bool enable);
    
    // 获取特定配置项
    bool isTimerOptimizationEnabled() const;
    bool isOptimizedQueueImplEnabled() const;
    int32_t getMaxThreads() const;
    size_t getQueueCapacity() const;

private:
    ThreadPoolConfigManager();
    ~ThreadPoolConfigManager() = default;
    
    // 禁用拷贝和赋值
    ThreadPoolConfigManager(const ThreadPoolConfigManager&) = delete;
    ThreadPoolConfigManager& operator=(const ThreadPoolConfigManager&) = delete;
    
private:
    mutable std::mutex mMutex;
    ThreadPoolConfig mConfig;
};

} // namespace task

#endif // __THREAD_POOL_CONFIG_H__