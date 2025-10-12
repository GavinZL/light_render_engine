#pragma once
#include "hy_render/resources/HY.hpp"
namespace hyengine { namespace render {
class VertexArrayGL : public HY {
public:
    VertexArrayGL() = default;
    virtual ~VertexArrayGL() = default;
    ResourceHandle getResourceHandle() const override { return ResourceHandle(); }
};
}}
