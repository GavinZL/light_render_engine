// 性能监控和指标收集系统
#ifndef __PERFORMANCE_MONITOR_H__
#define __PERFORMANCE_MONITOR_H__

#include <string>
#include <memory>
#include <unordered_map>
#include <vector>
#include <chrono>
#include <atomic>
#include <mutex>
#include <thread>
#include <functional>

namespace task
{

// 监控指标类型
enum class MetricType {
    Counter,     // 计数器
    Gauge,       // 仪表
    Histogram,   // 直方图
    Summary      // 摘要
};

// 监控指标数据
struct MetricData {
    std::string name;
    std::string description;
    MetricType type;
    double value;
    std::chrono::steady_clock::time_point timestamp;
    std::unordered_map<std::string, std::string> labels;
};

// 监控指标收集器接口
class MetricCollector {
public:
    virtual ~MetricCollector() = default;
    
    // 收集指标
    virtual std::vector<MetricData> collect() = 0;
    
    // 获取收集器名称
    virtual std::string getName() const = 0;
};

// 线程池指标收集器
class ThreadPoolCollector : public MetricCollector {
public:
    ThreadPoolCollector();
    virtual ~ThreadPoolCollector() = default;
    
    virtual std::vector<MetricData> collect() override;
    virtual std::string getName() const override { return "ThreadPoolCollector"; }
    
    // 设置线程池统计数据
    void setThreadPoolStats(int32_t activeThreads, int32_t idleThreads, int32_t maxThreads,
                          int64_t totalTasks, int64_t totalCreated, int64_t totalDestroyed);
    
private:
    std::atomic<int32_t> mActiveThreads{0};
    std::atomic<int32_t> mIdleThreads{0};
    std::atomic<int32_t> mMaxThreads{0};
    std::atomic<int64_t> mTotalTasks{0};
    std::atomic<int64_t> mTotalCreated{0};
    std::atomic<int64_t> mTotalDestroyed{0};
};

// 队列指标收集器
class QueueCollector : public MetricCollector {
public:
    QueueCollector();
    virtual ~QueueCollector() = default;
    
    virtual std::vector<MetricData> collect() override;
    virtual std::string getName() const override { return "QueueCollector"; }
    
    // 设置队列统计数据
    void setQueueStats(TaskQueuePriority priority, size_t queueDepth, 
                      double avgWaitTime, double taskThroughput);
    
private:
    struct QueueStats {
        TaskQueuePriority priority;
        size_t queueDepth;
        double avgWaitTime;
        double taskThroughput;
    };
    
    std::vector<QueueStats> mQueueStats;
    std::mutex mStatsMutex;
};

// 任务指标收集器
class TaskCollector : public MetricCollector {
public:
    TaskCollector();
    virtual ~TaskCollector() = default;
    
    virtual std::vector<MetricData> collect() override;
    virtual std::string getName() const override { return "TaskCollector"; }
    
    // 设置任务统计数据
    void setTaskStats(double avgExecutionTime, double maxExecutionTime,
                     int64_t totalExecuted, int64_t failedTasks,
                     double successRate);
    
private:
    std::atomic<double> mAvgExecutionTime{0.0};
    std::atomic<double> mMaxExecutionTime{0.0};
    std::atomic<int64_t> mTotalExecuted{0};
    std::atomic<int64_t> mFailedTasks{0};
    std::atomic<double> mSuccessRate{1.0};
};

// 性能指标聚合器
class MetricsAggregator {
public:
    static MetricsAggregator& getInstance();
    
    // 注册收集器
    void registerCollector(std::shared_ptr<MetricCollector> collector);
    
    // 收集所有指标
    std::vector<MetricData> collectAllMetrics();
    
    // 获取特定收集器的指标
    std::vector<MetricData> collectMetrics(const std::string& collectorName);
    
    // 获取所有收集器名称
    std::vector<std::string> getCollectorNames() const;
    
private:
    MetricsAggregator() = default;
    ~MetricsAggregator();
    
    mutable std::mutex mCollectorsMutex;
    std::unordered_map<std::string, std::shared_ptr<MetricCollector>> mCollectors;
};

// 警报级别
enum class AlertLevel {
    Info,     // 信息
    Warning,  // 警告
    Error,    // 错误
    Critical  // 严重
};

// 警报信息
struct AlertInfo {
    std::string id;
    std::string message;
    AlertLevel level;
    std::chrono::steady_clock::time_point timestamp;
    std::unordered_map<std::string, std::string> tags;
};

// 警报分析器
class AlertAnalyzer {
public:
    static AlertAnalyzer& getInstance();
    
    // 分析指标并生成警报
    std::vector<AlertInfo> analyzeMetrics(const std::vector<MetricData>& metrics);
    
    // 添加警报规则
    void addAlertRule(const std::string& metricName, 
                     std::function<bool(const MetricData&)> condition,
                     const std::string& alertMessage,
                     AlertLevel level = AlertLevel::Warning);
    
    // 获取当前警报
    std::vector<AlertInfo> getCurrentAlerts() const;
    
    // 清除过期警报
    void clearExpiredAlerts(std::chrono::seconds maxAge = std::chrono::seconds(3600));
    
private:
    AlertAnalyzer() = default;
    
    struct AlertRule {
        std::string metricName;
        std::function<bool(const MetricData&)> condition;
        std::string alertMessage;
        AlertLevel level;
    };
    
    mutable std::mutex mRulesMutex;
    mutable std::mutex mAlertsMutex;
    std::vector<AlertRule> mAlertRules;
    std::vector<AlertInfo> mActiveAlerts;
};

// 实时监控接口
class RealTimeMonitor {
public:
    static RealTimeMonitor& getInstance();
    
    // 启动监控
    void startMonitoring(std::chrono::milliseconds interval = std::chrono::milliseconds(1000));
    
    // 停止监控
    void stopMonitoring();
    
    // 获取最新指标
    std::vector<MetricData> getLatestMetrics();
    
    // 获取最新警报
    std::vector<AlertInfo> getLatestAlerts();
    
    // 设置指标回调
    void setMetricsCallback(std::function<void(const std::vector<MetricData>&)> callback);
    
    // 设置警报回调
    void setAlertCallback(std::function<void(const std::vector<AlertInfo>&)> callback);
    
private:
    RealTimeMonitor();
    ~RealTimeMonitor();
    
    void monitoringLoop();
    
    std::atomic<bool> mIsRunning{false};
    std::thread mMonitoringThread;
    std::chrono::milliseconds mInterval;
    
    std::function<void(const std::vector<MetricData>&)> mMetricsCallback;
    std::function<void(const std::vector<AlertInfo>&)> mAlertCallback;
    
    mutable std::mutex mMetricsMutex;
    mutable std::mutex mAlertsMutex;
    std::vector<MetricData> mLatestMetrics;
    std::vector<AlertInfo> mLatestAlerts;
};

} // namespace task

#endif // __PERFORMANCE_MONITOR_H__