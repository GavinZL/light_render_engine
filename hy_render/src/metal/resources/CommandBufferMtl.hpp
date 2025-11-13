#pragma once

#include "../../../include/hy_render/resources/HYCommandBuffer.hpp"
#include <vector>
#include <memory>
#include <string>

namespace hyengine {
namespace render {

/**
 * @brief Metal命令缓冲区实现
 * 
 * Metal采用命令编码器模式，这个实现更接近Metal的原生命令缓冲区概念
 * 支持延迟执行和GPU异步处理
 */
class CommandBufferMtl : public HYCommandBuffer {
public:
    CommandBufferMtl();
    virtual ~CommandBufferMtl();

    // ==== 命令缓冲区生命周期管理 ====
    void begin() override;
    void end() override;
    void reset() override;
    void commit(HYRenderPass* renderPass, bool waitForCompletion = false) override;

    // ==== 渲染命令记录接口 ====
    void cmdBeginPass(std::shared_ptr<HYFrameBuffer> framebuffer = nullptr) override;
    void cmdEndPass() override;
    void cmdSetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) override;
    void cmdClearBuffer(uint8_t mask, float r = 0.0f, float g = 0.0f, float b = 0.0f, float a = 1.0f) override;
    void cmdSetVertexBuffer(std::shared_ptr<HYBuffer> buffer, uint32_t binding = 0) override;
    void cmdSetIndexBuffer(std::shared_ptr<HYBuffer> buffer) override;
    void cmdSetShader(std::shared_ptr<HYShader> shader) override;
    void cmdSetPipelineState(std::shared_ptr<HYPipelineState> pso) override;
    void cmdBindTexture(std::shared_ptr<HYTexture> texture, 
                       uint32_t textureUnit, 
                       const std::string& samplerName) override;
    void cmdBindUniformBuffer(std::shared_ptr<HYBuffer> buffer,
                             uint32_t bindingPoint,
                             const std::string& blockName,
                             uint32_t offset = 0,
                             uint32_t size = 0) override;
    void cmdDraw(const DrawParams& params) override;
    void cmdDrawInstanced(const DrawParams& params, 
                         std::shared_ptr<HYBuffer> instanceData = nullptr) override;

    // ==== 高级命令接口 ====
    void cmdBindTextures(const std::vector<TextureBinding>& bindings) override;
    void cmdBindUniformBuffers(const std::vector<UniformBufferBinding>& bindings) override;
    void cmdApplyRenderState(const RenderState& state) override;
    void cmdCustom(std::function<void(HYRenderPass*)> customFunction) override;

    // ==== 命令批次管理 ====
    void beginBatch(const std::string& batchName = "") override;
    void endBatch() override;

    // ==== 状态查询 ====
    size_t getCommandCount() const override;
    std::string getDebugInfo() const override;

    // ==== 资源句柄 ====
    ResourceHandle getResourceHandle() const override;

protected:
    bool isValidStateTransition(CommandBufferState from, CommandBufferState to) const override;

private:
    // Metal原生对象（void*避免导入Metal头文件）
    void* mMtlCommandBuffer = nullptr;    // id<MTLCommandBuffer>
    void* mMtlCommandQueue = nullptr;     // id<MTLCommandQueue>
    
    // 命令记录
    std::vector<std::unique_ptr<RenderCommand>> mCommands;
    std::string mCurrentBatchName;
    size_t mCurrentBatchStartIndex = 0;
    
    // Metal特有的状态
    bool mUseNativeCommandBuffer = true;  // 是否使用原生Metal命令缓冲区
    
    // 初始化Metal资源
    void initializeMetal();
    void cleanupMetal();
    
    // 添加命令到缓冲区
    template<typename CommandType, typename... Args>
    void addCommand(Args&&... args);
};

} // namespace render
} // namespace hyengine