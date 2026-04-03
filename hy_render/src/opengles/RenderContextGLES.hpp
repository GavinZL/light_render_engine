#pragma once

#include "../gl_common/RenderContextGLBase.hpp"

namespace hyengine {
namespace render {

/**
 * @brief OpenGL ES 3.0/3.1 实现
 * 
 * 与 OpenGL 3.3+ 的关键差异：
 * 1. VAO 在 ES 3.0 中是可选的（通过 OES_vertex_array_object 扩展）
 * 2. 不支持分离式采样器对象（Sampler Objects）- ES 3.0 采样状态内嵌在纹理中
 * 3. 着色器必须使用精度限定符（precision mediump float;）
 * 4. 某些 API 参数类型不同（如 glDepthRangef vs glDepthRange）
 * 5. 纹理格式支持差异（ETC2/ASTC 压缩格式）
 */
class RenderContextGLES : public RenderContextGLBase {
public:
    RenderContextGLES();
    virtual ~RenderContextGLES() = default;

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
    bool mVAOSupported = false;  // VAO 扩展是否支持
    bool mES31 = false;          // 是否是 ES 3.1
};

} // namespace render
} // namespace hyengine
