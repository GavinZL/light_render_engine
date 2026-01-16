#pragma once

#include "../../include/hy_render/HYRenderContext.hpp"
#include <memory>
#include <string>

#ifdef __OBJC__
#import <Metal/Metal.h>
#import <QuartzCore/CAMetalLayer.h>
#endif

namespace hyengine {
namespace render {

/**
 * @brief Metal 渲染上下文实现
 * 
 * Metal 是 Apple 平台的现代图形 API，具有以下特点：
 * 1. 低开销、接近硬件级别的访问
 * 2. 显式的资源管理和生命周期控制
 * 3. 命令编码器模式（Command Encoder）
 * 4. 统一的着色器语言（Metal Shading Language）
 * 5. 计算和图形管线的深度集成
 * 
 * 平台支持：
 * - iOS 8.0+
 * - macOS 10.11+
 * - tvOS 9.0+
 */
class RenderContextMtl : public HYRenderContext {
public:
    RenderContextMtl();
    virtual ~RenderContextMtl() = default;

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

    /**
     * @brief 获取Metal设备信息
     */
    std::string getDeviceName() const;
    
    /**
     * @brief 获取Metal功能集
     */
    std::string getFeatureSet() const;
    
    /**
     * @brief 检查特性支持
     */
    bool supportsFeature(const std::string& feature) const;

private:
    void initializeMetalDevice();
    void detectCapabilities();

public:
    /**
     * @brief 获取Metal设备对象
     */
    void* getMetalDevice() const { return mDevice; }
    
    /**
     * @brief 获取Metal命令队列
     */
    void* getMetalCommandQueue() const { return mCommandQueue; }

private:
    // Metal 设备对象
    void* mDevice = nullptr;  // id<MTLDevice>
    
    // Metal 命令队列
    void* mCommandQueue = nullptr;  // id<MTLCommandQueue>
    
    // 设备能力标志
    bool mSupportsDepthClipMode = false;
    bool mSupportsArgumentBuffers = false;
    bool mSupportsTessellation = false;
    bool mSupportsRaytracing = false;
    
    // 设备信息
    std::string mDeviceName;
    uint32_t mRecommendedWorkingSetSize = 0;
};

} // namespace render
} // namespace hyengine
