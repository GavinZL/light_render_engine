#include "ShaderGL.hpp"
#include <iostream>

namespace hyengine {
namespace render {

ShaderGL::ShaderGL(const ShaderDesc& desc) : mDesc(desc) {
    mProgramID = 1; // 假设的ID
    std::cout << "[ShaderGL] Created shader (simulated): " << mDesc.name << std::endl;
}

ShaderGL::~ShaderGL() {
    if (mProgramID != 0) {
        std::cout << "[ShaderGL] Deleted shader (simulated) ID=" << mProgramID << std::endl;
        mProgramID = 0;
    }
}

void ShaderGL::doInit() {
    std::cout << "[ShaderGL] doInit called for: " << mDesc.name << std::endl;
    // TODO: 实际实现中这里会编译和链接着色器
}

bool ShaderGL::compile() {
    std::cout << "[ShaderGL] compile() called" << std::endl;
    // TODO: 实际实现中编译顶点和片段着色器
    // 1. 创建顶点着色器和片段着色器对象
    // 2. 编译着色器源代码
    // 3. 检查编译错误
    // 4. 创建程序对象并链接
    // 5. 验证程序
    return true;
}

void ShaderGL::bind() {
    std::cout << "[ShaderGL] bind() called" << std::endl;
    // TODO: glUseProgram(mProgramID);
}

void ShaderGL::unbind() {
    std::cout << "[ShaderGL] unbind() called" << std::endl;
    // TODO: glUseProgram(0);
}

void ShaderGL::setUniform(const std::string& name, const void* data, uint32_t size) {
    std::cout << "[ShaderGL] setUniform: " << name << " size=" << size << std::endl;
    // TODO: 根据size调用对应的glUniform*函数
    // 1. 获取uniform位置: GLint location = glGetUniformLocation(mProgramID, name.c_str());
    // 2. 根据size判断类型并调用对应的glUniform*
}

ResourceHandle ShaderGL::getVertexStageHandle() const {
    ResourceHandle handle;
    handle.uintHandle = mVertexShaderID;
    return handle;
}

ResourceHandle ShaderGL::getFragmentStageHandle() const {
    ResourceHandle handle;
    handle.uintHandle = mFragmentShaderID;
    return handle;
}

ResourceHandle ShaderGL::getResourceHandle() const {
    ResourceHandle handle;
    handle.uintHandle = mProgramID;
    return handle;
}

// ============================================================================
// 5.1.3 新增的Shader反射接口实现
// ============================================================================

int32_t ShaderGL::getUniformLocation(const std::string& name) const {
    std::cout << "[ShaderGL] getUniformLocation: " << name << std::endl;
    // TODO: 实际实现中调用 glGetUniformLocation
    // return glGetUniformLocation(mProgramID, name.c_str());
    
    // 模拟返回一些已知的uniform位置
    static std::unordered_map<std::string, int32_t> mockLocations = {
        {"u_mvpMatrix", 0}, {"u_modelMatrix", 1}, {"u_viewMatrix", 2}, 
        {"u_projMatrix", 3}, {"u_color", 4}, {"u_time", 5}
    };
    
    auto it = mockLocations.find(name);
    return (it != mockLocations.end()) ? it->second : -1;
}

int32_t ShaderGL::getAttributeLocation(const std::string& name) const {
    std::cout << "[ShaderGL] getAttributeLocation: " << name << std::endl;
    // TODO: 实际实现中调用 glGetAttribLocation
    // return glGetAttribLocation(mProgramID, name.c_str());
    
    // 模拟返回一些已知的attribute位置
    static std::unordered_map<std::string, int32_t> mockLocations = {
        {"a_position", 0}, {"a_normal", 1}, {"a_texCoord", 2}, {"a_color", 3}
    };
    
    auto it = mockLocations.find(name);
    return (it != mockLocations.end()) ? it->second : -1;
}

int32_t ShaderGL::getUniformBlockIndex(const std::string& name) const {
    std::cout << "[ShaderGL] getUniformBlockIndex: " << name << std::endl;
    // TODO: 实际实现中调用 glGetUniformBlockIndex
    // return glGetUniformBlockIndex(mProgramID, name.c_str());
    
    // 模拟返回一些已知的uniform block索引
    static std::unordered_map<std::string, int32_t> mockIndices = {
        {"CameraUBO", 0}, {"MaterialUBO", 1}, {"LightUBO", 2}
    };
    
    auto it = mockIndices.find(name);
    return (it != mockIndices.end()) ? it->second : -1;
}

UniformInfo ShaderGL::getUniformInfo(const std::string& name) const {
    // 确保反射信息已构建
    if (!mReflectionBuilt) {
        buildReflection();
    }
    
    // 查找uniform信息
    for (const auto& uniform : mReflection.uniforms) {
        if (uniform.name == name) {
            return uniform;
        }
    }
    
    // 未找到时返回空的UniformInfo
    UniformInfo info;
    info.name = name;
    info.location = -1;
    return info;
}

AttributeInfo ShaderGL::getAttributeInfo(const std::string& name) const {
    // 确保反射信息已构建
    if (!mReflectionBuilt) {
        buildReflection();
    }
    
    // 查找attribute信息
    for (const auto& attribute : mReflection.attributes) {
        if (attribute.name == name) {
            return attribute;
        }
    }
    
    // 未找到时返回空的AttributeInfo
    AttributeInfo info;
    info.name = name;
    info.location = -1;
    return info;
}

std::vector<UniformInfo> ShaderGL::getAllUniforms() const {
    if (!mReflectionBuilt) {
        buildReflection();
    }
    return mReflection.uniforms;
}

std::vector<AttributeInfo> ShaderGL::getAllAttributes() const {
    if (!mReflectionBuilt) {
        buildReflection();
    }
    return mReflection.attributes;
}

const ShaderReflection& ShaderGL::getReflection() const {
    if (!mReflectionBuilt) {
        buildReflection();
    }
    return mReflection;
}

void ShaderGL::buildReflection() const {
    std::cout << "[ShaderGL] 构建Shader反射信息..." << std::endl;
    
    mReflection.uniforms.clear();
    mReflection.attributes.clear();
    mReflection.uniformBlocks.clear();
    
    // TODO: 实际实现中使用OpenGL API获取反射信息
    // GLint numUniforms = 0;
    // glGetProgramiv(mProgramID, GL_ACTIVE_UNIFORMS, &numUniforms);
    // 
    // for (GLint i = 0; i < numUniforms; ++i) {
    //     GLchar name[256];
    //     GLsizei length;
    //     GLint size;
    //     GLenum glType;
    //     glGetActiveUniform(mProgramID, i, sizeof(name), &length, &size, &glType, name);
    //     
    //     UniformInfo info;
    //     info.name = std::string(name);
    //     info.type = glTypeToUniformType(glType);
    //     info.location = glGetUniformLocation(mProgramID, name);
    //     info.arraySize = size;
    //     mReflection.uniforms.push_back(info);
    // }
    
    // 模拟一些反射数据用于测试
    UniformInfo mvpUniform;
    mvpUniform.name = "u_mvpMatrix";
    mvpUniform.type = UniformType::kMat4;
    mvpUniform.location = 0;
    mvpUniform.arraySize = 1;
    mReflection.uniforms.push_back(mvpUniform);
    
    UniformInfo colorUniform;
    colorUniform.name = "u_color";
    colorUniform.type = UniformType::kVec4;
    colorUniform.location = 4;
    colorUniform.arraySize = 1;
    mReflection.uniforms.push_back(colorUniform);
    
    AttributeInfo positionAttr;
    positionAttr.name = "a_position";
    positionAttr.format = VertexFormat::kFloat3;
    positionAttr.location = 0;
    mReflection.attributes.push_back(positionAttr);
    
    AttributeInfo texCoordAttr;
    texCoordAttr.name = "a_texCoord";
    texCoordAttr.format = VertexFormat::kFloat2;
    texCoordAttr.location = 2;
    mReflection.attributes.push_back(texCoordAttr);
    
    mReflectionBuilt = true;
    std::cout << "[ShaderGL] Shader反射信息构建完成 - Uniforms: " << mReflection.uniforms.size() 
              << ", Attributes: " << mReflection.attributes.size() << std::endl;
}

UniformType ShaderGL::glTypeToUniformType(uint32_t glType) const {
    // TODO: 实际实现中转换OpenGL类型到UniformType
    // 模拟实现
    switch (glType) {
        case 0x1406: return UniformType::kFloat;      // GL_FLOAT
        case 0x8B50: return UniformType::kVec2;       // GL_FLOAT_VEC2
        case 0x8B51: return UniformType::kVec3;       // GL_FLOAT_VEC3
        case 0x8B52: return UniformType::kVec4;       // GL_FLOAT_VEC4
        case 0x1404: return UniformType::kInt;        // GL_INT
        case 0x8B53: return UniformType::kIVec2;      // GL_INT_VEC2
        case 0x8B54: return UniformType::kIVec3;      // GL_INT_VEC3
        case 0x8B55: return UniformType::kIVec4;      // GL_INT_VEC4
        case 0x8B5B: return UniformType::kMat3;       // GL_FLOAT_MAT3
        case 0x8B5C: return UniformType::kMat4;       // GL_FLOAT_MAT4
        case 0x8B5E: return UniformType::kSampler2D;  // GL_SAMPLER_2D
        case 0x8B60: return UniformType::kSamplerCube;// GL_SAMPLER_CUBE
        default: return UniformType::kFloat; // 默认值
    }
}

VertexFormat ShaderGL::glTypeToVertexFormat(uint32_t glType) const {
    // TODO: 实际实现中转换OpenGL类型到VertexFormat
    // 模拟实现
    switch (glType) {
        case 0x1406: return VertexFormat::kFloat;    // GL_FLOAT
        case 0x8B50: return VertexFormat::kFloat2;   // GL_FLOAT_VEC2
        case 0x8B51: return VertexFormat::kFloat3;   // GL_FLOAT_VEC3
        case 0x8B52: return VertexFormat::kFloat4;   // GL_FLOAT_VEC4
        case 0x1404: return VertexFormat::kInt;      // GL_INT
        case 0x8B53: return VertexFormat::kInt2;     // GL_INT_VEC2
        case 0x8B54: return VertexFormat::kInt3;     // GL_INT_VEC3
        case 0x8B55: return VertexFormat::kInt4;     // GL_INT_VEC4
        default: return VertexFormat::kFloat; // 默认值
    }
}

// ============================================================================
// 5.1.3 优化的Uniform设置接口（使用std::string参数）
// ============================================================================

void ShaderGL::setUniformFloat(const std::string& name, float value) {
    std::cout << "[ShaderGL] setUniformFloat: " << name << " = " << value << std::endl;
    // TODO: 实际实现中这里会调用glUniform1f
}

void ShaderGL::setUniformInt(const std::string& name, int value) {
    std::cout << "[ShaderGL] setUniformInt: " << name << " = " << value << std::endl;
    // TODO: 实际实现中这里会调用glUniform1i
}

void ShaderGL::setUniformVec2(const std::string& name, const float* value) {
    std::cout << "[ShaderGL] setUniformVec2: " << name << " = (" << value[0] << "," << value[1] << ")" << std::endl;
    // TODO: 实际实现中这里会调用glUniform2fv
}

void ShaderGL::setUniformVec3(const std::string& name, const float* value) {
    std::cout << "[ShaderGL] setUniformVec3: " << name << " = (" << value[0] << "," << value[1] << "," << value[2] << ")" << std::endl;
    // TODO: 实际实现中这里会调用glUniform3fv
}

void ShaderGL::setUniformVec4(const std::string& name, const float* value) {
    std::cout << "[ShaderGL] setUniformVec4: " << name << " = (" << value[0] << "," << value[1] << "," << value[2] << "," << value[3] << ")" << std::endl;
    // TODO: 实际实现中这里会调用glUniform4fv
}

void ShaderGL::setUniformMat3(const std::string& name, const float* value) {
    std::cout << "[ShaderGL] setUniformMat3: " << name << std::endl;
    // TODO: 实际实现中这里会调用glUniformMatrix3fv
}

void ShaderGL::setUniformMat4(const std::string& name, const float* value) {
    std::cout << "[ShaderGL] setUniformMat4: " << name << std::endl;
    // TODO: 实际实现中这里会调用glUniformMatrix4fv
}

// ============================================================================
// 5.1.3 已弃用的资源绑定接口（使用std::string参数）
// ============================================================================

void ShaderGL::bindTexture(uint32_t activeIndex, const std::string& name, 
                          std::shared_ptr<HYTexture> texture, uint32_t texIdx) {
    std::cout << "[ShaderGL] bindTexture: " << name << " to unit " << activeIndex << std::endl;
    // TODO: 实际实现中这里会调用glUniform1i设置纹理单元
}

void ShaderGL::bindUBO(uint32_t bindingPoint, const std::string& name, 
                      std::shared_ptr<HYBuffer> buffer) {
    std::cout << "[ShaderGL] bindUBO: " << name << " to binding " << bindingPoint << std::endl;
    // TODO: 实际实现中这里会调用glBindBufferBase
}

} // namespace render
} // namespace hyengine