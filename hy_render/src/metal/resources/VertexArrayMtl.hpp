#pragma once

#include "../../core/HYResourceCommon.hpp"

namespace hyengine {
namespace render {

/**
 * @brief Metal 顶点数组实现
 * 
 * 注意：Metal 没有 VAO 概念
 * 顶点描述通过 MTLVertexDescriptor 设置
 */
class VertexArrayMtl : public HYVertexArray {
public:
    VertexArrayMtl() = default;
    virtual ~VertexArrayMtl() = default;

    void setVertexBuffer(uint32_t binding, const std::shared_ptr<HYBuffer>& buffer,
                        uint32_t offset, uint32_t stride) override;
    void setIndexBuffer(const std::shared_ptr<HYBuffer>& buffer) override;
    void setVertexAttribute(uint32_t location, uint32_t binding,
                           VertexFormat format, uint32_t offset) override;
    void bind() override;
    void unbind() override;
    ResourceHandle getResourceHandle() const override;

private:
    std::shared_ptr<HYBuffer> mVertexBuffer;
    std::shared_ptr<HYBuffer> mIndexBuffer;
    void* mVertexDescriptor = nullptr;  // MTLVertexDescriptor
};

} // namespace render
} // namespace hyengine
