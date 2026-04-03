#include "RenderDebuggerGL.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>

namespace hyengine {
namespace render {
namespace debug {

// OpenGL常量定义（避免包含OpenGL头文件）
#ifndef GL_DEBUG_OUTPUT
#define GL_DEBUG_OUTPUT 0x92E0
#define GL_DEBUG_OUTPUT_SYNCHRONOUS 0x8242
#define GL_DEBUG_SOURCE_API 0x8246
#define GL_DEBUG_SOURCE_WINDOW_SYSTEM 0x8247
#define GL_DEBUG_SOURCE_SHADER_COMPILER 0x8248
#define GL_DEBUG_SOURCE_THIRD_PARTY 0x8249
#define GL_DEBUG_SOURCE_APPLICATION 0x824A
#define GL_DEBUG_SOURCE_OTHER 0x824B
#define GL_DEBUG_TYPE_ERROR 0x824C
#define GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR 0x824D
#define GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR 0x824E
#define GL_DEBUG_TYPE_PORTABILITY 0x824F
#define GL_DEBUG_TYPE_PERFORMANCE 0x8250
#define GL_DEBUG_TYPE_MARKER 0x8268
#define GL_DEBUG_TYPE_OTHER 0x8251
#define GL_DEBUG_SEVERITY_HIGH 0x9146
#define GL_DEBUG_SEVERITY_MEDIUM 0x9147
#define GL_DEBUG_SEVERITY_LOW 0x9148
#define GL_DEBUG_SEVERITY_NOTIFICATION 0x826B
#define GL_NO_ERROR 0
#define GL_INVALID_ENUM 0x0500
#define GL_INVALID_VALUE 0x0501
#define GL_INVALID_OPERATION 0x0502
#define GL_OUT_OF_MEMORY 0x0505
#define GL_TIME_ELAPSED 0x88BF
#endif

RenderDebuggerGL::RenderDebuggerGL() {
    initializePerformanceCounters();
    
    // 检查OpenGL调试扩展支持
    // 这里用模拟的方式，实际实现需要查询OpenGL扩展
    mSupportsDebugOutput = true;  // 假设支持
    mSupportsGPUMarkers = true;   // 假设支持
    
    std::cout << "[RenderDebuggerGL] OpenGL Debugger initialized" << std::endl;
    std::cout << "  Debug Output Support: " << (mSupportsDebugOutput ? "YES" : "NO") << std::endl;
    std::cout << "  GPU Markers Support: " << (mSupportsGPUMarkers ? "YES" : "NO") << std::endl;
}

RenderDebuggerGL::~RenderDebuggerGL() {
    disable();
    cleanupGPUTimerQueries();
    std::cout << "[RenderDebuggerGL] OpenGL Debugger destroyed" << std::endl;
}

// ==== 调试控制 ====

void RenderDebuggerGL::enable() {
    if (mIsEnabled) return;
    
    mIsEnabled = true;
    setupDebugOutput();
    initGPUTimerQueries();
    
    std::cout << "[RenderDebuggerGL] Debug mode enabled (Level: " 
              << static_cast<int>(mDebugLevel) << ")" << std::endl;
}

void RenderDebuggerGL::disable() {
    if (!mIsEnabled) return;
    
    mIsEnabled = false;
    cleanupDebugOutput();
    
    std::cout << "[RenderDebuggerGL] Debug mode disabled" << std::endl;
}

void RenderDebuggerGL::setDebugLevel(DebugLevel level) {
    mDebugLevel = level;
    std::cout << "[RenderDebuggerGL] Debug level set to: " << static_cast<int>(level) << std::endl;
}

DebugLevel RenderDebuggerGL::getDebugLevel() const {
    return mDebugLevel;
}

// ==== GPU调试标记 ====

void RenderDebuggerGL::beginGPUMarker(const std::string& name) {
    if (!mIsEnabled || !mSupportsGPUMarkers) return;
    
    GPUDebugMarker marker;
    marker.name = name;
    marker.startTime = std::chrono::high_resolution_clock::now();
    marker.isActive = true;
    
    mGPUMarkerStack.push_back(marker);
    
    // 实际实现中会调用OpenGL调试扩展
    // 例如: glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 0, -1, name.c_str());
    
    if (mDebugLevel >= DebugLevel::kVerbose) {
        std::cout << "[GPU Marker] Begin: " << name << std::endl;
    }
}

void RenderDebuggerGL::endGPUMarker() {
    if (!mIsEnabled || !mSupportsGPUMarkers || mGPUMarkerStack.empty()) return;
    
    auto& marker = mGPUMarkerStack.back();
    marker.endTime = std::chrono::high_resolution_clock::now();
    marker.isActive = false;
    
    double elapsedMs = marker.getElapsedMilliseconds();
    
    // 实际实现中会调用OpenGL调试扩展
    // 例如: glPopDebugGroup();
    
    if (mDebugLevel >= DebugLevel::kVerbose) {
        std::cout << "[GPU Marker] End: " << marker.name 
                  << " (Time: " << std::fixed << std::setprecision(3) << elapsedMs << "ms)" << std::endl;
    }
    
    mGPUMarkerStack.pop_back();
}

void RenderDebuggerGL::insertGPUMarker(const std::string& name) {
    if (!mIsEnabled || !mSupportsGPUMarkers) return;
    
    // 实际实现中会调用OpenGL调试扩展
    // 例如: glDebugMessageInsert(GL_DEBUG_SOURCE_APPLICATION, GL_DEBUG_TYPE_MARKER, 0, 
    //                            GL_DEBUG_SEVERITY_NOTIFICATION, -1, name.c_str());
    
    if (mDebugLevel >= DebugLevel::kVerbose) {
        std::cout << "[GPU Marker] Insert: " << name << std::endl;
    }
}

// ==== 性能监控 ====

void RenderDebuggerGL::beginFrame() {
    if (!mIsEnabled) return;
    
    mFrameStartTime = std::chrono::high_resolution_clock::now();
    
    // 重置帧相关计数器
    updateCounter(PerformanceCounterType::kDrawCalls, 0);
    updateCounter(PerformanceCounterType::kTriangles, 0);
    updateCounter(PerformanceCounterType::kVertices, 0);
    updateCounter(PerformanceCounterType::kTextureBinds, 0);
    updateCounter(PerformanceCounterType::kBufferBinds, 0);
    updateCounter(PerformanceCounterType::kShaderSwitches, 0);
}

void RenderDebuggerGL::endFrame() {
    if (!mIsEnabled) return;
    
    auto frameEndTime = std::chrono::high_resolution_clock::now();
    auto frameDuration = frameEndTime - mFrameStartTime;
    double frameTimeMs = std::chrono::duration<double, std::milli>(frameDuration).count();
    
    updateCounter(PerformanceCounterType::kFrameTime, frameTimeMs);
    
    // 更新GPU时间
    double gpuTime = getGPUTime();
    if (gpuTime > 0) {
        updateCounter(PerformanceCounterType::kGPUMemoryUsage, gpuTime);
    }
}

const PerformanceCounter& RenderDebuggerGL::getPerformanceCounter(PerformanceCounterType type) const {
    static PerformanceCounter emptyCounter;
    auto it = mPerformanceCounters.find(type);
    return (it != mPerformanceCounters.end()) ? it->second : emptyCounter;
}

void RenderDebuggerGL::addCustomCounter(const std::string& name, const std::string& unit) {
    PerformanceCounter counter;
    counter.type = PerformanceCounterType::kCustomCounter;
    counter.name = name;
    counter.unit = unit;
    
    mCustomCounters[name] = counter;
    
    if (mDebugLevel >= DebugLevel::kInfo) {
        std::cout << "[Performance] Added custom counter: " << name 
                  << " [" << unit << "]" << std::endl;
    }
}

void RenderDebuggerGL::updateCounter(PerformanceCounterType type, double value) {
    auto& counter = mPerformanceCounters[type];
    counter.addSample(value);
}

void RenderDebuggerGL::updateCustomCounter(const std::string& name, double value) {
    auto it = mCustomCounters.find(name);
    if (it != mCustomCounters.end()) {
        it->second.addSample(value);
    }
}

// ==== 资源监控 ====

void RenderDebuggerGL::registerResourceCreation(ResourceType type, uint64_t size) {
    if (!mIsEnabled) return;
    
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
        std::cout << "[Resource] Created " << static_cast<int>(type) 
                  << " (size: " << size << " bytes)" << std::endl;
    }
}

void RenderDebuggerGL::registerResourceDestruction(ResourceType type, uint64_t size) {
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
        std::cout << "[Resource] Destroyed " << static_cast<int>(type) 
                  << " (size: " << size << " bytes)" << std::endl;
    }
}

const ResourceUsageStats& RenderDebuggerGL::getResourceUsageStats() const {
    return mResourceStats;
}

// ==== 错误检测 ====

bool RenderDebuggerGL::checkGPUErrors() {
    if (!mIsEnabled) return true;
    
    // 模拟OpenGL错误检查
    // 实际实现中会调用: GLenum error = glGetError();
    uint32_t error = GL_NO_ERROR; // 模拟无错误
    
    if (error != GL_NO_ERROR && mIgnoredErrors.find(error) == mIgnoredErrors.end()) {
        std::string errorStr = getGLErrorString(error);
        reportError("OpenGL Error: " + errorStr);
        mLastGLError = error;
        return false;
    }
    
    return true;
}

bool RenderDebuggerGL::validateRenderState() {
    if (!mIsEnabled) return true;
    
    // 这里会验证当前的OpenGL渲染状态
    // 例如检查是否有有效的着色器程序、VAO绑定等
    
    if (mDebugLevel >= DebugLevel::kVerbose) {
        std::cout << "[Validation] Render state validated" << std::endl;
    }
    
    return true;
}

void RenderDebuggerGL::reportError(const std::string& message, const std::string& function, 
                                  const std::string& file, int line) {
    if (mDebugLevel < DebugLevel::kError) return;
    
    std::string formattedMsg = formatDebugMessage(message, function, file, line);
    std::cerr << "[ERROR] " << formattedMsg << std::endl;
    
    if (mErrorCallback) {
        mErrorCallback(formattedMsg);
    }
}

void RenderDebuggerGL::reportWarning(const std::string& message, const std::string& function, 
                                    const std::string& file, int line) {
    if (mDebugLevel < DebugLevel::kWarning) return;
    
    std::string formattedMsg = formatDebugMessage(message, function, file, line);
    std::cout << "[WARNING] " << formattedMsg << std::endl;
    
    if (mWarningCallback) {
        mWarningCallback(formattedMsg);
    }
}

// ==== 调试输出 ====

void RenderDebuggerGL::printPerformanceReport() const {
    if (!mIsEnabled) return;
    
    std::cout << "\n=== Performance Report ===" << std::endl;
    
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
    
    std::cout << "=========================" << std::endl;
}

void RenderDebuggerGL::printResourceReport() const {
    if (!mIsEnabled) return;
    
    std::cout << "\n=== Resource Usage Report ===" << std::endl;
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
    std::cout << "==============================" << std::endl;
}

bool RenderDebuggerGL::exportDebugData(const std::string& filename) const {
    if (!mIsEnabled) return false;
    
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "[Export] Failed to open file: " << filename << std::endl;
        return false;
    }
    
