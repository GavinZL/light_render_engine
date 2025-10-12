#pragma once

#include <string>

namespace hyengine {
namespace render {

/**
 * @brief GL工具函数
 */
class GLUtils {
public:
    /**
     * @brief 将着色器类型转换为GL枚举
     */
    static unsigned int shaderStageToGLType(int stage);

    /**
     * @brief 将纹理格式转换为GL格式
     */
    static void textureFormatToGL(int format, unsigned int& internalFormat, 
                                 unsigned int& format, unsigned int& type);

    /**
     * @brief 将缓冲区绑定标志转换为GL目标
     */
    static unsigned int bufferBindFlagToGLTarget(int bindFlags);

    /**
     * @brief 将缓冲区使用模式转换为GL枚举
     */
    static unsigned int bufferUsageToGL(int usage);

    /**
     * @brief 编译着色器并检查错误
     */
    static bool compileShader(unsigned int shader, const char* source, std::string& errorLog);

    /**
     * @brief 链接着色器程序并检查错误
     */
    static bool linkProgram(unsigned int program, std::string& errorLog);
};

} // namespace render
} // namespace hyengine
