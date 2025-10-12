#pragma once

#include "hy_render/resources/HYBuffer.hpp"

namespace hyengine {
namespace render {

/**
 * @brief OpenGL缓冲区实现
 */
class BufferGL : public HYBuffer {
public:
    explicit BufferGL(const BufferDesc& desc);
    virtual ~BufferGL();

    void updateData(const void* data, uint32_t offset, uint32_t size) override;
    const BufferDesc& getDesc() const override { return mDesc; }
    size_t getSize() const override { return mDesc.size; }
    uint32_t count() const override;
    ResourceHandle getResourceHandle() const override;

private:
    BufferDesc mDesc;
    unsigned int mBufferID = 0;
};

} // namespace render
} // namespace hyengine
