#include "BufferMtl.hpp"
#import <Metal/Metal.h>
#include <iostream>
#include <cstring>

namespace hyengine {
namespace render {

BufferMtl::BufferMtl(const BufferDesc& desc, void* device) 
    : mDesc(desc), mDevice(device) {
    
    std::cout << "[BufferMtl] 创建Metal缓冲区 (size: " << desc.size << " bytes)" << std::endl;
    
    id<MTLDevice> mtlDevice = (__bridge id<MTLDevice>)device;
    if (!mtlDevice) {
        std::cerr << "[BufferMtl] 错误: Metal设备无效" << std::endl;
        return;
    }
    
    // 根据用途选择存储模式
    MTLResourceOptions options = MTLResourceStorageModeShared;
    
    switch (desc.usage) {
        case BufferUsage::USAGE_STATIC_DRAW:
            // 静态数据可以使用Private模式以获得更好性能
            #if TARGET_OS_OSX
            options = MTLResourceStorageModeManaged;
            #else
            options = MTLResourceStorageModeShared;
            #endif
            break;
        case BufferUsage::USAGE_DYNAMIC_DRAW:
        case BufferUsage::USAGE_STREAM_DRAW:
            // 动态数据使用Shared模式便于CPU访问
            options = MTLResourceStorageModeShared;
            break;
    }
    
    // 创建Metal缓冲区
    id<MTLBuffer> buffer = [mtlDevice newBufferWithLength:desc.size options:options];
    if (!buffer) {
        std::cerr << "[BufferMtl] 错误: 无法创建Metal缓冲区" << std::endl;
        return;
    }
    
    mBuffer = (__bridge_retained void*)buffer;
    
    // 如果有初始数据，复制到缓冲区
    if (desc.initialData && desc.size > 0) {
        memcpy([buffer contents], desc.initialData, desc.size);
        
        #if TARGET_OS_OSX
        if (options == MTLResourceStorageModeManaged) {
            [buffer didModifyRange:NSMakeRange(0, desc.size)];
        }
        #endif
    }
    
    std::cout << "[BufferMtl] 缓冲区类型: ";
    if (desc.bindFlags & BIND_VERTEX_BUFFER) std::cout << "VERTEX ";
    if (desc.bindFlags & BIND_INDEX_BUFFER) std::cout << "INDEX ";
    if (desc.bindFlags & BIND_UNIFORM_BUFFER) std::cout << "UNIFORM ";
    std::cout << std::endl;
}

BufferMtl::~BufferMtl() {
    if (mBuffer) {
        // 释放Metal缓冲区
        id<MTLBuffer> buffer = (__bridge_transfer id<MTLBuffer>)mBuffer;
        buffer = nil;
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
    
    id<MTLBuffer> buffer = (__bridge id<MTLBuffer>)mBuffer;
    if (!buffer) {
        std::cerr << "[BufferMtl] 错误: 缓冲区无效" << std::endl;
        return;
    }
    
    // 复制数据到缓冲区
    uint8_t* bufferPointer = (uint8_t*)[buffer contents];
    memcpy(bufferPointer + offset, data, size);
    
    #if TARGET_OS_OSX
    // macOS上如果是Managed模式，需要通知修改范围
    if ([buffer storageMode] == MTLStorageModeManaged) {
        [buffer didModifyRange:NSMakeRange(offset, size)];
    }
    #endif
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
    
    switch (access) {
        case MemoryAccess::READ_ONLY: std::cout << "READ_ONLY"; break;
        case MemoryAccess::WRITE_ONLY: std::cout << "WRITE_ONLY"; break;
        case MemoryAccess::READ_WRITE: std::cout << "READ_WRITE"; break;
    }
    std::cout << ")" << std::endl;
    
    id<MTLBuffer> buffer = (__bridge id<MTLBuffer>)mBuffer;
    if (!buffer) {
        std::cerr << "[BufferMtl] 错误: 缓冲区无效" << std::endl;
        return nullptr;
    }
    
    // Metal的Shared和Managed模式可以直接映射
    if ([buffer storageMode] == MTLStorageModeShared || 
        [buffer storageMode] == MTLStorageModeManaged) {
        mMappedPtr = (uint8_t*)[buffer contents] + offset;
        mIsMapped = true;
        return mMappedPtr;
    } else {
        std::cerr << "[BufferMtl] 错误: Private存储模式的缓冲区无法直接映射" << std::endl;
        return nullptr;
    }
}

void BufferMtl::unmap() {
    if (!mIsMapped) {
        std::cerr << "[BufferMtl] 错误: 缓冲区没有映射!" << std::endl;
        return;
    }
    
    std::cout << "[BufferMtl] 取消Metal缓冲区映射" << std::endl;
    
    #if TARGET_OS_OSX
    // macOS上如果是Managed模式，需要通知修改
    id<MTLBuffer> buffer = (__bridge id<MTLBuffer>)mBuffer;
    if (buffer && [buffer storageMode] == MTLStorageModeManaged) {
        [buffer didModifyRange:NSMakeRange(0, mDesc.size)];
    }
    #endif
    
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
