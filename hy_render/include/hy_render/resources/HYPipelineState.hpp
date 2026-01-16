#pragma once

#include "../HYRenderDef.hpp"

namespace hyengine {
namespace render {

/**
 * @brief 管线状态对象抽象
 */
class HYPipelineState : public HYResource {
public:
    HYPipelineState() : HYResource(ResourceType::kPipelineState) {}
    virtual ~HYPipelineState() = default;

    /**
     * @brief 启用/禁用混合
     */
    virtual void enableBlend(bool flag) = 0;

    /**
     * @brief 启用/禁用深度测试
     */
    virtual void enableDepth(bool flag) = 0;

    /**
     * @brief 启用/禁用模板测试
     */
    virtual void enableStencil(bool flag) = 0;

    /**
     * @brief 启用/禁用裁剪测试
     */
    virtual void enableScissor(bool flag) = 0;

    /**
     * @brief 设置剥除模式
     */
    virtual void setCullMode(CullMode mode) = 0;

    /**
     * @brief 设置图元类型
     */
    virtual void setPrimitive(PrimitiveType mode) = 0;

    /**
     * @brief 获取描述
     */
    virtual const PipelineStateDesc& getDesc() const = 0;
};

} // namespace render
} // namespace hyengine
