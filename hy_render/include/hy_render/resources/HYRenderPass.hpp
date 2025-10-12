#pragma once

#include "HYRenderDef.hpp"
#include <memory>

namespace hyengine {
namespace render {

class HYFrameBuffer;
class HYBuffer;
class HYShader;
class HYPipelineState;

/**
 * @brief 渲染通道对象抽象
 */
class HYRenderPass : public HYResource {
public:
    HYRenderPass() : HYResource(ResourceType::kRenderPass) {}
    virtual ~HYRenderPass() = default;

    /**
     * @brief 开始渲染通道
     * @param framebuffer 帧缓冲（nullptr表示默认缓冲）
     */
    virtual void beginPass(std::shared_ptr<HYFrameBuffer> framebuffer = nullptr) = 0;

    /**
     * @brief 结束渲染通道
     */
    virtual void endPass() = 0;

    /**
     * @brief 设置视口
     */
    virtual void setViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) = 0;

    /**
     * @brief 设置清除颜色
     */
    virtual void setClearColor(float r, float g, float b, float a) = 0;

    /**
     * @brief 清除缓冲区
     * @param mask 清除掩码
     */
    virtual void clearBuffer(uint8_t mask) = 0;

    /**
     * @brief 设置顶点缓冲
     */
    virtual void setVertexBuffer(std::shared_ptr<HYBuffer> buffer) = 0;

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
     * @brief 实例化绘制索引
     */
    virtual void drawElementsInstanced(uint32_t indexCount, uint32_t instanceCount,
                                      uint32_t indexOffset = 0) = 0;
};

} // namespace render
} // namespace hyengine
