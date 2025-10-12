#include "BufferGL.hpp"
#include <iostream>
#include <cstring>

// OpenGL常量定义
#ifndef GL_ARRAY_BUFFER
#define GL_ARRAY_BUFFER 0x8892
#define GL_ELEMENT_ARRAY_BUFFER 0x8893
#define GL_UNIFORM_BUFFER 0x8A11
#define GL_STATIC_DRAW 0x88E4
#define GL_DYNAMIC_DRAW 0x88E8
#define GL_STREAM_DRAW 0x88E0
#endif

namespace hyengine {
namespace render {

// 辅助函数：将缓冲区绑定标志转换为GL目标
static unsigned int bufferBindFlagToGLTarget(uint8_t bindFlags) {
    if (bindFlags & BIND_VERTEX_BUFFER) {
        return GL_ARRAY_BUFFER;
    }
    if (bindFlags & BIND_INDEX_BUFFER) {
        return GL_ELEMENT_ARRAY_BUFFER;
    }
    if (bindFlags & BIND_UNIFORM_BUFFER) {
        return GL_UNIFORM_BUFFER;
    }
    return GL_ARRAY_BUFFER;
}

// 辅助函数：将缓冲区使用模式转换为GL使用模式
static unsigned int bufferUsageToGL(BufferUsage usage) {
    switch (usage) {
        case BufferUsage::USAGE_STATIC_DRAW:
            return GL_STATIC_DRAW;
        case BufferUsage::USAGE_DYNAMIC_DRAW:
            return GL_DYNAMIC_DRAW;
        case BufferUsage::USAGE_STREAM_DRAW:
            return GL_STREAM_DRAW;
        default:
            return GL_STATIC_DRAW;
    }
}

BufferGL::BufferGL(const BufferDesc& desc) : mDesc(desc) {
    // 简化实现：只存储描述，不实际创建GL对象
    // 在真实项目中这里会调用glGenBuffers等函数
    mBufferID = 1; // 假设的ID
    
    std::cout << "[BufferGL] Created buffer (simulated), size=" << mDesc.size 
              << " bytes, bindFlags=" << static_cast<int>(mDesc.bindFlags) << std::endl;
}

BufferGL::~BufferGL() {
    if (mBufferID != 0) {
        std::cout << "[BufferGL] Deleted buffer (simulated) ID=" << mBufferID << std::endl;
        mBufferID = 0;
    }
}

void BufferGL::updateData(const void* data, uint32_t offset, uint32_t size) {
    if (mBufferID == 0 || !data || size == 0) {
        std::cerr << "[BufferGL] Invalid parameters for updateData" << std::endl;
        return;
    }
    
    // 检查越界
    if (offset + size > mDesc.size) {
        std::cerr << "[BufferGL] Update data out of bounds: offset=" << offset 
                  << ", size=" << size << ", buffer_size=" << mDesc.size << std::endl;
        return;
    }
    
    // 简化实现：只打印日志，不实际更新GL缓冲区
    std::cout << "[BufferGL] Updated data (simulated): offset=" << offset 
              << ", size=" << size << " bytes" << std::endl;
}

uint32_t BufferGL::count() const {
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
    if (!mDesc.layoutElements.empty() && mDesc.layoutElements[0].stride > 0) {
        return static_cast<uint32_t>(mDesc.size / mDesc.layoutElements[0].stride);
    }
    
    // 默认情况，假设每个顶点3个float（位置）
    return mDesc.size / (3 * sizeof(float));
}

// ==== 5.1.1 新增的优化接口 ====
void* BufferGL::map(uint32_t offset, uint32_t size, MemoryAccess access) {
    if (mIsMapped) {
        std::cerr << "[BufferGL] 错误: 缓冲区已经映射!" << std::endl;
        return nullptr;
    }
    
    if (offset + size > mDesc.size) {
        std::cerr << "[BufferGL] 错误: 映射范围超出缓冲区大小!" << std::endl;
        return nullptr;
    }
    
    std::cout << "[BufferGL] 映射缓冲区 (offset: " << offset 
              << ", size: " << size << ", access: ";
    
    // 输出访问模式
    switch (access) {
        case MemoryAccess::READ_ONLY: std::cout << "READ_ONLY"; break;
        case MemoryAccess::WRITE_ONLY: std::cout << "WRITE_ONLY"; break;
        case MemoryAccess::READ_WRITE: std::cout << "READ_WRITE"; break;
    }
    std::cout << ")" << std::endl;
    
    // TODO: 在实际的OpenGL环境中：
    // unsigned int target = bufferBindFlagToGLTarget(mDesc.bindFlags);
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

void BufferGL::unmap() {
    if (!mIsMapped) {
        std::cerr << "[BufferGL] 错误: 缓冲区没有映射!" << std::endl;
        return;
    }
    
    std::cout << "[BufferGL] 取消缓冲区映射" << std::endl;
    
    // TODO: 在实际的OpenGL环境中：
    // unsigned int target = bufferBindFlagToGLTarget(mDesc.bindFlags);
    // glBindBuffer(target, mBufferID);
    // glUnmapBuffer(target);
    // glBindBuffer(target, 0);
    
    mIsMapped = false;
}

ResourceHandle BufferGL::getResourceHandle() const {
    ResourceHandle handle;
    handle.uintHandle = mBufferID;
    return handle;
}

} // namespace render
} // namespace hyengine