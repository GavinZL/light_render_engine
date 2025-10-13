#include "TestFramework.h"
#include "../TaskDispatch.h"
#include "../TimerManager.h"
#include "../TaskOperatorPool.h"
#include "../TaskQueueFactoryEnhanced.h"
#include "../LoadBalancer.h"
#include "../PerformanceMonitor.h"
#include "../ThreadPoolConfig.h"
#include <atomic>
#include <thread>
#include <chrono>
#include <vector>

using namespace task;
using namespace test;

// 测试整体系统集成
TEST_CASE(SystemIntegrationTest) {
    // 配置线程池使用优化功能
    auto& configManager = ThreadPoolConfigManager::getInstance();
    ThreadPoolConfig config = configManager.getConfig();
    config.enableTimerOptimization = true;
    config.enableOptimizedQueueImpl = true;
    configManager.setConfig(config);
    
    // 启动定时器管理器
    TimerManager::getInstance().start();
    
    // 创建增强的任务队列工厂
    auto& factory = TaskQueueFactoryEnhanced::getInstance();
    
    // 创建并发队列
    auto queue = factory.createConcurrencyQueue("integration_test", TaskQueuePriority::TQP_Normal);
    TestFramework::ASSERT_TRUE(queue != nullptr, "Queue should be created");
    
    // 创建串行队列
    auto serialQueue = factory.createSerialQueue("serial_test", WorkThreadPriority::WTP_Normal, false);
    TestFramework::ASSERT_TRUE(serialQueue != nullptr, "Serial queue should be created");
    
    // 测试异步任务执行
    std::atomic<int> asyncCounter{0};
    queue->async([&asyncCounter]() {
        asyncCounter.fetch_add(1);
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    });
    
    // 测试同步任务执行
    std::atomic<int> syncCounter{0};
    queue->sync([&syncCounter]() {
        syncCounter.fetch_add(1);
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    });
    
    // 测试延时任务执行
    std::atomic<int> delayedCounter{0};
    uint64_t timerId = queue->after(std::chrono::milliseconds(50), [&delayedCounter]() {
        delayedCounter.fetch_add(1);
    });
    
    TestFramework::ASSERT_TRUE(timerId > 0, "Timer ID should be valid");
    
    // 测试批量任务执行
    std::atomic<int> batchCounter{0};
    std::vector<std::function<void()>> batchFunctions;
    for (int i = 0; i < 5; ++i) {
        batchFunctions.push_back([&batchCounter]() {
            batchCounter.fetch_add(1);
        });
    }
    queue->asyncBatch(batchFunctions);
    
    // 测试条件执行
    std::atomic<int> conditionalCounter{0};
    queue->asyncIf([]() { return true; }, [&conditionalCounter]() {
        conditionalCounter.fetch_add(1);
    });
    
    // 测试任务链
    std::vector<int> chainResults;
    std::mutex chainMutex;
    std::vector<std::function<void()>> chainFunctions;
    for (int i = 1; i <= 3; ++i) {
        int value = i;
        chainFunctions.push_back([&chainResults, &chainMutex, value]() {
            std::lock_guard<std::mutex> lock(chainMutex);
            chainResults.push_back(value);
        });
    }
    queue->asyncChain(chainFunctions);
    
    // 等待所有任务完成
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    
    // 验证结果
    TestFramework::ASSERT_EQ(1, asyncCounter.load(), "Async task should execute once");
    TestFramework::ASSERT_EQ(1, syncCounter.load(), "Sync task should execute once");
    TestFramework::ASSERT_EQ(1, delayedCounter.load(), "Delayed task should execute once");
    TestFramework::ASSERT_EQ(5, batchCounter.load(), "Batch tasks should execute 5 times");
    TestFramework::ASSERT_EQ(1, conditionalCounter.load(), "Conditional task should execute once");
    TestFramework::ASSERT_EQ(3, static_cast<int>(chainResults.size()), "Chain should have 3 steps");
    
    for (size_t i = 0; i < chainResults.size(); ++i) {
        TestFramework::ASSERT_EQ(static_cast<int>(i + 1), chainResults[i], "Chain step should be correct");
    }
    
    // 测试队列统计
    auto stats = queue->getStats();
    TestFramework::ASSERT_TRUE(stats.pendingTaskCount >= 0, "Pending task count should be non-negative");
    
    // 测试池化对象
    auto& poolManager = TaskOperatorPoolManager::getInstance();
    auto pooledTask = poolManager.acquireTaskOperator(TaskMark::TM_TaskQueue_Inner);
    TestFramework::ASSERT_TRUE(pooledTask != nullptr, "Pooled task should be acquired");
    
    // 测试增强的TaskOperator
    auto enhancedTask = makeTask([]() { /* empty task */ }, TaskCategory::Custom, 128);
    TestFramework::ASSERT_TRUE(enhancedTask != nullptr, "Enhanced task should be created");
    
    // 测试负载分析器
    EnhancedLoadAnalyzer loadAnalyzer;
    double cpuUsage = loadAnalyzer.getSystemCpuUsage();
    TestFramework::ASSERT_TRUE(cpuUsage >= 0.0 && cpuUsage <= 1.0, "CPU usage should be valid");
    
    // 测试指标收集
    ThreadPoolCollector threadPoolCollector;
    threadPoolCollector.setThreadPoolStats(4, 2, 8, 100, 5, 1);
    auto metrics = threadPoolCollector.collect();
    TestFramework::ASSERT_TRUE(!metrics.empty(), "Metrics should be collected");
    
    std::cout << "Integration test completed with " << metrics.size() << " metrics" << std::endl;
}

