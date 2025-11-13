#pragma once

#include "../../core/HYResourceCommon.hpp"

namespace hyengine {
namespace render {

/**
 * @brief OpenGL ES 着色器实现
 * 
 * OpenGL ES 3.0 着色器特性：
 * - 必须使用 #version 300 es
 * - 必须声明精度限定符（precision mediump float;）
 * - 不支持几何着色器
 * - 支持顶点、片段着色器
 * - ES 3.1 支持计算着色器
 * 
 * 根据5.1.3优化：
 * - 增加Shader反射接口
 * - 使用glGetActiveUniform等API实现反射
 */
class ShaderGLES : public HYShader {
public:
    explicit ShaderGLES(const ShaderDesc& desc);
    virtual ~ShaderGLES();

    // 基本接口
    bool compile() override;
    void bind() override;
    void unbind() override;
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
    
    // Uniform设置接口
    void setUniform(const std::string& name, const void* data, uint32_t size) override;
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
    bool compileShader(uint32_t shader, const std::string& source);
    bool linkProgram();
    void buildReflection() const; // 5.1.3 新增：构建反射信息
    UniformType glTypeToUniformType(unsigned int glType) const; // 类型转换
    VertexFormat glTypeToVertexFormat(unsigned int glType) const; // 类型转换

    ShaderDesc mDesc;
    unsigned int mProgramID = 0;
    unsigned int mVertexShader = 0;
    unsigned int mFragmentShader = 0;
    
    // 5.1.3 新增：反射信息缓存
    mutable ShaderReflection mReflection;
    mutable bool mReflectionBuilt = false;
};

} // namespace render
} // namespace hyengine
