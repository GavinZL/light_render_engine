#pragma once

#include "HYRenderDef.hpp"

namespace hyengine {
namespace render {

/**
 * @brief 顶点数组对象抽象
 */
class HYVertexArray : public HYResource {
public:
    HYVertexArray() : HYResource(ResourceType::kVertexArray) {}
    virtual ~HYVertexArray() = default;

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
