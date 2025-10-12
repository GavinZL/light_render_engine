#pragma once

#include "HYRenderDef.hpp"
#include <memory>

namespace hyengine {
namespace render {

class HYBuffer;
class HYTexture;

/**
 * @brief 着色器对象抽象
 */
class HYShader : public HYResource {
public:
    HYShader() : HYResource(ResourceType::kShader) {}
    virtual ~HYShader() = default;

    /**
     * @brief 初始化（编译链接着色器）
     */
    virtual void doInit() = 0;

    /**
     * @brief 绑定纹理到Uniform
     * @param activeIndex 纹理单元索引
     * @param name Uniform名称
     * @param texture 纹理对象
     * @param texIdx 纹理句柄索引
     */
    virtual void bindTexture(uint32_t activeIndex, const char* name, 
                            std::shared_ptr<HYTexture> texture, uint32_t texIdx = 0) = 0;

    /**
     * @brief 绑定UBO到绑定点
     * @param bindingPoint 绑定点
     * @param name UBO名称
     * @param buffer 缓冲区对象
     */
    virtual void bindUBO(uint32_t bindingPoint, const char* name, 
                        std::shared_ptr<HYBuffer> buffer) = 0;

    /**
     * @brief 设置Uniform值（支持多种类型）
     */
    virtual void setUniformFloat(const char* name, float value) = 0;
    virtual void setUniformInt(const char* name, int value) = 0;
    virtual void setUniformVec2(const char* name, const float* value) = 0;
    virtual void setUniformVec3(const char* name, const float* value) = 0;
    virtual void setUniformVec4(const char* name, const float* value) = 0;
    virtual void setUniformMat3(const char* name, const float* value) = 0;
    virtual void setUniformMat4(const char* name, const float* value) = 0;

    /**
     * @brief 获取顶点着色器句柄
     */
    virtual ResourceHandle getVertexStageHandle() const = 0;

    /**
     * @brief 获取片段着色器句柄
     */
    virtual ResourceHandle getFragmentStageHandle() const = 0;

    /**
     * @brief 获取着色器描述
     */
    virtual const ShaderDesc& getDesc() const = 0;
};

} // namespace render
} // namespace hyengine
