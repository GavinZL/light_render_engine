#pragma once

#include "../../core/HYResourceCommon.hpp"

namespace hyengine {
namespace render {

/**
 * @brief OpenGL ES 采样器实现
 * 
 * 重要：OpenGL ES 3.0 不支持分离式采样器对象（glSamplerObject API）
 * 采样器状态必须通过 glTexParameteri 设置到纹理对象上
 * 
 * 这个类主要用于存储采样器描述，实际应用时需要将状态应用到纹理对象
 */
class SamplerGLES : public HYSampler {
public:
    explicit SamplerGLES(const SamplerDesc& desc);
    virtual ~SamplerGLES() = default;

    void bind(uint32_t unit) override;
    const SamplerDesc& getDesc() const override { return mDesc; }
    ResourceHandle getResourceHandle() const override;

private:
    SamplerDesc mDesc;
};

} // namespace render
} // namespace hyengine