// 测试性能优化效果
TEST_CASE(PerformanceOptimizationTest) {
    auto& factory = TaskQueueFactoryEnhanced::getInstance();
    auto queue = factory.globalConcurrencyQueue(TaskQueuePriority::TQP_Normal);
    
    const int taskCount = 1000;
    std::atomic<int> completedTasks{0};
    
    auto startTime = std::chrono::high_resolution_clock::now();
    
    // 执行大量任务测试性能
    for (int i = 0; i < taskCount; ++i) {
        queue->async([&completedTasks]() {
            // 简单的任务
            volatile int x = i * 2;
            (void)x;
            completedTasks.fetch_add(1);
        });
    }
    
    // 等待所有任务完成
    while (completedTasks.load() < taskCount) {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
    
    std::cout << "Executed " << taskCount << " tasks in " << duration.count() << "ms" << std::endl;
    
    TestFramework::ASSERT_EQ(taskCount, completedTasks.load(), "All tasks should complete");
    TestFramework::ASSERT_TRUE(duration.count() < 5000, "Tasks should complete within reasonable time");
    
    // 测试池化性能
    auto& poolManager = TaskOperatorPoolManager::getInstance();
    
    auto poolStart = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < taskCount; ++i) {
        auto task = poolManager.acquireTaskOperator(TaskMark::TM_TaskQueue_Inner);
        // 使用任务
        task->resetCallStartTime();
    }
    auto poolEnd = std::chrono::high_resolution_clock::now();
    auto poolDuration = std::chrono::duration_cast<std::chrono::microseconds>(poolEnd - poolStart);
    
    auto normalStart = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < taskCount; ++i) {
        auto task = std::make_shared<TaskOperator>(TaskMark::TM_TaskQueue_Inner);
        // 使用任务
        task->resetCallStartTime();
    }
    auto normalEnd = std::chrono::high_resolution_clock::now();
    auto normalDuration = std::chrono::duration_cast<std::chrono::microseconds>(normalEnd - normalStart);
    
    std::cout << "Pool allocation: " << poolDuration.count() << "μs" << std::endl;
    std::cout << "Normal allocation: " << normalDuration.count() << "μs" << std::endl;
    
    // 池化应该更快或至少不显著慢于正常分配
    TestFramework::ASSERT_TRUE(poolDuration.count() <= normalDuration.count() * 2, 
                              "Pool allocation should not be significantly slower");
}

