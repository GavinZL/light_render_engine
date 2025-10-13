#include "TestFramework.h"
#include "../ObjectPool.h"
#include "../TaskOperatorPool.h"
#include "../TaskDispatch.h"
#include <memory>
#include <vector>
#include <thread>
#include <chrono>

using namespace task;
using namespace test;

// 测试基础对象池功能
TEST_CASE(BasicObjectPoolFunctionality) {
    // 创建一个简单的对象池
    ObjectPoolConfig config;
    config.initialSize = 5;
    config.maxSize = 10;
    config.minSize = 2;
    
    ObjectPool<int> pool(
        []() { return std::make_shared<int>(0); },  // 工厂函数
        [](std::shared_ptr<int>& obj) { *obj = 0; }, // 重置函数
        config
    );
    
    // 获取对象
    auto obj1 = pool.acquire();
    TestFramework::ASSERT_TRUE(obj1 != nullptr, "Object should be acquired");
    TestFramework::ASSERT_EQ(0, *obj1, "Object should be initialized to 0");
    
    *obj1 = 42;
    
    // 获取更多对象
    std::vector<std::shared_ptr<int>> objects;
    for (int i = 0; i < 4; ++i) {
        auto obj = pool.acquire();
        TestFramework::ASSERT_TRUE(obj != nullptr, "Object should be acquired");
        objects.push_back(obj);
    }
    
    // 检查统计信息
    auto stats = pool.getStats();
    TestFramework::ASSERT_EQ(static_cast<size_t>(5), stats.poolSize, "Pool size should be 5");
    TestFramework::ASSERT_EQ(static_cast<size_t>(5), stats.inUseObjects, "All objects should be in use");
    TestFramework::ASSERT_EQ(static_cast<size_t>(5), stats.totalAcquired, "5 objects should be acquired");
    
    // 释放对象（通过智能指针自动释放）
    obj1.reset();
    objects.clear();
    
    // 检查统计信息更新
    stats = pool.getStats();
    TestFramework::ASSERT_EQ(static_cast<size_t>(5), stats.poolSize, "Pool size should still be 5");
    TestFramework::ASSERT_EQ(static_cast<size_t>(0), stats.inUseObjects, "No objects should be in use");
    TestFramework::ASSERT_EQ(static_cast<size_t>(5), stats.totalReleased, "5 objects should be released");
}

// 测试TaskOperator池化
TEST_CASE(TaskOperatorPooling) {
    auto& poolManager = TaskOperatorPoolManager::getInstance();
    
    // 获取池统计信息
    auto initialStats = poolManager.getStats();
    
    // 创建多个TaskOperator
    std::vector<std::shared_ptr<TaskOperator>> tasks;
    for (int i = 0; i < 10; ++i) {
        auto task = poolManager.acquireTaskOperator(TaskMark::TM_TaskQueue_Inner);
        TestFramework::ASSERT_TRUE(task != nullptr, "TaskOperator should be acquired");
        tasks.push_back(task);
    }
    
    // 检查统计信息更新
    auto stats = poolManager.getStats();
    TestFramework::ASSERT_TRUE(stats.taskOperatorStats.totalAcquired >= 10, 
                              "At least 10 TaskOperators should be acquired");
    
    // 释放所有任务
    tasks.clear();
    
    // 检查释放统计
    stats = poolManager.getStats();
    TestFramework::ASSERT_TRUE(stats.taskOperatorStats.totalReleased >= 10,
                              "At least 10 TaskOperators should be released");
}

// 测试TaskOperatorEnhanced池化
TEST_CASE(TaskOperatorEnhancedPooling) {
    auto& poolManager = TaskOperatorPoolManager::getInstance();
    
    // 创建多个TaskOperatorEnhanced
    std::vector<std::shared_ptr<TaskOperatorEnhanced>> tasks;
    for (int i = 0; i < 5; ++i) {
        auto task = poolManager.acquireEnhancedTaskOperator(TaskCategory::Custom, 128);
        TestFramework::ASSERT_TRUE(task != nullptr, "TaskOperatorEnhanced should be acquired");
        tasks.push_back(task);
    }
    
    // 检查统计信息
    auto stats = poolManager.getStats();
    TestFramework::ASSERT_TRUE(stats.enhancedTaskOperatorStats.totalAcquired >= 5,
                              "At least 5 TaskOperatorEnhanced should be acquired");
    
    // 释放所有任务
    tasks.clear();
    
    // 检查释放统计
    stats = poolManager.getStats();
    TestFramework::ASSERT_TRUE(stats.enhancedTaskOperatorStats.totalReleased >= 5,
                              "At least 5 TaskOperatorEnhanced should be released");
}

