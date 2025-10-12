#pragma once

#include "../HYRenderDef.hpp"
#include <memory>

namespace hyengine {
namespace render {

class HYBuffer;

/**
 * @brief 顶点数组对象抽象
 */
class HYVertexArray : public HYResource {
public:
    HYVertexArray() : HYResource(ResourceType::kVertexArray) {}
    virtual ~HYVertexArray() = default;

    /**
     * @brief 绑定顶点缓冲
     * @param binding 绑定点
     * @param buffer 缓冲区对象
     * @param offset 偏移量
     * @param stride 步长
     */
    virtual void setVertexBuffer(uint32_t binding, 
                                const std::shared_ptr<HYBuffer>& buffer,
                                uint32_t offset,
                                uint32_t stride) = 0;

    /**
     * @brief 绑定索引缓冲
     * @param buffer 索引缓冲区
     */
    virtual void setIndexBuffer(const std::shared_ptr<HYBuffer>& buffer) = 0;

    /**
     * @brief 设置顶点属性
     * @param location 属性位置
     * @param binding 绑定点
     * @param format 顶点格式
     * @param offset 偏移量
     */
    virtual void setVertexAttribute(uint32_t location,
                                   uint32_t binding,
                                   VertexFormat format,
                                   uint32_t offset) = 0;

    /**
     * @brief 绑定VAO
     */
    virtual void bind() = 0;

    /**
     * @brief 解绑VAO
     */
    virtual void unbind() = 0;
};

} // namespace render
} // namespace hyengine
