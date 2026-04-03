#include "VertexArrayGL.hpp"
#include <iostream>

namespace hyengine {
namespace render {

// ============================================================================
// VertexArrayGL 实现
// ============================================================================

VertexArrayGL::VertexArrayGL() {
    // TODO: 在实际的OpenGL环境中，这里应该调用：
    // glGenVertexArrays(1, &mVAO);
    
    // 现在使用模拟ID
    mVAO = 1; // 模拟VAO ID
    std::cout << "[VertexArrayGL] Created VAO (simulated ID: " << mVAO << ")" << std::endl;
}

VertexArrayGL::~VertexArrayGL() {
    if (mVAO != 0) {
        // TODO: 在实际的OpenGL环境中，这里应该调用：
        // glDeleteVertexArrays(1, &mVAO);
        std::cout << "[VertexArrayGL] Destroyed VAO (simulated ID: " << mVAO << ")" << std::endl;
        mVAO = 0;
    }
}

void VertexArrayGL::setVertexBuffer(uint32_t binding, 
                                    const std::shared_ptr<HYBuffer>& buffer,
                                    uint32_t offset, 
                                    uint32_t stride) {
    mVertexBuffer = buffer;
    
    std::cout << "[VertexArrayGL] setVertexBuffer("
              << "binding=" << binding 
              << ", offset=" << offset 
              << ", stride=" << stride << ")" << std::endl;
    
    // TODO: 在实际的OpenGL环境中，这里应该：
    // 1. 绑定VAO: glBindVertexArray(mVAO);
    // 2. 绑定VBO: glBindBuffer(GL_ARRAY_BUFFER, bufferGL_ID);
    // 3. 如果使用OpenGL 4.3+或ARB_vertex_attrib_binding扩展：
    //    glBindVertexBuffer(binding, bufferGL_ID, offset, stride);
    // 4. 如果不支持分离式绑定，则在setVertexAttribute中处理
}

void VertexArrayGL::setIndexBuffer(const std::shared_ptr<HYBuffer>& buffer) {
    mIndexBuffer = buffer;
    
    std::cout << "[VertexArrayGL] setIndexBuffer" << std::endl;
    
    // TODO: 在实际的OpenGL环境中，这里应该：
    // 1. 绑定VAO: glBindVertexArray(mVAO);
    // 2. 绑定IBO: glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufferGL_ID);
    // 
    // 注意：索引缓冲的绑定会被VAO记住，这是VAO的重要特性之一
}

void VertexArrayGL::setVertexAttribute(uint32_t location,
                                       uint32_t binding,
                                       VertexFormat format,
                                       uint32_t offset) {
    std::cout << "[VertexArrayGL] setVertexAttribute("
              << "location=" << location 
              << ", binding=" << binding 
              << ", offset=" << offset << ")" << std::endl;
    
    // TODO: 在实际的OpenGL环境中，这里应该：
    // 1. 绑定VAO: glBindVertexArray(mVAO);
    // 2. 根据format确定组件数量和类型：
    //    例如：VertexFormat::kFloat3 -> size=3, type=GL_FLOAT
    // 3. 如果使用OpenGL 4.3+或ARB_vertex_attrib_binding扩展：
    //    glVertexAttribFormat(location, size, type, normalized, offset);
    //    glVertexAttribBinding(location, binding);
    //    glEnableVertexAttribArray(location);
    // 4. 如果不支持分离式绑定，则使用传统方式：
    //    需要先绑定对应的VBO，然后调用：
    //    glVertexAttribPointer(location, size, type, normalized, stride, (void*)offset);
    //    glEnableVertexAttribArray(location);
    
    // 示例代码（需要根据实际format转换）：
    /*
    GLint size;
    GLenum type;
    GLboolean normalized = GL_FALSE;
    
    switch (format) {
        case VertexFormat::kFloat:
            size = 1; type = GL_FLOAT; break;
        case VertexFormat::kFloat2:
            size = 2; type = GL_FLOAT; break;
        case VertexFormat::kFloat3:
            size = 3; type = GL_FLOAT; break;
        case VertexFormat::kFloat4:
            size = 4; type = GL_FLOAT; break;
        case VertexFormat::kInt:
            size = 1; type = GL_INT; break;
        case VertexFormat::kInt2:
            size = 2; type = GL_INT; break;
        case VertexFormat::kInt3:
            size = 3; type = GL_INT; break;
        case VertexFormat::kInt4:
            size = 4; type = GL_INT; break;
        case VertexFormat::kUInt:
            size = 1; type = GL_UNSIGNED_INT; break;
        case VertexFormat::kUInt2:
            size = 2; type = GL_UNSIGNED_INT; break;
        case VertexFormat::kUInt3:
            size = 3; type = GL_UNSIGNED_INT; break;
        case VertexFormat::kUInt4:
            size = 4; type = GL_UNSIGNED_INT; break;
        default:
            size = 3; type = GL_FLOAT; break;
    }
    
    glBindVertexArray(mVAO);
    
    // 分离式绑定（推荐）
    glVertexAttribFormat(location, size, type, normalized, offset);
    glVertexAttribBinding(location, binding);
    glEnableVertexAttribArray(location);
    
    // 或传统方式
    // glBindBuffer(GL_ARRAY_BUFFER, vbo_id);
    // glVertexAttribPointer(location, size, type, normalized, stride, (void*)offset);
    // glEnableVertexAttribArray(location);
    */
}

void VertexArrayGL::bind() {
    std::cout << "[VertexArrayGL] Binding VAO (simulated ID: " << mVAO << ")" << std::endl;
    
    // TODO: 在实际的OpenGL环境中，这里应该调用：
    // glBindVertexArray(mVAO);
    //
    // 这会自动恢复：
    // - 所有之前设置的顶点属性配置
    // - 索引缓冲绑定（如果有的话）
    // - 顶点缓冲绑定（如果使用分离式绑定）
}

void VertexArrayGL::unbind() {
    std::cout << "[VertexArrayGL] Unbinding VAO" << std::endl;
    
    // TODO: 在实际的OpenGL环境中，这里应该调用：
    // glBindVertexArray(0);
    //
    // 解绑VAO后，顶点属性状态将恢复到默认状态
}

ResourceHandle VertexArrayGL::getResourceHandle() const {
    ResourceHandle handle;
    handle.uintHandle = mVAO;
    return handle;
}

} // namespace render
} // namespace hyengine
