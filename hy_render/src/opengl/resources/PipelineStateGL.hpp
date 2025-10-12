#pragma once

#include "../../core/HYResourceCommon.hpp"

namespace hyengine {
namespace render {

/**
 * @brief OpenGL管线状态实现
 */
class PipelineStateGL : public HYPipelineState {
public:
    explicit PipelineStateGL(const PipelineStateDesc& desc);
    virtual ~PipelineStateGL();

    void enableBlend(bool flag) override;
    void enableDepth(bool flag) override;
    void enableStencil(bool flag) override;
    void enableScissor(bool flag) override;
    void setPrimitive(PrimitiveType mode) override;
    void setCullMode(CullMode mode) override;
    const PipelineStateDesc& getDesc() const override { return mDesc; }
    ResourceHandle getResourceHandle() const override;

private:
    PipelineStateDesc mDesc;
};

} // namespace render
} // namespace hyengine
