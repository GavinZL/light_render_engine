#pragma once

#include "../../core/HYResourceCommon.hpp"

namespace hyengine {
namespace render {

/**
 * @brief OpenGL ES 缓冲区实现
 * 
 * OpenGL ES 3.0 支持的缓冲区类型：
 * - GL_ARRAY_BUFFER（顶点缓冲）
 * - GL_ELEMENT_ARRAY_BUFFER（索引缓冲）
 * - GL_UNIFORM_BUFFER（统一缓冲，ES 3.0+）
 * - GL_PIXEL_PACK_BUFFER（像素打包缓冲，ES 3.0+）
 * - GL_PIXEL_UNPACK_BUFFER（像素解包缓冲，ES 3.0+）
 * - GL_COPY_READ_BUFFER（ES 3.0+）
 * - GL_COPY_WRITE_BUFFER（ES 3.0+）
 * - GL_TRANSFORM_FEEDBACK_BUFFER（ES 3.0+）
 * 
 * 根据5.1.1优化增加：
 * - map/unmap接口（使用glMapBufferRange）
 * - isMapped查询接口
 */
class BufferGLES : public HYBuffer {
public:
    explicit BufferGLES(const BufferDesc& desc);
    virtual ~BufferGLES();

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
