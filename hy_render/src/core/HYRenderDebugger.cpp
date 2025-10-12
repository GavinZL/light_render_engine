#include "../../include/hy_render/debug/HYRenderDebugger.hpp"
#include "../gl_common/debug/RenderDebuggerGL.hpp"
#include "../metal/debug/RenderDebuggerMtl.hpp"
#include <iostream>
#include <sstream>
#include <fstream>
#include <iomanip>

namespace hyengine {
namespace render {
namespace debug {

// 全局调试器实例
std::shared_ptr<HYRenderDebugger> DebuggerFactory::sGlobalDebugger = nullptr;

// ==== HYRenderDebugger 基础实现 ====

std::string HYRenderDebugger::formatDebugMessage(const std::string& message, 
                                                 const std::string& function,
                                                 const std::string& file, 
                                                 int line) const {
    std::ostringstream oss;
    oss << "[HYRender Debug] ";
    
    if (!function.empty()) {
        oss << function << "() ";
    }
    
    if (!file.empty() && line > 0) {
        // 只显示文件名，不显示完整路径
        size_t lastSlash = file.find_last_of("/\\");
        std::string filename = (lastSlash != std::string::npos) ? 
            file.substr(lastSlash + 1) : file;
        oss << "[" << filename << ":" << line << "] ";
    }
    
    oss << message;
    return oss.str();
}

std::shared_ptr<HYRenderDebugger> HYRenderDebugger::create(RenderAPI api) {
    return DebuggerFactory::createDebugger(api);
}

// ==== DebuggerFactory 实现 ====

std::shared_ptr<HYRenderDebugger> DebuggerFactory::getGlobalDebugger() {
    return sGlobalDebugger;
}

void DebuggerFactory::setGlobalDebugger(std::shared_ptr<HYRenderDebugger> debugger) {
    sGlobalDebugger = debugger;
}

std::shared_ptr<HYRenderDebugger> DebuggerFactory::createDebugger(RenderAPI api) {
    switch (api) {
        case RenderAPI::kOpenGL:
            return std::make_shared<RenderDebuggerGL>();
        case RenderAPI::kOpenGLES:
            return std::make_shared<RenderDebuggerGLES>();
        case RenderAPI::kMetal:
            return std::make_shared<RenderDebuggerMtl>();
        default:
            std::cerr << "[DebuggerFactory] Unsupported API: " << static_cast<int>(api) << std::endl;
            return nullptr;
    }
}

} // namespace debug
} // namespace render
} // namespace hyengine