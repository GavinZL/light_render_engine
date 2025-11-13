#pragma once

#include "../../../include/hy_render/debug/HYRenderDebugger.hpp"
#include <unordered_set>

// OpenGL调试回调函数调用约定
#ifndef GLAPIENTRY
#ifdef _WIN32
#define GLAPIENTRY __stdcall
#else
#define GLAPIENTRY
#endif
#endif

namespace hyengine {
namespace render {
namespace debug {

/**
 * @brief OpenGL渲染调试器实现
 * 
 * 提供OpenGL特定的调试功能：
 * 1. OpenGL错误检查 (glGetError)
 * 2. OpenGL调试消息回调 (GL_KHR_debug)
 * 3. GPU性能分析
 * 4. OpenGL状态验证
 */
class RenderDebuggerGL : public HYRenderDebugger {
public:
    RenderDebuggerGL();
    virtual ~RenderDebuggerGL();

    // ==== 调试控制 ====
    void enable() override;
    void disable() override;
    void setDebugLevel(DebugLevel level) override;
    DebugLevel getDebugLevel() const override;

    // ==== GPU调试标记 ====
    void beginGPUMarker(const std::string& name) override;
    void endGPUMarker() override;
    void insertGPUMarker(const std::string& name) override;

    // ==== 性能监控 ====
    void beginFrame() override;
    void endFrame() override;
    const PerformanceCounter& getPerformanceCounter(PerformanceCounterType type) const override;
    void addCustomCounter(const std::string& name, const std::string& unit = "") override;
    void updateCounter(PerformanceCounterType type, double value) override;
    void updateCustomCounter(const std::string& name, double value) override;

    // ==== 资源监控 ====
    void registerResourceCreation(ResourceType type, uint64_t size = 0) override;
    void registerResourceDestruction(ResourceType type, uint64_t size = 0) override;
    const ResourceUsageStats& getResourceUsageStats() const override;

    // ==== 错误检测 ====
    bool checkGPUErrors() override;
    bool validateRenderState() override;
    void reportError(const std::string& message, const std::string& function = "", 
                    const std::string& file = "", int line = 0) override;
    void reportWarning(const std::string& message, const std::string& function = "", 
                      const std::string& file = "", int line = 0) override;

    // ==== 调试输出 ====
    void printPerformanceReport() const override;
    void printResourceReport() const override;
    bool exportDebugData(const std::string& filename) const override;

    // ==== 回调机制 ====
    void setErrorCallback(std::function<void(const std::string&)> callback) override;
    void setWarningCallback(std::function<void(const std::string&)> callback) override;

protected:
    void initializePerformanceCounters() override;

private:
    // OpenGL特定功能
    bool mSupportsDebugOutput = false;
    bool mSupportsGPUMarkers = false;
    
    // 错误追踪
    std::unordered_set<uint32_t> mIgnoredErrors;
    uint32_t mLastGLError = 0;
    
    // 性能查询对象
    std::vector<uint32_t> mGPUTimerQueries;
    uint32_t mCurrentQueryIndex = 0;
    
    // OpenGL调试回调
    static void GLAPIENTRY debugCallback(uint32_t source, uint32_t type, uint32_t id,
                                        uint32_t severity, int length,
                                        const char* message, const void* userParam);
    
    // 内部辅助方法
    void setupDebugOutput();
    void cleanupDebugOutput();
    std::string getGLErrorString(uint32_t error) const;
    std::string getGLDebugSourceString(uint32_t source) const;
    std::string getGLDebugTypeString(uint32_t type) const;
    std::string getGLDebugSeverityString(uint32_t severity) const;
    
    // GPU时间查询
    void initGPUTimerQueries();
    void cleanupGPUTimerQueries();
    double getGPUTime();
};

/**
 * @brief OpenGL ES调试器（继承自OpenGL调试器）
 */
class RenderDebuggerGLES : public RenderDebuggerGL {
public:
    RenderDebuggerGLES() = default;
    virtual ~RenderDebuggerGLES() = default;

    // OpenGL ES可能不支持某些调试功能，需要重写相关方法
    void beginGPUMarker(const std::string& name) override;
    void endGPUMarker() override;
    void insertGPUMarker(const std::string& name) override;

private:
    // OpenGL ES特定的实现差异
    void setupDebugOutputES();
};

} // namespace debug
} // namespace render
} // namespace hyengine