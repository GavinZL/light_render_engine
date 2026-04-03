#pragma once

#include "../HYRenderDef.hpp"
#include "HYTexture.hpp"
#include <memory>
#include <vector>
#include <string>
#include <utility>

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
     * @brief 设置深度附件
     */
    virtual void setDepthAttachment(std::shared_ptr<HYTexture> texture) = 0;

    /**
     * @brief 设置模板附件
     */
    virtual void setStencilAttachment(std::shared_ptr<HYTexture> texture) = 0;

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
    
    // ==== 5.1.5 新增：多渲染目标（MRT）支持 ====
    /**
     * @brief 批量设置颜色附件
     * @param textures 纹理列表（最多8个）
     */
    virtual void setColorAttachments(const std::vector<std::shared_ptr<HYTexture>>& textures) = 0;
    
    /**
     * @brief 获取所有颜色附件
     */
    virtual std::vector<std::shared_ptr<HYTexture>> getColorAttachments() const = 0;
    
    /**
     * @brief 获取颜色附件数量
     */
    virtual uint32_t getColorAttachmentCount() const = 0;
    
    /**
     * @brief 清除指定附件
     * @param attachmentIndex 附件索引
     */
    virtual void clearColorAttachment(uint32_t attachmentIndex) = 0;
    
    /**
     * @brief 清除深度附件
     */
    virtual void clearDepthAttachment() = 0;
    
    /**
     * @brief 清除模板附件
     */
    virtual void clearStencilAttachment() = 0;
    
    /**
     * @brief 清除所有附件
     */
    virtual void clearAllAttachments() = 0;
    
    // ==== 5.1.5 新增：附件管理优化 ====
    /**
     * @brief 获取附件描述
     * @param attachmentIndex 附件索引
     */
    virtual const AttachmentDesc& getAttachmentDesc(uint32_t attachmentIndex) const = 0;
    
    /**
     * @brief 设置附件描述
     * @param attachmentIndex 附件索引
     * @param desc 附件描述
     */
    virtual void setAttachmentDesc(uint32_t attachmentIndex, const AttachmentDesc& desc) = 0;
    
    /**
     * @brief 验证帧缓冲配置是否有效
     * @return 验证结果和错误信息
     */
    virtual std::pair<bool, std::string> validate() const = 0;
    
    /**
     * @brief 获取帧缓冲宽度
     */
    virtual uint32_t getWidth() const = 0;
    
    /**
     * @brief 获取帧缓冲高度
     */
    virtual uint32_t getHeight() const = 0;
    
    // ==== 5.1.5 新增：多重采样支持 ====
    /**
     * @brief 获取采样数量
     */
    virtual uint32_t getSampleCount() const = 0;
    
    /**
     * @brief 设置采样数量
     * @param sampleCount 采样数量（1, 2, 4, 8, 16）
     */
    virtual void setSampleCount(uint32_t sampleCount) = 0;
    
    /**
     * @brief 是否支持多重采样
     */
    virtual bool isMultisampled() const = 0;
    
    /**
     * @brief Resolve多重采样纹理到目标纹理
     * @param srcAttachmentIndex 源附件索引
     * @param dstTexture 目标纹理
     */
    virtual void resolveMultisample(uint32_t srcAttachmentIndex, std::shared_ptr<HYTexture> dstTexture) = 0;
};

} // namespace render
} // namespace hyengine
