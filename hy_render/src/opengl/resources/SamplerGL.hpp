#pragma once

#include "../../core/HYResourceCommon.hpp"

namespace hyengine {
namespace render {

/**
 * @brief OpenGL采样器实现
 */
class SamplerGL : public HYSampler {
public:
    explicit SamplerGL(const SamplerDesc& desc);
    virtual ~SamplerGL();

    const SamplerDesc& getDesc() const override { return mDesc; }
    void bind(uint32_t unit) override;
    ResourceHandle getResourceHandle() const override;

private:
    SamplerDesc mDesc;
    unsigned int mSamplerID = 0;
};

} // namespace render
} // namespace hyengine
