#pragma once
#include "hy_render/resources/HY.hpp"
namespace hyengine { namespace render {
class RenderPassGL : public HY {
public:
    RenderPassGL() = default;
    virtual ~RenderPassGL() = default;
    ResourceHandle getResourceHandle() const override { return ResourceHandle(); }
};
}}
