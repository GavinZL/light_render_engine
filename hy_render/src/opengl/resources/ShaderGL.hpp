#pragma once
#include "hy_render/resources/HY.hpp"
namespace hyengine { namespace render {
class ShaderGL : public HY {
public:
    ShaderGL() = default;
    virtual ~ShaderGL() = default;
    ResourceHandle getResourceHandle() const override { return ResourceHandle(); }
};
}}
