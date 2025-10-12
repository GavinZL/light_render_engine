#include "ShaderGLES.hpp"
#include "../../gl_common/GLUtils.hpp"
#include <iostream>
#include <unordered_map>

namespace hyengine {
namespace render {

// ============================================================================
// ShaderGLES 实现
// ============================================================================

ShaderGLES::ShaderGLES(const ShaderDesc& desc) : mDesc(desc) {
    std::cout << "[ShaderGLES] 创建Shader程序: " << desc.name << std::endl;
    
    // TODO: 在实际的OpenGL ES环境中：
    // mProgramID = glCreateProgram();
    
    mProgramID = 1; // 模拟ID
    
    std::cout << "[ShaderGLES] Shader语言: ";
    switch (desc.sourceLanguage) {
        case ShaderSourceLanguage::kShaderSourceLanguage_GLSL:
            std::cout << "GLSL ES";
            break;
        default:
            std::cout << "未知";
            break;
    }
    std::cout << std::endl;
}

ShaderGLES::~ShaderGLES() {
    if (mProgramID != 0) {
        // TODO:
        // if (mVertexShader) glDeleteShader(mVertexShader);
        // if (mFragmentShader) glDeleteShader(mFragmentShader);
        // glDeleteProgram(mProgramID);
        
        std::cout << "[ShaderGLES] Destroyed shader program" << std::endl;
        mProgramID = 0;
    }
}

bool ShaderGLES::compile() {
    std::cout << "[ShaderGLES] 编译Shader..." << std::endl;
    
    // OpenGL ES 3.0 着色器特殊要求：
    // 1. 必须以 #version 300 es 开头
    // 2. 必须声明精度限定符，例如：precision mediump float;
    // 3. 不支持几何着色器
    
    bool success = true;
    
    // 编译各个着色器阶段
    for (const auto& codeDesc : mDesc.shaderCodeDesc) {
        std::cout << "[ShaderGLES] 编译阶段: ";
        
        unsigned int shaderType = 0;
        const char* stageName = "";
        
        if (codeDesc.shaderStage == ShaderStage::kShaderStage_Vertex) {
            // TODO: shaderType = GL_VERTEX_SHADER;
            shaderType = 0x8B31; // GL_VERTEX_SHADER
            stageName = "顶点着色器";
            mVertexShader = 1; // 模拟ID
        } else if (codeDesc.shaderStage == ShaderStage::kShaderStage_Fragment) {
            // TODO: shaderType = GL_FRAGMENT_SHADER;
            shaderType = 0x8B30; // GL_FRAGMENT_SHADER
            stageName = "片段着色器";
            mFragmentShader = 2; // 模拟ID
        }
        
        std::cout << stageName << std::endl;
        
        // TODO: 实际编译
        // unsigned int shader = glCreateShader(shaderType);
        // if (!compileShader(shader, codeDesc.shaderCode)) {
        //     success = false;
        //     break;
        // }
        
        // 验证着色器代码中是否包含必要的ES特性
        if (codeDesc.shaderCode != nullptr) {
            std::string code(codeDesc.shaderCode);
            if (code.find("#version 300 es") == std::string::npos && 
                code.find("#version 310 es") == std::string::npos) {
                std::cout << "[ShaderGLES] 警告: Shader代码中未找到ES版本指示" << std::endl;
            }
            if (code.find("precision") == std::string::npos) {
                std::cout << "[ShaderGLES] 警告: Shader代码中未找到精度限定符" << std::endl;
            }
        }
    }
    
    if (success) {
        // TODO: 链接程序
        // success = linkProgram();
        std::cout << "[ShaderGLES] Shader程序链接成功" << std::endl;
    }
    
    return success;
}

void ShaderGLES::doInit() {
    std::cout << "[ShaderGLES] doInit called" << std::endl;
    // TODO: 实际实现中这里会编译和链接着色器
}

void ShaderGLES::bindTexture(uint32_t activeIndex, const std::string& name, std::shared_ptr<HYTexture> texture, uint32_t texIdx) {
    std::cout << "[ShaderGLES] bindTexture: " << name << " to unit " << activeIndex << std::endl;
    // TODO: 实际实现中这里会调用glUniform1i设置纹理单元
}

void ShaderGLES::bindUBO(uint32_t bindingPoint, const std::string& name, std::shared_ptr<HYBuffer> buffer) {
    std::cout << "[ShaderGLES] bindUBO: " << name << " to binding " << bindingPoint << std::endl;
    // TODO: 实际实现中这里会调用glBindBufferBase
}

ResourceHandle ShaderGLES::getVertexStageHandle() const {
    ResourceHandle handle;
    handle.uintHandle = mVertexShader;
    return handle;
}

ResourceHandle ShaderGLES::getFragmentStageHandle() const {
    ResourceHandle handle;
    handle.uintHandle = mFragmentShader;
    return handle;
}

bool ShaderGLES::compileShader(uint32_t shader, const std::string& source) {
    // TODO:
    // const char* src = source.c_str();
    // glShaderSource(shader, 1, &src, nullptr);
    // glCompileShader(shader);
    // 
    // GLint success;
    // glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    // if (!success) {
    //     char infoLog[512];
    //     glGetShaderInfoLog(shader, 512, nullptr, infoLog);
    //     std::cerr << "[ShaderGLES] Shader compilation failed: " << infoLog << std::endl;
    //     return false;
    // }
    return true;
}

bool ShaderGLES::linkProgram() {
    // TODO:
    // glAttachShader(mProgramID, mVertexShader);
    // glAttachShader(mProgramID, mFragmentShader);
    // glLinkProgram(mProgramID);
    // 
    // GLint success;
    // glGetProgramiv(mProgramID, GL_LINK_STATUS, &success);
    // if (!success) {
    //     char infoLog[512];
    //     glGetProgramInfoLog(mProgramID, 512, nullptr, infoLog);
    //     std::cerr << "[ShaderGLES] Program linking failed: " << infoLog << std::endl;
    //     return false;
    // }
    return true;
}

void ShaderGLES::bind() {
    std::cout << "[ShaderGLES] Binding shader program" << std::endl;
    // TODO: glUseProgram(mProgramID);
}

void ShaderGLES::unbind() {
    std::cout << "[ShaderGLES] Unbinding shader program" << std::endl;
    // TODO: glUseProgram(0);
}

void ShaderGLES::setUniform(const std::string& name, const void* data, uint32_t size) {
    std::cout << "[ShaderGLES] Setting uniform: " << name << std::endl;
    
    // TODO:
    // GLint location = glGetUniformLocation(mProgramID, name.c_str());
    // if (location == -1) {
    //     std::cerr << "[ShaderGLES] Uniform not found: " << name << std::endl;
    //     return;
    // }
    // 
    // // 根据 size 决定调用哪个 glUniform* 函数
    // // 例如：
    // // glUniform1f, glUniform2f, glUniform3f, glUniform4f
    // // glUniform1i, glUniform2i, glUniform3i, glUniform4i
    // // glUniformMatrix4fv 等
}

// ============================================================================
// 5.1.3 新增的Shader反射接口实现
// ============================================================================

int32_t ShaderGLES::getUniformLocation(const std::string& name) const {
    std::cout << "[ShaderGLES] getUniformLocation: " << name << std::endl;
    // TODO: 实际实现中调用 glGetUniformLocation
    // return glGetUniformLocation(mProgramID, name.c_str());
    
    // 模拟返回一些已知的uniform位置
    static std::unordered_map<std::string, int32_t> mockLocations = {
        {"u_mvpMatrix", 0},
        {"u_modelMatrix", 1},
        {"u_viewMatrix", 2},
        {"u_projMatrix", 3},
        {"u_color", 4},
        {"u_time", 5}
    };
    
    auto it = mockLocations.find(name);
    return (it != mockLocations.end()) ? it->second : -1;
}

int32_t ShaderGLES::getAttributeLocation(const std::string& name) const {
    std::cout << "[ShaderGLES] getAttributeLocation: " << name << std::endl;
    // TODO: 实际实现中调用 glGetAttribLocation
    // return glGetAttribLocation(mProgramID, name.c_str());
    
    // 模拟返回一些已知的attribute位置
    static std::unordered_map<std::string, int32_t> mockLocations = {
        {"a_position", 0},
        {"a_normal", 1},
        {"a_texCoord", 2},
        {"a_color", 3}
    };
    
    auto it = mockLocations.find(name);
    return (it != mockLocations.end()) ? it->second : -1;
}

int32_t ShaderGLES::getUniformBlockIndex(const std::string& name) const {
    std::cout << "[ShaderGLES] getUniformBlockIndex: " << name << std::endl;
    // TODO: 实际实现中调用 glGetUniformBlockIndex
    // return glGetUniformBlockIndex(mProgramID, name.c_str());
    
    // 模拟返回一些已知的uniform block索引
    static std::unordered_map<std::string, int32_t> mockIndices = {
        {"CameraUBO", 0}, {"MaterialUBO", 1}, {"LightUBO", 2}
    };
    
    auto it = mockIndices.find(name);
    return (it != mockIndices.end()) ? it->second : -1;
}

UniformInfo ShaderGLES::getUniformInfo(const std::string& name) const {
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

AttributeInfo ShaderGLES::getAttributeInfo(const std::string& name) const {
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

std::vector<UniformInfo> ShaderGLES::getAllUniforms() const {
    if (!mReflectionBuilt) {
        buildReflection();
    }
    return mReflection.uniforms;
}

std::vector<AttributeInfo> ShaderGLES::getAllAttributes() const {
    if (!mReflectionBuilt) {
        buildReflection();
    }
    return mReflection.attributes;
}

const ShaderReflection& ShaderGLES::getReflection() const {
    if (!mReflectionBuilt) {
        buildReflection();
    }
    return mReflection;
}

void ShaderGLES::buildReflection() const {
    std::cout << "[ShaderGLES] 构建Shader反射信息..." << std::endl;
    
    mReflection.uniforms.clear();
    mReflection.attributes.clear();
    mReflection.uniformBlocks.clear();
    
    // TODO: 实际实现中使用OpenGL ES API获取反射信息
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
    // 
    // GLint numAttributes = 0;
    // glGetProgramiv(mProgramID, GL_ACTIVE_ATTRIBUTES, &numAttributes);
    // 
    // for (GLint i = 0; i < numAttributes; ++i) {
    //     GLchar name[256];
    //     GLsizei length;
    //     GLint size;
    //     GLenum glType;
    //     glGetActiveAttrib(mProgramID, i, sizeof(name), &length, &size, &glType, name);
    //     
    //     AttributeInfo info;
    //     info.name = std::string(name);
    //     info.format = glTypeToVertexFormat(glType);
    //     info.location = glGetAttribLocation(mProgramID, name);
    //     mReflection.attributes.push_back(info);
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
    std::cout << "[ShaderGLES] Shader反射信息构建完成 - Uniforms: " << mReflection.uniforms.size() 
              << ", Attributes: " << mReflection.attributes.size() << std::endl;
}

UniformType ShaderGLES::glTypeToUniformType(uint32_t glType) const {
    // TODO: 实际实现中转换OpenGL类型到UniformType
    // switch (glType) {
    //     case GL_FLOAT:      return UniformType::kUniformType_Float;
    //     case GL_FLOAT_VEC2: return UniformType::kUniformType_Float2;
    //     case GL_FLOAT_VEC3: return UniformType::kUniformType_Float3;
    //     case GL_FLOAT_VEC4: return UniformType::kUniformType_Float4;
    //     case GL_INT:        return UniformType::kUniformType_Int;
    //     case GL_INT_VEC2:   return UniformType::kUniformType_Int2;
    //     case GL_INT_VEC3:   return UniformType::kUniformType_Int3;
    //     case GL_INT_VEC4:   return UniformType::kUniformType_Int4;
    //     case GL_FLOAT_MAT3: return UniformType::kUniformType_Mat3;
    //     case GL_FLOAT_MAT4: return UniformType::kUniformType_Mat4;
    //     case GL_SAMPLER_2D: return UniformType::kUniformType_Sampler2D;
    //     case GL_SAMPLER_CUBE: return UniformType::kUniformType_SamplerCube;
    //     default: return UniformType::kUniformType_Unknown;
    // }
    
    // 模拟实现
    switch (glType) {
        case 0x1406: return UniformType::kFloat;      // GL_FLOAT
        case 0x8B50: return UniformType::kVec2;     // GL_FLOAT_VEC2
        case 0x8B51: return UniformType::kVec3;     // GL_FLOAT_VEC3
        case 0x8B52: return UniformType::kVec4;     // GL_FLOAT_VEC4
        case 0x1404: return UniformType::kInt;        // GL_INT
        case 0x8B53: return UniformType::kIVec2;       // GL_INT_VEC2
        case 0x8B54: return UniformType::kIVec3;       // GL_INT_VEC3
        case 0x8B55: return UniformType::kIVec4;       // GL_INT_VEC4
        case 0x8B5B: return UniformType::kMat3;       // GL_FLOAT_MAT3
        case 0x8B5C: return UniformType::kMat4;       // GL_FLOAT_MAT4
        case 0x8B5E: return UniformType::kSampler2D;  // GL_SAMPLER_2D
        case 0x8B60: return UniformType::kSamplerCube;// GL_SAMPLER_CUBE
        default: return UniformType::kFloat; // 默认值
    }
}

VertexFormat ShaderGLES::glTypeToVertexFormat(uint32_t glType) const {
    // TODO: 实际实现中转换OpenGL类型到VertexFormat
    // switch (glType) {
    //     case GL_FLOAT:      return VertexFormat::kVertexFormat_Float;
    //     case GL_FLOAT_VEC2: return VertexFormat::kVertexFormat_Float2;
    //     case GL_FLOAT_VEC3: return VertexFormat::kVertexFormat_Float3;
    //     case GL_FLOAT_VEC4: return VertexFormat::kVertexFormat_Float4;
    //     case GL_INT:        return VertexFormat::kVertexFormat_Int;
    //     case GL_INT_VEC2:   return VertexFormat::kVertexFormat_Int2;
    //     case GL_INT_VEC3:   return VertexFormat::kVertexFormat_Int3;
    //     case GL_INT_VEC4:   return VertexFormat::kVertexFormat_Int4;
    //     default: return VertexFormat::kVertexFormat_Unknown;
    // }
    
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

void ShaderGLES::setUniformFloat(const std::string& name, float value) {
    std::cout << "[ShaderGLES] setUniformFloat: " << name << " = " << value << std::endl;
    // TODO: 实际实现中这里会调用glUniform1f
}

void ShaderGLES::setUniformInt(const std::string& name, int value) {
    std::cout << "[ShaderGLES] setUniformInt: " << name << " = " << value << std::endl;
    // TODO: 实际实现中这里会调用glUniform1i
}

void ShaderGLES::setUniformVec2(const std::string& name, const float* value) {
    std::cout << "[ShaderGLES] setUniformVec2: " << name << " = (" << value[0] << "," << value[1] << ")" << std::endl;
    // TODO: 实际实现中这里会调用glUniform2fv
}

void ShaderGLES::setUniformVec3(const std::string& name, const float* value) {
    std::cout << "[ShaderGLES] setUniformVec3: " << name << " = (" << value[0] << "," << value[1] << "," << value[2] << ")" << std::endl;
    // TODO: 实际实现中这里会调用glUniform3fv
}

void ShaderGLES::setUniformVec4(const std::string& name, const float* value) {
    std::cout << "[ShaderGLES] setUniformVec4: " << name << " = (" << value[0] << "," << value[1] << "," << value[2] << "," << value[3] << ")" << std::endl;
    // TODO: 实际实现中这里会调用glUniform4fv
}

void ShaderGLES::setUniformMat3(const std::string& name, const float* value) {
    std::cout << "[ShaderGLES] setUniformMat3: " << name << std::endl;
    // TODO: 实际实现中这里会调用glUniformMatrix3fv
}

void ShaderGLES::setUniformMat4(const std::string& name, const float* value) {
    std::cout << "[ShaderGLES] setUniformMat4: " << name << std::endl;
    // TODO: 实际实现中这里会调用glUniformMatrix4fv
}

ResourceHandle ShaderGLES::getResourceHandle() const {
    ResourceHandle handle;
    handle.uintHandle = mProgramID;
    return handle;
}

} // namespace render
} // namespace hyengine