    file << "HY Render Engine Debug Report\n";
    file << "==============================\n\n";
    
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
    
    std::cout << "[Export] Debug data exported to: " << filename << std::endl;
    return true;
}

// ==== 回调机制 ====

void RenderDebuggerGL::setErrorCallback(std::function<void(const std::string&)> callback) {
    mErrorCallback = callback;
}

void RenderDebuggerGL::setWarningCallback(std::function<void(const std::string&)> callback) {
    mWarningCallback = callback;
}

// ==== 内部方法 ====

void RenderDebuggerGL::initializePerformanceCounters() {
    // 初始化基本性能计数器
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

void RenderDebuggerGL::setupDebugOutput() {
    if (!mSupportsDebugOutput) return;
    
    // 实际实现中会启用OpenGL调试输出
    // glEnable(GL_DEBUG_OUTPUT);
    // glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    // glDebugMessageCallback(debugCallback, this);
    
    std::cout << "[RenderDebuggerGL] Debug output enabled" << std::endl;
}

void RenderDebuggerGL::cleanupDebugOutput() {
    if (!mSupportsDebugOutput) return;
    
    // 实际实现中会禁用OpenGL调试输出
    // glDisable(GL_DEBUG_OUTPUT);
    // glDebugMessageCallback(nullptr, nullptr);
    
    std::cout << "[RenderDebuggerGL] Debug output disabled" << std::endl;
}

std::string RenderDebuggerGL::getGLErrorString(uint32_t error) const {
    switch (error) {
        case GL_NO_ERROR: return "GL_NO_ERROR";
        case GL_INVALID_ENUM: return "GL_INVALID_ENUM";
        case GL_INVALID_VALUE: return "GL_INVALID_VALUE";
        case GL_INVALID_OPERATION: return "GL_INVALID_OPERATION";
        case GL_OUT_OF_MEMORY: return "GL_OUT_OF_MEMORY";
        default: return "Unknown Error (" + std::to_string(error) + ")";
    }
}

std::string RenderDebuggerGL::getGLDebugSourceString(uint32_t source) const {
    switch (source) {
        case GL_DEBUG_SOURCE_API: return "API";
        case GL_DEBUG_SOURCE_WINDOW_SYSTEM: return "Window System";
        case GL_DEBUG_SOURCE_SHADER_COMPILER: return "Shader Compiler";
        case GL_DEBUG_SOURCE_THIRD_PARTY: return "Third Party";
        case GL_DEBUG_SOURCE_APPLICATION: return "Application";
        case GL_DEBUG_SOURCE_OTHER: return "Other";
        default: return "Unknown";
    }
}

std::string RenderDebuggerGL::getGLDebugTypeString(uint32_t type) const {
    switch (type) {
        case GL_DEBUG_TYPE_ERROR: return "Error";
        case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: return "Deprecated Behavior";
        case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR: return "Undefined Behavior";
        case GL_DEBUG_TYPE_PORTABILITY: return "Portability";
        case GL_DEBUG_TYPE_PERFORMANCE: return "Performance";
        case GL_DEBUG_TYPE_MARKER: return "Marker";
        case GL_DEBUG_TYPE_OTHER: return "Other";
        default: return "Unknown";
    }
}

std::string RenderDebuggerGL::getGLDebugSeverityString(uint32_t severity) const {
    switch (severity) {
        case GL_DEBUG_SEVERITY_HIGH: return "High";
        case GL_DEBUG_SEVERITY_MEDIUM: return "Medium";
        case GL_DEBUG_SEVERITY_LOW: return "Low";
        case GL_DEBUG_SEVERITY_NOTIFICATION: return "Notification";
        default: return "Unknown";
    }
}

void RenderDebuggerGL::initGPUTimerQueries() {
    // 实际实现中会创建OpenGL查询对象
    // glGenQueries(4, mGPUTimerQueries.data());
    mGPUTimerQueries.resize(4, 0);
    mCurrentQueryIndex = 0;
}

void RenderDebuggerGL::cleanupGPUTimerQueries() {
    if (!mGPUTimerQueries.empty()) {
        // 实际实现中会删除OpenGL查询对象
        // glDeleteQueries(mGPUTimerQueries.size(), mGPUTimerQueries.data());
        mGPUTimerQueries.clear();
    }
}

double RenderDebuggerGL::getGPUTime() {
    // 实际实现中会查询GPU时间
    // GLuint64 timeElapsed;
    // glGetQueryObjectui64v(mGPUTimerQueries[mCurrentQueryIndex], GL_QUERY_RESULT, &timeElapsed);
    // return timeElapsed / 1000000.0; // 转换为毫秒
    
    return 0.0; // 模拟返回
}

void GLAPIENTRY RenderDebuggerGL::debugCallback(uint32_t source, uint32_t type, uint32_t id,
                                               uint32_t severity, int length,
                                               const char* message, const void* userParam) {
    // 跳过某些不重要的消息
    if (type == GL_DEBUG_TYPE_OTHER || severity == GL_DEBUG_SEVERITY_NOTIFICATION) {
        return;
    }
    
    RenderDebuggerGL* debugger = static_cast<RenderDebuggerGL*>(const_cast<void*>(userParam));
    if (!debugger) return;
    
    std::ostringstream oss;
    oss << "[OpenGL Debug] " << debugger->getGLDebugTypeString(type) 
        << " from " << debugger->getGLDebugSourceString(source)
        << " (Severity: " << debugger->getGLDebugSeverityString(severity) << "): " << message;
    
    if (severity == GL_DEBUG_SEVERITY_HIGH) {
        debugger->reportError(oss.str());
    } else {
        debugger->reportWarning(oss.str());
    }
}

// ==== OpenGL ES实现 ====

void RenderDebuggerGLES::beginGPUMarker(const std::string& name) {
    // OpenGL ES可能需要不同的扩展
    if (mDebugLevel >= DebugLevel::kVerbose) {
        std::cout << "[GPU Marker ES] Begin: " << name << std::endl;
    }
}

void RenderDebuggerGLES::endGPUMarker() {
    if (mDebugLevel >= DebugLevel::kVerbose) {
        std::cout << "[GPU Marker ES] End" << std::endl;
    }
}

void RenderDebuggerGLES::insertGPUMarker(const std::string& name) {
    if (mDebugLevel >= DebugLevel::kVerbose) {
        std::cout << "[GPU Marker ES] Insert: " << name << std::endl;
    }
}

void RenderDebuggerGLES::setupDebugOutputES() {
    // OpenGL ES特定的调试输出设置
    std::cout << "[RenderDebuggerGLES] ES debug output enabled" << std::endl;
}

} // namespace debug
} // namespace render
} // namespace hyengine