#pragma once

#include "../HYRenderDef.hpp"
#include <memory>

namespace hyengine {
namespace render {

class HYSampler;

/**
 * @brief 纹理对象抽象
 * 
 * 根据设计文档5.1.2优化：
 * - 引入TextureReadDesc描述读取配置
 * - 优化updateTexture参数设计
 * - 增加查询接口（getWidth、getHeight等）
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

    // ==== 数据更新接口（优化后） ====
    /**
     * @brief 更新纹理数据（简化版）
     * @param level Mipmap层级
     * @param data 像素数据
     * @param dataFormat 数据格式
     * @param region 更新区域
     */
    virtual void updateTexture(uint32_t level, const void* data, 
                              TextureFormat dataFormat, const TextureRegion& region) = 0;

    /**
     * @brief 读取纹理数据（优化后）
     * @param output 输出缓冲区
     * @param readDesc 读取描述
     */
    virtual bool readTexture(void* output, const TextureReadDesc& readDesc) = 0;

    // ==== Mipmap接口 ====
    /**
     * @brief 生成Mipmap链
     */
    virtual void generateMipmaps() = 0;

    // ==== 查询接口（5.1.2新增） ====
    /**
     * @brief 获取指定Mip层宽度
     */
    virtual uint32_t getWidth(uint32_t mipLevel = 0) const = 0;
    
    /**
     * @brief 获取指定Mip层高度
     */
    virtual uint32_t getHeight(uint32_t mipLevel = 0) const = 0;

    // ==== 采样器接口 ====
    /**
     * @brief 设置采样器
     */
    virtual void setSampler(std::shared_ptr<HYSampler> sampler) = 0;

    /**
     * @brief 绑定采样器到激活单元
     */
    virtual void bindSampler(uint32_t activeIndex) = 0;

    // ==== 资源接口 ====
    /**
     * @brief 获取纹理句柄
     */
    virtual ResourceHandle getTextureHandle(uint32_t idx = 0) const = 0;

    /**
     * @brief 获取纹理描述
     */
    virtual const TextureDesc& getDesc() const = 0;
};

} // namespace render
} // namespace hyengine
