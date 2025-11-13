#pragma once

#include "../../core/HYResourceCommon.hpp"

namespace hyengine {
namespace render {

/**
 * @brief OpenGL纹理实现
 * 
 * 根据5.1.2优化：
 * - 优化updateTexture参数
 * - 支持TextureReadDesc
 * - 增加getWidth/getHeight查询接口
 */
class TextureGL : public HYTexture {
public:
    explicit TextureGL(const TextureDesc& desc);
    virtual ~TextureGL();

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
    const TextureDesc& getDesc() const override { return mDesc; }
    void generateMipmaps() override;
    ResourceHandle getResourceHandle() const override;

private:
    TextureDesc mDesc;
    unsigned int mTextureID = 0;
    std::shared_ptr<HYSampler> mSampler;
};

} // namespace render
} // namespace hyengine