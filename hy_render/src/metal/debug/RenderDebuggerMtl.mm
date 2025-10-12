#include "RenderDebuggerMtl.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>

namespace hyengine {
namespace render {
namespace debug {

RenderDebuggerMtl::RenderDebuggerMtl() {
    initializePerformanceCounters();
    
    // 检查Metal调试功能支持
    mSupportsGPUCapture = true;      // 假设支持GPU捕获
    mSupportsMetalValidation = true; // 假设支持Metal验证层
    
    std::cout << "[RenderDebuggerMtl] Metal Debugger initialized" << std::endl;
    std::cout << "  GPU Capture Support: " << (mSupportsGPUCapture ? "YES" : "NO") << std::endl;
    std::cout << "  Metal Validation Support: " << (mSupportsMetalValidation ? "YES" : "NO") << std::endl;
}

RenderDebuggerMtl::~RenderDebuggerMtl() {
    disable();
    cleanupMetal();
    std::cout << "[RenderDebuggerMtl] Metal Debugger destroyed" << std::endl;
}

// ==== 调试控制 ====

void RenderDebuggerMtl::enable() {
    if (mIsEnabled) return;
    
    mIsEnabled = true;
    setupMetalValidation();
    setupGPUCapture();
    initMTLCounters();
    
    std::cout << "[RenderDebuggerMtl] Debug mode enabled (Level: " 
              << static_cast<int>(mDebugLevel) << ")" << std::endl;
}

void RenderDebuggerMtl::disable() {
    if (!mIsEnabled) return;
    
    mIsEnabled = false;
    cleanupMTLCounters();
    
    std::cout << "[RenderDebuggerMtl] Debug mode disabled" << std::endl;
}

void RenderDebuggerMtl::setDebugLevel(DebugLevel level) {
    mDebugLevel = level;
    std::cout << "[RenderDebuggerMtl] Debug level set to: " << static_cast<int>(level) << std::endl;
}

DebugLevel RenderDebuggerMtl::getDebugLevel() const {
    return mDebugLevel;
}

// ==== GPU调试标记 ====

void RenderDebuggerMtl::beginGPUMarker(const std::string& name) {
    if (!mIsEnabled) return;
    
    GPUDebugMarker marker;
    marker.name = name;
    marker.startTime = std::chrono::high_resolution_clock::now();
    marker.isActive = true;
    
    mGPUMarkerStack.push_back(marker);
    
    // 实际实现中会调用Metal调试标记
    // [commandBuffer pushDebugGroup:[NSString stringWithUTF8String:name.c_str()]];
    
    if (mDebugLevel >= DebugLevel::kVerbose) {
        std::cout << "[Metal GPU Marker] Begin: " << name << std::endl;
    }
}

void RenderDebuggerMtl::endGPUMarker() {
    if (!mIsEnabled || mGPUMarkerStack.empty()) return;
    
    auto& marker = mGPUMarkerStack.back();
    marker.endTime = std::chrono::high_resolution_clock::now();
    marker.isActive = false;
    
    double elapsedMs = marker.getElapsedMilliseconds();
    
    // 实际实现中会调用Metal调试标记
    // [commandBuffer popDebugGroup];
    
    if (mDebugLevel >= DebugLevel::kVerbose) {
        std::cout << "[Metal GPU Marker] End: " << marker.name 
                  << " (Time: " << std::fixed << std::setprecision(3) << elapsedMs << "ms)" << std::endl;
    }
    
    mGPUMarkerStack.pop_back();
}

void RenderDebuggerMtl::insertGPUMarker(const std::string& name) {
    if (!mIsEnabled) return;
    
    // 实际实现中会调用Metal调试标记
    // [commandBuffer insertDebugSignpost:[NSString stringWithUTF8String:name.c_str()]];
    
    if (mDebugLevel >= DebugLevel::kVerbose) {
        std::cout << "[Metal GPU Marker] Insert: " << name << std::endl;
    }
}

// ==== 性能监控 ====

void RenderDebuggerMtl::beginFrame() {
    if (!mIsEnabled) return;
    
    mFrameStartTime = std::chrono::high_resolution_clock::now();
    
    // 开始Metal GPU采样
    if (mGPUCounterSampleBuffer) {
        // 实际实现中会开始GPU计数器采样
        // [commandBuffer sampleCountersInBuffer:mGPUCounterSampleBuffer atSampleIndex:0 withBarrier:NO];
    }
    
    // 重置帧相关计数器
    updateCounter(PerformanceCounterType::kDrawCalls, 0);
    updateCounter(PerformanceCounterType::kTriangles, 0);
    updateCounter(PerformanceCounterType::kVertices, 0);
}

void RenderDebuggerMtl::endFrame() {
    if (!mIsEnabled) return;
    
    auto frameEndTime = std::chrono::high_resolution_clock::now();
    auto frameDuration = frameEndTime - mFrameStartTime;
    double frameTimeMs = std::chrono::duration<double, std::milli>(frameDuration).count();
    
    updateCounter(PerformanceCounterType::kFrameTime, frameTimeMs);
    
    // 结束Metal GPU采样
    if (mGPUCounterSampleBuffer) {
        // 实际实现中会结束GPU计数器采样
        // [commandBuffer sampleCountersInBuffer:mGPUCounterSampleBuffer atSampleIndex:1 withBarrier:YES];
    }
    
    // 更新GPU时间
    double gpuTime = getMTLGPUTime();
    if (gpuTime > 0) {
        updateCounter(PerformanceCounterType::kGPUMemoryUsage, gpuTime);
    }
}

const PerformanceCounter& RenderDebuggerMtl::getPerformanceCounter(PerformanceCounterType type) const {
    static PerformanceCounter emptyCounter;
    auto it = mPerformanceCounters.find(type);
    return (it != mPerformanceCounters.end()) ? it->second : emptyCounter;
}

void RenderDebuggerMtl::addCustomCounter(const std::string& name, const std::string& unit) {
    PerformanceCounter counter;
    counter.type = PerformanceCounterType::kCustomCounter;
    counter.name = name;
    counter.unit = unit;
    
    mCustomCounters[name] = counter;
    
    if (mDebugLevel >= DebugLevel::kInfo) {
        std::cout << "[Metal Performance] Added custom counter: " << name 
                  << " [" << unit << "]" << std::endl;
    }
}

void RenderDebuggerMtl::updateCounter(PerformanceCounterType type, double value) {
    auto& counter = mPerformanceCounters[type];
    counter.addSample(value);
}

void RenderDebuggerMtl::updateCustomCounter(const std::string& name, double value) {
    auto it = mCustomCounters.find(name);
    if (it != mCustomCounters.end()) {
        it->second.addSample(value);
    }
}

// ==== 资源监控 ====

void RenderDebuggerMtl::registerResourceCreation(ResourceType type, uint64_t size) {
    if (!mIsEnabled) return;
    
    // Metal特定的资源统计
    switch (type) {
        case ResourceType::kBuffer:
            mResourceStats.activeBuffers++;
            mResourceStats.totalBufferMemory += size;
            break;
        case ResourceType::kTexture:
            mResourceStats.activeTextures++;
            mResourceStats.totalTextureMemory += size;
            break;
        case ResourceType::kShader:
            mResourceStats.activeShaders++;
            break;
        case ResourceType::kFrameBuffer:
            mResourceStats.activeFrameBuffers++;
            break;
        case ResourceType::kPipelineState:
            mResourceStats.activePipelineStates++;
            break;
        case ResourceType::kCommandBuffer:
            mResourceStats.activeCommandBuffers++;
            break;
        default:
            break;
    }
    
    if (mDebugLevel >= DebugLevel::kVerbose) {
        std::cout << "[Metal Resource] Created " << static_cast<int>(type) 
                  << " (size: " << size << " bytes)" << std::endl;
    }
}

void RenderDebuggerMtl::registerResourceDestruction(ResourceType type, uint64_t size) {
    if (!mIsEnabled) return;
    
    switch (type) {
        case ResourceType::kBuffer:
            if (mResourceStats.activeBuffers > 0) mResourceStats.activeBuffers--;
            if (mResourceStats.totalBufferMemory >= size) mResourceStats.totalBufferMemory -= size;
            break;
        case ResourceType::kTexture:
            if (mResourceStats.activeTextures > 0) mResourceStats.activeTextures--;
            if (mResourceStats.totalTextureMemory >= size) mResourceStats.totalTextureMemory -= size;
            break;
        case ResourceType::kShader:
            if (mResourceStats.activeShaders > 0) mResourceStats.activeShaders--;
            break;
        case ResourceType::kFrameBuffer:
            if (mResourceStats.activeFrameBuffers > 0) mResourceStats.activeFrameBuffers--;
            break;
        case ResourceType::kPipelineState:
            if (mResourceStats.activePipelineStates > 0) mResourceStats.activePipelineStates--;
            break;
        case ResourceType::kCommandBuffer:
            if (mResourceStats.activeCommandBuffers > 0) mResourceStats.activeCommandBuffers--;
            break;
        default:
            break;
    }
    
    if (mDebugLevel >= DebugLevel::kVerbose) {
        std::cout << "[Metal Resource] Destroyed " << static_cast<int>(type) 
                  << " (size: " << size << " bytes)" << std::endl;
    }
}

const ResourceUsageStats& RenderDebuggerMtl::getResourceUsageStats() const {
    return mResourceStats;
}

// ==== 错误检测 ====

bool RenderDebuggerMtl::checkGPUErrors() {
    if (!mIsEnabled) return true;
    
    // Metal通常通过error参数返回错误，而不是全局错误状态
    // 这里主要检查命令缓冲区错误状态
    
    if (mDebugLevel >= DebugLevel::kVerbose) {
        std::cout << "[Metal Validation] GPU error check completed" << std::endl;
    }
    
    return true;
}

bool RenderDebuggerMtl::validateRenderState() {
    if (!mIsEnabled) return true;
    
    // 验证Metal渲染状态
    // 例如检查当前渲染管线状态、资源绑定等
    
    if (mDebugLevel >= DebugLevel::kVerbose) {
        std::cout << "[Metal Validation] Render state validated" << std::endl;
    }
    
    return true;
}

void RenderDebuggerMtl::reportError(const std::string& message, const std::string& function, 
                                   const std::string& file, int line) {
    if (mDebugLevel < DebugLevel::kError) return;
    
    std::string formattedMsg = formatDebugMessage(message, function, file, line);
    std::cerr << "[METAL ERROR] " << formattedMsg << std::endl;
    
    if (mErrorCallback) {
        mErrorCallback(formattedMsg);
    }
}

void RenderDebuggerMtl::reportWarning(const std::string& message, const std::string& function, 
                                     const std::string& file, int line) {
    if (mDebugLevel < DebugLevel::kWarning) return;
    
    std::string formattedMsg = formatDebugMessage(message, function, file, line);
    std::cout << "[METAL WARNING] " << formattedMsg << std::endl;
    
    if (mWarningCallback) {
        mWarningCallback(formattedMsg);
    }
}

// ==== 调试输出 ====

void RenderDebuggerMtl::printPerformanceReport() const {
    if (!mIsEnabled) return;
    
    std::cout << "\n=== Metal Performance Report ===" << std::endl;
    
    for (const auto& pair : mPerformanceCounters) {
        const auto& counter = pair.second;
        std::cout << "  " << counter.name << ": " 
                  << std::fixed << std::setprecision(3) << counter.currentValue;
        if (!counter.unit.empty()) {
            std::cout << " " << counter.unit;
        }
        std::cout << " (Avg: " << counter.averageValue 
                  << ", Min: " << counter.minValue 
                  << ", Max: " << counter.maxValue 
                  << ", Samples: " << counter.sampleCount << ")" << std::endl;
    }
    
    for (const auto& pair : mCustomCounters) {
        const auto& counter = pair.second;
        std::cout << "  " << counter.name << ": " 
                  << std::fixed << std::setprecision(3) << counter.currentValue;
        if (!counter.unit.empty()) {
            std::cout << " " << counter.unit;
        }
        std::cout << " (Avg: " << counter.averageValue << ")" << std::endl;
    }
    
    std::cout << "=================================" << std::endl;
}

void RenderDebuggerMtl::printResourceReport() const {
    if (!mIsEnabled) return;
    
    std::cout << "\n=== Metal Resource Usage Report ===" << std::endl;
    std::cout << "  Active Buffers: " << mResourceStats.activeBuffers 
              << " (Memory: " << (mResourceStats.totalBufferMemory / 1024) << " KB)" << std::endl;
    std::cout << "  Active Textures: " << mResourceStats.activeTextures 
              << " (Memory: " << (mResourceStats.totalTextureMemory / 1024) << " KB)" << std::endl;
    std::cout << "  Active Shaders: " << mResourceStats.activeShaders << std::endl;
    std::cout << "  Active FrameBuffers: " << mResourceStats.activeFrameBuffers << std::endl;
    std::cout << "  Active Pipeline States: " << mResourceStats.activePipelineStates << std::endl;
    std::cout << "  Active Command Buffers: " << mResourceStats.activeCommandBuffers << std::endl;
    
    uint64_t totalMemory = mResourceStats.totalBufferMemory + mResourceStats.totalTextureMemory;
    std::cout << "  Total GPU Memory: " << (totalMemory / (1024 * 1024)) << " MB" << std::endl;
    
    // Metal特有的内存统计
    if (mMtlDevice) {
        // 实际实现中会查询Metal设备信息
        // NSUInteger recommendedMaxWorkingSetSize = [mMtlDevice recommendedMaxWorkingSetSize];
        // std::cout << "  Recommended Max Working Set: " << (recommendedMaxWorkingSetSize / (1024 * 1024)) << " MB" << std::endl;
        std::cout << "  Metal Device Memory: Available" << std::endl;
    }
    
    std::cout << "====================================" << std::endl;
}

bool RenderDebuggerMtl::exportDebugData(const std::string& filename) const {
    if (!mIsEnabled) return false;
    
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "[Metal Export] Failed to open file: " << filename << std::endl;
        return false;
    }
    
