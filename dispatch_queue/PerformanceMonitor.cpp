#include "PerformanceMonitor.h"
#include "TaskQueueDefine.h"
#include <algorithm>
#include <sstream>

namespace task
{

// ThreadPoolCollector实现
ThreadPoolCollector::ThreadPoolCollector() = default;

std::vector<MetricData> ThreadPoolCollector::collect() {
    std::vector<MetricData> metrics;
    auto now = std::chrono::steady_clock::now();
    
    // 活跃线程数
    MetricData activeThreadsMetric;
    activeThreadsMetric.name = "threadpool_active_threads";
    activeThreadsMetric.description = "Number of active threads in the thread pool";
    activeThreadsMetric.type = MetricType::Gauge;
    activeThreadsMetric.value = static_cast<double>(mActiveThreads.load());
    activeThreadsMetric.timestamp = now;
    metrics.push_back(activeThreadsMetric);
    
    // 空闲线程数
    MetricData idleThreadsMetric;
    idleThreadsMetric.name = "threadpool_idle_threads";
    idleThreadsMetric.description = "Number of idle threads in the thread pool";
    idleThreadsMetric.type = MetricType::Gauge;
    idleThreadsMetric.value = static_cast<double>(mIdleThreads.load());
    idleThreadsMetric.timestamp = now;
    metrics.push_back(idleThreadsMetric);
    
    // 最大线程数
    MetricData maxThreadsMetric;
    maxThreadsMetric.name = "threadpool_max_threads";
    maxThreadsMetric.description = "Maximum number of threads in the thread pool";
    maxThreadsMetric.type = MetricType::Gauge;
    maxThreadsMetric.value = static_cast<double>(mMaxThreads.load());
    maxThreadsMetric.timestamp = now;
    metrics.push_back(maxThreadsMetric);
    
    // 总任务数
    MetricData totalTasksMetric;
    totalTasksMetric.name = "threadpool_total_tasks";
    totalTasksMetric.description = "Total number of tasks executed";
    totalTasksMetric.type = MetricType::Counter;
    totalTasksMetric.value = static_cast<double>(mTotalTasks.load());
    totalTasksMetric.timestamp = now;
    metrics.push_back(totalTasksMetric);
    
    // 创建的线程数
    MetricData totalCreatedMetric;
    totalCreatedMetric.name = "threadpool_threads_created";
    totalCreatedMetric.description = "Total number of threads created";
    totalCreatedMetric.type = MetricType::Counter;
    totalCreatedMetric.value = static_cast<double>(mTotalCreated.load());
    totalCreatedMetric.timestamp = now;
    metrics.push_back(totalCreatedMetric);
    
    // 销毁的线程数
    MetricData totalDestroyedMetric;
    totalDestroyedMetric.name = "threadpool_threads_destroyed";
    totalDestroyedMetric.description = "Total number of threads destroyed";
    totalDestroyedMetric.type = MetricType::Counter;
    totalDestroyedMetric.value = static_cast<double>(mTotalDestroyed.load());
    totalDestroyedMetric.timestamp = now;
    metrics.push_back(totalDestroyedMetric);
    
    return metrics;
}

void ThreadPoolCollector::setThreadPoolStats(int32_t activeThreads, int32_t idleThreads, int32_t maxThreads,
                                           int64_t totalTasks, int64_t totalCreated, int64_t totalDestroyed) {
    mActiveThreads.store(activeThreads);
    mIdleThreads.store(idleThreads);
    mMaxThreads.store(maxThreads);
    mTotalTasks.store(totalTasks);
    mTotalCreated.store(totalCreated);
    mTotalDestroyed.store(totalDestroyed);
}

// QueueCollector实现
QueueCollector::QueueCollector() = default;

std::vector<MetricData> QueueCollector::collect() {
    std::vector<MetricData> metrics;
    auto now = std::chrono::steady_clock::now();
    
    std::lock_guard<std::mutex> lock(mStatsMutex);
    
    for (const auto& stats : mQueueStats) {
        // 队列深度
        MetricData depthMetric;
        depthMetric.name = "queue_depth";
        depthMetric.description = "Current queue depth";
        depthMetric.type = MetricType::Gauge;
        depthMetric.value = static_cast<double>(stats.queueDepth);
        depthMetric.timestamp = now;
        
        std::stringstream priorityLabel;
        priorityLabel << static_cast<int>(stats.priority);
        depthMetric.labels["priority"] = priorityLabel.str();
        
        metrics.push_back(depthMetric);
        
        // 平均等待时间
        MetricData waitTimeMetric;
        waitTimeMetric.name = "queue_avg_wait_time";
        waitTimeMetric.description = "Average task wait time in queue (ms)";
        waitTimeMetric.type = MetricType::Gauge;
        waitTimeMetric.value = stats.avgWaitTime;
        waitTimeMetric.timestamp = now;
        waitTimeMetric.labels["priority"] = priorityLabel.str();
        
        metrics.push_back(waitTimeMetric);
        
        // 任务吞吐量
        MetricData throughputMetric;
        throughputMetric.name = "queue_throughput";
        throughputMetric.description = "Task throughput (tasks/second)";
        throughputMetric.type = MetricType::Gauge;
        throughputMetric.value = stats.taskThroughput;
        throughputMetric.timestamp = now;
        throughputMetric.labels["priority"] = priorityLabel.str();
        
        metrics.push_back(throughputMetric);
    }
    
    return metrics;
}

void QueueCollector::setQueueStats(TaskQueuePriority priority, size_t queueDepth, 
                                  double avgWaitTime, double taskThroughput) {
    std::lock_guard<std::mutex> lock(mStatsMutex);
    
    // 查找是否已存在该优先级的统计
    auto it = std::find_if(mQueueStats.begin(), mQueueStats.end(),
                          [priority](const QueueStats& stats) {
                              return stats.priority == priority;
                          });
    
    if (it != mQueueStats.end()) {
        // 更新现有统计
        it->queueDepth = queueDepth;
        it->avgWaitTime = avgWaitTime;
        it->taskThroughput = taskThroughput;
    } else {
        // 添加新统计
        mQueueStats.push_back({priority, queueDepth, avgWaitTime, taskThroughput});
    }
}

// TaskCollector实现
TaskCollector::TaskCollector() = default;

std::vector<MetricData> TaskCollector::collect() {
    std::vector<MetricData> metrics;
    auto now = std::chrono::steady_clock::now();
    
    // 平均执行时间
    MetricData avgExecTimeMetric;
    avgExecTimeMetric.name = "task_avg_execution_time";
    avgExecTimeMetric.description = "Average task execution time (ms)";
    avgExecTimeMetric.type = MetricType::Gauge;
    avgExecTimeMetric.value = mAvgExecutionTime.load();
    avgExecTimeMetric.timestamp = now;
    metrics.push_back(avgExecTimeMetric);
    
    // 最大执行时间
    MetricData maxExecTimeMetric;
    maxExecTimeMetric.name = "task_max_execution_time";
    maxExecTimeMetric.description = "Maximum task execution time (ms)";
    maxExecTimeMetric.type = MetricType::Gauge;
    maxExecTimeMetric.value = mMaxExecutionTime.load();
    maxExecTimeMetric.timestamp = now;
    metrics.push_back(maxExecTimeMetric);
    
    // 总执行任务数
    MetricData totalExecutedMetric;
    totalExecutedMetric.name = "task_total_executed";
    totalExecutedMetric.description = "Total number of tasks executed";
    totalExecutedMetric.type = MetricType::Counter;
    totalExecutedMetric.value = static_cast<double>(mTotalExecuted.load());
    totalExecutedMetric.timestamp = now;
    metrics.push_back(totalExecutedMetric);
    
    // 失败任务数
    MetricData failedTasksMetric;
    failedTasksMetric.name = "task_failed";
    failedTasksMetric.description = "Number of failed tasks";
    failedTasksMetric.type = MetricType::Counter;
    failedTasksMetric.value = static_cast<double>(mFailedTasks.load());
    failedTasksMetric.timestamp = now;
    metrics.push_back(failedTasksMetric);
    
    // 成功率
    MetricData successRateMetric;
    successRateMetric.name = "task_success_rate";
    successRateMetric.description = "Task execution success rate (0.0 - 1.0)";
    successRateMetric.type = MetricType::Gauge;
    successRateMetric.value = mSuccessRate.load();
    successRateMetric.timestamp = now;
    metrics.push_back(successRateMetric);
    
    return metrics;
}

void TaskCollector::setTaskStats(double avgExecutionTime, double maxExecutionTime,
                                int64_t totalExecuted, int64_t failedTasks,
                                double successRate) {
    mAvgExecutionTime.store(avgExecutionTime);
    mMaxExecutionTime.store(maxExecutionTime);
    mTotalExecuted.store(totalExecuted);
    mFailedTasks.store(failedTasks);
    mSuccessRate.store(successRate);
}

// MetricsAggregator实现
MetricsAggregator& MetricsAggregator::getInstance() {
    static MetricsAggregator instance;
    return instance;
}

void MetricsAggregator::registerCollector(std::shared_ptr<MetricCollector> collector) {
    if (!collector) {
        return;
    }
    
    std::lock_guard<std::mutex> lock(mCollectorsMutex);
    mCollectors[collector->getName()] = collector;
}

std::vector<MetricData> MetricsAggregator::collectAllMetrics() {
    std::vector<MetricData> allMetrics;
    
    std::lock_guard<std::mutex> lock(mCollectorsMutex);
    
    for (const auto& pair : mCollectors) {
        auto metrics = pair.second->collect();
        allMetrics.insert(allMetrics.end(), metrics.begin(), metrics.end());
    }
    
    return allMetrics;
}

std::vector<MetricData> MetricsAggregator::collectMetrics(const std::string& collectorName) {
    std::lock_guard<std::mutex> lock(mCollectorsMutex);
    
    auto it = mCollectors.find(collectorName);
    if (it != mCollectors.end()) {
        return it->second->collect();
    }
    
    return {};
}

std::vector<std::string> MetricsAggregator::getCollectorNames() const {
    std::vector<std::string> names;
    
    std::lock_guard<std::mutex> lock(mCollectorsMutex);
    
    for (const auto& pair : mCollectors) {
        names.push_back(pair.first);
    }
    
    return names;
}

MetricsAggregator::~MetricsAggregator() {
    std::lock_guard<std::mutex> lock(mCollectorsMutex);
    mCollectors.clear();
}

// AlertAnalyzer实现
AlertAnalyzer& AlertAnalyzer::getInstance() {
    static AlertAnalyzer instance;
    return instance;
}

std::vector<AlertInfo> AlertAnalyzer::analyzeMetrics(const std::vector<MetricData>& metrics) {
    std::vector<AlertInfo> alerts;
    auto now = std::chrono::steady_clock::now();
    
    std::lock_guard<std::mutex> lock(mRulesMutex);
    
    for (const auto& metric : metrics) {
        for (const auto& rule : mAlertRules) {
            if (metric.name == rule.metricName && rule.condition(metric)) {
                AlertInfo alert;
                alert.id = metric.name + "_" + std::to_string(now.time_since_epoch().count());
                alert.message = rule.alertMessage + " (value: " + std::to_string(metric.value) + ")";
                alert.level = rule.level;
                alert.timestamp = now;
                alert.tags["metric"] = metric.name;
                
                alerts.push_back(alert);
            }
        }
    }
    
    // 更新活动警报
    {
        std::lock_guard<std::mutex> alertLock(mAlertsMutex);
        mActiveAlerts.insert(mActiveAlerts.end(), alerts.begin(), alerts.end());
    }
    
    return alerts;
}

void AlertAnalyzer::addAlertRule(const std::string& metricName, 
                                std::function<bool(const MetricData&)> condition,
                                const std::string& alertMessage,
                                AlertLevel level) {
    AlertRule rule;
    rule.metricName = metricName;
    rule.condition = condition;
    rule.alertMessage = alertMessage;
    rule.level = level;
    
    std::lock_guard<std::mutex> lock(mRulesMutex);
    mAlertRules.push_back(rule);
}

std::vector<AlertInfo> AlertAnalyzer::getCurrentAlerts() const {
    std::lock_guard<std::mutex> lock(mAlertsMutex);
    return mActiveAlerts;
}

void AlertAnalyzer::clearExpiredAlerts(std::chrono::seconds maxAge) {
    auto now = std::chrono::steady_clock::now();
    
    std::lock_guard<std::mutex> lock(mAlertsMutex);
    
    mActiveAlerts.erase(
        std::remove_if(mActiveAlerts.begin(), mActiveAlerts.end(),
                      [now, maxAge](const AlertInfo& alert) {
                          auto age = std::chrono::duration_cast<std::chrono::seconds>(now - alert.timestamp);
                          return age > maxAge;
                      }),
        mActiveAlerts.end());
}

// RealTimeMonitor实现
RealTimeMonitor& RealTimeMonitor::getInstance() {
    static RealTimeMonitor instance;
    return instance;
}

RealTimeMonitor::RealTimeMonitor() 
    : mInterval(std::chrono::milliseconds(1000)) {
}

RealTimeMonitor::~RealTimeMonitor() {
    stopMonitoring();
}

void RealTimeMonitor::startMonitoring(std::chrono::milliseconds interval) {
    if (mIsRunning.exchange(true)) {
        return; // 已经在运行
    }
    
    mInterval = interval;
    mMonitoringThread = std::thread(&RealTimeMonitor::monitoringLoop, this);
}

void RealTimeMonitor::stopMonitoring() {
    if (!mIsRunning.exchange(false)) {
        return; // 已经停止
    }
    
    if (mMonitoringThread.joinable()) {
        mMonitoringThread.join();
    }
}

std::vector<MetricData> RealTimeMonitor::getLatestMetrics() {
    std::lock_guard<std::mutex> lock(mMetricsMutex);
    return mLatestMetrics;
}

std::vector<AlertInfo> RealTimeMonitor::getLatestAlerts() {
    std::lock_guard<std::mutex> lock(mAlertsMutex);
    return mLatestAlerts;
}

void RealTimeMonitor::setMetricsCallback(std::function<void(const std::vector<MetricData>&)> callback) {
    mMetricsCallback = callback;
}

void RealTimeMonitor::setAlertCallback(std::function<void(const std::vector<AlertInfo>&)> callback) {
    mAlertCallback = callback;
}

void RealTimeMonitor::monitoringLoop() {
    while (mIsRunning.load()) {
        // 收集指标
        auto metrics = MetricsAggregator::getInstance().collectAllMetrics();
        
        // 分析警报
        auto alerts = AlertAnalyzer::getInstance().analyzeMetrics(metrics);
        
        // 更新最新数据
        {
            std::lock_guard<std::mutex> lock(mMetricsMutex);
            mLatestMetrics = metrics;
        }
        
        {
            std::lock_guard<std::mutex> lock(mAlertsMutex);
            mLatestAlerts = alerts;
        }
        
        // 调用回调
        if (mMetricsCallback) {
            mMetricsCallback(metrics);
        }
        
        if (mAlertCallback) {
            mAlertCallback(alerts);
        }
        
        // 等待下一个周期
        std::this_thread::sleep_for(mInterval);
    }
}

} // namespace task