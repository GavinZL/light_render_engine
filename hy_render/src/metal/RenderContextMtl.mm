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
#import <Metal/Metal.h>
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
    
    // 创建默认Metal设备
    id<MTLDevice> device = MTLCreateSystemDefaultDevice();
    if (!device) {
        std::cerr << "[RenderContextMtl] 错误: 无法创建Metal设备" << std::endl;
        return;
    }
    
    mDevice = (__bridge_retained void*)device;
    
    // 创建命令队列
    id<MTLCommandQueue> commandQueue = [device newCommandQueue];
    if (!commandQueue) {
        std::cerr << "[RenderContextMtl] 错误: 无法创建命令队列" << std::endl;
        return;
    }
    
    mCommandQueue = (__bridge_retained void*)commandQueue;
    
    // 获取设备名称
    mDeviceName = std::string([[device name] UTF8String]);
    
    std::cout << "[RenderContextMtl] Metal设备初始化成功" << std::endl;
}

void RenderContextMtl::detectCapabilities() {
    std::cout << "[RenderContextMtl] 检测Metal功能..." << std::endl;
    
    id<MTLDevice> device = (__bridge id<MTLDevice>)mDevice;
    if (!device) {
        return;
    }
    
    // 检查特性支持
    #if TARGET_OS_OSX
    mSupportsDepthClipMode = [device supportsFamily:MTLGPUFamilyMac2];
    mSupportsArgumentBuffers = [device supportsFamily:MTLGPUFamilyMac2];
    mSupportsTessellation = [device supportsFamily:MTLGPUFamilyMac2];
    
    // 光线追踪需要 macOS 11.0+ 和支持的GPU
    if (@available(macOS 11.0, *)) {
        mSupportsRaytracing = [device supportsRaytracing];
    } else {
        mSupportsRaytracing = false;
    }
    #else
    mSupportsDepthClipMode = true;
    mSupportsArgumentBuffers = [device supportsFamily:MTLGPUFamilyApple4];
    mSupportsTessellation = [device supportsFamily:MTLGPUFamilyApple3];
    mSupportsRaytracing = false;
    #endif
    
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
