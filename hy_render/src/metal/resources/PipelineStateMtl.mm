#include "PipelineStateMtl.hpp"
#include "ShaderMtl.hpp"
#import <Metal/Metal.h>
#include <iostream>

namespace hyengine {
namespace render {

PipelineStateMtl::PipelineStateMtl(const PipelineStateDesc& desc, void* device)
    : mDesc(desc), mDevice(device) {
    std::cout << "[PipelineStateMtl] 创建Metal管线状态" << std::endl;
    // 注意：在Metal中，PipelineState需要shader对象才能创建
    // 所以这里只是初始化，真正的创建在 build() 方法中进行
}

PipelineStateMtl::~PipelineStateMtl() {
    if (mRenderPipelineState) {
        CFRelease(mRenderPipelineState);
        mRenderPipelineState = nullptr;
    }
    if (mDepthStencilState) {
        CFRelease(mDepthStencilState);
        mDepthStencilState = nullptr;
    }
    std::cout << "[PipelineStateMtl] 销毁Metal管线状态" << std::endl;
}

void PipelineStateMtl::enableBlend(bool flag) { mDesc.blendDesc.blendEnable = flag; }
void PipelineStateMtl::enableDepth(bool flag) { mDesc.depthStencilDesc.depthTestEnable = flag; }
void PipelineStateMtl::enableStencil(bool flag) { mDesc.depthStencilDesc.stencilTestEnable = flag; }
void PipelineStateMtl::setCullMode(CullMode mode) { mDesc.rasterizerDesc.cullMode = mode; }
void PipelineStateMtl::enableScissor(bool flag) { mDesc.rasterizerDesc.scissorEnable = flag; }
void PipelineStateMtl::setPrimitive(PrimitiveType mode) { mDesc.primitiveDesc.type = mode; }

void PipelineStateMtl::apply() {
    std::cout << "[PipelineStateMtl] 应用管线状态" << std::endl;
}

bool PipelineStateMtl::build(void* shader, void* vertexDesc) {
    id<MTLDevice> device = (__bridge id<MTLDevice>)mDevice;
    if (!device) {
        std::cerr << "[PipelineStateMtl] 错误: Metal设备无效" << std::endl;
        return false;
    }
    
    ShaderMtl* shaderMtl = static_cast<ShaderMtl*>(shader);
    if (!shaderMtl) {
        std::cerr << "[PipelineStateMtl] 错误: 着色器对象无效" << std::endl;
        return false;
    }
    
    id<MTLFunction> vertexFunc = (__bridge id<MTLFunction>)shaderMtl->getVertexStageHandle().ptrHandle;
    id<MTLFunction> fragmentFunc = (__bridge id<MTLFunction>)shaderMtl->getFragmentStageHandle().ptrHandle;
    
    if (!vertexFunc || !fragmentFunc) {
        std::cerr << "[PipelineStateMtl] 错误: 着色器函数无效" << std::endl;
        return false;
    }
    
    // 创建MTLRenderPipelineDescriptor
    MTLRenderPipelineDescriptor* pipelineDesc = [[MTLRenderPipelineDescriptor alloc] init];
    pipelineDesc.vertexFunction = vertexFunc;
    pipelineDesc.fragmentFunction = fragmentFunc;
    
    // 设置顶点描述符
    if (vertexDesc) {
        pipelineDesc.vertexDescriptor = (__bridge MTLVertexDescriptor*)vertexDesc;
    }
    
    // 设置颜色附件格式（默认BGRA8Unorm）
    pipelineDesc.colorAttachments[0].pixelFormat = MTLPixelFormatBGRA8Unorm;
    
    // 设置深度附件格式
    if (mDesc.depthStencilDesc.depthTestEnable) {
        pipelineDesc.depthAttachmentPixelFormat = MTLPixelFormatDepth32Float;
    }
    
    // 设置混合状态
    if (mDesc.blendDesc.blendEnable) {
        pipelineDesc.colorAttachments[0].blendingEnabled = YES;
        // TODO: 根据mDesc.blendDesc设置具体混合参数
    }
    
    // 创建管线状态对象
    NSError* error = nil;
    id<MTLRenderPipelineState> pipelineState = [device newRenderPipelineStateWithDescriptor:pipelineDesc error:&error];
    
    if (!pipelineState) {
        std::cerr << "[PipelineStateMtl] 创建管线状态失败: " 
                  << [[error localizedDescription] UTF8String] << std::endl;
        return false;
    }
    
    // 创建深度模板状态
    if (mDesc.depthStencilDesc.depthTestEnable) {
        MTLDepthStencilDescriptor* depthDesc = [[MTLDepthStencilDescriptor alloc] init];
        depthDesc.depthCompareFunction = MTLCompareFunctionLess;
        depthDesc.depthWriteEnabled = mDesc.depthStencilDesc.depthWriteEnable;
        
        id<MTLDepthStencilState> depthState = [device newDepthStencilStateWithDescriptor:depthDesc];
        if (depthState) {
            // 释放旧资源
            if (mDepthStencilState) CFRelease(mDepthStencilState);
            mDepthStencilState = (__bridge_retained void*)depthState;
        }
    }
    
    // 释放旧资源
    if (mRenderPipelineState) CFRelease(mRenderPipelineState);
    mRenderPipelineState = (__bridge_retained void*)pipelineState;
    
    std::cout << "[PipelineStateMtl] Metal管线状态创建成功" << std::endl;
    return true;
}

ResourceHandle PipelineStateMtl::getResourceHandle() const {
    ResourceHandle handle;
    handle.ptrHandle = mRenderPipelineState;
    return handle;
}

} // namespace render
} // namespace hyengine
