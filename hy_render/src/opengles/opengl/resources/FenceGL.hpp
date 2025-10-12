#pragma once
#include "hy_render/resources/HY.hpp"
namespace hyengine { namespace render {
class FenceGL : public HY {
public:
    FenceGL() = default;
    virtual ~FenceGL() = default;
    ResourceHandle getResourceHandle() const override { return ResourceHandle(); }
};
}}
