#pragma once

#include "../../core/HYResourceCommon.hpp"

namespace hyengine {
namespace render {

/**
 * @brief Metal 采样器实现
 */
class SamplerMtl : public HYSampler {
public:
    SamplerMtl(const SamplerDesc& desc, void* device);
    virtual ~SamplerMtl() = default;

    void bind(uint32_t unit) override;
    const SamplerDesc& getDesc() const override { return mDesc; }
    ResourceHandle getResourceHandle() const override;

private:
    SamplerDesc mDesc;
    void* mSamplerState = nullptr;  // id<MTLSamplerState>
};

} // namespace render
} // namespace hyengine
