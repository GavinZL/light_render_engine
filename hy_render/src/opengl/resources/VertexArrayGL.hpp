#pragma once

#include "../../core/HYResourceCommon.hpp"

namespace hyengine {
namespace render {

/**
 * @brief OpenGL顶点数组对象实现
 */
class VertexArrayGL : public HYVertexArray {
public:
    VertexArrayGL();
    virtual ~VertexArrayGL();

    void setVertexBuffer(uint32_t binding, const std::shared_ptr<HYBuffer>& buffer,
                        uint32_t offset, uint32_t stride) override;
    void setIndexBuffer(const std::shared_ptr<HYBuffer>& buffer) override;
    void setVertexAttribute(uint32_t location, uint32_t binding,
                           VertexFormat format, uint32_t offset) override;
    void bind() override;
    void unbind() override;
    ResourceHandle getResourceHandle() const override;

private:
    unsigned int mVAO = 0;
    std::shared_ptr<HYBuffer> mVertexBuffer;
    std::shared_ptr<HYBuffer> mIndexBuffer;
};

} // namespace render
} // namespace hyengine
