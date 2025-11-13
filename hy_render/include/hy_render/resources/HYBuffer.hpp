#pragma once

#include "../HYRenderDef.hpp"

namespace hyengine {
namespace render {

/**
 * @brief 缓冲区对象抽象
 * 
 * 根据设计文档5.1.1优化：
 * - 增加内存映射接口（map/unmap用于Vulkan/Metal）
 * - 增加查询接口（getSize、isMapped等）
 * - layoutElements设为可选项
 */
class HYBuffer : public HYResource {
public:
    HYBuffer() : HYResource(ResourceType::kBuffer) {}
    virtual ~HYBuffer() = default;

    // ==== 数据更新接口 ====
    /**
     * @brief 更新缓冲区数据
     * @param data 数据指针
     * @param offset 偏移量（字节）
     * @param size 数据大小（字节）
     */
    virtual void updateData(const void* data, uint32_t offset, uint32_t size) = 0;

    // ==== 内存映射接口（Vulkan/Metal用） ====
    /**
     * @brief 映射到CPU内存
     * @param offset 偏移量
     * @param size 映射大小
     * @param access 访问模式
     * @return 映射的内存指针，失败返回nullptr
     */
    virtual void* map(uint32_t offset, uint32_t size, MemoryAccess access) = 0;
    
    /**
     * @brief 取消映射
     */
    virtual void unmap() = 0;

    // ==== 查询接口 ====
    /**
     * @brief 获取缓冲区描述
     */
    virtual const BufferDesc& getDesc() const = 0;

    /**
     * @brief 获取缓冲区大小
     */
    virtual size_t getSize() const = 0;

    /**
     * @brief 是否已映射
     */
    virtual bool isMapped() const = 0;

    /**
     * @brief 根据布局计算数据个数
     * @note 仅适用于Vertex Buffer，其他类型返回0
     */
    virtual uint32_t count() const = 0;
};

} // namespace render
} // namespace hyengine
