#pragma once

#include "../../core/HYResourceCommon.hpp"

namespace hyengine {
namespace render {

/**
 * @brief OpenGL ES 顶点数组对象实现
 * 
 * 注意：OpenGL ES 3.0 中 VAO 是可选的（通过 OES_vertex_array_object 扩展）
 * 如果不支持 VAO，将使用传统的顶点属性绑定方式
 */
class VertexArrayGLES : public HYVertexArray {
public:
    explicit VertexArrayGLES(bool vaoSupported = false);
    virtual ~VertexArrayGLES();

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
    bool mVAOSupported = false;
    std::shared_ptr<HYBuffer> mVertexBuffer;
    std::shared_ptr<HYBuffer> mIndexBuffer;
};

} // namespace render
} // namespace hyengine
