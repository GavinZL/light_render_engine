#include "PipelineStateMtl.hpp"
#include <iostream>

namespace hyengine {
namespace render {

PipelineStateMtl::PipelineStateMtl(const PipelineStateDesc& desc, void* device)
    : mDesc(desc), mDevice(device) {
    std::cout << "[PipelineStateMtl] 创建Metal管线状态" << std::endl;
    mRenderPipelineState = reinterpret_cast<void*>(0x500);
    mDepthStencilState = reinterpret_cast<void*>(0x501);
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

ResourceHandle PipelineStateMtl::getResourceHandle() const {
    ResourceHandle handle;
    handle.ptrHandle = mRenderPipelineState;
    return handle;
}

} // namespace render
} // namespace hyengine
