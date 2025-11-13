#include "BufferMtl.hpp"
#include <iostream>
#include <cstring>

namespace hyengine {
namespace render {

BufferMtl::BufferMtl(const BufferDesc& desc, void* device) 
    : mDesc(desc), mDevice(device) {
    
    std::cout << "[BufferMtl] 创建Metal缓冲区 (size: " << desc.size << " bytes)" << std::endl;
    
    // TODO: 实际实现中应该：
    // MTLResourceOptions options = MTLResourceStorageModeShared;
    // 
    // switch (desc.usage) {
    //     case BufferUsage::USAGE_STATIC_DRAW:
    //         options = MTLResourceStorageModePrivate;
    //         break;
    //     case BufferUsage::USAGE_DYNAMIC_DRAW:
    //     case BufferUsage::USAGE_STREAM_DRAW:
    //         options = MTLResourceStorageModeShared;
    //         break;
    // }
    //
    // mBuffer = [(__bridge id<MTLDevice>)mDevice newBufferWithLength:desc.size options:options];
    //
    // if (desc.initialData) {
    //     if (options == MTLResourceStorageModeShared) {
    //         memcpy([mBuffer contents], desc.initialData, desc.size);
    //     } else {
    //         // 使用临时缓冲区传输数据
    //     }
    // }
    
    mBuffer = reinterpret_cast<void*>(0x100);
    
    std::cout << "[BufferMtl] 缓冲区类型: ";
    if (desc.bindFlags & BIND_VERTEX_BUFFER) std::cout << "VERTEX ";
    if (desc.bindFlags & BIND_INDEX_BUFFER) std::cout << "INDEX ";
    if (desc.bindFlags & BIND_UNIFORM_BUFFER) std::cout << "UNIFORM ";
    std::cout << std::endl;
}

BufferMtl::~BufferMtl() {
    if (mBuffer) {
        // TODO: [mBuffer release];
        std::cout << "[BufferMtl] 销毁Metal缓冲区" << std::endl;
        mBuffer = nullptr;
    }
}

void BufferMtl::updateData(const void* data, uint32_t offset, uint32_t size) {
    if (!data || size == 0) {
        std::cerr << "[BufferMtl] 错误: 无效数据" << std::endl;
        return;
    }
    
    if (offset + size > mDesc.size) {
        std::cerr << "[BufferMtl] 错误: 数据超出缓冲区范围" << std::endl;
        return;
    }
    
    std::cout << "[BufferMtl] 更新数据 (offset: " << offset << ", size: " << size << ")" << std::endl;
    
    // TODO: 实际实现
    // uint8_t* bufferPointer = (uint8_t*)[mBuffer contents];
    // memcpy(bufferPointer + offset, data, size);
}

uint32_t BufferMtl::count() const {
    // 如果是索引缓冲区，返回索引数量
    if (mDesc.bindFlags & BIND_INDEX_BUFFER) {
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
void* BufferMtl::map(uint32_t offset, uint32_t size, MemoryAccess access) {
    if (mIsMapped) {
        std::cerr << "[BufferMtl] 错误: 缓冲区已经映射!" << std::endl;
        return nullptr;
    }
    
    if (offset + size > mDesc.size) {
        std::cerr << "[BufferMtl] 错误: 映射范围超出缓冲区大小!" << std::endl;
        return nullptr;
    }
    
    std::cout << "[BufferMtl] 映射Metal缓冲区 (offset: " << offset 
              << ", size: " << size << ", access: ";
    
    // 输出访问模式
    switch (access) {
        case MemoryAccess::READ_ONLY: std::cout << "READ_ONLY"; break;
        case MemoryAccess::WRITE_ONLY: std::cout << "WRITE_ONLY"; break;
        case MemoryAccess::READ_WRITE: std::cout << "READ_WRITE"; break;
    }
    std::cout << ")" << std::endl;
    
    // TODO: 在实际的Metal环境中：
    // if (mBuffer && [mBuffer storageMode] == MTLStorageModeShared) {
    //     // Shared模式直接返回内存指针
    //     mMappedPtr = (uint8_t*)[mBuffer contents] + offset;
    //     mIsMapped = true;
    //     return mMappedPtr;
    // } else {
    //     // Private模式需要使用BlitCommandEncoder复制数据
    //     std::cerr << "[BufferMtl] Private storage mode buffers cannot be mapped directly" << std::endl;
    //     return nullptr;
    // }
    
    // 模拟映射成功
    mIsMapped = true;
    mMappedPtr = reinterpret_cast<void*>(0x12345678 + offset); // 模拟指针
    return mMappedPtr;
}

void BufferMtl::unmap() {
    if (!mIsMapped) {
        std::cerr << "[BufferMtl] 错误: 缓冲区没有映射!" << std::endl;
        return;
    }
    
    std::cout << "[BufferMtl] 取消Metal缓冲区映射" << std::endl;
    
    // TODO: 在实际的Metal环境中：
    // Metal中的Shared模式缓冲区不需要显式取消映射
    // 只需要清理指针即可
    
    mIsMapped = false;
    mMappedPtr = nullptr;
}

ResourceHandle BufferMtl::getResourceHandle() const {
    ResourceHandle handle;
    handle.ptrHandle = mBuffer;
    return handle;
}

} // namespace render
} // namespace hyengine
