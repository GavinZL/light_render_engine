#include "RenderContextGL.hpp"
#include "resources/BufferGL.hpp"
#include "resources/TextureGL.hpp"
#include "resources/ShaderGL.hpp"
#include "resources/SamplerGL.hpp"
#include "resources/FrameBufferGL.hpp"
#include "resources/PipelineStateGL.hpp"
#include "resources/RenderPassGL.hpp"
#include "resources/FenceGL.hpp"
#include "resources/VertexArrayGL.hpp"

namespace hyengine {
namespace render {

RenderContextGL::RenderContextGL() {
    initializeGLFunctions();
    detectExtensions();
}

std::shared_ptr<HYBuffer> RenderContextGL::createBuffer(const BufferDesc& desc) {
    return std::make_shared<BufferGL>(desc);
}

std::shared_ptr<HYTexture> RenderContextGL::createTexture(const TextureDesc& desc) {
    return std::make_shared<TextureGL>(desc);
}

std::shared_ptr<HYShader> RenderContextGL::createShader(const ShaderDesc& desc) {
    return std::make_shared<ShaderGL>(desc);
}

std::shared_ptr<HYSampler> RenderContextGL::createSampler(const SamplerDesc& desc) {
    return std::make_shared<SamplerGL>(desc);
}

std::shared_ptr<HYFrameBuffer> RenderContextGL::createFrameBuffer(const FrameBufferDesc& desc) {
    return std::make_shared<FrameBufferGL>(desc);
}

std::shared_ptr<HYPipelineState> RenderContextGL::createPipelineState(const PipelineStateDesc& desc) {
    return std::make_shared<PipelineStateGL>(desc);
}

std::shared_ptr<HYRenderPass> RenderContextGL::createRenderPass() {
    return std::make_shared<RenderPassGL>();
}

std::shared_ptr<HYFence> RenderContextGL::createFence() {
    return std::make_shared<FenceGL>();
}

std::shared_ptr<HYVertexArray> RenderContextGL::createVertexArray() {
    return std::make_shared<VertexArrayGL>();
}

} // namespace render
} // namespace hyengine
