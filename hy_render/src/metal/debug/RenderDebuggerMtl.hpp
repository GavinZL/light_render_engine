#pragma once

#include "../../../include/hy_render/debug/HYRenderDebugger.hpp"

namespace hyengine {
namespace render {
namespace debug {

/**
 * @brief Metal渲染调试器实现
 * 
 * 提供Metal特定的调试功能：
 * 1. Metal性能着色器 (GPU Frame Capture)
 * 2. Metal调试层验证
 * 3. Metal GPU调试标记
 * 4. Metal内存使用统计
 */
class RenderDebuggerMtl : public HYRenderDebugger {
public:
    RenderDebuggerMtl();
    virtual ~RenderDebuggerMtl();

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
    // Metal特定功能
    void* mMtlDevice = nullptr;           // id<MTLDevice>
    void* mMtlCommandQueue = nullptr;     // id<MTLCommandQueue>
    void* mMtlCaptureManager = nullptr;   // MTLCaptureManager
    
    bool mSupportsGPUCapture = false;
    bool mSupportsMetalValidation = false;
    
    // Metal性能计数器
    void* mGPUCounterSampleBuffer = nullptr;  // id<MTLCounterSampleBuffer>
    
    // 内部辅助方法
    void setupMetalValidation();
    void setupGPUCapture();
    void cleanupMetal();
    
    // Metal特定的GPU时间测量
    double getMTLGPUTime();
    void initMTLCounters();
    void cleanupMTLCounters();
};

} // namespace debug
} // namespace render
} // namespace hyengine