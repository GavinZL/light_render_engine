#pragma once

#include "../HYRenderDef.hpp"
#include <memory>
#include <vector>
#include <map>
#include <string>

namespace hyengine {
namespace render {

class HYFrameBuffer;
class HYBuffer;
class HYShader;
class HYPipelineState;

/**
 * @brief 渲染通道对象抽象
 * 
 * 根据设计文档5.1.4优化：
 * - 统一资源绑定管理（纹理、UBO从Shader迁移到RenderPass）
 * - 增强状态管理和缓存
 * - 优化绘制接口，减少重复
 * - 支持高级渲染特性
 */
class HYRenderPass : public HYResource {
public:
    HYRenderPass() : HYResource(ResourceType::kRenderPass) {}
    virtual ~HYRenderPass() = default;

    // ==== 基础渲染通道管理 ====
    /**
     * @brief 开始渲染通道
     * @param framebuffer 帧缓冲（nullptr表示默认缓冲）
     */
    virtual void beginPass(std::shared_ptr<HYFrameBuffer> framebuffer = nullptr) = 0;

    /**
     * @brief 结束渲染通道
     */
    virtual void endPass() = 0;

    // ==== 视口和清除操作 ====
    /**
     * @brief 设置视口
     */
    virtual void setViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) = 0;

    /**
     * @brief 设置裁剪区域
     */
    virtual void setScissor(uint32_t x, uint32_t y, uint32_t width, uint32_t height) = 0;

    /**
     * @brief 设置清除颜色
     */
    virtual void setClearColor(float r, float g, float b, float a) = 0;

    /**
     * @brief 清除缓冲区
     * @param mask 清除掩码
     */
    virtual void clearBuffer(uint8_t mask) = 0;

    // ==== 基础资源绑定接口 ====
    /**
     * @brief 设置顶点缓冲
     */
    virtual void setVertexBuffer(std::shared_ptr<HYBuffer> buffer) = 0;

    /**
     * @brief 设置顶点缓冲（带绑定点）
     */
    virtual void setVertexBuffer(std::shared_ptr<HYBuffer> buffer, uint32_t binding) = 0;

    /**
     * @brief 设置索引缓冲
     */
    virtual void setIndexBuffer(std::shared_ptr<HYBuffer> buffer) = 0;

    /**
     * @brief 设置着色器
     */
    virtual void setShader(std::shared_ptr<HYShader> shader) = 0;

    /**
     * @brief 设置管线状态
     */
    virtual void setPipelineState(std::shared_ptr<HYPipelineState> pso) = 0;

    // ==== 5.1.4 新增：高级资源绑定接口 ====
    /**
     * @brief 绑定纹理资源
     * @param texture 纹理对象
     * @param textureUnit 纹理单元索引
     * @param samplerName 着色器中sampler名称
     */
    virtual void bindTexture(std::shared_ptr<HYTexture> texture, 
                            uint32_t textureUnit, 
                            const std::string& samplerName) = 0;

    /**
     * @brief 绑定UBO资源
     * @param buffer UBO缓冲区
     * @param bindingPoint 绑定点
     * @param blockName 着色器中uniform block名称
     * @param offset 缓冲区偏移
     * @param size 绑定大小（0表示整个缓冲区）
     */
    virtual void bindUniformBuffer(std::shared_ptr<HYBuffer> buffer,
                                  uint32_t bindingPoint,
                                  const std::string& blockName,
                                  uint32_t offset = 0,
                                  uint32_t size = 0) = 0;

    /**
     * @brief 批量绑定纹理资源
     * @param bindings 纹理绑定列表
     */
    virtual void bindTextures(const std::vector<TextureBinding>& bindings) = 0;

    /**
     * @brief 批量绑定UBO资源
     * @param bindings UBO绑定列表
     */
    virtual void bindUniformBuffers(const std::vector<UniformBufferBinding>& bindings) = 0;

    // ==== 5.1.4 新增：高级绘制接口 ====
    /**
     * @brief 统一的绘制接口
     * @param params 绘制参数
     */
    virtual void draw(const DrawParams& params) = 0;

    /**
     * @brief 多实例绘制
     * @param params 绘制参数
     * @param instanceData 实例数据缓冲区（可选）
     */
    virtual void drawInstanced(const DrawParams& params, 
                              std::shared_ptr<HYBuffer> instanceData = nullptr) = 0;

    // ==== 5.1.4 新增：状态管理接口 ====
    /**
     * @brief 获取当前渲染状态快照
     */
    virtual const RenderState& getRenderState() const = 0;

    /**
     * @brief 应用渲染状态快照
     * @param state 状态快照
     */
    virtual void applyRenderState(const RenderState& state) = 0;

    /**
     * @brief 刷新渲染状态（强制更新所有状态）
     */
    virtual void flushRenderState() = 0;

    // ==== 原有绘制接口（保持兼容性） ====
    /**
     * @brief 绘制顶点
     */
    virtual void drawArrays(uint32_t vertexStart, uint32_t vertexCount) = 0;

    /**
     * @brief 实例化绘制顶点
     */
    virtual void drawArraysInstanced(uint32_t vertexStart, uint32_t vertexCount, 
                                    uint32_t instanceCount) = 0;

    /**
     * @brief 绘制索引
     */
    virtual void drawElements(uint32_t indexCount, uint32_t indexOffset = 0) = 0;

    /**
     * @brief 绘制索引（别名）
     */
    virtual void drawIndexed(uint32_t indexCount, uint32_t indexStart = 0) = 0;

    /**
     * @brief 实例化绘制索引
     */
    virtual void drawElementsInstanced(uint32_t indexCount, uint32_t instanceCount,
                                      uint32_t indexOffset = 0) = 0;

    // ==== 5.1.4 新增：工具方法 ====
    /**
     * @brief 从着色器反射信息自动绑定资源
     * @param shader 着色器对象
     * @param textures 纹理资源映射（sampler名称 -> 纹理对象）
     * @param uniformBuffers UBO资源映射（block名称 -> 缓冲区对象）
     */
    virtual void autoBindResources(std::shared_ptr<HYShader> shader,
                                  const std::map<std::string, std::shared_ptr<HYTexture>>& textures = {},
                                  const std::map<std::string, std::shared_ptr<HYBuffer>>& uniformBuffers = {}) = 0;

    /**
     * @brief 简化的绘制接口（基于管线状态自动推导绘制参数）
     */
    virtual void drawAuto() = 0;

protected:
    // ==== 5.1.4 新增：内部状态管理 ====
    /**
     * @brief 检查状态是否变化，如有变化则刷新
     */
    virtual void updateStateIfDirty() = 0;

    /**
     * @brief 缓存着色器反射信息以优化性能
     */
    virtual void cacheShaderReflection(std::shared_ptr<HYShader> shader) = 0;
};

} // namespace render
} // namespace hyengine