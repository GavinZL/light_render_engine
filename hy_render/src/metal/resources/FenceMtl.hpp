#pragma once

#include "../../core/HYResourceCommon.hpp"

namespace hyengine {
namespace render {

/**
 * @brief Metal 围栏实现
 */
class FenceMtl : public HYFence {
public:
    explicit FenceMtl(void* device);
    virtual ~FenceMtl() = default;

    void wait() override;
    void reset() override;
    bool isSignaled() const override;
    ResourceHandle getResourceHandle() const override;

private:
    void* mDevice = nullptr;  // id<MTLDevice>
    void* mEvent = nullptr;   // id<MTLEvent>
    bool mSignaled = false;
    uint64_t mEventValue = 0;
};

} // namespace render
} // namespace hyengine
