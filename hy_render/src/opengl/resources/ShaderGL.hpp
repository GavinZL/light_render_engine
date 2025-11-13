#pragma once

#include "../../core/HYResourceCommon.hpp"

namespace hyengine {
namespace render {

/**
 * @brief OpenGL着色器实现
 */
class ShaderGL : public HYShader {
public:
    explicit ShaderGL(const ShaderDesc& desc);
    virtual ~ShaderGL();

    // 基类核心接口
    bool compile() override;
    void bind() override;
    void unbind() override;
    void setUniform(const std::string& name, const void* data, uint32_t size) override;
    void doInit() override;
    
    // 5.1.3 新增的Shader反射接口
    int32_t getUniformLocation(const std::string& name) const override;
    int32_t getAttributeLocation(const std::string& name) const override;
    int32_t getUniformBlockIndex(const std::string& name) const override;
    UniformInfo getUniformInfo(const std::string& name) const override;
    AttributeInfo getAttributeInfo(const std::string& name) const override;
    std::vector<UniformInfo> getAllUniforms() const override;
    std::vector<AttributeInfo> getAllAttributes() const override;
    const ShaderReflection& getReflection() const override;
    
    // 5.1.3 优化的Uniform设置接口（使用std::string参数）
    void setUniformFloat(const std::string& name, float value) override;
    void setUniformInt(const std::string& name, int value) override;
    void setUniformVec2(const std::string& name, const float* value) override;
    void setUniformVec3(const std::string& name, const float* value) override;
    void setUniformVec4(const std::string& name, const float* value) override;
    void setUniformMat3(const std::string& name, const float* value) override;
    void setUniformMat4(const std::string& name, const float* value) override;
    
    // 已弃用的资源绑定接口
    void bindTexture(uint32_t activeIndex, const std::string& name, 
                    std::shared_ptr<HYTexture> texture, uint32_t texIdx = 0) override;
    void bindUBO(uint32_t bindingPoint, const std::string& name, 
                std::shared_ptr<HYBuffer> buffer) override;
    
    ResourceHandle getVertexStageHandle() const override;
    ResourceHandle getFragmentStageHandle() const override;
    const ShaderDesc& getDesc() const override { return mDesc; }
    ResourceHandle getResourceHandle() const override;

private:
    void buildReflection() const; // 5.1.3 新增：构建反射信息
    UniformType glTypeToUniformType(unsigned int glType) const; // 类型转换
    VertexFormat glTypeToVertexFormat(unsigned int glType) const; // 类型转换
    
    ShaderDesc mDesc;
    unsigned int mProgramID = 0;
    unsigned int mVertexShaderID = 0;
    unsigned int mFragmentShaderID = 0;
    
    // 5.1.3 新增：反射信息缓存
    mutable ShaderReflection mReflection;
    mutable bool mReflectionBuilt = false;
};

} // namespace render
} // namespace hyengine