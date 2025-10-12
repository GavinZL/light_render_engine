#pragma once

#include "HYRenderDef.hpp"
#include <memory>

namespace hyengine {
namespace render {

class HYSampler;

/**
 * @brief 纹理对象抽象
 */
class HYTexture : public HYResource {
public:
    HYTexture() : HYResource(ResourceType::kTexture) {}
    virtual ~HYTexture() = default;

    /**
     * @brief 初始化纹理（离屏渲染用）
     */
    virtual void doInit() = 0;

    /**
     * @brief 调整纹理尺寸
     */
    virtual void doResize(uint32_t width, uint32_t height) = 0;

    /**
     * @brief 更新纹理数据
     * @param level Mipmap层级
     * @param data 像素数据
     * @param format 数据格式
     * @param width 宽度
     * @param height 高度
     * @param region 更新区域
     */
    virtual void updateTexture(uint32_t level, const void* data, 
                              TextureFormat format, uint32_t width, uint32_t height,
                              const TextureRegion& region) = 0;

    /**
     * @brief 读取纹理数据
     */
    virtual bool readTexture(void* output, TextureFormat format, const TextureRegion& region) = 0;

    /**
     * @brief 设置采样器
     */
    virtual void setSampler(std::shared_ptr<HYSampler> sampler) = 0;

    /**
     * @brief 绑定采样器到激活单元
     */
    virtual void bindSampler(uint32_t activeIndex) = 0;

    /**
     * @brief 获取纹理句柄
     */
    virtual ResourceHandle getTextureHandle(uint32_t idx = 0) const = 0;

    /**
     * @brief 获取纹理描述
     */
    virtual const TextureDesc& getDesc() const = 0;

    /**
     * @brief 生成Mipmap链
     */
    virtual void generateMipmaps() = 0;
};

} // namespace render
} // namespace hyengine
