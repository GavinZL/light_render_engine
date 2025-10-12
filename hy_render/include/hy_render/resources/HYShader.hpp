#pragma once

#include "../HYRenderDef.hpp"
#include <memory>

namespace hyengine {
namespace render {

class HYBuffer;
class HYTexture;

/**
 * @brief 着色器对象抽象
 * 
 * 根据设计文档5.1.3优化：
 * - 增加Shader反射接口，自动解析着色器中的变量信息
 * - 将资源绑定逻辑移至RenderPass或CommandBuffer
 * - Shader主要负责编译和反射
 */
class HYShader : public HYResource {
public:
    HYShader() : HYResource(ResourceType::kShader) {}
    virtual ~HYShader() = default;

    // ==== 编译相关接口 ====
    /**
     * @brief 初始化（编译链接着色器）
     */
    virtual void doInit() = 0;

    /**
     * @brief 编译着色器
     */
    virtual bool compile() = 0;

    // ==== Shader反射接口（5.1.3新增） ====
    /**
     * @brief 获取Uniform位置
     */
    virtual int32_t getUniformLocation(const std::string& name) const = 0;
    
    /**
     * @brief 获取Attribute位置
     */
    virtual int32_t getAttributeLocation(const std::string& name) const = 0;
    
    /**
     * @brief 获取UniformBlock索引
     */
    virtual int32_t getUniformBlockIndex(const std::string& name) const = 0;
    
    /**
     * @brief 获取Uniform详情
     */
    virtual UniformInfo getUniformInfo(const std::string& name) const = 0;
    
    /**
     * @brief 获取Attribute详情
     */
    virtual AttributeInfo getAttributeInfo(const std::string& name) const = 0;
    
    /**
     * @brief 获取所有Uniform
     */
    virtual std::vector<UniformInfo> getAllUniforms() const = 0;
    
    /**
     * @brief 获取所有Attribute
     */
    virtual std::vector<AttributeInfo> getAllAttributes() const = 0;
    
    /**
     * @brief 获取完整反射信息
     */
    virtual const ShaderReflection& getReflection() const = 0;

    // ==== 简化后的Uniform设置接口（为RenderPass提供） ====
    /**
     * @brief 设置Uniform值（通用接口）
     */
    virtual void setUniform(const std::string& name, const void* data, uint32_t size) = 0;
    
    /**
     * @brief 设置Uniform值（支持多种类型）
     */
    virtual void setUniformFloat(const std::string& name, float value) = 0;
    virtual void setUniformInt(const std::string& name, int value) = 0;
    virtual void setUniformVec2(const std::string& name, const float* value) = 0;
    virtual void setUniformVec3(const std::string& name, const float* value) = 0;
    virtual void setUniformVec4(const std::string& name, const float* value) = 0;
    virtual void setUniformMat3(const std::string& name, const float* value) = 0;
    virtual void setUniformMat4(const std::string& name, const float* value) = 0;

    // ==== 绑定相关接口 ====
    /**
     * @brief 绑定着色器（仅在渲染时使用）
     */
    virtual void bind() = 0;

    /**
     * @brief 解绑着色器
     */
    virtual void unbind() = 0;

    // ==== 资源接口 ====
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

    // ==== 已弃用的接口（将在RenderPass中实现） ====
    /**
     * @deprecated 将在RenderPass中实现统一的资源绑定
     */
    virtual void bindTexture(uint32_t activeIndex, const std::string& name, 
                            std::shared_ptr<HYTexture> texture, uint32_t texIdx = 0) = 0;

    /**
     * @deprecated 将在RenderPass中实现统一的资源绑定
     */
    virtual void bindUBO(uint32_t bindingPoint, const std::string& name, 
                        std::shared_ptr<HYBuffer> buffer) = 0;
};

} // namespace render
} // namespace hyengine
