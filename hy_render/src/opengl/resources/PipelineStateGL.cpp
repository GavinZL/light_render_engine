#include "PipelineStateGL.hpp"
#include <iostream>

namespace hyengine {
namespace render {

PipelineStateGL::PipelineStateGL(const PipelineStateDesc& desc) : mDesc(desc) {
    std::cout << "[PipelineStateGL] Created:" << std::endl;
    std::cout << "  Blend: " << (mDesc.blendDesc.blendEnable ? "enabled" : "disabled") << std::endl;
    std::cout << "  Depth Test: " << (mDesc.depthStencilDesc.depthTestEnable ? "enabled" : "disabled") << std::endl;
    std::cout << "  Cull Mode: " << static_cast<int>(mDesc.rasterizerDesc.cullMode) << std::endl;
    std::cout << "  Primitive: " << static_cast<int>(mDesc.primitiveDesc.type) << std::endl;
}

PipelineStateGL::~PipelineStateGL() {
    std::cout << "[PipelineStateGL] Destroyed" << std::endl;
}

void PipelineStateGL::enableBlend(bool flag) {
    mDesc.blendDesc.blendEnable = flag;
    std::cout << "[PipelineStateGL] Blend " << (flag ? "enabled" : "disabled") << std::endl;
    
    // TODO: 实际实现中这里会调用 OpenGL 函数
    // if (flag) {
    //     glEnable(GL_BLEND);
    //     // 设置混合函数
    //     GLenum srcFactor = blendFactorToGL(mDesc.blendDesc.srcBlend);
    //     GLenum dstFactor = blendFactorToGL(mDesc.blendDesc.dstBlend);
    //     glBlendFunc(srcFactor, dstFactor);
    //     
    //     // 设置混合方程
    //     GLenum blendOp = blendOperationToGL(mDesc.blendDesc.blendOp);
    //     glBlendEquation(blendOp);
    // } else {
    //     glDisable(GL_BLEND);
    // }
}

void PipelineStateGL::enableDepth(bool flag) {
    mDesc.depthStencilDesc.depthTestEnable = flag;
    std::cout << "[PipelineStateGL] Depth test " << (flag ? "enabled" : "disabled") << std::endl;
    
    // TODO: 实际实现中这里会调用 OpenGL 函数
    // if (flag) {
    //     glEnable(GL_DEPTH_TEST);
    //     GLenum depthFunc = compareFunctionToGL(mDesc.depthStencilDesc.depthFunc);
    //     glDepthFunc(depthFunc);
    //     glDepthMask(mDesc.depthStencilDesc.depthWriteEnable ? GL_TRUE : GL_FALSE);
    // } else {
    //     glDisable(GL_DEPTH_TEST);
    // }
}

void PipelineStateGL::enableStencil(bool flag) {
    mDesc.depthStencilDesc.stencilTestEnable = flag;
    std::cout << "[PipelineStateGL] Stencil test " << (flag ? "enabled" : "disabled") << std::endl;
    
    // TODO: 实际实现中这里会调用 OpenGL 函数
    // if (flag) {
    //     glEnable(GL_STENCIL_TEST);
    //     glStencilMask(mDesc.depthStencilDesc.stencilMask);
    // } else {
    //     glDisable(GL_STENCIL_TEST);
    // }
}

void PipelineStateGL::enableScissor(bool flag) {
    mDesc.rasterizerDesc.scissorEnable = flag;
    std::cout << "[PipelineStateGL] Scissor test " << (flag ? "enabled" : "disabled") << std::endl;
    
    // TODO: 实际实现中这里会调用 OpenGL 函数
    // if (flag) {
    //     glEnable(GL_SCISSOR_TEST);
    //     const auto& rect = mDesc.rasterizerDesc.scissorRect;
    //     glScissor(rect[0], rect[1], rect[2], rect[3]);
    // } else {
    //     glDisable(GL_SCISSOR_TEST);
    // }
}

void PipelineStateGL::setPrimitive(PrimitiveType mode) {
    mDesc.primitiveDesc.type = mode;
    std::cout << "[PipelineStateGL] Primitive type set to " << static_cast<int>(mode) << std::endl;
    
    // 注意：图元类型在绘制时使用，不是在这里设置
}

void PipelineStateGL::setCullMode(CullMode mode) {
    mDesc.rasterizerDesc.cullMode = mode;
    std::cout << "[PipelineStateGL] Cull mode set to " << static_cast<int>(mode) << std::endl;
    
    // TODO: 实际实现中这里会调用 OpenGL 函数
    // switch (mode) {
    //     case CullMode::kNone:
    //         glDisable(GL_CULL_FACE);
    //         break;
    //     case CullMode::kBack:
    //         glEnable(GL_CULL_FACE);
    //         glCullFace(GL_BACK);
    //         break;
    //     case CullMode::kFront:
    //         glEnable(GL_CULL_FACE);
    //         glCullFace(GL_FRONT);
    //         break;
    // }
}

ResourceHandle PipelineStateGL::getResourceHandle() const {
    ResourceHandle handle;
    handle.uintHandle = 0; // PipelineState 是状态集合，没有单一的GL句柄
    return handle;
}

} // namespace render
} // namespace hyengine