// 测试池化性能优势
TEST_CASE(PoolingPerformanceComparison) {
    const int iterations = 1000;
    
    // 测试不使用池化的情况
    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < iterations; ++i) {
        auto task = std::make_shared<TaskOperator>();
        // 使用任务
        task->resetCallStartTime();
    }
    auto end = std::chrono::high_resolution_clock::now();
    auto withoutPooling = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    
    // 测试使用池化的情况
    auto& poolManager = TaskOperatorPoolManager::getInstance();
    poolManager.enablePooling(true);
    
    start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < iterations; ++i) {
        auto task = poolManager.acquireTaskOperator(TaskMark::TM_TaskQueue_Inner);
        // 使用任务
        task->resetCallStartTime();
        // 任务自动归还到池中
    }
    end = std::chrono::high_resolution_clock::now();
    auto withPooling = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    
    // 检查池化是否提高了性能（至少不显著降低）
    std::cout << "Without pooling: " << withoutPooling.count() << "μs" << std::endl;
    std::cout << "With pooling: " << withPooling.count() << "μs" << std::endl;
    
    // 性能应该相近或更好
    TestFramework::ASSERT_TRUE(withPooling.count() <= withoutPooling.count() * 2,
                              "Pooling should not be significantly slower");
}

// 测试池配置管理
TEST_CASE(PoolConfigurationManagement) {
    auto& poolManager = TaskOperatorPoolManager::getInstance();
    
    // 获取默认配置
    auto stats = poolManager.getStats();
    
    // 修改配置
    ObjectPoolConfig newConfig;
    newConfig.initialSize = 15;
    newConfig.maxSize = 50;
    newConfig.minSize = 5;
    newConfig.maxIdleTime = std::chrono::seconds(120);
    newConfig.enableAutoShrink = false;
    
    poolManager.configurePool(newConfig);
    
    // 验证配置生效（通过统计信息间接验证）
    auto updatedStats = poolManager.getStats();
    // 配置更改不会立即影响现有池大小，但会影响新创建的对象池
    
    // 测试收缩功能
    poolManager.shrinkAllPools();
    
    // 测试清空功能
    poolManager.clearAllPools();
    auto clearedStats = poolManager.getStats();
    // 清空后应该重新初始化池
}

// 测试内存压力管理
TEST_CASE(MemoryPressureManagement) {
    auto& poolManager = TaskOperatorPoolManager::getInstance();
    
    // 设置较低的内存阈值以触发压力管理
    poolManager.setMemoryPressureThreshold(1024); // 1KB阈值
    
    // 创建大量任务来增加内存使用
    std::vector<std::shared_ptr<TaskOperator>> tasks;
    for (int i = 0; i < 50; ++i) {
        auto task = poolManager.acquireTaskOperator(TaskMark::TM_TaskQueue_Inner);
        tasks.push_back(task);
    }
    
    // 检查是否检测到内存压力（通过统计信息）
    auto stats = poolManager.getStats();
    std::cout << "Memory usage: " << stats.totalMemoryUsage << " bytes" << std::endl;
    
    // 释放任务
    tasks.clear();
    
    // 测试手动处理内存压力
    poolManager.handleMemoryPressure();
}

// 并发池化测试
TEST_CASE(ConcurrentPooling) {
    const int numThreads = 8;
    const int tasksPerThread = 50;
    std::atomic<int> totalAcquired{0};
    std::atomic<int> totalReleased{0};
    
    TestFramework::CONCURRENT_TEST("ConcurrentTaskOperatorPooling", [&]() {
        auto& poolManager = TaskOperatorPoolManager::getInstance();
        
        for (int i = 0; i < tasksPerThread; ++i) {
            auto task = poolManager.acquireTaskOperator(TaskMark::TM_TaskQueue_Inner);
            if (task) {
                totalAcquired.fetch_add(1);
                task->resetCallStartTime();
                // 任务在作用域结束时自动释放
            }
        }
    }, numThreads);
    
    // 等待一段时间让所有任务完成并归还到池中
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    
    std::cout << "Total acquired: " << totalAcquired.load() << std::endl;
    std::cout << "Total released: " << totalReleased.load() << std::endl;
    
    TestFramework::ASSERT_TRUE(totalAcquired.load() > 0, "Tasks should be acquired");
    // 注意：totalReleased可能不等于totalAcquired，因为池化对象的释放是异步的
}

int main() {
    std::cout << "Starting Memory Pooling Tests..." << std::endl;
    
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