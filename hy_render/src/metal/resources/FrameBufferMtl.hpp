#pragma once

#include "../../core/HYResourceCommon.hpp"
#include <array>

namespace hyengine {
namespace render {

/**
 * @brief Metal 帧缓冲实现
 * 
 * 注意：Metal 没有传统的 FrameBuffer 概念
 * 使用 MTLRenderPassDescriptor 来描述渲染目标
 */
class FrameBufferMtl : public HYFrameBuffer {
public:
    explicit FrameBufferMtl(const FrameBufferDesc& desc);
    virtual ~FrameBufferMtl() = default;

    void setColorAttachment(uint32_t idx, std::shared_ptr<HYTexture> texture) override;
    void setDepthStencilAttachment(std::shared_ptr<HYTexture> texture) override;
    void setDepthAttachment(std::shared_ptr<HYTexture> texture) override;
    void setStencilAttachment(std::shared_ptr<HYTexture> texture) override;
    std::shared_ptr<HYTexture> getColorAttachment(uint32_t idx) const override;
    std::shared_ptr<HYTexture> getDepthStencilAttachment() const override;
    void resize(uint32_t width, uint32_t height) override;
    bool isComplete() const override;
    uint32_t getAttachCount() const override;
    const FrameBufferDesc& getDesc() const override { return mDesc; }
    ResourceHandle getResourceHandle() const override;
    
    // 5.1.5 新增
    void setColorAttachments(const std::vector<std::shared_ptr<HYTexture>>& textures) override;
    std::vector<std::shared_ptr<HYTexture>> getColorAttachments() const override;
    uint32_t getColorAttachmentCount() const override;
    void clearColorAttachment(uint32_t attachmentIndex) override;
    void clearDepthAttachment() override;
    void clearStencilAttachment() override;
    void clearAllAttachments() override;
    const AttachmentDesc& getAttachmentDesc(uint32_t attachmentIndex) const override;
    void setAttachmentDesc(uint32_t attachmentIndex, const AttachmentDesc& desc) override;
    std::pair<bool, std::string> validate() const override;
    uint32_t getWidth() const override;
    uint32_t getHeight() const override;
    uint32_t getSampleCount() const override;
    void setSampleCount(uint32_t sampleCount) override;
    bool isMultisampled() const override;
    void resolveMultisample(uint32_t srcAttachmentIndex, std::shared_ptr<HYTexture> dstTexture) override;

private:
    void updateFramebufferState();
    void validateAttachments();
    
private:
    FrameBufferDesc mDesc;
    std::array<std::shared_ptr<HYTexture>, 8> mColorAttachments;
    std::shared_ptr<HYTexture> mDepthAttachment;
    std::shared_ptr<HYTexture> mStencilAttachment;
    std::vector<AttachmentDesc> mAttachmentDescs;
};

} // namespace render
} // namespace hyengine
