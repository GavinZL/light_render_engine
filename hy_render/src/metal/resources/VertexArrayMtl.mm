/**
 * @file VertexArrayMtl.mm
 * @brief Metal顶点数组实现
 * 
 * Metal没有VAO概念，使用MTLVertexDescriptor管理顶点布局
 */

#include "VertexArrayMtl.hpp"
#import <Metal/Metal.h>
#include <iostream>

namespace hyengine {
namespace render {

void VertexArrayMtl::setVertexBuffer(uint32_t binding, const std::shared_ptr<HYBuffer>& buffer,
                                     uint32_t offset, uint32_t stride) {
    std::cout << "[VertexArrayMtl] setVertexBuffer(binding=" << binding 
              << ", offset=" << offset << ", stride=" << stride << ")" << std::endl;
    
    if (binding == 0) {
        mVertexBuffer = buffer;
    }
}

void VertexArrayMtl::setIndexBuffer(const std::shared_ptr<HYBuffer>& buffer) {
    std::cout << "[VertexArrayMtl] setIndexBuffer" << std::endl;
    mIndexBuffer = buffer;
}

void VertexArrayMtl::setVertexAttribute(uint32_t location, uint32_t binding,
                                        VertexFormat format, uint32_t offset) {
    std::cout << "[VertexArrayMtl] setVertexAttribute(location=" << location 
              << ", binding=" << binding << ", offset=" << offset << ")" << std::endl;
    
    // Metal中顶点属性配置通过MTLVertexDescriptor在管线创建时设置
    // 这里只是记录配置，实际使用在渲染管线中
}

void VertexArrayMtl::bind() {
    std::cout << "[VertexArrayMtl] bind (Metal不需要显式绑定VAO)" << std::endl;
}

void VertexArrayMtl::unbind() {
    // Metal不需要unbind操作
}

ResourceHandle VertexArrayMtl::getResourceHandle() const {
    ResourceHandle handle;
    handle.ptrHandle = mVertexDescriptor;
    return handle;
}

} // namespace render
} // namespace hyengine
