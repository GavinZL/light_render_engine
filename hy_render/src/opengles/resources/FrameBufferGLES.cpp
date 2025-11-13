#include "FrameBufferGLES.hpp"
#include "../../gl_common/GLUtils.hpp"
#include <iostream>

namespace hyengine {
namespace render {

// ============================================================================
// FrameBufferGLES 实现
// ============================================================================

FrameBufferGLES::FrameBufferGLES(const FrameBufferDesc& desc) : mDesc(desc) {
    std::cout << "[FrameBufferGLES] 创建帧缓冲 (" << desc.width << "x" << desc.height << ")" << std::endl;
    
    // TODO: 在实际的OpenGL ES环境中：
    // glGenFramebuffers(1, &mFBO);
    // glBindFramebuffer(GL_FRAMEBUFFER, mFBO);
    // 
    // // 如果需要预设附件，可以在这里创建
    // if (desc.needDepth) {
    //     // 创建深度附件纹理...
    // }
    // 
    // glBindFramebuffer(GL_FRAMEBUFFER, 0);
    // GLUtils::checkGLError("FrameBufferGLES::FrameBufferGLES");
    
    mFBO = 1; // 模拟ID
    
    std::cout << "[FrameBufferGLES] 是否需要深度: " << (desc.needDepth ? "是" : "否") << std::endl;
    std::cout << "[FrameBufferGLES] 多采样: " << (desc.isMultSample ? "是" : "否") 
              << " (" << desc.multSampleCnt << "x)" << std::endl;
}

FrameBufferGLES::~FrameBufferGLES() {
    if (mFBO != 0) {
        // TODO: glDeleteFramebuffers(1, &mFBO);
        std::cout << "[FrameBufferGLES] Destroyed framebuffer (ID: " << mFBO << ")" << std::endl;
        mFBO = 0;
    }
}

void FrameBufferGLES::setColorAttachment(uint32_t idx, std::shared_ptr<HYTexture> texture) {
    if (idx >= 8) {
        std::cerr << "[FrameBufferGLES] Error: Color attachment index out of range" << std::endl;
        return;
    }
    
    mColorAttachments[idx] = texture;
    std::cout << "[FrameBufferGLES] Set color attachment " << idx << std::endl;
    
    // TODO:
    // glBindFramebuffer(GL_FRAMEBUFFER, mFBO);
    // GLuint texID = static_cast<GLuint>(texture->getTextureHandle().uintHandle);
    // glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + idx,
    //                        GL_TEXTURE_2D, texID, 0);
    // glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void FrameBufferGLES::setDepthAttachment(std::shared_ptr<HYTexture> texture) {
    mDepthAttachment = texture;
    std::cout << "[FrameBufferGLES] Set depth attachment" << std::endl;
    
    // TODO:
    // glBindFramebuffer(GL_FRAMEBUFFER, mFBO);
    // GLuint texID = static_cast<GLuint>(texture->getTextureHandle().uintHandle);
    // glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
    //                        GL_TEXTURE_2D, texID, 0);
    // glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void FrameBufferGLES::setStencilAttachment(std::shared_ptr<HYTexture> texture) {
    mStencilAttachment = texture;
    std::cout << "[FrameBufferGLES] Set stencil attachment" << std::endl;
    
    // TODO:
    // glBindFramebuffer(GL_FRAMEBUFFER, mFBO);
    // GLuint texID = static_cast<GLuint>(texture->getTextureHandle().uintHandle);
    // glFramebufferTexture2D(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT,
    //                        GL_TEXTURE_2D, texID, 0);
    // glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void FrameBufferGLES::setDepthStencilAttachment(std::shared_ptr<HYTexture> texture) {
    mDepthAttachment = texture;
    mStencilAttachment = texture;
    std::cout << "[FrameBufferGLES] Set depth-stencil attachment" << std::endl;
    
    // TODO:
    // glBindFramebuffer(GL_FRAMEBUFFER, mFBO);
    // GLuint texID = static_cast<GLuint>(texture->getTextureHandle().uintHandle);
    // glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT,
    //                        GL_TEXTURE_2D, texID, 0);
    // glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

std::shared_ptr<HYTexture> FrameBufferGLES::getColorAttachment(uint32_t idx) const {
    if (idx >= 8) {
        return nullptr;
    }
    return mColorAttachments[idx];
}

std::shared_ptr<HYTexture> FrameBufferGLES::getDepthStencilAttachment() const {
    return mDepthAttachment;
}

void FrameBufferGLES::resize(uint32_t width, uint32_t height) {
    mDesc.width = width;
    mDesc.height = height;
    std::cout << "[FrameBufferGLES] Resize to " << width << "x" << height << std::endl;
    
    // TODO: 重新创建附件纹理
}

bool FrameBufferGLES::isComplete() const {
    std::cout << "[FrameBufferGLES] 检查帧缓冲完整性" << std::endl;
    
    // TODO: 实际实现中：
    // glBindFramebuffer(GL_FRAMEBUFFER, mFBO);
    // GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    // glBindFramebuffer(GL_FRAMEBUFFER, 0);
    // 
    // if (status != GL_FRAMEBUFFER_COMPLETE) {
    //     std::cerr << "[FrameBufferGLES] Framebuffer 不完整: 0x" 
    //               << std::hex << status << std::dec << std::endl;
    //     return false;
    // }
    // 
    // return true;
    
    // 简单验证：至少有一个附件
    bool hasAttachment = getAttachCount() > 0;
    std::cout << "[FrameBufferGLES] 附件数量: " << getAttachCount() << std::endl;
    return hasAttachment;
}

uint32_t FrameBufferGLES::getAttachCount() const {
    uint32_t count = 0;
    for (const auto& attachment : mColorAttachments) {
        if (attachment) count++;
    }
    if (mDepthAttachment) count++;
    if (mStencilAttachment) count++;
    return count;
}

ResourceHandle FrameBufferGLES::getResourceHandle() const {
    ResourceHandle handle;
    handle.uintHandle = mFBO;
    return handle;
}

// 5.1.5 新增：简化实现
void FrameBufferGLES::setColorAttachments(const std::vector<std::shared_ptr<HYTexture>>& textures) {
    for (size_t i = 0; i < textures.size() && i < 8; ++i) {
        setColorAttachment(static_cast<uint32_t>(i), textures[i]);
    }
}

std::vector<std::shared_ptr<HYTexture>> FrameBufferGLES::getColorAttachments() const {
    std::vector<std::shared_ptr<HYTexture>> result;
    for (const auto& att : mColorAttachments) {
        if (att) result.push_back(att);
    }
    return result;
}

uint32_t FrameBufferGLES::getColorAttachmentCount() const { return getAttachCount(); }
void FrameBufferGLES::clearColorAttachment(uint32_t) {}
void FrameBufferGLES::clearDepthAttachment() {}
void FrameBufferGLES::clearStencilAttachment() {}
void FrameBufferGLES::clearAllAttachments() {}

const AttachmentDesc& FrameBufferGLES::getAttachmentDesc(uint32_t idx) const {
    static AttachmentDesc def;
    return idx < mAttachmentDescs.size() ? mAttachmentDescs[idx] : def;
}

void FrameBufferGLES::setAttachmentDesc(uint32_t idx, const AttachmentDesc& desc) {
    if (mAttachmentDescs.size() <= idx) mAttachmentDescs.resize(idx + 1);
    mAttachmentDescs[idx] = desc;
}

std::pair<bool, std::string> FrameBufferGLES::validate() const {
    return {isComplete(), ""};
}

uint32_t FrameBufferGLES::getWidth() const { return mDesc.width; }
uint32_t FrameBufferGLES::getHeight() const { return mDesc.height; }
uint32_t FrameBufferGLES::getSampleCount() const { return mDesc.isMultSample ? mDesc.multSampleCnt : 1; }
void FrameBufferGLES::setSampleCount(uint32_t cnt) { mDesc.multSampleCnt = cnt; mDesc.isMultSample = (cnt > 1); }
bool FrameBufferGLES::isMultisampled() const { return mDesc.isMultSample && mDesc.multSampleCnt > 1; }
void FrameBufferGLES::resolveMultisample(uint32_t, std::shared_ptr<HYTexture>) {}
void FrameBufferGLES::updateFramebufferState() {}
void FrameBufferGLES::validateAttachments() {}

} // namespace render
} // namespace hyengine
