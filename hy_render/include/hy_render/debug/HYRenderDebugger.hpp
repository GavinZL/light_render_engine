#pragma once

#include "../HYRenderDef.hpp"
#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include <chrono>
#include <functional>

namespace hyengine {
namespace render {
namespace debug {

/**
 * @brief 调试级别枚举
 */
enum class DebugLevel : uint8_t {
    kOff = 0,         // 关闭调试
    kError,           // 仅错误信息
    kWarning,         // 错误和警告
    kInfo,            // 信息级别
    kVerbose,         // 详细信息
    kAll              // 所有调试信息
};

/**
 * @brief 性能计数器类型
 */
enum class PerformanceCounterType : uint8_t {
    kFrameTime = 0,   // 帧时间
    kDrawCalls,       // 绘制调用次数
    kTriangles,       // 三角形数量
    kVertices,        // 顶点数量
    kTextureBinds,    // 纹理绑定次数
    kBufferBinds,     // 缓冲区绑定次数
    kShaderSwitches,  // 着色器切换次数
    kGPUMemoryUsage,  // GPU内存使用
    kCPUMemoryUsage,  // CPU内存使用
    kCustomCounter    // 自定义计数器
};

/**
 * @brief 性能计数器数据
 */
struct PerformanceCounter {
    PerformanceCounterType type;
    std::string name;
    double currentValue = 0.0;
    double averageValue = 0.0;
    double minValue = 0.0;
    double maxValue = 0.0;
    uint64_t sampleCount = 0;
    std::string unit;
    
    void reset() {
        currentValue = averageValue = minValue = maxValue = 0.0;
        sampleCount = 0;
    }
    
    void addSample(double value) {
        currentValue = value;
        if (sampleCount == 0) {
            minValue = maxValue = averageValue = value;
        } else {
            minValue = std::min(minValue, value);
            maxValue = std::max(maxValue, value);
            averageValue = (averageValue * sampleCount + value) / (sampleCount + 1);
        }
        sampleCount++;
    }
};

/**
 * @brief GPU调试标记
 */
struct GPUDebugMarker {
    std::string name;
    std::chrono::high_resolution_clock::time_point startTime;
    std::chrono::high_resolution_clock::time_point endTime;
    bool isActive = false;
    
    double getElapsedMilliseconds() const {
        auto duration = endTime - startTime;
        return std::chrono::duration<double, std::milli>(duration).count();
    }
};

/**
 * @brief 资源使用统计
 */
struct ResourceUsageStats {
    // 缓冲区统计
    uint32_t activeBuffers = 0;
    uint64_t totalBufferMemory = 0;
    
    // 纹理统计
    uint32_t activeTextures = 0;
    uint64_t totalTextureMemory = 0;
    
    // 着色器统计
    uint32_t activeShaders = 0;
    
    // 帧缓冲统计
    uint32_t activeFrameBuffers = 0;
    
    // 管线状态统计
    uint32_t activePipelineStates = 0;
    
    // 命令缓冲区统计
    uint32_t activeCommandBuffers = 0;
    
    void reset() {
        activeBuffers = activeTextures = activeShaders = 0;
        activeFrameBuffers = activePipelineStates = activeCommandBuffers = 0;
        totalBufferMemory = totalTextureMemory = 0;
    }
};

/**
 * @brief 渲染调试器抽象基类
 * 
 * 提供跨平台的渲染调试功能，包括：
 * 1. GPU调试标记和性能分析
 * 2. 资源使用监控
 * 3. 渲染状态验证
 * 4. 错误检测和报告
 */
class HYRenderDebugger {
public:
    HYRenderDebugger() = default;
    virtual ~HYRenderDebugger() = default;

    // ==== 调试控制 ====
    /**
     * @brief 启用调试功能
     */
    virtual void enable() = 0;

    /**
     * @brief 禁用调试功能
     */
    virtual void disable() = 0;

    /**
     * @brief 设置调试级别
     */
    virtual void setDebugLevel(DebugLevel level) = 0;

