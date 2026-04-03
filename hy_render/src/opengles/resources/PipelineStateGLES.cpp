#include "PipelineStateGLES.hpp"
#include "../../gl_common/GLUtils.hpp"
#include <iostream>

namespace hyengine {
namespace render {

// ============================================================================
// PipelineStateGLES 实现
// ============================================================================

PipelineStateGLES::PipelineStateGLES(const PipelineStateDesc& desc) : mDesc(desc) {
    std::cout << "[PipelineStateGLES] Created pipeline state" << std::endl;
}

void PipelineStateGLES::enableBlend(bool flag) {
    mDesc.blendDesc.blendEnable = flag;
    std::cout << "[PipelineStateGLES] Blend " << (flag ? "enabled" : "disabled") << std::endl;
}

void PipelineStateGLES::enableDepth(bool flag) {
    mDesc.depthStencilDesc.depthTestEnable = flag;
    std::cout << "[PipelineStateGLES] Depth test " << (flag ? "enabled" : "disabled") << std::endl;
}

void PipelineStateGLES::enableStencil(bool flag) {
    mDesc.depthStencilDesc.stencilTestEnable = flag;
    std::cout << "[PipelineStateGLES] Stencil test " << (flag ? "enabled" : "disabled") << std::endl;
}

void PipelineStateGLES::setCullMode(CullMode mode) {
    mDesc.rasterizerDesc.cullMode = mode;
    std::cout << "[PipelineStateGLES] Cull mode set" << std::endl;
}

void PipelineStateGLES::enableScissor(bool flag) {
    mDesc.rasterizerDesc.scissorEnable = flag;
    std::cout << "[PipelineStateGLES] Scissor test " << (flag ? "enabled" : "disabled") << std::endl;
}

void PipelineStateGLES::setPrimitive(PrimitiveType mode) {
    mDesc.primitiveDesc.type = mode;
    std::cout << "[PipelineStateGLES] Primitive type set to " << static_cast<int>(mode) << std::endl;
}

void PipelineStateGLES::apply() {
    std::cout << "[PipelineStateGLES] Applying pipeline state..." << std::endl;
    
    // TODO: OpenGL ES 状态设置
    
    // 混合状态
    if (mDesc.blendDesc.blendEnable) {
        // glEnable(GL_BLEND);
        // glBlendFuncSeparate(srcColorFactor, dstColorFactor, srcAlphaFactor, dstAlphaFactor);
        // glBlendEquationSeparate(colorBlendOp, alphaBlendOp);
    } else {
        // glDisable(GL_BLEND);
    }
    
    // 深度测试
    if (mDesc.depthStencilDesc.depthTestEnable) {
        // glEnable(GL_DEPTH_TEST);
        // glDepthFunc(depthFunc);
        // glDepthMask(mDesc.depthStencilDesc.depthWriteEnable ? GL_TRUE : GL_FALSE);
    } else {
        // glDisable(GL_DEPTH_TEST);
    }
    
    // 模板测试
    if (mDesc.depthStencilDesc.stencilTestEnable) {
        // glEnable(GL_STENCIL_TEST);
        // glStencilOp(stencilFailOp, depthFailOp, stencilPassOp);
        // glStencilFunc(stencilFunc, ref, mask);
    } else {
        // glDisable(GL_STENCIL_TEST);
    }
    
    // 剔除模式
    // switch (mDesc.rasterizerDesc.cullMode) {
    //     case CullMode::kNone:
    //         glDisable(GL_CULL_FACE);
    //         break;
    //     case CullMode::kFront:
    //         glEnable(GL_CULL_FACE);
    //         glCullFace(GL_FRONT);
    //         break;
    //     case CullMode::kBack:
    //         glEnable(GL_CULL_FACE);
    //         glCullFace(GL_BACK);
    //         break;
    //     case CullMode::kFrontAndBack:
    //         glEnable(GL_CULL_FACE);
    //         glCullFace(GL_FRONT_AND_BACK);
    //         break;
    // }
    
    // 正面朝向
    // glFrontFace(mDesc.rasterizerDesc.frontFace == FrontFace::kCW ? GL_CW : GL_CCW);
    
    // 深度偏移
    // if (mDesc.rasterizerDesc.depthBiasEnable) {
    //     glEnable(GL_POLYGON_OFFSET_FILL);
    //     glPolygonOffset(mDesc.rasterizerDesc.depthBiasSlopeFactor,
    //                     mDesc.rasterizerDesc.depthBiasConstantFactor);
    // } else {
    //     glDisable(GL_POLYGON_OFFSET_FILL);
    // }
}

ResourceHandle PipelineStateGLES::getResourceHandle() const {
    ResourceHandle handle;
    handle.uintHandle = 0; // 管线状态没有独立的GL对象
    return handle;
}

} // namespace render
} // namespace hyengine
