#include "ThreadPoolConfig.h"
#include "common/SysUtils.h"
#include <mutex>

namespace task
{

ThreadPoolConfigManager& ThreadPoolConfigManager::getInstance() {
    static ThreadPoolConfigManager instance;
    return instance;
}

ThreadPoolConfigManager::ThreadPoolConfigManager() {
    // 设置默认的最大线程数为CPU核数
    mConfig.maxThreads = SysUtils::cpuCount();
}

void ThreadPoolConfigManager::setConfig(const ThreadPoolConfig& config) {
    std::lock_guard<std::mutex> lock(mMutex);
    mConfig = config;
    
    // 如果maxThreads为0，设置为CPU核数
    if (mConfig.maxThreads == 0) {
        mConfig.maxThreads = SysUtils::cpuCount();
    }
}

ThreadPoolConfig ThreadPoolConfigManager::getConfig() const {
    std::lock_guard<std::mutex> lock(mMutex);
    return mConfig;
}

void ThreadPoolConfigManager::resetToDefault() {
    std::lock_guard<std::mutex> lock(mMutex);
    mConfig = ThreadPoolConfig();
    mConfig.maxThreads = SysUtils::cpuCount();
}

void ThreadPoolConfigManager::setMaxThreads(int32_t maxThreads) {
    std::lock_guard<std::mutex> lock(mMutex);
    mConfig.maxThreads = maxThreads > 0 ? maxThreads : SysUtils::cpuCount();
}

void ThreadPoolConfigManager::setQueueCapacity(size_t capacity) {
    std::lock_guard<std::mutex> lock(mMutex);
    mConfig.queueCapacity = capacity;
}

void ThreadPoolConfigManager::enableTimerOptimization(bool enable) {
    std::lock_guard<std::mutex> lock(mMutex);
    mConfig.enableTimerOptimization = enable;
}

void ThreadPoolConfigManager::enableOptimizedQueueImpl(bool enable) {
    std::lock_guard<std::mutex> lock(mMutex);  
    mConfig.enableOptimizedQueueImpl = enable;
}

bool ThreadPoolConfigManager::isTimerOptimizationEnabled() const {
    std::lock_guard<std::mutex> lock(mMutex);
    return mConfig.enableTimerOptimization;
}

bool ThreadPoolConfigManager::isOptimizedQueueImplEnabled() const {
    std::lock_guard<std::mutex> lock(mMutex);
    return mConfig.enableOptimizedQueueImpl;
}

int32_t ThreadPoolConfigManager::getMaxThreads() const {
    std::lock_guard<std::mutex> lock(mMutex);
    return mConfig.maxThreads;
}

size_t ThreadPoolConfigManager::getQueueCapacity() const {
    std::lock_guard<std::mutex> lock(mMutex);
    return mConfig.queueCapacity;
}

} // namespace task