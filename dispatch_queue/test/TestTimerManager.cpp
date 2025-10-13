#include "../TimerManager.h"
#include "../TaskDispatch.h"
#include <cassert>
#include <chrono>
#include <iostream>
#include <thread>
#include <atomic>

using namespace task;

void testBasicTimerFunctionality() {
    std::cout << "=== Testing Basic Timer Functionality ===" << std::endl;
    
    std::atomic<bool> taskExecuted{false};
    auto startTime = std::chrono::steady_clock::now();
    
    auto task = std::make_shared<TaskOperator>(TaskMark::TM_TaskQueue_Inner, 
        [&taskExecuted](const std::shared_ptr<TaskOperator>&) {
            taskExecuted.store(true);
            std::cout << "Timer task executed!" << std::endl;
        });
    
    // 调度100ms后执行的任务
    uint64_t timerId = TimerManager::getInstance().scheduleTask(task, std::chrono::milliseconds(100));
    std::cout << "Task scheduled with ID: " << timerId << std::endl;
    
    // 等待任务执行
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    auto endTime = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
    
    assert(taskExecuted.load());
    assert(duration.count() >= 100);
    assert(duration.count() < 300); // 允许一些执行误差
    
    std::cout << "Task executed in " << duration.count() << "ms" << std::endl;
    std::cout << "✓ Basic timer functionality test passed" << std::endl;
}

void testTaskCancellation() {
    std::cout << "\n=== Testing Task Cancellation ===" << std::endl;
    
    std::atomic<bool> taskExecuted{false};
    
    auto task = std::make_shared<TaskOperator>(TaskMark::TM_TaskQueue_Inner,
        [&taskExecuted](const std::shared_ptr<TaskOperator>&) {
            taskExecuted.store(true);
            std::cout << "This task should not execute!" << std::endl;
        });
    
    // 调度1秒后执行的任务
    uint64_t timerId = TimerManager::getInstance().scheduleTask(task, std::chrono::milliseconds(1000));
    std::cout << "Task scheduled with ID: " << timerId << std::endl;
    
    // 50ms后取消任务
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    bool cancelled = TimerManager::getInstance().cancelTask(timerId);
    std::cout << "Task cancellation result: " << (cancelled ? "success" : "failed") << std::endl;
    
    // 等待原本的执行时间
    std::this_thread::sleep_for(std::chrono::milliseconds(1100));
    
    assert(cancelled);
    assert(!taskExecuted.load());
    
    std::cout << "✓ Task cancellation test passed" << std::endl;
}

void testMultipleTimers() {
    std::cout << "\n=== Testing Multiple Timers ===" << std::endl;
    
    std::atomic<int> executionOrder{0};
    std::vector<int> results;
    std::mutex resultsMutex;
    
    // 创建多个不同延时的任务
    for (int i = 0; i < 5; ++i) {
        int delay = (5 - i) * 50; // 250ms, 200ms, 150ms, 100ms, 50ms
        int expectedOrder = 5 - i; // 5, 4, 3, 2, 1
        
        auto task = std::make_shared<TaskOperator>(TaskMark::TM_TaskQueue_Inner,
            [&results, &resultsMutex, expectedOrder](const std::shared_ptr<TaskOperator>&) {
                std::lock_guard<std::mutex> lock(resultsMutex);
                results.push_back(expectedOrder);
                std::cout << "Task " << expectedOrder << " executed" << std::endl;
            });
        
        uint64_t timerId = TimerManager::getInstance().scheduleTask(task, std::chrono::milliseconds(delay));
        std::cout << "Task " << expectedOrder << " scheduled for " << delay << "ms, ID: " << timerId << std::endl;
    }
    
    // 等待所有任务执行完成
    std::this_thread::sleep_for(std::chrono::milliseconds(400));
    
    // 验证执行顺序
    assert(results.size() == 5);
    for (size_t i = 0; i < results.size(); ++i) {
        assert(results[i] == static_cast<int>(i + 1));
    }
    
    std::cout << "✓ Multiple timers test passed" << std::endl;
}

void testTimerManagerConfig() {
    std::cout << "\n=== Testing Timer Manager Configuration ===" << std::endl;
    
    // 获取默认配置
    TimerConfig defaultConfig = TimerManager::getInstance().getConfig();
    std::cout << "Default config - minDelay: " << defaultConfig.minDelay.count() 
              << "ms, maxDelay: " << defaultConfig.maxDelay.count() 
              << "ms, maxPendingTasks: " << defaultConfig.maxPendingTasks << std::endl;
    
    // 设置新配置
    TimerConfig newConfig;
    newConfig.minDelay = std::chrono::milliseconds(10);
    newConfig.maxDelay = std::chrono::seconds(30);
    newConfig.maxPendingTasks = 5000;
    
    TimerManager::getInstance().setConfig(newConfig);
    
    // 验证配置更新
    TimerConfig updatedConfig = TimerManager::getInstance().getConfig();
    assert(updatedConfig.minDelay == newConfig.minDelay);
    assert(updatedConfig.maxDelay == newConfig.maxDelay);
    assert(updatedConfig.maxPendingTasks == newConfig.maxPendingTasks);
    
    std::cout << "✓ Timer manager configuration test passed" << std::endl;
    
    // 恢复默认配置
    TimerManager::getInstance().setConfig(defaultConfig);
}

void testPendingTaskCount() {
    std::cout << "\n=== Testing Pending Task Count ===" << std::endl;
    
    size_t initialCount = TimerManager::getInstance().getPendingTaskCount();
    std::cout << "Initial pending task count: " << initialCount << std::endl;
    
    // 添加几个长延时任务
    std::vector<uint64_t> timerIds;
    for (int i = 0; i < 3; ++i) {
        auto task = std::make_shared<TaskOperator>(TaskMark::TM_TaskQueue_Inner,
            [i](const std::shared_ptr<TaskOperator>&) {
                std::cout << "Long delay task " << i << " executed" << std::endl;
            });
        
        uint64_t timerId = TimerManager::getInstance().scheduleTask(task, std::chrono::seconds(10));
        timerIds.push_back(timerId);
    }
    
    size_t afterAddCount = TimerManager::getInstance().getPendingTaskCount();
    std::cout << "After adding 3 tasks: " << afterAddCount << std::endl;
    assert(afterAddCount >= initialCount + 3);
    
    // 取消所有任务
    for (uint64_t timerId : timerIds) {
        TimerManager::getInstance().cancelTask(timerId);
    }
    
    // 等待一下让取消操作生效
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    
    std::cout << "✓ Pending task count test passed" << std::endl;
}

int main() {
    std::cout << "Starting Timer Manager Tests..." << std::endl;
    
    try {
        testBasicTimerFunctionality();
        testTaskCancellation();
        testMultipleTimers();
        testTimerManagerConfig();
        testPendingTaskCount();
        
        std::cout << "\n=== All Timer Tests Passed! ===" << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Test failed with exception: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "Test failed with unknown exception" << std::endl;
        return 1;
    }
    
    return 0;
}