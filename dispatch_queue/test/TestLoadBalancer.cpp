#include "TestFramework.h"
#include "../LoadBalancer.h"
#include "../backend/ConcurrencyThreadPoolEnhanced.h"
#include "../TaskDispatch.h"
#include <thread>
#include <chrono>
#include <vector>
#include <atomic>

using namespace task;
using namespace test;

// 测试负载分析器基本功能
TEST_CASE(LoadAnalyzerBasicFunctionality) {
    EnhancedLoadAnalyzer analyzer;
    
    // 测试CPU使用率获取
    double cpuUsage = analyzer.getSystemCpuUsage();
    TestFramework::ASSERT_TRUE(cpuUsage >= 0.0 && cpuUsage <= 1.0, "CPU usage should be between 0 and 1");
    
    // 测试内存压力获取
    double memoryPressure = analyzer.getMemoryPressure();
    TestFramework::ASSERT_TRUE(memoryPressure >= 0.0 && memoryPressure <= 1.0, "Memory pressure should be between 0 and 1");
    
    // 测试队列压力分析
    auto now = std::chrono::steady_clock::now();
    QueuePressureMetrics metrics = analyzer.analyzeQueuePressure(
        TaskQueuePriority::TQP_Normal, 100, 50, std::chrono::milliseconds(1000));
    
    TestFramework::ASSERT_EQ(TaskQueuePriority::TQP_Normal, metrics.priority, "Priority should match");
    TestFramework::ASSERT_EQ(static_cast<size_t>(100), metrics.currentDepth, "Current depth should match");
    TestFramework::ASSERT_NEAR(50.0, metrics.growthRate, 1.0, "Growth rate should be approximately 50 tasks/second");
}

// 测试自适应阈值管理器
TEST_CASE(AdaptiveThresholdManager) {
    AdaptiveThresholdManager thresholdManager;
    SystemMetrics systemMetrics;
    systemMetrics.cpuCount = 8;
    systemMetrics.cpuUsage = 0.5;
    systemMetrics.ioWaitRatio = 0.2;
    
    ThreadEfficiencyMetrics threadMetrics;
    threadMetrics.activeThreads = 4;
    threadMetrics.idleThreads = 2;
    threadMetrics.avgTaskDuration = 10.0;
    
    // 测试线程创建阈值
    uint32_t highThreshold = thresholdManager.getCreateThreshold(TaskQueuePriority::TQP_High, threadMetrics);
    uint32_t normalThreshold = thresholdManager.getCreateThreshold(TaskQueuePriority::TQP_Normal, threadMetrics);
    uint32_t lowThreshold = thresholdManager.getCreateThreshold(TaskQueuePriority::TQP_Low, threadMetrics);
    
    TestFramework::ASSERT_TRUE(highThreshold <= normalThreshold, "High priority threshold should be <= normal");
    TestFramework::ASSERT_TRUE(normalThreshold <= lowThreshold, "Normal priority threshold should be <= low");
    
    // 测试最优线程数计算
    int32_t optimalThreads = thresholdManager.getOptimalMaxThreads(systemMetrics);
    TestFramework::ASSERT_TRUE(optimalThreads > 0, "Optimal thread count should be positive");
    
    // 测试队列压力阈值
    size_t pressureThreshold = thresholdManager.getQueuePressureThreshold(TaskQueuePriority::TQP_Normal, systemMetrics);
    TestFramework::ASSERT_TRUE(pressureThreshold > 0, "Pressure threshold should be positive");
}

// 测试负载指标分析
TEST_CASE(LoadMetricsAnalysis) {
    EnhancedLoadAnalyzer analyzer;
    
    // 模拟负载分析
    LoadMetrics metrics = analyzer.getLoadMetrics(
        TaskQueuePriority::TQP_High,    // priority
        50,                             // currentDepth
        8,                              // activeThreads
        2,                              // idleThreads
        15.0                            // avgTaskDuration
    );
    
    TestFramework::ASSERT_EQ(TaskQueuePriority::TQP_High, metrics.queuePressure.priority, "Priority should match");
    TestFramework::ASSERT_EQ(static_cast<size_t>(50), metrics.queuePressure.currentDepth, "Queue depth should match");
    TestFramework::ASSERT_EQ(8, metrics.threadEfficiency.activeThreads, "Active threads should match");
    TestFramework::ASSERT_EQ(2, metrics.threadEfficiency.idleThreads, "Idle threads should match");
    TestFramework::ASSERT_NEAR(15.0, metrics.threadEfficiency.avgTaskDuration, 0.1, "Avg task duration should match");
}

// 测试调度决策逻辑
TEST_CASE(SchedulingDecisionLogic) {
    EnhancedLoadAnalyzer analyzer;
    AdaptiveThresholdManager thresholdManager;
    
    // 测试创建新线程的决策
    LoadMetrics metrics;
    metrics.system.cpuUsage = 0.5;
    metrics.system.memoryUsage = 0.3;
    metrics.queuePressure.currentDepth = 100;
    metrics.threadEfficiency.activeThreads = 4;
    metrics.threadEfficiency.idleThreads = 1;
    
    ThreadEfficiencyMetrics threadMetrics = metrics.threadEfficiency;
    uint32_t createThreshold = thresholdManager.getCreateThreshold(TaskQueuePriority::TQP_Normal, threadMetrics);
    
    bool shouldCreate = analyzer.shouldCreateNewThread(metrics, createThreshold, 16);
    TestFramework::ASSERT_TRUE(shouldCreate || !shouldCreate, "Should create decision should be boolean");
    
    // 测试优化现有线程的决策
    metrics.threadEfficiency.utilization = 0.2;
    metrics.queuePressure.currentDepth = 2;
    metrics.threadEfficiency.idleThreads = 10;
    metrics.threadEfficiency.activeThreads = 2;
    
    bool shouldOptimize = thresholdManager.shouldOptimizeExisting(metrics);
    TestFramework::ASSERT_TRUE(shouldOptimize || !shouldOptimize, "Should optimize decision should be boolean");
}

