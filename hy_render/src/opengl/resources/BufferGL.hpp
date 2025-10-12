#pragma once

#include "../../core/HYResourceCommon.hpp"

namespace hyengine {
namespace render {

/**
 * @brief OpenGL缓冲区实现
 * 
 * 根据5.1.1优化增加：
 * - map/unmap接口（使用glMapBufferRange）
 * - isMapped查询接口
 */
class BufferGL : public HYBuffer {
public:
    explicit BufferGL(const BufferDesc& desc);
    virtual ~BufferGL();

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
    unsigned int mBufferID = 0;
    bool mIsMapped = false; // 跟踪映射状态
};

} // namespace render
} // namespace hyengine
