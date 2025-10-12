#pragma once

#include <memory>
#include <cstdint>

namespace hyengine {
namespace render {

// 前置声明
class HYBuffer;
class HYTexture;
class HYShader;
class HYSampler;
class HYFrameBuffer;
class HYPipelineState;
class HYRenderPass;
class HYFence;
class HYVertexArray;

// 描述结构前置声明
struct BufferDesc;
struct TextureDesc;
struct ShaderDesc;
struct SamplerDesc;
struct FrameBufferDesc;
struct PipelineStateDesc;

/**
 * @brief 渲染上下文抽象基类
 * 
 * 作为资源工厂，负责创建所有渲染相关的资源对象
 */
class HYRenderContext {
public:
    virtual ~HYRenderContext() = default;

    /**
     * @brief 创建缓冲区对象
     * @param desc 缓冲区描述
     * @return 缓冲区智能指针
     */
    virtual std::shared_ptr<HYBuffer> createBuffer(const BufferDesc& desc) = 0;

    /**
     * @brief 创建纹理对象
     * @param desc 纹理描述
     * @return 纹理智能指针
     */
    virtual std::shared_ptr<HYTexture> createTexture(const TextureDesc& desc) = 0;

    /**
     * @brief 创建着色器对象
     * @param desc 着色器描述
     * @return 着色器智能指针
     */
    virtual std::shared_ptr<HYShader> createShader(const ShaderDesc& desc) = 0;

    /**
     * @brief 创建采样器对象
     * @param desc 采样器描述
     * @return 采样器智能指针
     */
    virtual std::shared_ptr<HYSampler> createSampler(const SamplerDesc& desc) = 0;

    /**
     * @brief 创建帧缓冲对象
     * @param desc 帧缓冲描述
     * @return 帧缓冲智能指针
     */
    virtual std::shared_ptr<HYFrameBuffer> createFrameBuffer(const FrameBufferDesc& desc) = 0;

    /**
     * @brief 创建管线状态对象
     * @param desc 管线状态描述
     * @return 管线状态智能指针
     */
    virtual std::shared_ptr<HYPipelineState> createPipelineState(const PipelineStateDesc& desc) = 0;

    /**
     * @brief 创建渲染通道对象
     * @return 渲染通道智能指针
     */
    virtual std::shared_ptr<HYRenderPass> createRenderPass() = 0;

    /**
     * @brief 创建同步栅栏对象
     * @return 栅栏智能指针
     */
    virtual std::shared_ptr<HYFence> createFence() = 0;

    /**
     * @brief 创建顶点数组对象
     * @return 顶点数组智能指针
     */
    virtual std::shared_ptr<HYVertexArray> createVertexArray() = 0;

protected:
    HYRenderContext() = default;
};

} // namespace render
} // namespace hyengine
