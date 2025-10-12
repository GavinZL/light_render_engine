#pragma once
#include "hy_render/resources/HY.hpp"
namespace hyengine { namespace render {
class FrameBufferGL : public HY {
public:
    FrameBufferGL() = default;
    virtual ~FrameBufferGL() = default;
    ResourceHandle getResourceHandle() const override { return ResourceHandle(); }
};
}}
