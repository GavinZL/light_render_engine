#pragma once

#include "../../core/HYResourceCommon.hpp"

namespace hyengine {
namespace render {

/**
 * @brief OpenGL ES 纹理实现
 * 
 * OpenGL ES 3.0 特性：
 * - 不支持分离式采样器对象（采样状态通过 glTexParameteri 设置到纹理上）
 * - 支持 ETC2/EAC 压缩纹理格式（必须支持）
 * - 支持 3D 纹理、纹理数组
 * - 支持浮点纹理（需要扩展）
 * - 支持深度纹理和阴影采样
 */
class TextureGLES : public HYTexture {
public:
    explicit TextureGLES(const TextureDesc& desc);
    virtual ~TextureGLES();

    void doInit() override;
    void doResize(uint32_t width, uint32_t height) override;
    
    // 5.1.2 优化后的接口
    void updateTexture(uint32_t level, const void* data, TextureFormat dataFormat,
                      const TextureRegion& region) override;
    bool readTexture(void* output, const TextureReadDesc& readDesc) override;
    
    // 5.1.2 新增查询接口
    uint32_t getWidth(uint32_t mipLevel = 0) const override;
    uint32_t getHeight(uint32_t mipLevel = 0) const override;
    
    void setSampler(std::shared_ptr<HYSampler> sampler) override;
    void bindSampler(uint32_t activeIndex) override;
    ResourceHandle getTextureHandle(uint32_t idx = 0) const override;
    ResourceHandle getResourceHandle() const override;
    const TextureDesc& getDesc() const override { return mDesc; }
    void generateMipmaps() override;

private:
    void applySamplerState();  // 应用采样器状态到纹理

    TextureDesc mDesc;
    unsigned int mTextureID = 0;
    std::shared_ptr<HYSampler> mSampler;  // ES 3.0 中采样状态存储在纹理上
};

} // namespace render
} // namespace hyengine
