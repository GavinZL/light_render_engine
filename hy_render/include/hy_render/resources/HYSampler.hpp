#pragma once

#include "../HYRenderDef.hpp"

namespace hyengine {
namespace render {

/**
 * @brief 采样器对象抽象
 */
class HYSampler : public HYResource {
public:
    HYSampler() : HYResource(ResourceType::kSampler) {}
    virtual ~HYSampler() = default;

    /**
     * @brief 获取采样器描述
     */
    virtual const SamplerDesc& getDesc() const = 0;

    /**
     * @brief 绑定到纹理单元
     */
    virtual void bind(uint32_t unit) = 0;
};

} // namespace render
} // namespace hyengine