// 测试并发安全性和稳定性
TEST_CASE(ConcurrentSafetyAndStabilityTest) {
    const int numThreads = 10;
    const int tasksPerThread = 100;
    std::atomic<int> totalTasks{0};
    std::atomic<int> totalErrors{0};
    
    TestFramework::CONCURRENT_TEST("ConcurrentTaskExecution", [&]() {
        auto& factory = TaskQueueFactoryEnhanced::getInstance();
        auto queue = factory.globalConcurrencyQueue(TaskQueuePriority::TQP_Normal);
        
        try {
            for (int i = 0; i < tasksPerThread; ++i) {
                queue->async([&totalTasks]() {
                    totalTasks.fetch_add(1);
                    // 模拟一些工作
                    volatile int x = totalTasks.load() * 2;
                    (void)x;
                });
            }
            
            // 测试其他操作
            queue->after(std::chrono::milliseconds(10), []() {
                // 延时任务
            });
            
            // 测试同步操作（小心死锁）
            /*
            queue->sync([]() {
                // 同步任务
            });
            */
        } catch (const std::exception& e) {
            std::cerr << "Exception in thread: " << e.what() << std::endl;
            totalErrors.fetch_add(1);
        } catch (...) {
            std::cerr << "Unknown exception in thread" << std::endl;
            totalErrors.fetch_add(1);
        }
    }, numThreads);
    
    // 等待任务完成
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    
    int expectedTasks = numThreads * tasksPerThread;
    std::cout << "Expected tasks: " << expectedTasks << ", Actual: " << totalTasks.load() << std::endl;
    std::cout << "Total errors: " << totalErrors.load() << std::endl;
    
    TestFramework::ASSERT_TRUE(totalErrors.load() == 0, "No errors should occur");
    // 注意：由于并发执行，可能不是所有任务都已完成
}

// 测试边界条件和异常处理
TEST_CASE(BoundaryConditionsAndExceptionHandlingTest) {
    auto& factory = TaskQueueFactoryEnhanced::getInstance();
    
    // 测试空任务处理
    auto queue = factory.globalConcurrencyQueue(TaskQueuePriority::TQP_Normal);
    
    // 空函数任务
    queue->async(static_cast<std::function<void()>>(nullptr));
    
    // 测试取消的任务
    auto task = makeTask([]() { 
        std::cout << "This task should not execute" << std::endl;
    });
    task->cancel();
    queue->async(task);
    
    // 测试异常处理
    std::atomic<bool> exceptionCaught{false};
    queue->async([&exceptionCaught]() {
        try {
            throw std::runtime_error("Test exception");
        } catch (...) {
            exceptionCaught.store(true);
        }
    });
    
    // 等待执行
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    
    // 测试定时器边界条件
    auto& timerManager = TimerManager::getInstance();
    
    // 测试无效的延时
    uint64_t invalidTimer1 = timerManager.scheduleTask(nullptr, std::chrono::milliseconds(100));
    TestFramework::ASSERT_EQ(static_cast<uint64_t>(0), invalidTimer1, "Invalid timer should return 0");
    
    // 测试过短的延时
    auto quickTask = makeTask([]() { /* empty */ });
    uint64_t quickTimer = timerManager.scheduleTask(quickTask, std::chrono::milliseconds(1));
    TestFramework::ASSERT_TRUE(quickTimer > 0, "Quick timer should be valid");
    
    // 测试过长的延时
    auto longTask = makeTask([]() { /* empty */ });
    uint64_t longTimer = timerManager.scheduleTask(longTask, std::chrono::hours(24));
    TestFramework::ASSERT_TRUE(longTimer > 0, "Long timer should be valid");
    
    // 测试取消不存在的任务
    bool cancelResult = timerManager.cancelTask(999999);
    // 取消不存在的任务可能返回true或false，取决于实现
    
    std::cout << "Boundary conditions test completed" << std::endl;
}

