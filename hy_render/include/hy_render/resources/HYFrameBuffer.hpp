#pragma once

#include "HYRenderDef.hpp"
#include "HYTexture.hpp"
#include <memory>

namespace hyengine {
namespace render {

/**
 * @brief 帧缓冲对象抽象
 */
class HYFrameBuffer : public HYResource {
public:
    HYFrameBuffer() : HYResource(ResourceType::kFrameBuffer) {}
    virtual ~HYFrameBuffer() = default;

    /**
     * @brief 设置颜色附件
     * @param idx 附件索引（0-7）
     * @param texture 纹理对象
     */
    virtual void setColorAttachment(uint32_t idx, std::shared_ptr<HYTexture> texture) = 0;

    /**
     * @brief 设置深度模板附件
     */
    virtual void setDepthStencilAttachment(std::shared_ptr<HYTexture> texture) = 0;

    /**
     * @brief 获取颜色附件
     */
    virtual std::shared_ptr<HYTexture> getColorAttachment(uint32_t idx) const = 0;

    /**
     * @brief 获取深度模板附件
     */
    virtual std::shared_ptr<HYTexture> getDepthStencilAttachment() const = 0;

    /**
     * @brief 获取附件数量
     */
    virtual uint32_t getAttachCount() const = 0;

    /**
     * @brief 调整大小
     */
    virtual void resize(uint32_t width, uint32_t height) = 0;

    /**
     * @brief 检查完整性
     */
    virtual bool isComplete() const = 0;

    /**
     * @brief 获取描述
     */
    virtual const FrameBufferDesc& getDesc() const = 0;
};

} // namespace render
} // namespace hyengine
