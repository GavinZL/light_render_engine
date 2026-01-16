#pragma once

#include "../../core/HYResourceCommon.hpp"

namespace hyengine {
namespace render {

/**
 * @brief Metal 纹理实现
 */
/**
 * @brief Metal 纹理实现
 * 
 * Metal 纹理特点：
 * - 使用 MTLTexture 对象
 * - 支持多种像素格式
 * - 原生支持Mipmap生成
 * - GPU高效的纹理采样
 * 
 * 根据5.1.2优化：
 * - 优化updateTexture参数
 * - 支持TextureReadDesc
 * - 增加getWidth/getHeight查询接口
 */
class TextureMtl : public HYTexture {
public:
    TextureMtl(const TextureDesc& desc, void* device);
    virtual ~TextureMtl();

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
    TextureDesc mDesc;
    void* mTexture = nullptr;  // id<MTLTexture>
    void* mDevice = nullptr;   // id<MTLDevice>
    std::shared_ptr<HYSampler> mSampler;
};

} // namespace render
} // namespace hyengine
