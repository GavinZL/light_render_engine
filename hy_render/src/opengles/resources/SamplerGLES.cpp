#include "SamplerGLES.hpp"
#include "../../gl_common/GLUtils.hpp"
#include <iostream>

namespace hyengine {
namespace render {

// ============================================================================
// SamplerGLES 实现
// ============================================================================

SamplerGLES::SamplerGLES(const SamplerDesc& desc) : mDesc(desc) {
    std::cout << "[SamplerGLES] 创建采样器 (ES 3.0 - 状态存储在描述符中)" << std::endl;
    
    // 重要提示：OpenGL ES 3.0 不支持 glGenSamplers/glSamplerParameteri API
    // 采样器状态需要通过 glTexParameteri 应用到纹理对象上
    // 
    // 这个类主要用于：
    // 1. 存储采样器描述符
    // 2. 在 TextureGLES::setSampler 中将状态应用到纹理
    
    std::cout << "[SamplerGLES] 注意: 采样器状态将应用到纹理对象" << std::endl;
    std::cout << "[SamplerGLES] Min过滤: " << static_cast<int>(desc.minFilter) << std::endl;
    std::cout << "[SamplerGLES] Mag过滤: " << static_cast<int>(desc.magFilter) << std::endl;
}

void SamplerGLES::bind(uint32_t unit) {
    std::cout << "[SamplerGLES] bind() called for unit " << unit << std::endl;
    // 注意：ES 3.0 没有独立的采样器对象，这个方法主要用于接口一致性
    // 实际的采样器状态应该通过 TextureGLES::setSampler 应用到纹理上
}

ResourceHandle SamplerGLES::getResourceHandle() const {
    // ES 3.0 中没有独立的采样器对象，返回0
    ResourceHandle handle;
    handle.uintHandle = 0;
    return handle;
}

} // namespace render
} // namespace hyengine
