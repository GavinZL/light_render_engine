#pragma once
#include "hy_render/resources/HYTexture.hpp"

namespace hyengine { namespace render {
class TextureGL : public HYTexture {
public:
    explicit TextureGL(const TextureDesc& desc);
    virtual ~TextureGL();
    void doInit() override {}
    void doResize(uint32_t width, uint32_t height) override {}
    void updateTexture(uint32_t level, const void* data, TextureFormat format, 
                      uint32_t width, uint32_t height, const TextureRegion& region) override {}
    bool readTexture(void* output, TextureFormat format, const TextureRegion& region) override { return false; }
    void setSampler(std::shared_ptr<HYSampler> sampler) override {}
    void bindSampler(uint32_t activeIndex) override {}
    ResourceHandle getTextureHandle(uint32_t idx = 0) const override { return ResourceHandle(); }
    const TextureDesc& getDesc() const override { return mDesc; }
    void generateMipmaps() override {}
private:
    TextureDesc mDesc;
    unsigned int mTextureID = 0;
};
}}