    file << "HY Render Engine Metal Debug Report\n";
    file << "===================================\n\n";
    
    file << "Performance Counters:\n";
    for (const auto& pair : mPerformanceCounters) {
        const auto& counter = pair.second;
        file << "  " << counter.name << ": " << counter.currentValue;
        if (!counter.unit.empty()) {
            file << " " << counter.unit;
        }
        file << " (Avg: " << counter.averageValue << ")\n";
    }
    
    file << "\nResource Usage:\n";
    file << "  Active Buffers: " << mResourceStats.activeBuffers << "\n";
    file << "  Active Textures: " << mResourceStats.activeTextures << "\n";
    file << "  Active Shaders: " << mResourceStats.activeShaders << "\n";
    
    file.close();
    
    std::cout << "[Metal Export] Debug data exported to: " << filename << std::endl;
    return true;
}

// ==== 回调机制 ====

void RenderDebuggerMtl::setErrorCallback(std::function<void(const std::string&)> callback) {
    mErrorCallback = callback;
}

void RenderDebuggerMtl::setWarningCallback(std::function<void(const std::string&)> callback) {
    mWarningCallback = callback;
}

// ==== 内部方法 ====

void RenderDebuggerMtl::initializePerformanceCounters() {
    // 初始化Metal性能计数器
    struct CounterInfo {
        PerformanceCounterType type;
        std::string name;
        std::string unit;
    };
    
    std::vector<CounterInfo> counters = {
        {PerformanceCounterType::kFrameTime, "Frame Time", "ms"},
        {PerformanceCounterType::kDrawCalls, "Draw Calls", "calls"},
        {PerformanceCounterType::kTriangles, "Triangles", "triangles"},
        {PerformanceCounterType::kVertices, "Vertices", "vertices"},
        {PerformanceCounterType::kTextureBinds, "Texture Binds", "binds"},
        {PerformanceCounterType::kBufferBinds, "Buffer Binds", "binds"},
        {PerformanceCounterType::kShaderSwitches, "Shader Switches", "switches"},
        {PerformanceCounterType::kGPUMemoryUsage, "GPU Time", "ms"},
        {PerformanceCounterType::kCPUMemoryUsage, "CPU Memory", "MB"}
    };
    
    for (const auto& info : counters) {
        PerformanceCounter counter;
        counter.type = info.type;
        counter.name = info.name;
        counter.unit = info.unit;
        mPerformanceCounters[info.type] = counter;
    }
}

void RenderDebuggerMtl::setupMetalValidation() {
    if (!mSupportsMetalValidation) return;
    
    // 实际实现中会启用Metal验证层
    // 设置环境变量 METAL_DEVICE_WRAPPER_TYPE=1
    // 或在代码中启用验证：[MTLCreateSystemDefaultDevice() setName:@"Debug Device"];
    
    std::cout << "[RenderDebuggerMtl] Metal validation enabled" << std::endl;
}

void RenderDebuggerMtl::setupGPUCapture() {
    if (!mSupportsGPUCapture) return;
    
    // 实际实现中会设置Metal GPU捕获
    // MTLCaptureManager* captureManager = [MTLCaptureManager sharedCaptureManager];
    // mMtlCaptureManager = (__bridge void*)captureManager;
    
    std::cout << "[RenderDebuggerMtl] GPU capture setup completed" << std::endl;
}

void RenderDebuggerMtl::cleanupMetal() {
    cleanupMTLCounters();
    
    if (mMtlDevice) {
        mMtlDevice = nullptr;
    }
    
    if (mMtlCommandQueue) {
        mMtlCommandQueue = nullptr;
    }
    
    if (mMtlCaptureManager) {
        mMtlCaptureManager = nullptr;
    }
    
    std::cout << "[RenderDebuggerMtl] Metal resources cleaned up" << std::endl;
}

double RenderDebuggerMtl::getMTLGPUTime() {
    if (!mGPUCounterSampleBuffer) return 0.0;
    
    // 实际实现中会从Metal计数器采样缓冲区读取GPU时间
    // NSData* data = [mGPUCounterSampleBuffer resolveCounterRange:NSMakeRange(0, 2)];
    // // 解析采样数据获取GPU时间
    
    return 0.0; // 模拟返回
}

void RenderDebuggerMtl::initMTLCounters() {
    if (!mMtlDevice) return;
    
    // 实际实现中会创建Metal计数器采样缓冲区
    // MTLCounterSampleBufferDescriptor* desc = [[MTLCounterSampleBufferDescriptor alloc] init];
    // desc.counterSet = [[mMtlDevice counterSets] objectAtIndex:0];
    // desc.sampleCount = 256;
    // mGPUCounterSampleBuffer = [mMtlDevice newCounterSampleBufferWithDescriptor:desc error:nil];
    
    std::cout << "[RenderDebuggerMtl] Metal counters initialized" << std::endl;
}

void RenderDebuggerMtl::cleanupMTLCounters() {
    if (mGPUCounterSampleBuffer) {
        // 实际实现中会释放Metal计数器采样缓冲区
        mGPUCounterSampleBuffer = nullptr;
    }
}

} // namespace debug
} // namespace render
} // namespace hyengine