#include "TestFramework.h"
#include "../TaskDispatch.h"  
#include "../TimerManager.h"
#include "../ThreadPoolConfig.h"
#include <atomic>
#include <thread>
#include <chrono>
#include <vector>

using namespace task;
using namespace test;

// 测试基本的TaskOperator取消功能
TEST_CASE(BasicTaskOperatorCancellation) {
    std::atomic<bool> taskExecuted{false};
    
    auto task = std::make_shared<TaskOperator>(TaskMark::TM_TaskQueue_Inner,
        [&taskExecuted](const std::shared_ptr<TaskOperator>&) {
            taskExecuted.store(true);
        });
    
    // 取消任务
    task->cancel();
    TestFramework::ASSERT_TRUE(task->isCancelled(), "Task should be cancelled");
    
    // 执行取消的任务
    (*task)();
    
    // 任务不应该被执行
    TestFramework::ASSERT_FALSE(taskExecuted.load(), "Cancelled task should not execute");
}

// 测试TimerManager基本功能
TEST_CASE(TimerManagerBasicFunctionality) {
    std::atomic<bool> taskExecuted{false};
    auto startTime = std::chrono::steady_clock::now();
    
    auto task = std::make_shared<TaskOperator>(TaskMark::TM_TaskQueue_Inner,
        [&taskExecuted](const std::shared_ptr<TaskOperator>&) {
            taskExecuted.store(true);
        });
    
    // 调度100ms后执行的任务
    uint64_t timerId = TimerManager::getInstance().scheduleTask(task, std::chrono::milliseconds(100));
    TestFramework::ASSERT_TRUE(timerId > 0, "Timer ID should be valid");
    
    // 等待任务执行
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    auto endTime = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
    
    TestFramework::ASSERT_TRUE(taskExecuted.load(), "Timer task should execute");
    TestFramework::ASSERT_TRUE(duration.count() >= 100, "Task should execute after delay");
    TestFramework::ASSERT_TRUE(duration.count() < 300, "Task should not take too long");
}

// 测试TimerManager任务取消
TEST_CASE(TimerManagerTaskCancellation) {
    std::atomic<bool> taskExecuted{false};
    
    auto task = std::make_shared<TaskOperator>(TaskMark::TM_TaskQueue_Inner,
        [&taskExecuted](const std::shared_ptr<TaskOperator>&) {
            taskExecuted.store(true);
        });
    
    // 调度1秒后执行的任务
    uint64_t timerId = TimerManager::getInstance().scheduleTask(task, std::chrono::milliseconds(1000));
    TestFramework::ASSERT_TRUE(timerId > 0, "Timer ID should be valid");
    
    // 50ms后取消任务
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    bool cancelled = TimerManager::getInstance().cancelTask(timerId);
    TestFramework::ASSERT_TRUE(cancelled, "Task cancellation should succeed");
    
    // 等待原本的执行时间
    std::this_thread::sleep_for(std::chrono::milliseconds(1100));
    
    TestFramework::ASSERT_FALSE(taskExecuted.load(), "Cancelled task should not execute");
}

// 测试多个定时器的执行顺序
TEST_CASE(MultipleTimersExecutionOrder) {
    std::atomic<int> executionCounter{0};
    std::vector<int> executionOrder;
    std::mutex orderMutex;
    
    // 创建5个不同延时的任务
    for (int i = 1; i <= 5; ++i) {
        int delay = i * 20; // 20ms, 40ms, 60ms, 80ms, 100ms
        
        auto task = std::make_shared<TaskOperator>(TaskMark::TM_TaskQueue_Inner,
            [&executionOrder, &orderMutex, &executionCounter, i](const std::shared_ptr<TaskOperator>&) {
                std::lock_guard<std::mutex> lock(orderMutex);
                executionOrder.push_back(i);
                executionCounter.fetch_add(1);
            });
        
        uint64_t timerId = TimerManager::getInstance().scheduleTask(task, std::chrono::milliseconds(delay));
        TestFramework::ASSERT_TRUE(timerId > 0, "Timer ID should be valid");
    }
    
    // 等待所有任务执行完成
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    
    TestFramework::ASSERT_EQ(5, static_cast<int>(executionOrder.size()), "All tasks should execute");
    TestFramework::ASSERT_EQ(5, executionCounter.load(), "Counter should match");
    
    // 验证执行顺序
    for (size_t i = 0; i < executionOrder.size(); ++i) {
        TestFramework::ASSERT_EQ(static_cast<int>(i + 1), executionOrder[i], "Execution order should be correct");
    }
}

