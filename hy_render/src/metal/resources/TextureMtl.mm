#include "TextureMtl.hpp"
#include <iostream>

namespace hyengine {
namespace render {

TextureMtl::TextureMtl(const TextureDesc& desc, void* device) 
    : mDesc(desc), mDevice(device) {
    std::cout << "[TextureMtl] 创建Metal纹理 (" << desc.width << "x" << desc.height << ")" << std::endl;
    mTexture = reinterpret_cast<void*>(0x300);
}

TextureMtl::~TextureMtl() {
    std::cout << "[TextureMtl] 销毁Metal纹理" << std::endl;
}

void TextureMtl::doInit() {
    std::cout << "[TextureMtl] 初始化Metal纹理" << std::endl;
}

void TextureMtl::doResize(uint32_t width, uint32_t height) { 
    std::cout << "[TextureMtl] 调整尺寸到 " << width << "x" << height << std::endl;
    mDesc.width = width; 
    mDesc.height = height; 
}

// ==== 5.1.2 优化后的接口实现 ====
void TextureMtl::updateTexture(uint32_t level, const void* data, TextureFormat dataFormat,
                              const TextureRegion& region) {
    std::cout << "[TextureMtl] 更新Metal纹理 (level " << level 
              << ", region: " << region.offsetX << "," << region.offsetY 
              << " " << region.width << "x" << region.height 
              << ", format: " << static_cast<int>(dataFormat) << ")" << std::endl;
    
    // TODO: 实际Metal实现：
    // MTLRegion mtlRegion = MTLRegionMake2D(region.offsetX, region.offsetY, 
    //                                       region.width, region.height);
    // 
    // NSUInteger bytesPerRow = region.width * getBytesPerPixel(dataFormat);
    // 
    // [(__bridge id<MTLTexture>)mTexture replaceRegion:mtlRegion
    //                                      mipmapLevel:level
    //                                            slice:0
    //                                        withBytes:data
    //                                      bytesPerRow:bytesPerRow
    //                                    bytesPerImage:0];
}

bool TextureMtl::readTexture(void* output, const TextureReadDesc& readDesc) {
    std::cout << "[TextureMtl] 读取Metal纹理 (format: " 
              << static_cast<int>(readDesc.format)
              << ", colorRange: " << static_cast<int>(readDesc.colorRange)
              << ", colorSpace: " << static_cast<int>(readDesc.colorSpace)
              << ")" << std::endl;
    
    // TODO: 实际Metal实现：
    // MTLRegion region = MTLRegionMake2D(readDesc.region.offsetX, readDesc.region.offsetY,
    //                                    readDesc.region.width, readDesc.region.height);
    // 
    // NSUInteger bytesPerRow = readDesc.region.width * getBytesPerPixel(readDesc.format);
    // 
    // [(__bridge id<MTLTexture>)mTexture getBytes:output
    //                                 bytesPerRow:bytesPerRow
    //                               bytesPerImage:0
    //                                  fromRegion:region
    //                                 mipmapLevel:0
    //                                       slice:0];
    // 
    // // 处理颜色空间转换
    // if (readDesc.colorSpace == TextureReadDesc::ColorSpace::SRGB) {
    //     // Metal的sRGB支持
    // }
    
    return false;
}

// ==== 5.1.2 新增查询接口 ====
uint32_t TextureMtl::getWidth(uint32_t mipLevel) const {
    uint32_t width = mDesc.width;
    for (uint32_t i = 0; i < mipLevel && width > 1; ++i) {
        width = std::max(1u, width / 2);
    }
    return width;
}

uint32_t TextureMtl::getHeight(uint32_t mipLevel) const {
    uint32_t height = mDesc.height;
    for (uint32_t i = 0; i < mipLevel && height > 1; ++i) {
        height = std::max(1u, height / 2);
    }
    return height;
}

void TextureMtl::setSampler(std::shared_ptr<HYSampler> sampler) { 
    mSampler = sampler;
    std::cout << "[TextureMtl] 设置采样器" << std::endl;
}

void TextureMtl::bindSampler(uint32_t activeIndex) {
    std::cout << "[TextureMtl] 绑定采样器到单元 " << activeIndex << std::endl;
}

void TextureMtl::generateMipmaps() { 
    std::cout << "[TextureMtl] 生成Mipmaps" << std::endl;
    
    // TODO: 实际Metal实现：
    // id<MTLCommandQueue> commandQueue = [mDevice newCommandQueue];
    // id<MTLCommandBuffer> commandBuffer = [commandQueue commandBuffer];
    // id<MTLBlitCommandEncoder> blitEncoder = [commandBuffer blitCommandEncoder];
    // 
    // [blitEncoder generateMipmapsForTexture:(__bridge id<MTLTexture>)mTexture];
    // [blitEncoder endEncoding];
    // [commandBuffer commit];
    // [commandBuffer waitUntilCompleted];
}

ResourceHandle TextureMtl::getTextureHandle(uint32_t idx) const {
    ResourceHandle handle;
    handle.ptrHandle = mTexture;
    return handle;
}

ResourceHandle TextureMtl::getResourceHandle() const {
    ResourceHandle handle;
    handle.ptrHandle = mTexture;
    return handle;
}

} // namespace render
} // namespace hyengine
