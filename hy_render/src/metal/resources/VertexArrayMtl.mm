#include "VertexArrayMtl.hpp"
#include <iostream>

namespace hyengine {
namespace render {

void VertexArrayMtl::setVertexBuffer(uint32_t binding, const std::shared_ptr<HYBuffer>& buffer, uint32_t offset, uint32_t stride) {
    mVertexBuffer = buffer;
    std::cout << "[VertexArrayMtl] 设置顶点缓冲区" << std::endl;
}

void VertexArrayMtl::setIndexBuffer(const std::shared_ptr<HYBuffer>& buffer) {
    mIndexBuffer = buffer;
    std::cout << "[VertexArrayMtl] 设置索引缓冲区" << std::endl;
}

void VertexArrayMtl::setVertexAttribute(uint32_t location, uint32_t binding, VertexFormat format, uint32_t offset) {
    std::cout << "[VertexArrayMtl] 设置顶点属性 " << location << std::endl;
}

void VertexArrayMtl::bind() {
    std::cout << "[VertexArrayMtl] 绑定顶点描述符" << std::endl;
}

void VertexArrayMtl::unbind() {}

ResourceHandle VertexArrayMtl::getResourceHandle() const {
    ResourceHandle handle;
    handle.ptrHandle = mVertexDescriptor;
    return handle;
}

} // namespace render
} // namespace hyengine
