#include "GLUtils.hpp"
#include <iostream>
#include <vector>
#include <algorithm>

// OpenGL常量定义（假设使用GLAD或类似的加载器）
#ifndef GL_VERTEX_SHADER
#define GL_VERTEX_SHADER 0x8B31
#define GL_FRAGMENT_SHADER 0x8B30
#define GL_ARRAY_BUFFER 0x8892
#define GL_ELEMENT_ARRAY_BUFFER 0x8893
#define GL_UNIFORM_BUFFER 0x8A11
#define GL_STATIC_DRAW 0x88E4
#define GL_DYNAMIC_DRAW 0x88E8
#define GL_STREAM_DRAW 0x88E0
#define GL_TEXTURE_2D 0x0DE1
#define GL_TEXTURE_CUBE_MAP 0x8513
#define GL_RGBA8 0x8058
#define GL_RGBA 0x1908
#define GL_UNSIGNED_BYTE 0x1401
#define GL_FLOAT 0x1406
#define GL_NEAREST 0x2600
#define GL_LINEAR 0x2601
#define GL_LINEAR_MIPMAP_LINEAR 0x2703
#define GL_REPEAT 0x2901
#define GL_CLAMP_TO_EDGE 0x812F
#define GL_MIRRORED_REPEAT 0x8370
#define GL_ZERO 0
#define GL_ONE 1
#define GL_SRC_ALPHA 0x0302
#define GL_ONE_MINUS_SRC_ALPHA 0x0303
#define GL_FUNC_ADD 0x8006
#define GL_FUNC_SUBTRACT 0x800A
#define GL_FUNC_REVERSE_SUBTRACT 0x800B
#define GL_FRONT 0x0404
#define GL_BACK 0x0405
#define GL_FRONT_AND_BACK 0x0408
#define GL_NEVER 0x0200
#define GL_LESS 0x0201
#define GL_EQUAL 0x0202
#define GL_LEQUAL 0x0203
#define GL_GREATER 0x0204
#define GL_NOTEQUAL 0x0205
#define GL_GEQUAL 0x0206
#define GL_ALWAYS 0x0207
#define GL_POINTS 0x0000
#define GL_LINES 0x0001
#define GL_TRIANGLES 0x0004
#define GL_TRIANGLE_STRIP 0x0005
#define GL_NO_ERROR 0
#define GL_COMPILE_STATUS 0x8B81
#define GL_LINK_STATUS 0x8B82
#define GL_INFO_LOG_LENGTH 0x8B84
#define GL_TEXTURE_3D 0x806F
#define GL_CULL_FACE 0x0B44
#define GL_SCISSOR_TEST 0x0C11
#define GL_DEPTH_TEST 0x0B71
#define GL_STENCIL_TEST 0x0B90
#define GL_BLEND 0x0BE2
#define GL_CCW 0x0901
#define GL_CW 0x0900
#define GL_TRUE 1
#define GL_FALSE 0
#define GL_COLOR_BUFFER_BIT 0x00004000
#define GL_DEPTH_BUFFER_BIT 0x00000100
#define GL_STENCIL_BUFFER_BIT 0x00000400
#endif

