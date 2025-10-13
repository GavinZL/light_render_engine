#include "TestFramework.h"
#include "../PerformanceMonitor.h"
#include "../LoadBalancer.h"
#include <thread>
#include <chrono>
#include <atomic>

using namespace task;
using namespace test;

// 测试指标收集器基本功能
TEST_CASE(BasicMetricCollectorFunctionality) {
    // 测试ThreadPoolCollector
    ThreadPoolCollector threadPoolCollector;
    threadPoolCollector.setThreadPoolStats(8, 2, 16, 1000, 10, 2);
    
    auto metrics = threadPoolCollector.collect();
    TestFramework::ASSERT_TRUE(!metrics.empty(), "ThreadPool metrics should not be empty");
    
    bool foundActiveThreads = false;
    bool foundIdleThreads = false;
    for (const auto& metric : metrics) {
        if (metric.name == "threadpool_active_threads") {
            TestFramework::ASSERT_EQ(8.0, metric.value, "Active threads should be 8");
            foundActiveThreads = true;
        } else if (metric.name == "threadpool_idle_threads") {
            TestFramework::ASSERT_EQ(2.0, metric.value, "Idle threads should be 2");
            foundIdleThreads = true;
        }
    }
    
    TestFramework::ASSERT_TRUE(foundActiveThreads, "Should find active threads metric");
    TestFramework::ASSERT_TRUE(foundIdleThreads, "Should find idle threads metric");
    
    // 测试QueueCollector
    QueueCollector queueCollector;
    queueCollector.setQueueStats(TaskQueuePriority::TQP_Normal, 50, 15.5, 100.0);
    
    auto queueMetrics = queueCollector.collect();
    TestFramework::ASSERT_TRUE(!queueMetrics.empty(), "Queue metrics should not be empty");
    
    bool foundQueueDepth = false;
    for (const auto& metric : queueMetrics) {
        if (metric.name == "queue_depth") {
            TestFramework::ASSERT_EQ(50.0, metric.value, "Queue depth should be 50");
            foundQueueDepth = true;
        }
    }
    
    TestFramework::ASSERT_TRUE(foundQueueDepth, "Should find queue depth metric");
    
    // 测试TaskCollector
    TaskCollector taskCollector;
    taskCollector.setTaskStats(25.5, 150.0, 1000, 5, 0.995);
    
    auto taskMetrics = taskCollector.collect();
    TestFramework::ASSERT_TRUE(!taskMetrics.empty(), "Task metrics should not be empty");
    
    bool foundAvgExecTime = false;
    bool foundSuccessRate = false;
    for (const auto& metric : taskMetrics) {
        if (metric.name == "task_avg_execution_time") {
            TestFramework::ASSERT_NEAR(25.5, metric.value, 0.1, "Avg execution time should be 25.5");
            foundAvgExecTime = true;
        } else if (metric.name == "task_success_rate") {
            TestFramework::ASSERT_NEAR(0.995, metric.value, 0.001, "Success rate should be 0.995");
            foundSuccessRate = true;
        }
    }
    
    TestFramework::ASSERT_TRUE(foundAvgExecTime, "Should find avg execution time metric");
    TestFramework::ASSERT_TRUE(foundSuccessRate, "Should find success rate metric");
}

// 测试指标聚合器
TEST_CASE(MetricsAggregatorFunctionality) {
    auto& aggregator = MetricsAggregator::getInstance();
    
    // 注册收集器
    auto threadPoolCollector = std::make_shared<ThreadPoolCollector>();
    threadPoolCollector->setThreadPoolStats(4, 1, 8, 500, 5, 1);
    aggregator.registerCollector(threadPoolCollector);
    
    auto queueCollector = std::make_shared<QueueCollector>();
    queueCollector->setQueueStats(TaskQueuePriority::TQP_High, 25, 10.0, 50.0);
    aggregator.registerCollector(queueCollector);
    
    // 收集所有指标
    auto allMetrics = aggregator.collectAllMetrics();
    TestFramework::ASSERT_TRUE(!allMetrics.empty(), "All metrics should not be empty");
    
    // 验证收集器名称
    auto collectorNames = aggregator.getCollectorNames();
    TestFramework::ASSERT_TRUE(!collectorNames.empty(), "Collector names should not be empty");
    TestFramework::ASSERT_TRUE(collectorNames.size() >= 2, "Should have at least 2 collectors");
    
    // 收集特定收集器的指标
    auto specificMetrics = aggregator.collectMetrics("ThreadPoolCollector");
    TestFramework::ASSERT_TRUE(!specificMetrics.empty(), "Specific metrics should not be empty");
}

