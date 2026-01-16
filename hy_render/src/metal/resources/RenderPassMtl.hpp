#pragma once

#include "../../core/HYResourceCommon.hpp"

namespace hyengine {
namespace render {

/**
 * @brief Metal 渲染通道实现
 */
class RenderPassMtl : public HYRenderPass {
public:
    explicit RenderPassMtl(void* commandQueue);
    virtual ~RenderPassMtl() = default;

    void beginPass(std::shared_ptr<HYFrameBuffer> framebuffer) override;
    void endPass() override;
    void setShader(std::shared_ptr<HYShader> shader) override;
    void setPipelineState(std::shared_ptr<HYPipelineState> state) override;
    void setVertexBuffer(std::shared_ptr<HYBuffer> buffer, uint32_t binding) override;
    void setVertexBuffer(std::shared_ptr<HYBuffer> buffer) override;
    void setClearColor(float r, float g, float b, float a) override;
    void clearBuffer(uint8_t mask) override;
    void drawArraysInstanced(uint32_t vertexStart, uint32_t vertexCount, uint32_t instanceCount) override;
    void drawElements(uint32_t indexCount, uint32_t indexOffset) override;
    void drawElementsInstanced(uint32_t indexCount, uint32_t instanceCount, uint32_t indexOffset) override;
    void setIndexBuffer(std::shared_ptr<HYBuffer> buffer) override;
    void setViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) override;
    void setScissor(uint32_t x, uint32_t y, uint32_t width, uint32_t height) override;
    void drawArrays(uint32_t vertexStart, uint32_t vertexCount) override;
    void drawIndexed(uint32_t indexCount, uint32_t indexStart = 0) override;
    ResourceHandle getResourceHandle() const override;
    
    // 5.1.4 新增：高级资源绑定接口
    void bindTexture(std::shared_ptr<HYTexture> texture, uint32_t textureUnit, 
                    const std::string& samplerName) override;
    void bindUniformBuffer(std::shared_ptr<HYBuffer> buffer, uint32_t bindingPoint,
                          const std::string& blockName, uint32_t offset = 0, uint32_t size = 0) override;
    void bindTextures(const std::vector<TextureBinding>& bindings) override;
    void bindUniformBuffers(const std::vector<UniformBufferBinding>& bindings) override;
    
    // 5.1.4 新增：统一绘制接口
    void draw(const DrawParams& params) override;
    void drawInstanced(const DrawParams& params, std::shared_ptr<HYBuffer> instanceData = nullptr) override;
    
    // 5.1.4 新增：状态管理接口
    const RenderState& getRenderState() const override;
    void applyRenderState(const RenderState& state) override;
    void flushRenderState() override;
    
    // 5.1.4 新增：工具方法
    void autoBindResources(std::shared_ptr<HYShader> shader,
                          const std::map<std::string, std::shared_ptr<HYTexture>>& textures = {},
                          const std::map<std::string, std::shared_ptr<HYBuffer>>& uniformBuffers = {}) override;
    void drawAuto() override;

private:
    // 5.1.4 新增：内部状态管理方法
    void updateStateIfDirty();
    void cacheShaderReflection(std::shared_ptr<HYShader> shader);
    
private:
    void* mCommandQueue = nullptr;         // id<MTLCommandQueue>
    void* mCommandBuffer = nullptr;        // id<MTLCommandBuffer>
    void* mRenderCommandEncoder = nullptr; // id<MTLRenderCommandEncoder>
    
    bool mIsInPass = false;
    std::shared_ptr<HYFrameBuffer> mCurrentFramebuffer;
    std::shared_ptr<HYShader> mCurrentShader;
    std::shared_ptr<HYPipelineState> mCurrentPipelineState;
    std::shared_ptr<HYBuffer> mCurrentVertexBuffer;              // 顶点缓冲区
    std::shared_ptr<HYBuffer> mCurrentIndexBuffer;               // 索引缓冲区
    std::shared_ptr<HYBuffer> mInstanceBuffer;                   // 实例数据缓冲区
    
    // 5.1.4 新增：高级状态管理
    mutable RenderState mRenderState;
    std::vector<TextureBinding> mTextureBindings;
    std::vector<UniformBufferBinding> mUniformBufferBindings;
    
    // 着色器反射信息缓存
    mutable std::map<std::shared_ptr<HYShader>, ShaderReflection> mShaderReflectionCache;
};

} // namespace render
} // namespace hyengine