namespace hyengine {
namespace render {

bool GLUtils::checkGLError(const char* operation) {
    // TODO: 需要实际的glGetError函数
    // GLenum err = glGetError();
    // if (err != GL_NO_ERROR) {
    //     std::cerr << "GL Error after " << operation << ": 0x" << std::hex << err << std::dec << std::endl;
    //     return false;
    // }
    return true;
}

unsigned int GLUtils::shaderStageToGLType(ShaderStage stage) {
    switch (stage) {
        case ShaderStage::kShaderStage_Vertex:
            return GL_VERTEX_SHADER;
        case ShaderStage::kShaderStage_Fragment:
            return GL_FRAGMENT_SHADER;
        default:
            return 0;
    }
}

void GLUtils::textureFormatToGL(TextureFormat format, unsigned int& internalFormat, 
                               unsigned int& glFormat, unsigned int& type) {
    switch (format) {
        case TextureFormat::TEX_FORMAT_RGBA8_UNORM:
            internalFormat = GL_RGBA8;
            glFormat = GL_RGBA;
            type = GL_UNSIGNED_BYTE;
            break;
        // TODO: 添加更多格式支持
        default:
            internalFormat = GL_RGBA8;
            glFormat = GL_RGBA;
            type = GL_UNSIGNED_BYTE;
            break;
    }
}

unsigned int GLUtils::bufferBindFlagToGLTarget(uint8_t bindFlags) {
    if (bindFlags & BIND_VERTEX_BUFFER) {
        return GL_ARRAY_BUFFER;
    }
    if (bindFlags & BIND_INDEX_BUFFER) {
        return GL_ELEMENT_ARRAY_BUFFER;
    }
    if (bindFlags & BIND_UNIFORM_BUFFER) {
        return GL_UNIFORM_BUFFER;
    }
    return GL_ARRAY_BUFFER;
}

unsigned int GLUtils::bufferUsageToGL(BufferUsage usage) {
    switch (usage) {
        case BufferUsage::USAGE_STATIC_DRAW:
            return GL_STATIC_DRAW;
        case BufferUsage::USAGE_DYNAMIC_DRAW:
            return GL_DYNAMIC_DRAW;
        case BufferUsage::USAGE_STREAM_DRAW:
            return GL_STREAM_DRAW;
        default:
            return GL_STATIC_DRAW;
    }
}

bool GLUtils::compileShader(unsigned int shader, const char* source, std::string& errorLog) {
    // TODO: 实现着色器编译检查
    // 这里需要实际的OpenGL函数调用
    // GLint success = 0;
    // glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    // if (!success) {
    //     GLint logLength = 0;
    //     glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLength);
    //     if (logLength > 0) {
    //         std::vector<GLchar> log(logLength);
    //         glGetShaderInfoLog(shader, logLength, nullptr, log.data());
    //         errorLog = std::string(log.begin(), log.end());
    //     }
    //     return false;
    // }
    errorLog.clear();
    return true;
}

bool GLUtils::linkProgram(unsigned int program, std::string& errorLog) {
    // TODO: 实现程序链接检查
    // 这里需要实际的OpenGL函数调用
    // GLint success = 0;
    // glGetProgramiv(program, GL_LINK_STATUS, &success);
    // if (!success) {
    //     GLint logLength = 0;
    //     glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logLength);
    //     if (logLength > 0) {
    //         std::vector<GLchar> log(logLength);
    //         glGetProgramInfoLog(program, logLength, nullptr, log.data());
    //         errorLog = std::string(log.begin(), log.end());
    //     }
    //     return false;
    // }
    errorLog.clear();
    return true;
}

void GLUtils::vertexFormatToGL(VertexFormat format, 
                              unsigned int& glType, 
                              int& size, 
                              bool& normalized) {
    normalized = false;
    
    switch (format) {
        case VertexFormat::kFloat:
            glType = GL_FLOAT;
            size = 1;
            break;
        case VertexFormat::kFloat2:
            glType = GL_FLOAT;
            size = 2;
            break;
        case VertexFormat::kFloat3:
            glType = GL_FLOAT;
            size = 3;
            break;
        case VertexFormat::kFloat4:
            glType = GL_FLOAT;
            size = 4;
            break;
        // TODO: 添加更多格式
        default:
            glType = GL_FLOAT;
            size = 1;
            break;
    }
}

unsigned int GLUtils::filterTypeToGLFilter(FilterType filter) {
    switch (filter) {
        case FilterType::FILTER_TYPE_NEAREST:
            return GL_NEAREST;
        case FilterType::FILTER_TYPE_LINEAR:
            return GL_LINEAR;
        case FilterType::FILTER_TYPE_LINEAR_MIPMAP_LINEAR:
            return GL_LINEAR_MIPMAP_LINEAR;
        default:
            return GL_LINEAR;
    }
}

unsigned int GLUtils::addressModeToGLWrap(TextureAddressMode mode) {
    switch (mode) {
        case TextureAddressMode::TEXTURE_ADDRESS_REPEAT:
            return GL_REPEAT;
        case TextureAddressMode::TEXTURE_ADDRESS_CLAMP_TO_EDGE:
            return GL_CLAMP_TO_EDGE;
        case TextureAddressMode::TEXTURE_ADDRESS_MIRROR_REPEAT:
            return GL_MIRRORED_REPEAT;
        default:
            return GL_REPEAT;
    }
}

unsigned int GLUtils::blendFactorToGL(BlendFactor factor) {
    switch (factor) {
        case BlendFactor::BLEND_FACTOR_ZERO:
            return GL_ZERO;
        case BlendFactor::BLEND_FACTOR_ONE:
            return GL_ONE;
        case BlendFactor::BLEND_FACTOR_SRC_ALPHA:
            return GL_SRC_ALPHA;
        case BlendFactor::BLEND_FACTOR_ONE_MINUS_SRC_ALPHA:
            return GL_ONE_MINUS_SRC_ALPHA;
        default:
            return GL_ONE;
    }
}

unsigned int GLUtils::blendOperationToGL(BlendOperation op) {
    switch (op) {
        case BlendOperation::BLEND_OP_ADD:
            return GL_FUNC_ADD;
        case BlendOperation::BLEND_OP_SUBTRACT:
            return GL_FUNC_SUBTRACT;
        case BlendOperation::BLEND_OP_REVERSE_SUBTRACT:
            return GL_FUNC_REVERSE_SUBTRACT;
        default:
            return GL_FUNC_ADD;
    }
}

unsigned int GLUtils::cullModeToGL(CullMode mode) {
    switch (mode) {
        case CullMode::CULL_MODE_FRONT:
            return GL_FRONT;
        case CullMode::CULL_MODE_BACK:
            return GL_BACK;
        case CullMode::CULL_MODE_NONE:
        default:
            return 0; // 不剔除
    }
}

unsigned int GLUtils::compareFunctionToGL(CompareFunction func) {
    switch (func) {
        case CompareFunction::COMPARE_FUNC_NEVER:
            return GL_NEVER;
        case CompareFunction::COMPARE_FUNC_LESS:
            return GL_LESS;
        case CompareFunction::COMPARE_FUNC_EQUAL:
            return GL_EQUAL;
        case CompareFunction::COMPARE_FUNC_LESS_EQUAL:
            return GL_LEQUAL;
        case CompareFunction::COMPARE_FUNC_GREATER:
            return GL_GREATER;
        case CompareFunction::COMPARE_FUNC_NOT_EQUAL:
            return GL_NOTEQUAL;
        case CompareFunction::COMPARE_FUNC_GREATER_EQUAL:
            return GL_GEQUAL;
        case CompareFunction::COMPARE_FUNC_ALWAYS:
            return GL_ALWAYS;
        default:
            return GL_LESS;
    }
}

unsigned int GLUtils::primitiveTypeToGL(PrimitiveType type) {
    switch (type) {
        case PrimitiveType::PRIMITIVE_POINTS:
            return GL_POINTS;
        case PrimitiveType::PRIMITIVE_LINES:
            return GL_LINES;
        case PrimitiveType::PRIMITIVE_TRIANGLES:
            return GL_TRIANGLES;
        case PrimitiveType::PRIMITIVE_TRIANGLE_STRIP:
            return GL_TRIANGLE_STRIP;
        default:
            return GL_TRIANGLES;
    }
}

// 纹理相关工具函数实现
unsigned int GLUtils::textureTypeToGL(TextureDimension type) {
    switch (type) {
        case TextureDimension::TEXTURE_DIM_TEX_2D:
            return GL_TEXTURE_2D;
        case TextureDimension::TEXTURE_DIM_TEX_CUBE:
            return GL_TEXTURE_CUBE_MAP;
        case TextureDimension::TEXTURE_DIM_TEX_3D:
            // OpenGL ES可能不支持3D纹理，这里返回2D纹理
            return GL_TEXTURE_2D;
        default:
            return GL_TEXTURE_2D;
    }
}

unsigned int GLUtils::textureUsageToGL(uint8_t usage) {
    // 根据使用情况返回相应的GL绑定目标
    if (usage & TEXTURE_USAGE_RENDER_TARGET) {
        return GL_TEXTURE_2D; // 用于帧缓冲附件
    }
    return GL_TEXTURE_2D; // 默认用于着色器读取
}

void GLUtils::getTextureSize(TextureDimension type, uint32_t width, uint32_t height,
                            uint32_t& glWidth, uint32_t& glHeight) {
    switch (type) {
        case TextureDimension::TEXTURE_DIM_TEX_CUBE:
            // 立方体纹理要求宽高相等
            glWidth = glHeight = std::max(width, height);
            break;
        default:
            glWidth = width;
            glHeight = height;
            break;
    }
}

uint32_t GLUtils::calculateMipLevels(uint32_t width, uint32_t height) {
    uint32_t maxDimension = std::max(width, height);
    uint32_t mipLevels = 1;
    while (maxDimension > 1) {
        maxDimension >>= 1;
        mipLevels++;
    }
    return mipLevels;
}

size_t GLUtils::calculateTextureSize(TextureFormat format, uint32_t width, uint32_t height) {
    size_t bytesPerPixel = 0;
    
    switch (format) {
        case TextureFormat::TEX_FORMAT_RGBA8_UNORM:
        case TextureFormat::TEX_FORMAT_DEPTH24_STENCIL8:
            bytesPerPixel = 4;
            break;
        case TextureFormat::TEX_FORMAT_RGBA16_FLOAT:
            bytesPerPixel = 8;
            break;
        case TextureFormat::TEX_FORMAT_DEPTH32_FLOAT:
            bytesPerPixel = 4;
            break;
        case TextureFormat::TEX_FORMAT_R8_UNORM:
            bytesPerPixel = 1;
            break;
        case TextureFormat::TEX_FORMAT_RG8_UNORM:
            bytesPerPixel = 2;
            break;
        case TextureFormat::TEX_FORMAT_RGB8_UNORM:
            bytesPerPixel = 3;
            break;
        default:
            bytesPerPixel = 4;
            break;
    }
    
    return width * height * bytesPerPixel;
}

bool GLUtils::isDepthFormat(TextureFormat format) {
    switch (format) {
        case TextureFormat::TEX_FORMAT_DEPTH24_STENCIL8:
        case TextureFormat::TEX_FORMAT_DEPTH32_FLOAT:
            return true;
        default:
            return false;
    }
}

bool GLUtils::isCompressedFormat(TextureFormat format) {
    // 当前定义的格式都不是压缩格式
    return false;
}

// OpenGL状态管理函数实现
void GLUtils::enableBlend(bool enable) {
    // TODO: 需要实际的OpenGL函数调用
    // if (enable) {
    //     glEnable(GL_BLEND);
    // } else {
    //     glDisable(GL_BLEND);
    // }
}

void GLUtils::enableDepthTest(bool enable) {
    // TODO: 需要实际的OpenGL函数调用
    // if (enable) {
    //     glEnable(GL_DEPTH_TEST);
    // } else {
    //     glDisable(GL_DEPTH_TEST);
    // }
}

void GLUtils::enableStencilTest(bool enable) {
    // TODO: 需要实际的OpenGL函数调用
    // if (enable) {
    //     glEnable(GL_STENCIL_TEST);
    // } else {
    //     glDisable(GL_STENCIL_TEST);
    // }
}

void GLUtils::setViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) {
    // TODO: 需要实际的OpenGL函数调用
    // glViewport(x, y, width, height);
}

void GLUtils::setScissor(uint32_t x, uint32_t y, uint32_t width, uint32_t height) {
    // TODO: 需要实际的OpenGL函数调用
    // glScissor(x, y, width, height);
}

void GLUtils::enableScissorTest(bool enable) {
    // TODO: 需要实际的OpenGL函数调用
    // if (enable) {
    //     glEnable(GL_SCISSOR_TEST);
    // } else {
    //     glDisable(GL_SCISSOR_TEST);
    // }
}

void GLUtils::setDepthMask(bool enable) {
    // TODO: 需要实际的OpenGL函数调用
    // glDepthMask(enable ? GL_TRUE : GL_FALSE);
}

void GLUtils::setColorMask(bool r, bool g, bool b, bool a) {
    // TODO: 需要实际的OpenGL函数调用
    // glColorMask(r ? GL_TRUE : GL_FALSE, g ? GL_TRUE : GL_FALSE, 
    //            b ? GL_TRUE : GL_FALSE, a ? GL_TRUE : GL_FALSE);
}

void GLUtils::setBlendState(const BlendDesc& desc) {
    if (desc.blendEnable) {
        enableBlend(true);
        // TODO: 需要实际的OpenGL函数调用
        // glBlendFuncSeparate(blendFactorToGL(desc.srcBlend), 
        //                    blendFactorToGL(desc.dstBlend),
        //                    blendFactorToGL(desc.srcBlendAlpha), 
        //                    blendFactorToGL(desc.dstBlendAlpha));
        // glBlendEquationSeparate(blendOperationToGL(desc.blendOp), 
        //                        blendOperationToGL(desc.blendOpAlpha));
    } else {
        enableBlend(false);
    }
}

void GLUtils::setDepthStencilState(const DepthStencilDesc& desc) {
    enableDepthTest(desc.depthTestEnable);
    setDepthMask(desc.depthWriteEnable);
    
    if (desc.depthTestEnable) {
        // TODO: 需要实际的OpenGL函数调用
        // glDepthFunc(compareFunctionToGL(desc.depthFunc));
    }
    
    if (desc.stencilTestEnable) {
        enableStencilTest(true);
        // TODO: 设置模板测试参数
        // glStencilMask(desc.stencilMask);
    } else {
        enableStencilTest(false);
    }
}

void GLUtils::setRasterizerState(const RasterizerDesc& desc) {
    unsigned int cullMode = cullModeToGL(desc.cullMode);
    if (cullMode != 0) {
        // TODO: 需要实际的OpenGL函数调用
        // glEnable(GL_CULL_FACE);
        // glCullFace(cullMode);
        // glFrontFace(desc.frontCCW ? GL_CCW : GL_CW);
    } else {
        // TODO: 需要实际的OpenGL函数调用
        // glDisable(GL_CULL_FACE);
    }
    
    enableScissorTest(desc.scissorEnable);
    if (desc.scissorEnable) {
        setScissor(desc.scissorRect[0], desc.scissorRect[1], 
                  desc.scissorRect[2], desc.scissorRect[3]);
    }
}

void GLUtils::setClearColor(float r, float g, float b, float a) {
    // TODO: 需要实际的OpenGL函数调用
    // glClearColor(r, g, b, a);
}

void GLUtils::clearBuffers(uint8_t clearMask) {
    // TODO: 需要实际的OpenGL函数调用
    // GLbitfield mask = 0;
    // if (clearMask & CLEAR_COLOR_BUFFER) {
    //     mask |= GL_COLOR_BUFFER_BIT;
    // }
    // if (clearMask & CLEAR_DEPTH_BUFFER) {
    //     mask |= GL_DEPTH_BUFFER_BIT;
    // }
    // if (clearMask & CLEAR_STENCIL_BUFFER) {
    //     mask |= GL_STENCIL_BUFFER_BIT;
    // }
    // if (mask != 0) {
    //     glClear(mask);
    // }
}

} // namespace render
} // namespace hyengine
