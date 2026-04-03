#include "ShaderMtl.hpp"
#import <Metal/Metal.h>
#include <iostream>
#include <unordered_map>
#include <fstream>
#include <sstream>

namespace hyengine {
namespace render {

ShaderMtl::ShaderMtl(const ShaderDesc& desc, void* device) 
    : mDesc(desc), mDevice(device) {
    std::cout << "[ShaderMtl] 创建Metal着色器: " << desc.name << std::endl;
    mLibrary = reinterpret_cast<void*>(0x200);
}

ShaderMtl::~ShaderMtl() {
    if (mVertexFunction) {
        CFRelease(mVertexFunction);
        mVertexFunction = nullptr;
    }
    if (mFragmentFunction) {
        CFRelease(mFragmentFunction);
        mFragmentFunction = nullptr;
    }
    if (mLibrary) {
        CFRelease(mLibrary);
        mLibrary = nullptr;
    }
    std::cout << "[ShaderMtl] 销毁Metal着色器" << std::endl;
}

bool ShaderMtl::compile() {
    id<MTLDevice> device = (__bridge id<MTLDevice>)mDevice;
    if (!device) {
        std::cerr << "[ShaderMtl] 错误: Metal设备无效" << std::endl;
        return false;
    }
    
    NSError* error = nil;
    id<MTLLibrary> library = nil;
    std::string vertexEntryPoint;
    std::string fragmentEntryPoint;
    
    // 检查是否有着色器代码
    if (mDesc.shaderCodeDesc.empty()) {
        std::cerr << "[ShaderMtl] 错误: 没有提供着色器代码" << std::endl;
        return false;
    }
    
    // 假设所有阶段的代码在同一个源文件中（Metal常见做法）
    // 获取第一个着色器代码作为源
    const auto& firstShader = mDesc.shaderCodeDesc[0];
    
    std::cout << "[ShaderMtl] 从源代码编译Metal着色器" << std::endl;
    
    NSString* source = [NSString stringWithUTF8String:firstShader.shaderCode];
    library = [device newLibraryWithSource:source options:nil error:&error];
    
    if (!library) {
        std::cerr << "[ShaderMtl] 编译着色器源代码失败: " 
                  << [[error localizedDescription] UTF8String] << std::endl;
        return false;
    }
    
    // 从 shaderCodeDesc 中提取入口点函数名
    id<MTLFunction> vertexFunc = nil;
    id<MTLFunction> fragmentFunc = nil;
    
    for (const auto& shaderCode : mDesc.shaderCodeDesc) {
        if (shaderCode.shaderStage == ShaderStage::kShaderStage_Vertex) {
            vertexEntryPoint = shaderCode.entryFunction;
            NSString* funcName = [NSString stringWithUTF8String:vertexEntryPoint.c_str()];
            vertexFunc = [library newFunctionWithName:funcName];
            if (!vertexFunc) {
                std::cerr << "[ShaderMtl] 找不到顶点函数: " << vertexEntryPoint << std::endl;
                return false;
            }
        } else if (shaderCode.shaderStage == ShaderStage::kShaderStage_Fragment) {
            fragmentEntryPoint = shaderCode.entryFunction;
            NSString* funcName = [NSString stringWithUTF8String:fragmentEntryPoint.c_str()];
            fragmentFunc = [library newFunctionWithName:funcName];
            if (!fragmentFunc) {
                std::cerr << "[ShaderMtl] 找不到片段函数: " << fragmentEntryPoint << std::endl;
                return false;
            }
        }
    }
    
    if (!vertexFunc || !fragmentFunc) {
        std::cerr << "[ShaderMtl] 缺少顶点或片段着色器" << std::endl;
        return false;
    }
    
    // 释放旧的资源
    if (mLibrary) CFRelease(mLibrary);
    if (mVertexFunction) CFRelease(mVertexFunction);
    if (mFragmentFunction) CFRelease(mFragmentFunction);
    
    // 保存到成员变量
    mLibrary = (__bridge_retained void*)library;
    mVertexFunction = (__bridge_retained void*)vertexFunc;
    mFragmentFunction = (__bridge_retained void*)fragmentFunc;
    
    std::cout << "[ShaderMtl] 着色器编译成功" << std::endl;
    return true;
}

void ShaderMtl::bind() {
    std::cout << "[ShaderMtl] 绑定着色器" << std::endl;
}

void ShaderMtl::unbind() {}

void ShaderMtl::setUniform(const std::string& name, const void* data, uint32_t size) {
    std::cout << "[ShaderMtl] setUniform: " << name << std::endl;
}

void ShaderMtl::doInit() {}

// ============================================================================
// 5.1.3 新增的Shader反射接口实现
// ============================================================================

int32_t ShaderMtl::getUniformLocation(const std::string& name) const {
    std::cout << "[ShaderMtl] getUniformLocation: " << name << std::endl;
    // TODO: Metal中使用binding index而不是location
    // 这里模拟返回一些已知值
    static std::unordered_map<std::string, int32_t> mockIndices = {
        {"u_mvpMatrix", 0}, {"u_modelMatrix", 1}, {"u_viewMatrix", 2}, 
        {"u_projMatrix", 3}, {"u_color", 4}, {"u_time", 5}
    };
    
    auto it = mockIndices.find(name);
    return (it != mockIndices.end()) ? it->second : -1;
}

int32_t ShaderMtl::getAttributeLocation(const std::string& name) const {
    std::cout << "[ShaderMtl] getAttributeLocation: " << name << std::endl;
    // TODO: Metal中使用attribute index
    static std::unordered_map<std::string, int32_t> mockIndices = {
        {"a_position", 0}, {"a_normal", 1}, {"a_texCoord", 2}, {"a_color", 3}
    };
    
    auto it = mockIndices.find(name);
    return (it != mockIndices.end()) ? it->second : -1;
}

int32_t ShaderMtl::getUniformBlockIndex(const std::string& name) const {
    std::cout << "[ShaderMtl] getUniformBlockIndex: " << name << std::endl;
    // TODO: Metal中使用buffer binding
    static std::unordered_map<std::string, int32_t> mockIndices = {
        {"CameraUBO", 0}, {"MaterialUBO", 1}, {"LightUBO", 2}
    };
    
    auto it = mockIndices.find(name);
    return (it != mockIndices.end()) ? it->second : -1;
}

UniformInfo ShaderMtl::getUniformInfo(const std::string& name) const {
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

AttributeInfo ShaderMtl::getAttributeInfo(const std::string& name) const {
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

std::vector<UniformInfo> ShaderMtl::getAllUniforms() const {
    if (!mReflectionBuilt) {
        buildReflection();
    }
    return mReflection.uniforms;
}

std::vector<AttributeInfo> ShaderMtl::getAllAttributes() const {
    if (!mReflectionBuilt) {
        buildReflection();
    }
    return mReflection.attributes;
}

const ShaderReflection& ShaderMtl::getReflection() const {
    if (!mReflectionBuilt) {
        buildReflection();
    }
    return mReflection;
}

void ShaderMtl::buildReflection() const {
    std::cout << "[ShaderMtl] 构建Metal Shader反射信息..." << std::endl;
    
    mReflection.uniforms.clear();
    mReflection.attributes.clear();
    mReflection.uniformBlocks.clear();
    
    // TODO: 实际实现中使用Metal的MTLFunction反射获取信息
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
    std::cout << "[ShaderMtl] Metal Shader反射信息构建完成 - Uniforms: " << mReflection.uniforms.size() 
              << ", Attributes: " << mReflection.attributes.size() << std::endl;
}

// ============================================================================
// 5.1.3 优化的Uniform设置接口（使用std::string参数）
// ============================================================================

void ShaderMtl::setUniformFloat(const std::string& name, float value) {
    std::cout << "[ShaderMtl] setUniformFloat: " << name << " = " << value << std::endl;
}

void ShaderMtl::setUniformInt(const std::string& name, int value) {
    std::cout << "[ShaderMtl] setUniformInt: " << name << " = " << value << std::endl;
}

void ShaderMtl::setUniformVec2(const std::string& name, const float* value) {
    std::cout << "[ShaderMtl] setUniformVec2: " << name << " = (" << value[0] << "," << value[1] << ")" << std::endl;
}

void ShaderMtl::setUniformVec3(const std::string& name, const float* value) {
    std::cout << "[ShaderMtl] setUniformVec3: " << name << " = (" << value[0] << "," << value[1] << "," << value[2] << ")" << std::endl;
}

void ShaderMtl::setUniformVec4(const std::string& name, const float* value) {
    std::cout << "[ShaderMtl] setUniformVec4: " << name << " = (" << value[0] << "," << value[1] << "," << value[2] << "," << value[3] << ")" << std::endl;
}

void ShaderMtl::setUniformMat3(const std::string& name, const float* value) {
    std::cout << "[ShaderMtl] setUniformMat3: " << name << std::endl;
}

void ShaderMtl::setUniformMat4(const std::string& name, const float* value) {
    std::cout << "[ShaderMtl] setUniformMat4: " << name << std::endl;
}

// ============================================================================
// 5.1.3 已弃用的资源绑定接口（使用std::string参数）
// ============================================================================

void ShaderMtl::bindTexture(uint32_t activeIndex, const std::string& name, 
                           std::shared_ptr<HYTexture> texture, uint32_t texIdx) {
    std::cout << "[ShaderMtl] bindTexture: " << name << " to index " << activeIndex << std::endl;
}

void ShaderMtl::bindUBO(uint32_t bindingPoint, const std::string& name, 
                       std::shared_ptr<HYBuffer> buffer) {
    std::cout << "[ShaderMtl] bindUBO: " << name << " to binding " << bindingPoint << std::endl;
}

ResourceHandle ShaderMtl::getVertexStageHandle() const {
    ResourceHandle handle;
    handle.ptrHandle = mVertexFunction;
    return handle;
}

ResourceHandle ShaderMtl::getFragmentStageHandle() const {
    ResourceHandle handle;
    handle.ptrHandle = mFragmentFunction;
    return handle;
}

void* ShaderMtl::getMetalLibrary() const {
    return mLibrary;
}

ResourceHandle ShaderMtl::getResourceHandle() const {
    ResourceHandle handle;
    handle.ptrHandle = mLibrary;
    return handle;
}

} // namespace render
} // namespace hyengine