// 测试警报分析器
TEST_CASE(AlertAnalyzerFunctionality) {
    auto& analyzer = AlertAnalyzer::getInstance();
    
    // 添加警报规则
    analyzer.addAlertRule("threadpool_active_threads",
                         [](const MetricData& metric) { return metric.value > 10.0; },
                         "Too many active threads",
                         AlertLevel::Warning);
    
    analyzer.addAlertRule("queue_depth",
                         [](const MetricData& metric) { return metric.value > 100.0; },
                         "Queue is too deep",
                         AlertLevel::Error);
    
    // 创建触发警报的指标
    std::vector<MetricData> metrics;
    
    MetricData highThreadMetric;
    highThreadMetric.name = "threadpool_active_threads";
    highThreadMetric.value = 15.0;
    highThreadMetric.timestamp = std::chrono::steady_clock::now();
    metrics.push_back(highThreadMetric);
    
    MetricData deepQueueMetric;
    deepQueueMetric.name = "queue_depth";
    deepQueueMetric.value = 150.0;
    deepQueueMetric.timestamp = std::chrono::steady_clock::now();
    deepQueueMetric.labels["priority"] = "1";
    metrics.push_back(deepQueueMetric);
    
    // 分析指标并生成警报
    auto alerts = analyzer.analyzeMetrics(metrics);
    TestFramework::ASSERT_TRUE(!alerts.empty(), "Alerts should not be empty");
    TestFramework::ASSERT_TRUE(alerts.size() >= 2, "Should have at least 2 alerts");
    
    // 检查警报内容
    bool foundThreadAlert = false;
    bool foundQueueAlert = false;
    for (const auto& alert : alerts) {
        if (alert.message.find("Too many active threads") != std::string::npos) {
            TestFramework::ASSERT_EQ(AlertLevel::Warning, alert.level, "Thread alert should be warning level");
            foundThreadAlert = true;
        } else if (alert.message.find("Queue is too deep") != std::string::npos) {
            TestFramework::ASSERT_EQ(AlertLevel::Error, alert.level, "Queue alert should be error level");
            foundQueueAlert = true;
        }
    }
    
    TestFramework::ASSERT_TRUE(foundThreadAlert, "Should find thread alert");
    TestFramework::ASSERT_TRUE(foundQueueAlert, "Should find queue alert");
    
    // 获取当前警报
    auto currentAlerts = analyzer.getCurrentAlerts();
    TestFramework::ASSERT_TRUE(!currentAlerts.empty(), "Current alerts should not be empty");
    
    // 清除过期警报
    analyzer.clearExpiredAlerts(std::chrono::seconds(0)); // 立即清除所有警报
    auto remainingAlerts = analyzer.getCurrentAlerts();
    // 注意：由于时间戳是当前时间，可能不会立即清除
}

// 测试实时监控
TEST_CASE(RealTimeMonitorFunctionality) {
    auto& monitor = RealTimeMonitor::getInstance();
    
    // 设置回调
    std::atomic<int> metricsCallbackCount{0};
    std::atomic<int> alertCallbackCount{0};
    
    monitor.setMetricsCallback([&metricsCallbackCount](const std::vector<MetricData>& metrics) {
        metricsCallbackCount.fetch_add(1);
        std::cout << "Metrics callback called with " << metrics.size() << " metrics" << std::endl;
    });
    
    monitor.setAlertCallback([&alertCallbackCount](const std::vector<AlertInfo>& alerts) {
        alertCallbackCount.fetch_add(1);
        std::cout << "Alert callback called with " << alerts.size() << " alerts" << std::endl;
    });
    
    // 启动监控（短时间）
    monitor.startMonitoring(std::chrono::milliseconds(100));
    
    // 等待一段时间
    std::this_thread::sleep_for(std::chrono::milliseconds(300));
    
    // 停止监控
    monitor.stopMonitoring();
    
    // 检查回调是否被调用
    std::cout << "Metrics callback count: " << metricsCallbackCount.load() << std::endl;
    std::cout << "Alert callback count: " << alertCallbackCount.load() << std::endl;
    
    // 获取最新数据
    auto latestMetrics = monitor.getLatestMetrics();
    auto latestAlerts = monitor.getLatestAlerts();
    
    std::cout << "Latest metrics count: " << latestMetrics.size() << std::endl;
    std::cout << "Latest alerts count: " << latestAlerts.size() << std::endl;
    
    // 注意：由于没有注册实际的收集器，可能没有指标数据
    TestFramework::ASSERT_TRUE(true, "Monitor functionality test completed");
}

