#include "FrameBufferMtl.hpp"
#include <iostream>

namespace hyengine {
namespace render {

FrameBufferMtl::FrameBufferMtl(const FrameBufferDesc& desc) : mDesc(desc) {
    std::cout << "[FrameBufferMtl] 创建Metal渲染目标描述" << std::endl;
}

void FrameBufferMtl::setColorAttachment(uint32_t idx, std::shared_ptr<HYTexture> texture) {
    if (idx < 8) mColorAttachments[idx] = texture;
}

void FrameBufferMtl::setDepthStencilAttachment(std::shared_ptr<HYTexture> texture) {
    mDepthAttachment = texture;
    mStencilAttachment = texture;
}

void FrameBufferMtl::setDepthAttachment(std::shared_ptr<HYTexture> texture) {
    mDepthAttachment = texture;
}

void FrameBufferMtl::setStencilAttachment(std::shared_ptr<HYTexture> texture) {
    mStencilAttachment = texture;
}

std::shared_ptr<HYTexture> FrameBufferMtl::getColorAttachment(uint32_t idx) const {
    return idx < 8 ? mColorAttachments[idx] : nullptr;
}

std::shared_ptr<HYTexture> FrameBufferMtl::getDepthStencilAttachment() const {
    return mDepthAttachment;
}

void FrameBufferMtl::resize(uint32_t width, uint32_t height) {
    mDesc.width = width;
    mDesc.height = height;
}

bool FrameBufferMtl::isComplete() const {
    return getAttachCount() > 0;
}

uint32_t FrameBufferMtl::getAttachCount() const {
    uint32_t count = 0;
    for (const auto& att : mColorAttachments) if (att) count++;
    if (mDepthAttachment) count++;
    if (mStencilAttachment) count++;
    return count;
}

ResourceHandle FrameBufferMtl::getResourceHandle() const {
    ResourceHandle handle;
    handle.uintHandle = 0;
    return handle;
}

// 5.1.5 新增：简化实现
void FrameBufferMtl::setColorAttachments(const std::vector<std::shared_ptr<HYTexture>>& textures) {
    for (size_t i = 0; i < textures.size() && i < 8; ++i) {
        setColorAttachment(static_cast<uint32_t>(i), textures[i]);
    }
}

std::vector<std::shared_ptr<HYTexture>> FrameBufferMtl::getColorAttachments() const {
    std::vector<std::shared_ptr<HYTexture>> result;
    for (const auto& att : mColorAttachments) {
        if (att) result.push_back(att);
    }
    return result;
}

uint32_t FrameBufferMtl::getColorAttachmentCount() const { return getAttachCount(); }
void FrameBufferMtl::clearColorAttachment(uint32_t) {}
void FrameBufferMtl::clearDepthAttachment() {}
void FrameBufferMtl::clearStencilAttachment() {}
void FrameBufferMtl::clearAllAttachments() {}

const AttachmentDesc& FrameBufferMtl::getAttachmentDesc(uint32_t idx) const {
    static AttachmentDesc def;
    return idx < mAttachmentDescs.size() ? mAttachmentDescs[idx] : def;
}

void FrameBufferMtl::setAttachmentDesc(uint32_t idx, const AttachmentDesc& desc) {
    if (mAttachmentDescs.size() <= idx) mAttachmentDescs.resize(idx + 1);
    mAttachmentDescs[idx] = desc;
}

std::pair<bool, std::string> FrameBufferMtl::validate() const {
    return {isComplete(), ""};
}

uint32_t FrameBufferMtl::getWidth() const { return mDesc.width; }
uint32_t FrameBufferMtl::getHeight() const { return mDesc.height; }
uint32_t FrameBufferMtl::getSampleCount() const { return mDesc.isMultSample ? mDesc.multSampleCnt : 1; }
void FrameBufferMtl::setSampleCount(uint32_t cnt) { mDesc.multSampleCnt = cnt; mDesc.isMultSample = (cnt > 1); }
bool FrameBufferMtl::isMultisampled() const { return mDesc.isMultSample && mDesc.multSampleCnt > 1; }
void FrameBufferMtl::resolveMultisample(uint32_t, std::shared_ptr<HYTexture>) {}
void FrameBufferMtl::updateFramebufferState() {}
void FrameBufferMtl::validateAttachments() {}

} // namespace render
} // namespace hyengine
