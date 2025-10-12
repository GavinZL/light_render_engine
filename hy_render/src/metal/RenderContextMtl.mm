#include "RenderContextMtl.hpp"
#include "resources/BufferMtl.hpp"
#include "resources/TextureMtl.hpp"
#include "resources/ShaderMtl.hpp"
#include "resources/SamplerMtl.hpp"
#include "resources/FrameBufferMtl.hpp"
#include "resources/PipelineStateMtl.hpp"
#include "resources/RenderPassMtl.hpp"
#include "resources/FenceMtl.hpp"
#include "resources/VertexArrayMtl.hpp"
#include <iostream>

namespace hyengine {
namespace render {

RenderContextMtl::RenderContextMtl() {
    std::cout << "[RenderContextMtl] 初始化Metal渲染上下文" << std::endl;
    
    initializeMetalDevice();
    detectCapabilities();
    
    std::cout << "[RenderContextMtl] 设备: " << getDeviceName() << std::endl;
    std::cout << "[RenderContextMtl] 功能集: " << getFeatureSet() << std::endl;
}

void RenderContextMtl::initializeMetalDevice() {
    std::cout << "[RenderContextMtl] 初始化Metal设备..." << std::endl;
    
    // TODO: 实际实现中应该：
    // mDevice = MTLCreateSystemDefaultDevice();
    // if (!mDevice) {
    //     std::cerr << "[RenderContextMtl] 错误: 无法创建Metal设备" << std::endl;
    //     return;
    // }
    //
    // // 创建命令队列
    // mCommandQueue = [mDevice newCommandQueue];
    // if (!mCommandQueue) {
    //     std::cerr << "[RenderContextMtl] 错误: 无法创建命令队列" << std::endl;
    //     return;
    // }
    //
    // // 获取设备名称
    // mDeviceName = std::string([[mDevice name] UTF8String]);
    
    // 模拟设备初始化
    mDevice = reinterpret_cast<void*>(0x1);
    mCommandQueue = reinterpret_cast<void*>(0x2);
    mDeviceName = "Apple M1/M2/M3 (模拟)";
    
    std::cout << "[RenderContextMtl] Metal设备初始化成功" << std::endl;
}

void RenderContextMtl::detectCapabilities() {
    std::cout << "[RenderContextMtl] 检测Metal功能..." << std::endl;
    
    // TODO: 实际实现中应该检查设备特性
    // mSupportsDepthClipMode = [mDevice supportsFeatureSet:MTLFeatureSet_macOS_GPUFamily1_v1];
    // mSupportsArgumentBuffers = [mDevice supportsFeatureSet:MTLFeatureSet_macOS_GPUFamily1_v3];
    // mSupportsTessellation = [mDevice supportsFeatureSet:MTLFeatureSet_macOS_GPUFamily1_v2];
    // mSupportsRaytracing = [mDevice supportsRaytracing];
    
    // 模拟功能检测（假设为现代Apple芯片）
    mSupportsDepthClipMode = true;
    mSupportsArgumentBuffers = true;
    mSupportsTessellation = true;
    mSupportsRaytracing = true; // M3及以上
    
    std::cout << "[RenderContextMtl] 深度裁剪模式: " << (mSupportsDepthClipMode ? "支持" : "不支持") << std::endl;
    std::cout << "[RenderContextMtl] 参数缓冲区: " << (mSupportsArgumentBuffers ? "支持" : "不支持") << std::endl;
    std::cout << "[RenderContextMtl] 曲面细分: " << (mSupportsTessellation ? "支持" : "不支持") << std::endl;
    std::cout << "[RenderContextMtl] 光线追踪: " << (mSupportsRaytracing ? "支持" : "不支持") << std::endl;
}

std::string RenderContextMtl::getDeviceName() const {
    return mDeviceName;
}

std::string RenderContextMtl::getFeatureSet() const {
    return "Metal 3.0+ (Apple Silicon)";
}

bool RenderContextMtl::supportsFeature(const std::string& feature) const {
    if (feature == "depth_clip_mode") return mSupportsDepthClipMode;
    if (feature == "argument_buffers") return mSupportsArgumentBuffers;
    if (feature == "tessellation") return mSupportsTessellation;
    if (feature == "raytracing") return mSupportsRaytracing;
    return false;
}

std::shared_ptr<HYBuffer> RenderContextMtl::createBuffer(const BufferDesc& desc) {
    return std::make_shared<BufferMtl>(desc, mDevice);
}

std::shared_ptr<HYTexture> RenderContextMtl::createTexture(const TextureDesc& desc) {
    return std::make_shared<TextureMtl>(desc, mDevice);
}

std::shared_ptr<HYShader> RenderContextMtl::createShader(const ShaderDesc& desc) {
    return std::make_shared<ShaderMtl>(desc, mDevice);
}

std::shared_ptr<HYSampler> RenderContextMtl::createSampler(const SamplerDesc& desc) {
    return std::make_shared<SamplerMtl>(desc, mDevice);
}

std::shared_ptr<HYFrameBuffer> RenderContextMtl::createFrameBuffer(const FrameBufferDesc& desc) {
    return std::make_shared<FrameBufferMtl>(desc);
}

std::shared_ptr<HYPipelineState> RenderContextMtl::createPipelineState(const PipelineStateDesc& desc) {
    return std::make_shared<PipelineStateMtl>(desc, mDevice);
}

std::shared_ptr<HYRenderPass> RenderContextMtl::createRenderPass() {
    return std::make_shared<RenderPassMtl>(mCommandQueue);
}

std::shared_ptr<HYFence> RenderContextMtl::createFence() {
    return std::make_shared<FenceMtl>(mDevice);
}

std::shared_ptr<HYVertexArray> RenderContextMtl::createVertexArray() {
    return std::make_shared<VertexArrayMtl>();
}

} // namespace render
} // namespace hyengine