    /**
     * @brief 获取调试级别
     */
    virtual DebugLevel getDebugLevel() const = 0;

    // ==== GPU调试标记 ====
    /**
     * @brief 开始GPU调试标记
     */
    virtual void beginGPUMarker(const std::string& name) = 0;

    /**
     * @brief 结束GPU调试标记
     */
    virtual void endGPUMarker() = 0;

    /**
     * @brief 插入GPU调试标记
     */
    virtual void insertGPUMarker(const std::string& name) = 0;

    // ==== 性能监控 ====
    /**
     * @brief 开始帧性能分析
     */
    virtual void beginFrame() = 0;

    /**
     * @brief 结束帧性能分析
     */
    virtual void endFrame() = 0;

    /**
     * @brief 获取性能计数器
     */
    virtual const PerformanceCounter& getPerformanceCounter(PerformanceCounterType type) const = 0;

    /**
     * @brief 添加自定义性能计数器
     */
    virtual void addCustomCounter(const std::string& name, const std::string& unit = "") = 0;

    /**
     * @brief 更新性能计数器
     */
    virtual void updateCounter(PerformanceCounterType type, double value) = 0;

    /**
     * @brief 更新自定义计数器
     */
    virtual void updateCustomCounter(const std::string& name, double value) = 0;

    // ==== 资源监控 ====
    /**
     * @brief 注册资源创建
     */
    virtual void registerResourceCreation(ResourceType type, uint64_t size = 0) = 0;

    /**
     * @brief 注册资源销毁
     */
    virtual void registerResourceDestruction(ResourceType type, uint64_t size = 0) = 0;

    /**
     * @brief 获取资源使用统计
     */
    virtual const ResourceUsageStats& getResourceUsageStats() const = 0;

    // ==== 错误检测 ====
    /**
     * @brief 检查GPU错误
     */
    virtual bool checkGPUErrors() = 0;

    /**
     * @brief 验证渲染状态
     */
    virtual bool validateRenderState() = 0;

    /**
     * @brief 报告错误
     */
    virtual void reportError(const std::string& message, const std::string& function = "", 
                           const std::string& file = "", int line = 0) = 0;

    /**
     * @brief 报告警告
     */
    virtual void reportWarning(const std::string& message, const std::string& function = "", 
                             const std::string& file = "", int line = 0) = 0;

    // ==== 调试输出 ====
    /**
     * @brief 打印性能报告
     */
    virtual void printPerformanceReport() const = 0;

    /**
     * @brief 打印资源使用报告
     */
    virtual void printResourceReport() const = 0;

    /**
     * @brief 导出调试数据到文件
     */
    virtual bool exportDebugData(const std::string& filename) const = 0;

    // ==== 回调机制 ====
    /**
     * @brief 设置错误回调
     */
    virtual void setErrorCallback(std::function<void(const std::string&)> callback) = 0;

    /**
     * @brief 设置警告回调
     */
    virtual void setWarningCallback(std::function<void(const std::string&)> callback) = 0;

    // ==== 工厂方法 ====
    /**
     * @brief 创建平台相关的调试器实例
     */
    static std::shared_ptr<HYRenderDebugger> create(RenderAPI api);

protected:
    DebugLevel mDebugLevel = DebugLevel::kInfo;
    bool mIsEnabled = false;
    
    // 性能计数器
    std::unordered_map<PerformanceCounterType, PerformanceCounter> mPerformanceCounters;
    std::unordered_map<std::string, PerformanceCounter> mCustomCounters;
    
    // 资源统计
    ResourceUsageStats mResourceStats;
    
    // GPU标记栈
    std::vector<GPUDebugMarker> mGPUMarkerStack;
    
    // 回调函数
    std::function<void(const std::string&)> mErrorCallback;
    std::function<void(const std::string&)> mWarningCallback;
    
    // 帧时间测量
    std::chrono::high_resolution_clock::time_point mFrameStartTime;
    
