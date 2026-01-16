#include "FrameBufferGL.hpp"
#include "FrameBufferGL.hpp"
#include <iostream>
#include <sstream>

namespace hyengine {
namespace render {

FrameBufferGL::FrameBufferGL(const FrameBufferDesc& desc) : mDesc(desc) {
    // 预分配颜色附件数组（最多8个MRT）
    mColorAttachments.resize(8);
    
    // 简化实现：模拟FBO创建
    mFramebufferID = 1; // 假设的ID
    
    std::cout << "[FrameBufferGL] Created FBO:" << std::endl;
    std::cout << "  Size: " << mDesc.width << "x" << mDesc.height << std::endl;
    std::cout << "  Color Format: " << static_cast<int>(mDesc.colorAttachmentFmt) << std::endl;
    if (mDesc.needDepth) {
        std::cout << "  Depth Format: " << static_cast<int>(mDesc.depthAttachmentFmt) << std::endl;
    }
    if (mDesc.isMultSample) {
        std::cout << "  MSAA Samples: " << mDesc.multSampleCnt << std::endl;
    }
    
    // TODO: 实际实现中这里会创建OpenGL FBO
    // glGenFramebuffers(1, &mFramebufferID);
    // glBindFramebuffer(GL_FRAMEBUFFER, mFramebufferID);
    // 
    // // 创建并附加颜色纹理
    // // ...
    // 
    // // 创建并附加深度纹理
    // if (mDesc.needDepth) {
    //     // ...
    // }
    // 
    // // 检查完整性
    // GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    // if (status != GL_FRAMEBUFFER_COMPLETE) {
    //     std::cerr << "Framebuffer not complete!" << std::endl;
    // }
    // 
    // glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

FrameBufferGL::~FrameBufferGL() {
    if (mFramebufferID != 0) {
        std::cout << "[FrameBufferGL] Deleted FBO ID=" << mFramebufferID << std::endl;
        // TODO: 实际实现中这里会删除FBO
        // glDeleteFramebuffers(1, &mFramebufferID);
        mFramebufferID = 0;
    }
    
    // 清理附件
    mColorAttachments.clear();
    mDepthStencilAttachment.reset();
}

void FrameBufferGL::setColorAttachment(uint32_t idx, std::shared_ptr<HYTexture> texture) {
    if (idx >= 8) {
        std::cerr << "[FrameBufferGL] Error: Color attachment index " << idx << " out of range (max 7)" << std::endl;
        return;
    }
    
    mColorAttachments[idx] = texture;
    
    if (texture) {
        std::cout << "[FrameBufferGL] Set color attachment " << idx << std::endl;
        // TODO: 实际实现中这里会附加纹理到FBO
        // glBindFramebuffer(GL_FRAMEBUFFER, mFramebufferID);
        // auto handle = texture->getResourceHandle();
        // glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + idx, 
        //                       GL_TEXTURE_2D, handle.uintHandle, 0);
        // glBindFramebuffer(GL_FRAMEBUFFER, 0);
    } else {
        std::cout << "[FrameBufferGL] Clear color attachment " << idx << std::endl;
    }
}

void FrameBufferGL::setDepthStencilAttachment(std::shared_ptr<HYTexture> texture) {
    mDepthStencilAttachment = texture;
    
    if (texture) {
        std::cout << "[FrameBufferGL] Set depth-stencil attachment" << std::endl;
        // TODO: 实际实现中这里会附加深度模板纹理
        // glBindFramebuffer(GL_FRAMEBUFFER, mFramebufferID);
        // auto handle = texture->getResourceHandle();
        // glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT,
        //                       GL_TEXTURE_2D, handle.uintHandle, 0);
        // glBindFramebuffer(GL_FRAMEBUFFER, 0);
    } else {
        std::cout << "[FrameBufferGL] Clear depth-stencil attachment" << std::endl;
    }
}

void FrameBufferGL::setDepthAttachment(std::shared_ptr<HYTexture> texture) {
    mDepthStencilAttachment = texture;
    
    if (texture) {
        std::cout << "[FrameBufferGL] Set depth attachment" << std::endl;
        // TODO: 实际实现中这里会附加深度纹理
        // glBindFramebuffer(GL_FRAMEBUFFER, mFramebufferID);
        // auto handle = texture->getResourceHandle();
        // glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
        //                       GL_TEXTURE_2D, handle.uintHandle, 0);
        // glBindFramebuffer(GL_FRAMEBUFFER, 0);
    } else {
        std::cout << "[FrameBufferGL] Clear depth attachment" << std::endl;
    }
}

void FrameBufferGL::setStencilAttachment(std::shared_ptr<HYTexture> texture) {
    if (texture) {
        std::cout << "[FrameBufferGL] Set stencil attachment" << std::endl;
        // TODO: 实际实现中这里会附加模板纹理
        // glBindFramebuffer(GL_FRAMEBUFFER, mFramebufferID);
        // auto handle = texture->getResourceHandle();
        // glFramebufferTexture2D(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT,
        //                       GL_TEXTURE_2D, handle.uintHandle, 0);
        // glBindFramebuffer(GL_FRAMEBUFFER, 0);
    } else {
        std::cout << "[FrameBufferGL] Clear stencil attachment" << std::endl;
    }
}

std::shared_ptr<HYTexture> FrameBufferGL::getColorAttachment(uint32_t idx) const {
    if (idx >= 8) {
        std::cerr << "[FrameBufferGL] Error: Color attachment index " << idx << " out of range" << std::endl;
        return nullptr;
    }
    return mColorAttachments[idx];
}

std::shared_ptr<HYTexture> FrameBufferGL::getDepthStencilAttachment() const {
    return mDepthStencilAttachment;
}

uint32_t FrameBufferGL::getAttachCount() const {
    uint32_t count = 0;
    for (const auto& attachment : mColorAttachments) {
        if (attachment) {
            count++;
        }
    }
    return count;
}

void FrameBufferGL::resize(uint32_t width, uint32_t height) {
    std::cout << "[FrameBufferGL] Resize to " << width << "x" << height << std::endl;
    
    mDesc.width = width;
    mDesc.height = height;
    
    // TODO: 实际实现中需要重新创建附件纹理
    // 1. 删除旧的附件纹理
    // 2. 创建新尺寸的纹理
    // 3. 重新附加到FBO
}

bool FrameBufferGL::isComplete() const {
    // TODO: 实际实现中这里会检查FBO完整性
    // glBindFramebuffer(GL_FRAMEBUFFER, mFramebufferID);
    // GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    // glBindFramebuffer(GL_FRAMEBUFFER, 0);
    // return status == GL_FRAMEBUFFER_COMPLETE;
    
    // 简化实现：检查是否至少有一个颜色附件
    return getAttachCount() > 0;
}

ResourceHandle FrameBufferGL::getResourceHandle() const {
    ResourceHandle handle;
    handle.uintHandle = mFramebufferID;
    return handle;
}

// 5.1.5 新增：多渲染目标（MRT）支持
void FrameBufferGL::setColorAttachments(const std::vector<std::shared_ptr<HYTexture>>& textures) {
    if (textures.size() > 8) {
        std::cerr << "[FrameBufferGL] Error: Too many color attachments (max 8, got " 
                  << textures.size() << ")" << std::endl;
        return;
    }
    
    // 设置所有颜色附件
    for (size_t i = 0; i < textures.size(); ++i) {
        setColorAttachment(static_cast<uint32_t>(i), textures[i]);
    }
    
    // 清除剩余的附件
    for (size_t i = textures.size(); i < mColorAttachments.size(); ++i) {
        setColorAttachment(static_cast<uint32_t>(i), nullptr);
    }
    
    updateFramebufferState();
    
    std::cout << "[FrameBufferGL] Set " << textures.size() << " color attachments" << std::endl;
}

std::vector<std::shared_ptr<HYTexture>> FrameBufferGL::getColorAttachments() const {
    std::vector<std::shared_ptr<HYTexture>> result;
    for (const auto& attachment : mColorAttachments) {
        if (attachment) {
            result.push_back(attachment);
        }
    }
    return result;
}

uint32_t FrameBufferGL::getColorAttachmentCount() const {
    return getAttachCount();
}

void FrameBufferGL::clearColorAttachment(uint32_t attachmentIndex) {
    if (attachmentIndex >= 8) {
        std::cerr << "[FrameBufferGL] Error: Invalid attachment index " << attachmentIndex << std::endl;
        return;
    }
    
    std::cout << "[FrameBufferGL] Clear color attachment " << attachmentIndex << std::endl;
    
    // TODO: 实际实现
    // glBindFramebuffer(GL_FRAMEBUFFER, mFramebufferID);
    // if (mAttachmentDescs.size() > attachmentIndex) {
    //     const auto& desc = mAttachmentDescs[attachmentIndex];
    //     glClearBufferfv(GL_COLOR, attachmentIndex, desc.clearColor);
    // }
    // glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void FrameBufferGL::clearDepthAttachment() {
    std::cout << "[FrameBufferGL] Clear depth attachment" << std::endl;
    
    // TODO: 实际实现
    // glBindFramebuffer(GL_FRAMEBUFFER, mFramebufferID);
    // float clearDepth = mDesc.depthStencilAttachment.clearDepth;
    // glClearBufferfv(GL_DEPTH, 0, &clearDepth);
    // glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void FrameBufferGL::clearStencilAttachment() {
    std::cout << "[FrameBufferGL] Clear stencil attachment" << std::endl;
    
    // TODO: 实际实现
    // glBindFramebuffer(GL_FRAMEBUFFER, mFramebufferID);
    // int clearStencil = mDesc.depthStencilAttachment.clearStencil;
    // glClearBufferiv(GL_STENCIL, 0, &clearStencil);
    // glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void FrameBufferGL::clearAllAttachments() {
    std::cout << "[FrameBufferGL] Clear all attachments" << std::endl;
    
    // 清除所有颜色附件
    for (uint32_t i = 0; i < getColorAttachmentCount(); ++i) {
        clearColorAttachment(i);
    }
    
    // 清除深度附件
    if (mDepthStencilAttachment || mDepthAttachment) {
        clearDepthAttachment();
    }
    
    // 清除模板附件
    if (mDepthStencilAttachment || mStencilAttachment) {
        clearStencilAttachment();
    }
}

// 5.1.5 新增：附件管理优化
const AttachmentDesc& FrameBufferGL::getAttachmentDesc(uint32_t attachmentIndex) const {
    if (attachmentIndex >= mAttachmentDescs.size()) {
        static AttachmentDesc defaultDesc;
        return defaultDesc;
    }
    return mAttachmentDescs[attachmentIndex];
}

void FrameBufferGL::setAttachmentDesc(uint32_t attachmentIndex, const AttachmentDesc& desc) {
    // 确保数组大小
    if (mAttachmentDescs.size() <= attachmentIndex) {
        mAttachmentDescs.resize(attachmentIndex + 1);
    }
    
    mAttachmentDescs[attachmentIndex] = desc;
    
    std::cout << "[FrameBufferGL] Set attachment " << attachmentIndex << " descriptor" << std::endl;
}

std::pair<bool, std::string> FrameBufferGL::validate() const {
    std::ostringstream errors;
    
    // 检查尺寸
    if (mDesc.width == 0 || mDesc.height == 0) {
        errors << "Invalid framebuffer size: " << mDesc.width << "x" << mDesc.height << "; ";
    }
    
    // 检查至少有一个附件
    if (getAttachCount() == 0 && !mDepthStencilAttachment && !mDepthAttachment) {
        errors << "No attachments configured; ";
    }
    
    // 检查颜色附件数量
    if (mDesc.colorAttachmentCount > 8) {
        errors << "Too many color attachments: " << mDesc.colorAttachmentCount << " (max 8); ";
    }
    
    // 检查多重采样配置
    if (mDesc.isMultSample) {
        if (mDesc.multSampleCnt != 1 && mDesc.multSampleCnt != 2 && 
            mDesc.multSampleCnt != 4 && mDesc.multSampleCnt != 8 && 
            mDesc.multSampleCnt != 16) {
            errors << "Invalid sample count: " << mDesc.multSampleCnt << " (must be 1, 2, 4, 8, or 16); ";
        }
    }
    
    // TODO: 实际实现中还需要检查OpenGL FBO状态
    // glBindFramebuffer(GL_FRAMEBUFFER, mFramebufferID);
    // GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    // if (status != GL_FRAMEBUFFER_COMPLETE) {
    //     errors << "OpenGL framebuffer incomplete: " << status << "; ";
    // }
    // glBindFramebuffer(GL_FRAMEBUFFER, 0);
    
    std::string errorStr = errors.str();
    bool isValid = errorStr.empty();
    
    if (!isValid) {
        std::cerr << "[FrameBufferGL] Validation failed: " << errorStr << std::endl;
    }
    
    return {isValid, errorStr};
}

uint32_t FrameBufferGL::getWidth() const {
    return mDesc.width;
}

uint32_t FrameBufferGL::getHeight() const {
    return mDesc.height;
}

// 5.1.5 新增：多重采样支持
uint32_t FrameBufferGL::getSampleCount() const {
    return mDesc.isMultSample ? mDesc.multSampleCnt : 1;
}

void FrameBufferGL::setSampleCount(uint32_t sampleCount) {
    if (sampleCount != 1 && sampleCount != 2 && sampleCount != 4 && 
        sampleCount != 8 && sampleCount != 16) {
        std::cerr << "[FrameBufferGL] Error: Invalid sample count " << sampleCount 
                  << " (must be 1, 2, 4, 8, or 16)" << std::endl;
        return;
    }
    
    mDesc.multSampleCnt = sampleCount;
    mDesc.isMultSample = (sampleCount > 1);
    
    std::cout << "[FrameBufferGL] Set sample count to " << sampleCount << std::endl;
    
    // TODO: 实际实现中需要重新创建纹理附件
}

bool FrameBufferGL::isMultisampled() const {
    return mDesc.isMultSample && mDesc.multSampleCnt > 1;
}

void FrameBufferGL::resolveMultisample(uint32_t srcAttachmentIndex, std::shared_ptr<HYTexture> dstTexture) {
    if (!isMultisampled()) {
        std::cerr << "[FrameBufferGL] Error: Cannot resolve non-multisampled framebuffer" << std::endl;
        return;
    }
    
    if (srcAttachmentIndex >= getColorAttachmentCount()) {
        std::cerr << "[FrameBufferGL] Error: Invalid source attachment index " << srcAttachmentIndex << std::endl;
        return;
    }
    
    if (!dstTexture) {
        std::cerr << "[FrameBufferGL] Error: Destination texture is null" << std::endl;
        return;
    }
    
    std::cout << "[FrameBufferGL] Resolve MSAA attachment " << srcAttachmentIndex 
              << " to texture" << std::endl;
    
    // TODO: 实际实现
    // unsigned int resolveFBO;
    // glGenFramebuffers(1, &resolveFBO);
    // glBindFramebuffer(GL_READ_FRAMEBUFFER, mFramebufferID);
    // glBindFramebuffer(GL_DRAW_FRAMEBUFFER, resolveFBO);
    // 
    // auto dstHandle = dstTexture->getResourceHandle();
    // glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
    //                       GL_TEXTURE_2D, dstHandle.uintHandle, 0);
    // 
    // glReadBuffer(GL_COLOR_ATTACHMENT0 + srcAttachmentIndex);
    // glDrawBuffer(GL_COLOR_ATTACHMENT0);
    // glBlitFramebuffer(0, 0, mDesc.width, mDesc.height,
    //                  0, 0, mDesc.width, mDesc.height,
    //                  GL_COLOR_BUFFER_BIT, GL_LINEAR);
    // 
    // glBindFramebuffer(GL_FRAMEBUFFER, 0);
    // glDeleteFramebuffers(1, &resolveFBO);
}

// 5.1.5 新增：内部方法
void FrameBufferGL::updateFramebufferState() {
    // 更新帧缓冲状态
    validateAttachments();
    
    // TODO: 实际实现中需要更新OpenGL状态
    // 例如设置绘制缓冲（glDrawBuffers）
}

void FrameBufferGL::validateAttachments() {
    // 验证所有附件的尺寸和格式是否一致
    bool firstAttachment = true;
    uint32_t refWidth = 0;
    uint32_t refHeight = 0;
    
    for (const auto& attachment : mColorAttachments) {
        if (attachment) {
            // 这里假设纹理有getWidth/getHeight方法
            // 实际实现时需要根据HYTexture接口调整
            if (firstAttachment) {
                // refWidth = attachment->getWidth();
                // refHeight = attachment->getHeight();
                refWidth = mDesc.width;
                refHeight = mDesc.height;
                firstAttachment = false;
            } else {
                // 检查尺寸是否一致
                // if (attachment->getWidth() != refWidth || 
                //     attachment->getHeight() != refHeight) {
                //     std::cerr << "[FrameBufferGL] Warning: Attachment size mismatch" << std::endl;
                // }
            }
        }
    }
}

} // namespace render
} // namespace hyengine