// 测试ThreadPoolConfig配置管理
TEST_CASE(ThreadPoolConfigManagement) {
    auto& configManager = ThreadPoolConfigManager::getInstance();
    
    // 获取默认配置
    ThreadPoolConfig defaultConfig = configManager.getConfig();
    TestFramework::ASSERT_TRUE(defaultConfig.maxThreads > 0, "Default max threads should be positive");
    TestFramework::ASSERT_TRUE(defaultConfig.enableTimerOptimization, "Timer optimization should be enabled by default");
    
    // 修改配置
    ThreadPoolConfig newConfig = defaultConfig;
    newConfig.maxThreads = 8;
    newConfig.enableTimerOptimization = false;
    newConfig.queueCapacity = 2000;
    
    configManager.setConfig(newConfig);
    
    // 验证配置更新
    ThreadPoolConfig updatedConfig = configManager.getConfig();
    TestFramework::ASSERT_EQ(8, updatedConfig.maxThreads, "Max threads should be updated");
    TestFramework::ASSERT_FALSE(updatedConfig.enableTimerOptimization, "Timer optimization should be disabled");
    TestFramework::ASSERT_EQ(static_cast<size_t>(2000), updatedConfig.queueCapacity, "Queue capacity should be updated");
    
    // 测试单项设置
    configManager.setMaxThreads(16);
    configManager.enableTimerOptimization(true);
    
    ThreadPoolConfig finalConfig = configManager.getConfig();
    TestFramework::ASSERT_EQ(16, finalConfig.maxThreads, "Max threads should be updated via setter");
    TestFramework::ASSERT_TRUE(finalConfig.enableTimerOptimization, "Timer optimization should be enabled via setter");
    
    // 恢复默认配置
    configManager.resetToDefault();
    ThreadPoolConfig resetConfig = configManager.getConfig();
    TestFramework::ASSERT_TRUE(resetConfig.enableTimerOptimization, "Timer optimization should be enabled after reset");
}

// 测试TimerManager配置
TEST_CASE(TimerManagerConfiguration) {
    TimerConfig defaultConfig = TimerManager::getInstance().getConfig();
    TestFramework::ASSERT_TRUE(defaultConfig.minDelay.count() > 0, "Min delay should be positive");
    TestFramework::ASSERT_TRUE(defaultConfig.maxDelay.count() > defaultConfig.minDelay.count(), "Max delay should be greater than min delay");
    TestFramework::ASSERT_TRUE(defaultConfig.maxPendingTasks > 0, "Max pending tasks should be positive");
    
    // 设置新配置
    TimerConfig newConfig;
    newConfig.minDelay = std::chrono::milliseconds(5);
    newConfig.maxDelay = std::chrono::seconds(10);
    newConfig.maxPendingTasks = 500;
    
    TimerManager::getInstance().setConfig(newConfig);
    
    // 验证配置更新
    TimerConfig updatedConfig = TimerManager::getInstance().getConfig();
    TestFramework::ASSERT_EQ(static_cast<long>(5), updatedConfig.minDelay.count(), "Min delay should be updated");
    TestFramework::ASSERT_EQ(static_cast<long>(10000), updatedConfig.maxDelay.count(), "Max delay should be updated");
    TestFramework::ASSERT_EQ(static_cast<size_t>(500), updatedConfig.maxPendingTasks, "Max pending tasks should be updated");
    
    // 恢复默认配置
    TimerManager::getInstance().setConfig(defaultConfig);
}

// 测试TaskOperator用户数据存储（向后兼容性）
TEST_CASE(TaskOperatorUserDataCompatibility) {
    auto task = std::make_shared<TaskOperator>(TaskMark::TM_TaskQueue_Inner);
    
    // 测试原有的void*用户数据接口
    int testValue = 42;
    auto userData = std::make_shared<int>(testValue);
    task->setUserData(userData);
    
    auto retrievedData = task->userData<int>();
    TestFramework::ASSERT_TRUE(retrievedData != nullptr, "User data should be retrievable");
    TestFramework::ASSERT_EQ(testValue, *retrievedData, "User data value should match");
}

// 并发安全测试
TEST_CASE(TimerManagerConcurrentAccess) {
    const int numThreads = 10;
    const int tasksPerThread = 20;
    std::atomic<int> executedTasks{0};
    std::vector<uint64_t> timerIds;
    std::mutex timerIdsMutex;
    
    TestFramework::CONCURRENT_TEST("TimerManagerConcurrentScheduling", [&]() {
        for (int i = 0; i < tasksPerThread; ++i) {
            auto task = std::make_shared<TaskOperator>(TaskMark::TM_TaskQueue_Inner,
                [&executedTasks](const std::shared_ptr<TaskOperator>&) {
                    executedTasks.fetch_add(1);
                });
            
            uint64_t timerId = TimerManager::getInstance().scheduleTask(task, std::chrono::milliseconds(10 + i));
            
            if (timerId > 0) {
                std::lock_guard<std::mutex> lock(timerIdsMutex);
                timerIds.push_back(timerId);
            }
        }
    }, numThreads);
    
    // 等待所有任务执行完成
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    
    TestFramework::ASSERT_TRUE(executedTasks.load() > 0, "Some tasks should have executed");
    TestFramework::ASSERT_TRUE(timerIds.size() > 0, "Some timer IDs should have been generated");
}

// 性能基准测试
TEST_CASE(TimerManagerPerformanceBenchmark) {
    TestFramework::BENCHMARK("TimerScheduling", []() {
        auto task = std::make_shared<TaskOperator>(TaskMark::TM_TaskQueue_Inner,
            [](const std::shared_ptr<TaskOperator>&) {
                // 空任务
            });
        
        TimerManager::getInstance().scheduleTask(task, std::chrono::milliseconds(1000));
    }, 100);
}

int main() {
    std::cout << "Starting Optimization Tests..." << std::endl;
    
    try {
        TestFramework::getInstance().runAllTests();
        
        const auto& stats = TestFramework::getInstance().getStatistics();
        return (stats.failedTests == 0) ? 0 : 1;
    } catch (const std::exception& e) {
        std::cerr << "Test suite failed with exception: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "Test suite failed with unknown exception" << std::endl;
        return 1;
    }
}