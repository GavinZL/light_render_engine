#pragma once

#include "../../core/HYResourceCommon.hpp"

namespace hyengine {
namespace render {

/**
 * @brief Metal 缓冲区实现
 * 
 * Metal 缓冲区特点：
 * - 使用 MTLBuffer 对象
 * - 支持共享和私有存储模式
 * - 可以使用托管模式（macOS）
 * - 支持无缝的 CPU-GPU 数据传输
 * 
 * 根据5.1.1优化增加：
 * - map/unmap接口（Metal原生支持）
 * - isMapped查询接口
 */
class BufferMtl : public HYBuffer {
public:
    BufferMtl(const BufferDesc& desc, void* device);
    virtual ~BufferMtl();

    // 基本接口
    void updateData(const void* data, uint32_t offset, uint32_t size) override;
    const BufferDesc& getDesc() const override { return mDesc; }
    size_t getSize() const override { return mDesc.size; }
    uint32_t count() const override;
    ResourceHandle getResourceHandle() const override;

    // 5.1.1 新增的优化接口
    void* map(uint32_t offset, uint32_t size, MemoryAccess access) override;
    void unmap() override;
    bool isMapped() const override { return mIsMapped; }

private:
    BufferDesc mDesc;
    void* mBuffer = nullptr;  // id<MTLBuffer>
    void* mDevice = nullptr;  // id<MTLDevice>
    bool mIsMapped = false;   // 跟踪映射状态
    void* mMappedPtr = nullptr; // 映射指针
};

} // namespace render
} // namespace hyengine
