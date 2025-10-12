#pragma once
#include "hy_render/resources/HY.hpp"
namespace hyengine { namespace render {
class SamplerGL : public HY {
public:
    SamplerGL() = default;
    virtual ~SamplerGL() = default;
    ResourceHandle getResourceHandle() const override { return ResourceHandle(); }
};
}}