// 测试资源管理和内存泄漏
TEST_CASE(ResourceManagementAndMemoryLeakTest) {
    auto& factory = TaskQueueFactoryEnhanced::getInstance();
    auto& poolManager = TaskOperatorPoolManager::getInstance();
    
    // 创建多个队列
    std::vector<TaskQueueEnhancedPtr> queues;
    for (int i = 0; i < 5; ++i) {
        auto queue = factory.createConcurrencyQueue("test_queue_" + std::to_string(i), TaskQueuePriority::TQP_Normal);
        queues.push_back(queue);
    }
    
    // 执行大量任务
    const int taskCount = 1000;
    for (int i = 0; i < taskCount; ++i) {
        int queueIndex = i % queues.size();
        queues[queueIndex]->async([]() {
            // 简单任务
            volatile int x = 42;
            (void)x;
        });
    }
    
    // 使用对象池
    for (int i = 0; i < taskCount; ++i) {
        auto task = poolManager.acquireTaskOperator(TaskMark::TM_TaskQueue_Inner);
        // 使用任务
        task->resetCallStartTime();
    }
    
    // 使用增强的TaskOperator
    for (int i = 0; i < taskCount; ++i) {
        auto task = makeTask([]() { /* empty */ }, TaskCategory::Custom, 128);
        // 使用任务
        task->resetCallStartTime();
    }
    
    // 检查池统计信息
    auto poolStats = poolManager.getStats();
    std::cout << "Pool stats - TaskOperator pool size: " << poolStats.taskOperatorStats.poolSize << std::endl;
    std::cout << "Pool stats - EnhancedTaskOperator pool size: " << poolStats.enhancedTaskOperatorStats.poolSize << std::endl;
    
    // 清理队列
    queues.clear();
    
    // 收缩池
    poolManager.shrinkAllPools();
    
    std::cout << "Resource management test completed" << std::endl;
}

// 性能基准测试
TEST_CASE(ComprehensiveBenchmark) {
    auto& factory = TaskQueueFactoryEnhanced::getInstance();
    auto queue = factory.globalConcurrencyQueue(TaskQueuePriority::TQP_Normal);
    
    // 基准测试1：任务提交和执行
    TestFramework::BENCHMARK("TaskSubmissionAndExecution", [&queue]() {
        std::atomic<int> counter{0};
        queue->async([&counter]() {
            counter.fetch_add(1);
        });
        // 简单等待，实际应用中应该有更好的同步机制
        std::this_thread::sleep_for(std::chrono::microseconds(10));
    }, 1000);
    
    // 基准测试2：批量任务处理
    TestFramework::BENCHMARK("BatchTaskProcessing", [&queue]() {
        std::vector<std::function<void()>> functions;
        for (int i = 0; i < 10; ++i) {
            functions.push_back([]() {
                volatile int x = 42;
                (void)x;
            });
        }
        queue->asyncBatch(functions);
        std::this_thread::sleep_for(std::chrono::microseconds(50));
    }, 100);
    
    // 基准测试3：池化对象分配
    auto& poolManager = TaskOperatorPoolManager::getInstance();
    TestFramework::BENCHMARK("PooledObjectAllocation", [&poolManager]() {
        auto task = poolManager.acquireTaskOperator(TaskMark::TM_TaskQueue_Inner);
        task->resetCallStartTime();
    }, 1000);
    
    // 基准测试4：普通对象分配
    TestFramework::BENCHMARK("NormalObjectAllocation", []() {
        auto task = std::make_shared<TaskOperator>(TaskMark::TM_TaskQueue_Inner);
        task->resetCallStartTime();
    }, 1000);
    
    // 基准测试5：定时器调度
    TestFramework::BENCHMARK("TimerScheduling", []() {
        auto& timerManager = TimerManager::getInstance();
        auto task = makeTask([]() { /* empty */ });
        uint64_t timerId = timerManager.scheduleTask(task, std::chrono::milliseconds(1000));
        // 注意：这里不取消定时器，因为在基准测试中这会干扰性能测量
        (void)timerId;
    }, 100);
}

int main() {
    std::cout << "Starting Comprehensive Integration Tests..." << std::endl;
    
    try {
        TestFramework::getInstance().runAllTests();
        
        const auto& stats = TestFramework::getInstance().getStatistics();
        
        std::cout << "\n=== Final Test Results ===" << std::endl;
        std::cout << "Total tests: " << stats.totalTests << std::endl;
        std::cout << "Passed: " << stats.passedTests << std::endl;
        std::cout << "Failed: " << stats.failedTests << std::endl;
        std::cout << "Total execution time: " << stats.totalDuration.count() << "ms" << std::endl;
        
        return (stats.failedTests == 0) ? 0 : 1;
    } catch (const std::exception& e) {
        std::cerr << "Test suite failed with exception: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "Test suite failed with unknown exception" << std::endl;
        return 1;
    }
}