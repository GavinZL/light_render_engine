#include "GLUtils.hpp"

namespace hyengine {
namespace render {

unsigned int GLUtils::shaderStageToGLType(int stage) {
    // TODO: 实现转换逻辑
    // 例如: kShaderStage_Vertex -> GL_VERTEX_SHADER
    return 0;
}

void GLUtils::textureFormatToGL(int format, unsigned int& internalFormat, 
                               unsigned int& glFormat, unsigned int& type) {
    // TODO: 实现纹理格式转换
    // 例如: TEX_FORMAT_RGBA8_UNORM -> GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE
}

unsigned int GLUtils::bufferBindFlagToGLTarget(int bindFlags) {
    // TODO: 实现缓冲区目标转换
    // 例如: BIND_VERTEX_BUFFER -> GL_ARRAY_BUFFER
    return 0;
}

unsigned int GLUtils::bufferUsageToGL(int usage) {
    // TODO: 实现使用模式转换
    // 例如: USAGE_STATIC_DRAW -> GL_STATIC_DRAW
    return 0;
}

bool GLUtils::compileShader(unsigned int shader, const char* source, std::string& errorLog) {
    // TODO: 实现着色器编译
    // glShaderSource(shader, 1, &source, nullptr);
    // glCompileShader(shader);
    // 检查编译状态并获取错误日志
    return true;
}

bool GLUtils::linkProgram(unsigned int program, std::string& errorLog) {
    // TODO: 实现程序链接
    // glLinkProgram(program);
    // 检查链接状态并获取错误日志
    return true;
}

} // namespace render
} // namespace hyengine