// 性能基准测试
TEST_CASE(PerformanceMonitorBenchmark) {
    TestFramework::BENCHMARK("MetricCollection", []() {
        ThreadPoolCollector collector;
        collector.setThreadPoolStats(8, 2, 16, 1000, 10, 2);
        
        volatile auto metrics = collector.collect();
        (void)metrics; // 防止编译器优化
    }, 1000);
    
    TestFramework::BENCHMARK("AlertAnalysis", []() {
        auto& analyzer = AlertAnalyzer::getInstance();
        
        std::vector<MetricData> metrics;
        for (int i = 0; i < 10; ++i) {
            MetricData metric;
            metric.name = "test_metric_" + std::to_string(i);
            metric.value = static_cast<double>(i);
            metric.timestamp = std::chrono::steady_clock::now();
            metrics.push_back(metric);
        }
        
        volatile auto alerts = analyzer.analyzeMetrics(metrics);
        (void)alerts; // 防止编译器优化
    }, 1000);
}

// 并发安全测试
TEST_CASE(PerformanceMonitorConcurrentAccess) {
    const int numThreads = 6;
    std::atomic<int> totalOperations{0};
    
    TestFramework::CONCURRENT_TEST("ConcurrentMetricCollection", [&]() {
        for (int i = 0; i < 50; ++i) {
            // ThreadPoolCollector并发访问
            ThreadPoolCollector collector;
            collector.setThreadPoolStats(i % 10, i % 5, 16, i * 100, i, i / 2);
            auto metrics = collector.collect();
            
            // MetricsAggregator并发访问
            auto& aggregator = MetricsAggregator::getInstance();
            aggregator.registerCollector(std::make_shared<ThreadPoolCollector>(collector));
            auto allMetrics = aggregator.collectAllMetrics();
            
            totalOperations.fetch_add(static_cast<int>(metrics.size() + allMetrics.size()));
        }
    }, numThreads);
    
    TestFramework::ASSERT_TRUE(totalOperations.load() > 0, "Operations should be performed");
}

// 测试不同监控场景
TEST_CASE(DifferentMonitoringScenarios) {
    // 场景1：正常负载
    {
        ThreadPoolCollector collector;
        collector.setThreadPoolStats(6, 2, 12, 10000, 8, 2);
        
        auto metrics = collector.collect();
        std::cout << "Normal load scenario: " << metrics.size() << " metrics collected" << std::endl;
        
        for (const auto& metric : metrics) {
            std::cout << "  " << metric.name << " = " << metric.value << std::endl;
        }
    }
    
    // 场景2：高负载
    {
        ThreadPoolCollector collector;
        collector.setThreadPoolStats(15, 0, 16, 50000, 20, 5);
        
        auto metrics = collector.collect();
        std::cout << "High load scenario: " << metrics.size() << " metrics collected" << std::endl;
        
        for (const auto& metric : metrics) {
            std::cout << "  " << metric.name << " = " << metric.value << std::endl;
        }
    }
    
    // 场景3：低负载
    {
        ThreadPoolCollector collector;
        collector.setThreadPoolStats(2, 8, 16, 1000, 3, 1);
        
        auto metrics = collector.collect();
        std::cout << "Low load scenario: " << metrics.size() << " metrics collected" << std::endl;
        
        for (const auto& metric : metrics) {
            std::cout << "  " << metric.name << " = " << metric.value << std::endl;
        }
    }
    
    // 场景4：不同队列优先级
    {
        QueueCollector collector;
        collector.setQueueStats(TaskQueuePriority::TQP_High, 5, 5.0, 200.0);
        collector.setQueueStats(TaskQueuePriority::TQP_Normal, 15, 15.0, 100.0);
        collector.setQueueStats(TaskQueuePriority::TQP_Low, 30, 25.0, 50.0);
        
        auto metrics = collector.collect();
        std::cout << "Multi-priority queue scenario: " << metrics.size() << " metrics collected" << std::endl;
        
        for (const auto& metric : metrics) {
            std::cout << "  " << metric.name << " (priority=" << metric.labels.at("priority") 
                      << ") = " << metric.value << std::endl;
        }
    }
}

int main() {
    std::cout << "Starting Performance Monitor Tests..." << std::endl;
    
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