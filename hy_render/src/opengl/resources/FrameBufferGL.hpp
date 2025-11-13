#pragma once

#include "../../core/HYResourceCommon.hpp"
#include <vector>

namespace hyengine {
namespace render {

/**
 * @brief OpenGL帧缓冲实现
 */
class FrameBufferGL : public HYFrameBuffer {
public:
    explicit FrameBufferGL(const FrameBufferDesc& desc);
    virtual ~FrameBufferGL();

    void setColorAttachment(uint32_t idx, std::shared_ptr<HYTexture> texture) override;
    void setDepthStencilAttachment(std::shared_ptr<HYTexture> texture) override;
    void setDepthAttachment(std::shared_ptr<HYTexture> texture) override;
    void setStencilAttachment(std::shared_ptr<HYTexture> texture) override;
    std::shared_ptr<HYTexture> getColorAttachment(uint32_t idx) const override;
    std::shared_ptr<HYTexture> getDepthStencilAttachment() const override;
    uint32_t getAttachCount() const override;
    void resize(uint32_t width, uint32_t height) override;
    bool isComplete() const override;
    const FrameBufferDesc& getDesc() const override { return mDesc; }
    ResourceHandle getResourceHandle() const override;
    
    // 5.1.5 新增：多渲柗目标（MRT）支持
    void setColorAttachments(const std::vector<std::shared_ptr<HYTexture>>& textures) override;
    std::vector<std::shared_ptr<HYTexture>> getColorAttachments() const override;
    uint32_t getColorAttachmentCount() const override;
    void clearColorAttachment(uint32_t attachmentIndex) override;
    void clearDepthAttachment() override;
    void clearStencilAttachment() override;
    void clearAllAttachments() override;
    
    // 5.1.5 新增：附件管理优化
    const AttachmentDesc& getAttachmentDesc(uint32_t attachmentIndex) const override;
    void setAttachmentDesc(uint32_t attachmentIndex, const AttachmentDesc& desc) override;
    std::pair<bool, std::string> validate() const override;
    uint32_t getWidth() const override;
    uint32_t getHeight() const override;
    
    // 5.1.5 新增：多重采样支持
    uint32_t getSampleCount() const override;
    void setSampleCount(uint32_t sampleCount) override;
    bool isMultisampled() const override;
    void resolveMultisample(uint32_t srcAttachmentIndex, std::shared_ptr<HYTexture> dstTexture) override;

private:
    void updateFramebufferState();  // 5.1.5 新增：更新帧缓冲状态
    void validateAttachments();      // 5.1.5 新增：验证附件配置
    
private:
    FrameBufferDesc mDesc;
    unsigned int mFramebufferID = 0;
    std::vector<std::shared_ptr<HYTexture>> mColorAttachments;
    std::shared_ptr<HYTexture> mDepthStencilAttachment;
    std::shared_ptr<HYTexture> mDepthAttachment;     // 5.1.5 新增：单独的深度附件
    std::shared_ptr<HYTexture> mStencilAttachment;   // 5.1.5 新增：单独的模板附件
    std::vector<AttachmentDesc> mAttachmentDescs;    // 5.1.5 新增：附件描述列表
};

} // namespace render
} // namespace hyengine
