#include "VertexArrayGLES.hpp"
#include "../../gl_common/GLUtils.hpp"
#include <iostream>

namespace hyengine {
namespace render {

// ============================================================================
// VertexArrayGLES 实现
// ============================================================================

VertexArrayGLES::VertexArrayGLES(bool vaoSupported) 
    : mVAOSupported(vaoSupported) {
    
    if (mVAOSupported) {
        // TODO: 在支持 VAO 的情况下（OES_vertex_array_object 扩展）
        // glGenVertexArraysOES(1, &mVAO);
        mVAO = 1; // 模拟ID
        std::cout << "[VertexArrayGLES] Created VAO (ID: " << mVAO << ")" << std::endl;
    } else {
        std::cout << "[VertexArrayGLES] VAO not supported, using legacy vertex attributes" << std::endl;
    }
}

VertexArrayGLES::~VertexArrayGLES() {
    if (mVAOSupported && mVAO != 0) {
        // TODO: glDeleteVertexArraysOES(1, &mVAO);
        std::cout << "[VertexArrayGLES] Destroyed VAO (ID: " << mVAO << ")" << std::endl;
        mVAO = 0;
    }
}

void VertexArrayGLES::setVertexBuffer(uint32_t binding, 
                                      const std::shared_ptr<HYBuffer>& buffer,
                                      uint32_t offset, 
                                      uint32_t stride) {
    mVertexBuffer = buffer;
    
    std::cout << "[VertexArrayGLES] setVertexBuffer(binding=" << binding 
              << ", offset=" << offset << ", stride=" << stride << ")" << std::endl;
    
    // TODO:
    // if (mVAOSupported) {
    //     glBindVertexArrayOES(mVAO);
    // }
    // GLuint vbo = static_cast<GLuint>(buffer->getResourceHandle().uintHandle);
    // glBindBuffer(GL_ARRAY_BUFFER, vbo);
    // if (mVAOSupported) {
    //     glBindVertexArrayOES(0);
    // }
}

void VertexArrayGLES::setIndexBuffer(const std::shared_ptr<HYBuffer>& buffer) {
    mIndexBuffer = buffer;
    
    std::cout << "[VertexArrayGLES] setIndexBuffer" << std::endl;
    
    // TODO:
    // if (mVAOSupported) {
    //     glBindVertexArrayOES(mVAO);
    //     GLuint ibo = static_cast<GLuint>(buffer->getResourceHandle().uintHandle);
    //     glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    //     glBindVertexArrayOES(0);
    // } else {
    //     // 在不支持VAO时，需要在绘制时手动绑定
    // }
}

void VertexArrayGLES::setVertexAttribute(uint32_t location,
                                         uint32_t binding,
                                         VertexFormat format,
                                         uint32_t offset) {
    std::cout << "[VertexArrayGLES] setVertexAttribute(location=" << location 
              << ", binding=" << binding << ", offset=" << offset << ")" << std::endl;
    
    // TODO: 根据 format 确定组件数量和类型
    // GLint size;
    // GLenum type;
    // GLboolean normalized = GL_FALSE;
    // 
    // switch (format) {
    //     case VertexFormat::kFloat:  size = 1; type = GL_FLOAT; break;
    //     case VertexFormat::kFloat2: size = 2; type = GL_FLOAT; break;
    //     case VertexFormat::kFloat3: size = 3; type = GL_FLOAT; break;
    //     case VertexFormat::kFloat4: size = 4; type = GL_FLOAT; break;
    //     case VertexFormat::kInt:    size = 1; type = GL_INT; break;
    //     case VertexFormat::kInt2:   size = 2; type = GL_INT; break;
    //     case VertexFormat::kInt3:   size = 3; type = GL_INT; break;
    //     case VertexFormat::kInt4:   size = 4; type = GL_INT; break;
    //     default: size = 3; type = GL_FLOAT; break;
    // }
    // 
    // if (mVAOSupported) {
    //     glBindVertexArrayOES(mVAO);
    // }
    // 
    // // 需要先绑定对应的 VBO
    // glBindBuffer(GL_ARRAY_BUFFER, vbo_id);
    // glVertexAttribPointer(location, size, type, normalized, stride, (void*)offset);
    // glEnableVertexAttribArray(location);
    // 
    // if (mVAOSupported) {
    //     glBindVertexArrayOES(0);
    // }
}

void VertexArrayGLES::bind() {
    if (mVAOSupported) {
        std::cout << "[VertexArrayGLES] Binding VAO (ID: " << mVAO << ")" << std::endl;
        // TODO: glBindVertexArrayOES(mVAO);
    } else {
        std::cout << "[VertexArrayGLES] Binding vertex attributes (legacy mode)" << std::endl;
        // TODO: 在不支持VAO时，需要手动设置所有顶点属性
        // 这通常在渲染循环中完成
    }
}

void VertexArrayGLES::unbind() {
    if (mVAOSupported) {
        std::cout << "[VertexArrayGLES] Unbinding VAO" << std::endl;
        // TODO: glBindVertexArrayOES(0);
    } else {
        std::cout << "[VertexArrayGLES] Unbinding vertex attributes (legacy mode)" << std::endl;
        // TODO: 禁用所有顶点属性
        // for (each enabled attribute) {
        //     glDisableVertexAttribArray(location);
        // }
    }
}

ResourceHandle VertexArrayGLES::getResourceHandle() const {
    ResourceHandle handle;
    handle.uintHandle = mVAO;
    return handle;
}

} // namespace render
} // namespace hyengine
