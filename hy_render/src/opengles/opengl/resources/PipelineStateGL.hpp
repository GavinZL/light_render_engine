#pragma once
#include "hy_render/resources/HY.hpp"
namespace hyengine { namespace render {
class PipelineStateGL : public HY {
public:
    PipelineStateGL() = default;
    virtual ~PipelineStateGL() = default;
    ResourceHandle getResourceHandle() const override { return ResourceHandle(); }
};
}}
