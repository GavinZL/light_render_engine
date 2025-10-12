#pragma once

#include "HYRenderDef.hpp"

namespace hyengine {
namespace render {

/**
 * @brief 缓冲区对象抽象
 */
class HYBuffer : public HYResource {
public:
    HYBuffer() : HYResource(ResourceType::kBuffer) {}
    virtual ~HYBuffer() = default;

    /**
     * @brief 更新缓冲区数据
     * @param data 数据指针
     * @param offset 偏移量（字节）
     * @param size 数据大小（字节）
     */
    virtual void updateData(const void* data, uint32_t offset, uint32_t size) = 0;

    /**
     * @brief 获取缓冲区描述
     */
    virtual const BufferDesc& getDesc() const = 0;

    /**
     * @brief 获取缓冲区大小
     */
    virtual size_t getSize() const = 0;

    /**
     * @brief 根据布局计算数据个数
     */
    virtual uint32_t count() const = 0;
};

} // namespace render
} // namespace hyengine
