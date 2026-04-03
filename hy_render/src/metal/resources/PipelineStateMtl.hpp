#pragma once

#include "../../core/HYResourceCommon.hpp"

namespace hyengine {
namespace render {

/**
 * @brief Metal 管线状态实现
 */
class PipelineStateMtl : public HYPipelineState {
public:
    PipelineStateMtl(const PipelineStateDesc& desc, void* device);
    virtual ~PipelineStateMtl();

    void enableBlend(bool flag) override;
    void enableDepth(bool flag) override;
    void enableStencil(bool flag) override;
    void setCullMode(CullMode mode) override;
    void enableScissor(bool flag) override;
    void setPrimitive(PrimitiveType mode) override;
    void apply();  // Metal特有方法，不是override
    bool build(void* shader, void* vertexDesc);  // 构建Metal管线状态
    const PipelineStateDesc& getDesc() const override { return mDesc; }
    ResourceHandle getResourceHandle() const override;

private:
    PipelineStateDesc mDesc;
    void* mRenderPipelineState = nullptr;  // id<MTLRenderPipelineState>
    void* mDepthStencilState = nullptr;    // id<MTLDepthStencilState>
    void* mDevice = nullptr;               // id<MTLDevice>
};

} // namespace render
} // namespace hyengine
