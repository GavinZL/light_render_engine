#pragma once

#include "../../core/HYResourceCommon.hpp"

namespace hyengine {
namespace render {

/**
 * @brief OpenGL ES 同步栅栏对象实现
 * 
 * OpenGL ES 3.0 支持 glFenceSync 和同步对象
 */
class FenceGLES : public HYFence {
public:
    FenceGLES();
    virtual ~FenceGLES();

    void wait() override;
    void reset() override;
    bool isSignaled() const override;
    ResourceHandle getResourceHandle() const override;

private:
    void* mSync = nullptr;  // GLsync
    mutable bool mSignaled = false;
};

} // namespace render
} // namespace hyengine
