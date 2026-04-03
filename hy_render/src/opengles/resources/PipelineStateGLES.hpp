#pragma once

#include "../../core/HYResourceCommon.hpp"

namespace hyengine {
namespace render {

/**
 * @brief OpenGL ES 管线状态实现
 */
class PipelineStateGLES : public HYPipelineState {
public:
    explicit PipelineStateGLES(const PipelineStateDesc& desc);
    virtual ~PipelineStateGLES() = default;

    void enableBlend(bool flag) override;
    void enableDepth(bool flag) override;
    void enableStencil(bool flag) override;
    void enableScissor(bool flag) override;
    void setPrimitive(PrimitiveType mode) override;
    void setCullMode(CullMode mode) override;
    const PipelineStateDesc& getDesc() const override { return mDesc; }
    ResourceHandle getResourceHandle() const override;

    void apply();  // 应用所有状态

private:
    PipelineStateDesc mDesc;
};

} // namespace render
} // namespace hyengine