    // ==== 内部辅助方法 ====
    /**
     * @brief 初始化性能计数器
     */
    virtual void initializePerformanceCounters() = 0;

    /**
     * @brief 格式化调试消息
     */
    std::string formatDebugMessage(const std::string& message, const std::string& function,
                                  const std::string& file, int line) const;
};

/**
 * @brief 调试工具工厂类
 */
class DebuggerFactory {
public:
    /**
     * @brief 创建指定API的调试器
     */
    static std::shared_ptr<HYRenderDebugger> createDebugger(RenderAPI api);

    /**
     * @brief 获取全局调试器实例
     */
    static std::shared_ptr<HYRenderDebugger> getGlobalDebugger();

    /**
     * @brief 设置全局调试器实例
     */
    static void setGlobalDebugger(std::shared_ptr<HYRenderDebugger> debugger);

private:
    static std::shared_ptr<HYRenderDebugger> sGlobalDebugger;
};

// ==== 便捷宏定义 ====
#ifdef HY_RENDER_DEBUG
    #define HY_DEBUG_MARKER_BEGIN(name) \
        do { \
            auto debugger = DebuggerFactory::getGlobalDebugger(); \
            if (debugger) debugger->beginGPUMarker(name); \
        } while(0)

    #define HY_DEBUG_MARKER_END() \
        do { \
            auto debugger = DebuggerFactory::getGlobalDebugger(); \
            if (debugger) debugger->endGPUMarker(); \
        } while(0)

    #define HY_DEBUG_MARKER_INSERT(name) \
        do { \
            auto debugger = DebuggerFactory::getGlobalDebugger(); \
            if (debugger) debugger->insertGPUMarker(name); \
        } while(0)

    #define HY_DEBUG_COUNTER_UPDATE(type, value) \
        do { \
            auto debugger = DebuggerFactory::getGlobalDebugger(); \
            if (debugger) debugger->updateCounter(type, value); \
        } while(0)

    #define HY_DEBUG_RESOURCE_CREATED(type, size) \
        do { \
            auto debugger = DebuggerFactory::getGlobalDebugger(); \
            if (debugger) debugger->registerResourceCreation(type, size); \
        } while(0)

    #define HY_DEBUG_RESOURCE_DESTROYED(type, size) \
        do { \
            auto debugger = DebuggerFactory::getGlobalDebugger(); \
            if (debugger) debugger->registerResourceDestruction(type, size); \
        } while(0)

    #define HY_DEBUG_CHECK_ERRORS() \
        do { \
            auto debugger = DebuggerFactory::getGlobalDebugger(); \
            if (debugger) debugger->checkGPUErrors(); \
        } while(0)

    #define HY_DEBUG_REPORT_ERROR(msg) \
        do { \
            auto debugger = DebuggerFactory::getGlobalDebugger(); \
            if (debugger) debugger->reportError(msg, __FUNCTION__, __FILE__, __LINE__); \
        } while(0)

    #define HY_DEBUG_REPORT_WARNING(msg) \
        do { \
            auto debugger = DebuggerFactory::getGlobalDebugger(); \
            if (debugger) debugger->reportWarning(msg, __FUNCTION__, __FILE__, __LINE__); \
        } while(0)
#else
    #define HY_DEBUG_MARKER_BEGIN(name) do {} while(0)
    #define HY_DEBUG_MARKER_END() do {} while(0)
    #define HY_DEBUG_MARKER_INSERT(name) do {} while(0)
    #define HY_DEBUG_COUNTER_UPDATE(type, value) do {} while(0)
    #define HY_DEBUG_RESOURCE_CREATED(type, size) do {} while(0)
    #define HY_DEBUG_RESOURCE_DESTROYED(type, size) do {} while(0)
    #define HY_DEBUG_CHECK_ERRORS() do {} while(0)
    #define HY_DEBUG_REPORT_ERROR(msg) do {} while(0)
    #define HY_DEBUG_REPORT_WARNING(msg) do {} while(0)
#endif

} // namespace debug
} // namespace render
} // namespace hyengine