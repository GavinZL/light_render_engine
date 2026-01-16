#include "BufferGLES.hpp"
#include "../../gl_common/GLUtils.hpp"
#include <iostream>
#include <cstring>

namespace hyengine {
namespace render {

// ============================================================================
// BufferGLES 实现
// ============================================================================

BufferGLES::BufferGLES(const BufferDesc& desc) : mDesc(desc) {
    std::cout << "[BufferGLES] 创建缓冲区 (size: " << desc.size << " bytes)" << std::endl;
    
    // TODO: 在实际的OpenGL ES环境中，这里应该调用：
    // glGenBuffers(1, &mBufferID);
    // 
    // // 转换缓冲区目标类型
    // GLenum target = GLUtils::bufferBindFlagToGLTarget(desc.bindFlags);
    // glBindBuffer(target, mBufferID);
    // 
    // // 转换使用模式
    // GLenum usage = GLUtils::bufferUsageToGL(desc.usage);
    // 
    // // 创建缓冲区
    // glBufferData(target, desc.size, desc.initialData, usage);
    // glBindBuffer(target, 0);
    
    // 模拟Buffer ID
    mBufferID = 1;
    
    // 输出缓冲区信息
    std::cout << "[BufferGLES] 缓冲区类型: ";
    if (desc.bindFlags & BIND_VERTEX_BUFFER) std::cout << "VERTEX ";
    if (desc.bindFlags & BIND_INDEX_BUFFER) std::cout << "INDEX ";
    if (desc.bindFlags & BIND_UNIFORM_BUFFER) std::cout << "UNIFORM ";
    std::cout << std::endl;
    
    std::cout << "[BufferGLES] 使用模式: ";
    switch (desc.usage) {
        case BufferUsage::USAGE_STATIC_DRAW: std::cout << "STATIC_DRAW"; break;
        case BufferUsage::USAGE_DYNAMIC_DRAW: std::cout << "DYNAMIC_DRAW"; break;
        case BufferUsage::USAGE_STREAM_DRAW: std::cout << "STREAM_DRAW"; break;
    }
    std::cout << std::endl;
}

BufferGLES::~BufferGLES() {
    if (mBufferID != 0) {
        // TODO: 在实际的OpenGL ES环境中，这里应该调用：
        // glDeleteBuffers(1, &mBufferID);
        
        std::cout << "[BufferGLES] Destroyed buffer (simulated ID: " << mBufferID << ")" << std::endl;
        mBufferID = 0;
    }
}

void BufferGLES::updateData(const void* data, uint32_t offset, uint32_t size) {
    if (data == nullptr || size == 0) {
        std::cerr << "[BufferGLES] 错误: 无效的数据或大小!" << std::endl;
        return;
    }
    
    if (offset + size > mDesc.size) {
        std::cerr << "[BufferGLES] 错误: 数据超出缓冲区大小!" << std::endl;
        return;
    }
    
    std::cout << "[BufferGLES] 更新缓冲区数据 (offset: " << offset 
              << ", size: " << size << ")" << std::endl;
    
    // TODO: 在实际的OpenGL ES环境中：
    // GLenum target = GLUtils::bufferBindFlagToGLTarget(mDesc.bindFlags);
    // glBindBuffer(target, mBufferID);
    // 
    // // OpenGL ES 3.0+ 支持glBufferSubData
    // glBufferSubData(target, offset, size, data);
    // 
    // // 或者使用glMapBufferRange (ES 3.0+)
    // void* ptr = glMapBufferRange(target, offset, size, 
    //                               GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_RANGE_BIT);
    // if (ptr) {
    //     memcpy(ptr, data, size);
    //     glUnmapBuffer(target);
    // }
    // 
    // glBindBuffer(target, 0);
    // GLUtils::checkGLError("BufferGLES::updateData");
}

uint32_t BufferGLES::count() const {
    // 如果是索引缓冲区，返回索引数量
    if (mDesc.bindFlags & BIND_INDEX_BUFFER) {
        // 假设使用uint16_t索引
        return mDesc.size / sizeof(uint16_t);
    }
    
    // 如果是Uniform Buffer，返回1（一个统一块）
    if (mDesc.bindFlags & BIND_UNIFORM_BUFFER) {
        return 1;
    }
    
    // 如果是顶点缓冲区，根据布局计算顶点数
    if (!mDesc.layoutElements.empty()) {
        uint32_t stride = mDesc.layoutElements[0].stride;
        if (stride > 0) {
            return mDesc.size / stride;
        }
    }
    
    // 默认情况，假设每个顶点3个float（位置）
    return mDesc.size / (3 * sizeof(float));
}

// ==== 5.1.1 新增的优化接口 ====
void* BufferGLES::map(uint32_t offset, uint32_t size, MemoryAccess access) {
    if (mIsMapped) {
        std::cerr << "[BufferGLES] 错误: 缓冲区已经映射!" << std::endl;
        return nullptr;
    }
    
    if (offset + size > mDesc.size) {
        std::cerr << "[BufferGLES] 错误: 映射范围超出缓冲区大小!" << std::endl;
        return nullptr;
    }
    
    std::cout << "[BufferGLES] 映射缓冲区 (offset: " << offset 
              << ", size: " << size << ", access: ";
    
    // 输出访问模式
    switch (access) {
        case MemoryAccess::READ_ONLY: std::cout << "READ_ONLY"; break;
        case MemoryAccess::WRITE_ONLY: std::cout << "WRITE_ONLY"; break;
        case MemoryAccess::READ_WRITE: std::cout << "READ_WRITE"; break;
    }
    std::cout << ")" << std::endl;
    
    // TODO: 在实际的OpenGL ES环境中：
    // GLenum target = GLUtils::bufferBindFlagToGLTarget(mDesc.bindFlags);
    // glBindBuffer(target, mBufferID);
    // 
    // // 转换访问模式
    // GLbitfield accessBits = 0;
    // if (access == MemoryAccess::READ_ONLY) {
    //     accessBits = GL_MAP_READ_BIT;
    // } else if (access == MemoryAccess::WRITE_ONLY) {
    //     accessBits = GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_RANGE_BIT;
    // } else {
    //     accessBits = GL_MAP_READ_BIT | GL_MAP_WRITE_BIT;
    // }
    // 
    // void* ptr = glMapBufferRange(target, offset, size, accessBits);
    // if (ptr) {
    //     mIsMapped = true;
    // }
    // glBindBuffer(target, 0);
    // return ptr;
    
    // 模拟映射成功
    mIsMapped = true;
    return reinterpret_cast<void*>(0x12345678); // 模拟指针
}

void BufferGLES::unmap() {
    if (!mIsMapped) {
        std::cerr << "[BufferGLES] 错误: 缓冲区没有映射!" << std::endl;
        return;
    }
    
    std::cout << "[BufferGLES] 取消缓冲区映射" << std::endl;
    
    // TODO: 在实际的OpenGL ES环境中：
    // GLenum target = GLUtils::bufferBindFlagToGLTarget(mDesc.bindFlags);
    // glBindBuffer(target, mBufferID);
    // glUnmapBuffer(target);
    // glBindBuffer(target, 0);
    
    mIsMapped = false;
}

ResourceHandle BufferGLES::getResourceHandle() const {
    ResourceHandle handle;
    handle.uintHandle = mBufferID;
    return handle;
}

} // namespace render
} // namespace hyengine
