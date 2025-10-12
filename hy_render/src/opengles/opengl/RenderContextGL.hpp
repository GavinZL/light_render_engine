#pragma once

#include "../gl_common/RenderContextGLBase.hpp"

namespace hyengine {
namespace render {

/**
 * @brief OpenGL 3.3+ Core Profile实现
 */
class RenderContextGL : public RenderContextGLBase {
public:
    RenderContextGL();
    virtual ~RenderContextGL() = default;

    // 实现工厂方法
    std::shared_ptr<HYBuffer> createBuffer(const BufferDesc& desc) override;
    std::shared_ptr<HYTexture> createTexture(const TextureDesc& desc) override;
    std::shared_ptr<HYShader> createShader(const ShaderDesc& desc) override;
    std::shared_ptr<HYSampler> createSampler(const SamplerDesc& desc) override;
    std::shared_ptr<HYFrameBuffer> createFrameBuffer(const FrameBufferDesc& desc) override;
    std::shared_ptr<HYPipelineState> createPipelineState(const PipelineStateDesc& desc) override;
    std::shared_ptr<HYRenderPass> createRenderPass() override;
    std::shared_ptr<HYFence> createFence() override;
    std::shared_ptr<HYVertexArray> createVertexArray() override;

private:
    bool mCoreProfile = true;
};

} // namespace render
} // namespace hyengine
