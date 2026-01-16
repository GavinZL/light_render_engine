#include "TextureGL.hpp"
#include <iostream>

namespace hyengine {
namespace render {

TextureGL::TextureGL(const TextureDesc& desc) : mDesc(desc) {
    // 简化实现：只存储描述，不实际创建GL对象
    mTextureID = 1; // 假设的ID
    
    std::cout << "[TextureGL] Created texture (simulated), size=" << mDesc.width 
              << "x" << mDesc.height << ", format=" << static_cast<int>(mDesc.format) << std::endl;
}

TextureGL::~TextureGL() {
    if (mTextureID != 0) {
        std::cout << "[TextureGL] Deleted texture (simulated) ID=" << mTextureID << std::endl;
        mTextureID = 0;
    }
}

void TextureGL::doInit() {
    std::cout << "[TextureGL] doInit called" << std::endl;
    // TODO: 实际实现中这里会初始化GL纹理对象
}

void TextureGL::doResize(uint32_t width, uint32_t height) {
    std::cout << "[TextureGL] doResize to " << width << "x" << height << std::endl;
    mDesc.width = width;
    mDesc.height = height;
    // TODO: 实际实现中这里会重新分配纹理内存
}

// ==== 5.1.2 优化后的接口实现 ====
void TextureGL::updateTexture(uint32_t level, const void* data, TextureFormat dataFormat,
                             const TextureRegion& region) {
    std::cout << "[TextureGL] 更新纹理数据: level=" << level 
              << ", region=(" << region.offsetX << "," << region.offsetY 
              << "," << region.width << "," << region.height 
              << "), format=" << static_cast<int>(dataFormat) << std::endl;
    
    // TODO: 实际实现中这里会调用glTexSubImage2D等
    // glBindTexture(GL_TEXTURE_2D, mTextureID);
    // 
    // unsigned int internalFormat, glFormat, type;
    // GLUtils::textureFormatToGL(dataFormat, internalFormat, glFormat, type);
    // 
    // glTexSubImage2D(GL_TEXTURE_2D, level, region.offsetX, region.offsetY,
    //                 region.width, region.height, glFormat, type, data);
    // glBindTexture(GL_TEXTURE_2D, 0);
    // GLUtils::checkGLError("TextureGL::updateTexture");
}

bool TextureGL::readTexture(void* output, const TextureReadDesc& readDesc) {
    std::cout << "[TextureGL] 读取纹理数据: format=" << static_cast<int>(readDesc.format)
              << ", region=(" << readDesc.region.offsetX << "," << readDesc.region.offsetY 
              << "," << readDesc.region.width << "," << readDesc.region.height
              << "), colorRange=" << static_cast<int>(readDesc.colorRange)
              << ", colorSpace=" << static_cast<int>(readDesc.colorSpace) << std::endl;
    
    // TODO: 实际实现中这里会调用glReadPixels等
    // 需要考虑colorRange和colorSpace的转换
    // if (readDesc.colorSpace == TextureReadDesc::ColorSpace::SRGB) {
    //     // 应用sRGB解码
    // }
    // if (readDesc.colorRange == TextureReadDesc::ColorRange::RANGE_LIMITED) {
    //     // 应用颜色范围转换
    // }
    return true;
}

// ==== 5.1.2 新增查询接口 ====
uint32_t TextureGL::getWidth(uint32_t mipLevel) const {
    uint32_t width = mDesc.width;
    for (uint32_t i = 0; i < mipLevel && width > 1; ++i) {
        width = std::max(1u, width / 2);
    }
    return width;
}

uint32_t TextureGL::getHeight(uint32_t mipLevel) const {
    uint32_t height = mDesc.height;
    for (uint32_t i = 0; i < mipLevel && height > 1; ++i) {
        height = std::max(1u, height / 2);
    }
    return height;
}

void TextureGL::setSampler(std::shared_ptr<HYSampler> sampler) {
    std::cout << "[TextureGL] setSampler called" << std::endl;
    mSampler = sampler;
}

void TextureGL::bindSampler(uint32_t activeIndex) {
    std::cout << "[TextureGL] bindSampler to unit " << activeIndex << std::endl;
    // TODO: 实际实现中这里会调用glActiveTexture和glBindTexture
}

ResourceHandle TextureGL::getTextureHandle(uint32_t idx) const {
    ResourceHandle handle;
    handle.uintHandle = mTextureID;
    return handle;
}

void TextureGL::generateMipmaps() {
    std::cout << "[TextureGL] generateMipmaps called" << std::endl;
    // TODO: 实际实现中这里会调用glGenerateMipmap
}

ResourceHandle TextureGL::getResourceHandle() const {
    ResourceHandle handle;
    handle.uintHandle = mTextureID;
    return handle;
}

} // namespace render
} // namespace hyengine

