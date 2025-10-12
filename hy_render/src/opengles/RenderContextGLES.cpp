#include "RenderContextGLES.hpp"
#include "resources/BufferGLES.hpp"
#include "resources/TextureGLES.hpp"
#include "resources/ShaderGLES.hpp"
#include "resources/SamplerGLES.hpp"
#include "resources/FrameBufferGLES.hpp"
#include "resources/PipelineStateGLES.hpp"
#include "resources/RenderPassGLES.hpp"
#include "resources/FenceGLES.hpp"
#include "resources/VertexArrayGLES.hpp"
#include <iostream>

namespace hyengine {
namespace render {

RenderContextGLES::RenderContextGLES() {
    std::cout << "[RenderContextGLES] 初始化OpenGL ES上下文" << std::endl;
    
    // 初始化GL函数
    initializeGLFunctions();
    
    // 检测扩展
    detectExtensions();
    
    // 检测VAO支持
    // TODO: 实际实现中需要检查GL_OES_vertex_array_object扩展
    if (mMajorVersion >= 3) {
        // OpenGL ES 3.0+ 原生支持VAO
        mVAOSupported = true;
        std::cout << "[RenderContextGLES] VAO支持: 是 (ES 3.0+)" << std::endl;
    } else {
        // ES 2.0 需要检查扩展
        mVAOSupported = hasExtension("GL_OES_vertex_array_object");
        std::cout << "[RenderContextGLES] VAO支持: " << (mVAOSupported ? "是" : "否") << " (ES 2.0 + 扩展)" << std::endl;
    }
    
    // 检测ES版本
    if (mMajorVersion > 3 || (mMajorVersion == 3 && mMinorVersion >= 1)) {
        mES31 = true;
        std::cout << "[RenderContextGLES] OpenGL ES 3.1+ 支持" << std::endl;
    } else {
        mES31 = false;
        std::cout << "[RenderContextGLES] OpenGL ES 3.0 支持" << std::endl;
    }
    
    // 验证上下文
    if (!validateContext()) {
        std::cerr << "[RenderContextGLES] 警告: 上下文验证失败" << std::endl;
    }
    
    // 输出版本信息
    std::cout << "[RenderContextGLES] 版本: " << getVersionString() << std::endl;
    std::cout << "[RenderContextGLES] 渲染器: " << getRendererString() << std::endl;
    std::cout << "[RenderContextGLES] 供应商: " << getVendorString() << std::endl;
}

std::shared_ptr<HYBuffer> RenderContextGLES::createBuffer(const BufferDesc& desc) {
    return std::make_shared<BufferGLES>(desc);
}

std::shared_ptr<HYTexture> RenderContextGLES::createTexture(const TextureDesc& desc) {
    return std::make_shared<TextureGLES>(desc);
}

std::shared_ptr<HYShader> RenderContextGLES::createShader(const ShaderDesc& desc) {
    return std::make_shared<ShaderGLES>(desc);
}

std::shared_ptr<HYSampler> RenderContextGLES::createSampler(const SamplerDesc& desc) {
    return std::make_shared<SamplerGLES>(desc);
}

std::shared_ptr<HYFrameBuffer> RenderContextGLES::createFrameBuffer(const FrameBufferDesc& desc) {
    return std::make_shared<FrameBufferGLES>(desc);
}

std::shared_ptr<HYPipelineState> RenderContextGLES::createPipelineState(const PipelineStateDesc& desc) {
    return std::make_shared<PipelineStateGLES>(desc);
}

std::shared_ptr<HYRenderPass> RenderContextGLES::createRenderPass() {
    return std::make_shared<RenderPassGLES>();
}

std::shared_ptr<HYFence> RenderContextGLES::createFence() {
    return std::make_shared<FenceGLES>();
}

std::shared_ptr<HYVertexArray> RenderContextGLES::createVertexArray() {
    return std::make_shared<VertexArrayGLES>(mVAOSupported);
}

} // namespace render
} // namespace hyengine
