#include "TextureGLES.hpp"
#include "../../gl_common/GLUtils.hpp"
#include <iostream>

namespace hyengine {
namespace render {

// ============================================================================
// TextureGLES 实现
// ============================================================================

TextureGLES::TextureGLES(const TextureDesc& desc) : mDesc(desc) {
    std::cout << "[TextureGLES] 创建纹理 (" << desc.width << "x" << desc.height << ")" << std::endl;
    
    // 输出纹理信息
    std::cout << "[TextureGLES] 纹理类型: ";
    switch (desc.type) {
        case TextureDimension::TEXTURE_DIM_TEX_2D:
            std::cout << "2D";
            break;
        case TextureDimension::TEXTURE_DIM_TEX_CUBE:
            std::cout << "CUBE";
            break;
        case TextureDimension::TEXTURE_DIM_TEX_3D:
            std::cout << "3D";
            break;
    }
    std::cout << std::endl;
    
    std::cout << "[TextureGLES] 纹理格式: ";
    switch (desc.format) {
        case TextureFormat::TEX_FORMAT_RGBA8_UNORM:
            std::cout << "RGBA8";
            break;
        case TextureFormat::TEX_FORMAT_RGBA16_FLOAT:
            std::cout << "RGBA16F";
            break;
        case TextureFormat::TEX_FORMAT_DEPTH24_STENCIL8:
            std::cout << "DEPTH24_STENCIL8";
            break;
        default:
            std::cout << "未知";
            break;
    }
    std::cout << std::endl;
    
    std::cout << "[TextureGLES] Mip级别: " << desc.mipLevels << std::endl;
    
    // TODO: 在实际的OpenGL ES环境中：
    // glGenTextures(1, &mTextureID);
    // 
    // GLenum target = GLUtils::textureTypeToGL(desc.type);
    // glBindTexture(target, mTextureID);
    // 
    // unsigned int internalFormat, format, type;
    // GLUtils::textureFormatToGL(desc.format, internalFormat, format, type);
    // 
    // if (target == GL_TEXTURE_2D) {
    //     glTexImage2D(target, 0, internalFormat, desc.width, desc.height, 
    //                  0, format, type, nullptr);
    // } else if (target == GL_TEXTURE_CUBE_MAP) {
    //     for (int i = 0; i < 6; i++) {
    //         glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, internalFormat,
    //                      desc.width, desc.height, 0, format, type, nullptr);
    //     }
    // }
    // 
    // glBindTexture(target, 0);
    // GLUtils::checkGLError("TextureGLES::TextureGLES");
    
    mTextureID = 1; // 模拟ID
}

TextureGLES::~TextureGLES() {
    if (mTextureID != 0) {
        // TODO: glDeleteTextures(1, &mTextureID);
        std::cout << "[TextureGLES] Destroyed texture (ID: " << mTextureID << ")" << std::endl;
        mTextureID = 0;
    }
}

void TextureGLES::doInit() {
    std::cout << "[TextureGLES] Initializing texture..." << std::endl;
    // TODO: 纹理初始化逻辑
}

void TextureGLES::doResize(uint32_t width, uint32_t height) {
    mDesc.width = width;
    mDesc.height = height;
    std::cout << "[TextureGLES] Resizing texture to " << width << "x" << height << std::endl;
    
    // TODO: 
    // glBindTexture(GL_TEXTURE_2D, mTextureID);
    // glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, type, nullptr);
    // glBindTexture(GL_TEXTURE_2D, 0);
}

// ==== 5.1.2 优化后的接口实现 ====
void TextureGLES::updateTexture(uint32_t level, const void* data, TextureFormat dataFormat,
                                const TextureRegion& region) {
    std::cout << "[TextureGLES] 更新纹理数据 (level " << level 
              << ", region: " << region.offsetX << "," << region.offsetY 
              << " " << region.width << "x" << region.height 
              << ", format: " << static_cast<int>(dataFormat) << ")" << std::endl;
    
    // TODO: 实际实现中：
    // glBindTexture(GL_TEXTURE_2D, mTextureID);
    // 
    // unsigned int internalFormat, glFormat, type;
    // GLUtils::textureFormatToGL(dataFormat, internalFormat, glFormat, type);
    // 
    // glTexSubImage2D(GL_TEXTURE_2D, level, region.offsetX, region.offsetY,
    //                 region.width, region.height, glFormat, type, data);
    // glBindTexture(GL_TEXTURE_2D, 0);
    // GLUtils::checkGLError("TextureGLES::updateTexture");
}

bool TextureGLES::readTexture(void* output, const TextureReadDesc& readDesc) {
    std::cout << "[TextureGLES] 读取纹理数据 (format: " 
              << static_cast<int>(readDesc.format)
              << ", colorRange: " << static_cast<int>(readDesc.colorRange)
              << ", colorSpace: " << static_cast<int>(readDesc.colorSpace)
              << ")" << std::endl;
    
    // TODO: ES 3.0 可以使用 PBO 或 glReadPixels
    // 需要先绑定到FBO，然后使用glReadPixels读取
    // 
    // 根据colorRange和colorSpace进行颜色空间转换
    // if (readDesc.colorSpace == TextureReadDesc::ColorSpace::SRGB) {
    //     // 应用sRGB解码
    // }
    // if (readDesc.colorRange == TextureReadDesc::ColorRange::RANGE_LIMITED) {
    //     // 应用颜色范围转换
    // }
    
    return false;
}

// ==== 5.1.2 新增查询接口 ====
uint32_t TextureGLES::getWidth(uint32_t mipLevel) const {
    // 计算指定Mip层的宽度
    uint32_t width = mDesc.width;
    for (uint32_t i = 0; i < mipLevel && width > 1; ++i) {
        width = std::max(1u, width / 2);
    }
    return width;
}

uint32_t TextureGLES::getHeight(uint32_t mipLevel) const {
    // 计算指定Mip层的高度
    uint32_t height = mDesc.height;
    for (uint32_t i = 0; i < mipLevel && height > 1; ++i) {
        height = std::max(1u, height / 2);
    }
    return height;
}

void TextureGLES::setSampler(std::shared_ptr<HYSampler> sampler) {
    mSampler = sampler;
    applySamplerState();
}

void TextureGLES::bindSampler(uint32_t activeIndex) {
    std::cout << "[TextureGLES] Binding texture to unit " << activeIndex << std::endl;
    
    // TODO:
    // glActiveTexture(GL_TEXTURE0 + activeIndex);
    // glBindTexture(GL_TEXTURE_2D, mTextureID);
}

void TextureGLES::applySamplerState() {
    if (!mSampler) {
        std::cout << "[TextureGLES] 没有采样器，使用默认设置" << std::endl;
        return;
    }
    
    std::cout << "[TextureGLES] 应用采样器状态到纹理 (ES 3.0方式)" << std::endl;
    
    // OpenGL ES 3.0 中，采样器状态通过 glTexParameteri 设置到纹理上
    // 注意：ES 3.0 不支持分离式采样器对象
    
    const SamplerDesc& samplerDesc = mSampler->getDesc();
    
    // TODO: 实际实现中：
    // GLenum target = GLUtils::textureTypeToGL(mDesc.type);
    // glBindTexture(target, mTextureID);
    // 
    // // 过滤模式
    // GLenum minFilter = GLUtils::filterTypeToGLFilter(samplerDesc.minFilter);
    // GLenum magFilter = GLUtils::filterTypeToGLFilter(samplerDesc.magFilter);
    // glTexParameteri(target, GL_TEXTURE_MIN_FILTER, minFilter);
    // glTexParameteri(target, GL_TEXTURE_MAG_FILTER, magFilter);
    // 
    // // 寻址模式
    // GLenum wrapS = GLUtils::addressModeToGLWrap(samplerDesc.addressModeU);
    // GLenum wrapT = GLUtils::addressModeToGLWrap(samplerDesc.addressModeV);
    // GLenum wrapR = GLUtils::addressModeToGLWrap(samplerDesc.addressModeW);
    // glTexParameteri(target, GL_TEXTURE_WRAP_S, wrapS);
    // glTexParameteri(target, GL_TEXTURE_WRAP_T, wrapT);
    // if (mDesc.type == TextureDimension::TEXTURE_DIM_TEX_3D || 
    //     mDesc.type == TextureDimension::TEXTURE_DIM_TEX_CUBE) {
    //     glTexParameteri(target, GL_TEXTURE_WRAP_R, wrapR);
    // }
    // 
    // glBindTexture(target, 0);
    // GLUtils::checkGLError("TextureGLES::applySamplerState");
    
    std::cout << "[TextureGLES] 采样器状态应用完成" << std::endl;
}

ResourceHandle TextureGLES::getTextureHandle(uint32_t idx) const {
    ResourceHandle handle;
    handle.uintHandle = mTextureID;
    return handle;
}

ResourceHandle TextureGLES::getResourceHandle() const {
    ResourceHandle handle;
    handle.uintHandle = mTextureID;
    return handle;
}

void TextureGLES::generateMipmaps() {
    std::cout << "[TextureGLES] Generating mipmaps" << std::endl;
    
    // TODO:
    // glBindTexture(GL_TEXTURE_2D, mTextureID);
    // glGenerateMipmap(GL_TEXTURE_2D);
    // glBindTexture(GL_TEXTURE_2D, 0);
}

} // namespace render
} // namespace hyengine