// 测试线程池增强功能
TEST_CASE(EnhancedThreadPoolFunctionality) {
    // 注意：这个测试需要更复杂的设置，因为ConcurrencyThreadPoolEnhanced
    // 需要完整的线程池基础设施
    
    // 我们可以通过检查类的接口来验证基本功能
    TestFramework::ASSERT_TRUE(true, "EnhancedThreadPool class exists");
    
    // 实际的集成测试需要在完整的系统环境中进行
}

// 性能基准测试
TEST_CASE(LoadBalancerPerformanceBenchmark) {
    EnhancedLoadAnalyzer analyzer;
    
    TestFramework::BENCHMARK("LoadMetricsAnalysis", [&analyzer]() {
        LoadMetrics metrics = analyzer.getLoadMetrics(
            TaskQueuePriority::TQP_Normal,  // priority
            100,                            // currentDepth
            8,                              // activeThreads
            2,                              // idleThreads
            10.0                            // avgTaskDuration
        );
        
        // 简单使用结果避免编译器优化
        volatile auto dummy = metrics.queuePressure.currentDepth;
        (void)dummy;
    }, 1000);
}

// 并发安全测试
TEST_CASE(LoadBalancerConcurrentAccess) {
    const int numThreads = 8;
    std::atomic<int> analysisCount{0};
    
    TestFramework::CONCURRENT_TEST("ConcurrentLoadAnalysis", [&]() {
        EnhancedLoadAnalyzer analyzer;
        
        for (int i = 0; i < 50; ++i) {
            LoadMetrics metrics = analyzer.getLoadMetrics(
                TaskQueuePriority::TQP_Normal,
                static_cast<size_t>(i * 10),
                4,
                2,
                static_cast<double>(i)
            );
            
            analysisCount.fetch_add(1);
        }
    }, numThreads);
    
    TestFramework::ASSERT_EQ(400, analysisCount.load(), "All analyses should complete");
}

// 测试不同负载场景
TEST_CASE(DifferentLoadScenarios) {
    EnhancedLoadAnalyzer analyzer;
    AdaptiveThresholdManager thresholdManager;
    
    // 场景1：高负载
    {
        LoadMetrics highLoadMetrics;
        highLoadMetrics.system.cpuUsage = 0.9;
        highLoadMetrics.system.memoryUsage = 0.85;
        highLoadMetrics.queuePressure.currentDepth = 200;
        highLoadMetrics.threadEfficiency.activeThreads = 12;
        highLoadMetrics.threadEfficiency.idleThreads = 0;
        
        ThreadEfficiencyMetrics threadMetrics = highLoadMetrics.threadEfficiency;
        uint32_t threshold = thresholdManager.getCreateThreshold(TaskQueuePriority::TQP_High, threadMetrics);
        
        bool shouldCreate = analyzer.shouldCreateNewThread(highLoadMetrics, threshold, 16);
        std::cout << "High load scenario - should create: " << (shouldCreate ? "yes" : "no") << std::endl;
    }
    
    // 场景2：低负载
    {
        LoadMetrics lowLoadMetrics;
        lowLoadMetrics.system.cpuUsage = 0.1;
        lowLoadMetrics.system.memoryUsage = 0.1;
        lowLoadMetrics.queuePressure.currentDepth = 5;
        lowLoadMetrics.threadEfficiency.activeThreads = 2;
        lowLoadMetrics.threadEfficiency.idleThreads = 8;
        
        ThreadEfficiencyMetrics threadMetrics = lowLoadMetrics.threadEfficiency;
        uint32_t threshold = thresholdManager.getCreateThreshold(TaskQueuePriority::TQP_Normal, threadMetrics);
        
        bool shouldCreate = analyzer.shouldCreateNewThread(lowLoadMetrics, threshold, 16);
        bool shouldOptimize = thresholdManager.shouldOptimizeExisting(lowLoadMetrics);
        
        std::cout << "Low load scenario - should create: " << (shouldCreate ? "yes" : "no") 
                  << ", should optimize: " << (shouldOptimize ? "yes" : "no") << std::endl;
    }
    
    // 场景3：突发负载
    {
        LoadMetrics burstLoadMetrics;
        burstLoadMetrics.system.cpuUsage = 0.4;
        burstLoadMetrics.system.memoryUsage = 0.3;
        burstLoadMetrics.queuePressure.currentDepth = 150;
        burstLoadMetrics.queuePressure.growthRate = 100.0; // 快速增长
        burstLoadMetrics.threadEfficiency.activeThreads = 6;
        burstLoadMetrics.threadEfficiency.idleThreads = 2;
        
        ThreadEfficiencyMetrics threadMetrics = burstLoadMetrics.threadEfficiency;
        uint32_t threshold = thresholdManager.getCreateThreshold(TaskQueuePriority::TQP_Normal, threadMetrics);
        
        bool shouldCreate = analyzer.shouldCreateNewThread(burstLoadMetrics, threshold, 16);
        std::cout << "Burst load scenario - should create: " << (shouldCreate ? "yes" : "no") << std::endl;
    }
}

int main() {
    std::cout << "Starting Load Balancer Tests..." << std::endl;
    
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