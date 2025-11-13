#pragma once

#include "../HYRenderDef.hpp"

namespace hyengine {
namespace render {

/**
 * @brief 同步栅栏对象抽象
 */
class HYFence : public HYResource {
public:
    HYFence() : HYResource(ResourceType::kFence) {}
    virtual ~HYFence() = default;

    /**
     * @brief 等待栅栏信号
     */
    virtual void wait() = 0;

    /**
     * @brief 重置栅栏
     */
    virtual void reset() = 0;

    /**
     * @brief 查询是否已触发
     */
    virtual bool isSignaled() const = 0;
};

} // namespace render
} // namespace hyengine
