#pragma once

#include "../../include/hy_render/HYRenderDef.hpp"
#include <string>

namespace hyengine {
namespace render {

/**
 * @brief OpenGL工具函数类
 */
class GLUtils {
public:
    /**
     * @brief 检查GL错误
     * @param operation 操作名称
     * @return 是否有错误
     */
    static bool checkGLError(const char* operation);

    /**
     * @brief 将着色器阶段转换为GL类型
     */
    static unsigned int shaderStageToGLType(ShaderStage stage);

    /**
     * @brief 将纹理格式转换为GL格式
     */
    static void textureFormatToGL(TextureFormat format, unsigned int& internalFormat, 
                                 unsigned int& glFormat, unsigned int& type);

    /**
     * @brief 将缓冲区绑定标志转换为GL目标
     */
    static unsigned int bufferBindFlagToGLTarget(uint8_t bindFlags);

    /**
     * @brief 将缓冲区使用模式转换为GL使用模式
     */
    static unsigned int bufferUsageToGL(BufferUsage usage);

    /**
     * @brief 编译着色器并检查错误
     */
    static bool compileShader(unsigned int shader, const char* source, std::string& errorLog);

    /**
     * @brief 链接着色器程序并检查错误
     */
    static bool linkProgram(unsigned int program, std::string& errorLog);

    /**
     * @brief 将顶点格式转换为GL类型和大小
     */
    static void vertexFormatToGL(VertexFormat format, 
                                unsigned int& glType, 
                                int& size, 
                                bool& normalized);

    /**
     * @brief 将过滤类型转换为GL过滤
     */
    static unsigned int filterTypeToGLFilter(FilterType filter);

    /**
     * @brief 将寻址模式转换为GL寻址模式
     */
    static unsigned int addressModeToGLWrap(TextureAddressMode mode);

    /**
     * @brief 将混合因子转换为GL混合因子
     */
    static unsigned int blendFactorToGL(BlendFactor factor);

    /**
     * @brief 将混合操作转换为GL混合操作
     */
    static unsigned int blendOperationToGL(BlendOperation op);

    /**
     * @brief 将剔除模式转换为GL剔除模式
     */
    static unsigned int cullModeToGL(CullMode mode);

    /**
     * @brief 将比较函数转换为GL比较函数
     */
    static unsigned int compareFunctionToGL(CompareFunction func);

    /**
     * @brief 将图元类型转换为GL图元类型
     */
    static unsigned int primitiveTypeToGL(PrimitiveType type);

    // 纹理相关工具函数
    /**
     * @brief 将纹理维度转换为GL纹理类型
     */
    static unsigned int textureTypeToGL(TextureDimension type);
    
    /**
     * @brief 将纹理用途转换为GL绑定目标
     */
    static unsigned int textureUsageToGL(uint8_t usage);
    
    /**
     * @brief 获取纹理尺寸
     */
    static void getTextureSize(TextureDimension type, uint32_t width, uint32_t height, 
                              uint32_t& glWidth, uint32_t& glHeight);
    
    /**
     * @brief 计算Mip级别
     */
    static uint32_t calculateMipLevels(uint32_t width, uint32_t height);
    
    /**
     * @brief 计算纹理大小
     */
    static size_t calculateTextureSize(TextureFormat format, uint32_t width, uint32_t height);
    
    /**
     * @brief 检查是否为深度格式
     */
    static bool isDepthFormat(TextureFormat format);
    
    /**
     * @brief 检查是否为压缩格式
     */
    static bool isCompressedFormat(TextureFormat format);

    // OpenGL状态管理
    /**
     * @brief 启用或禁用混合
     */
    static void enableBlend(bool enable);
    
    /**
     * @brief 启用或禁用深度测试
     */
    static void enableDepthTest(bool enable);
    
    /**
     * @brief 启用或禁用模板测试
     */
    static void enableStencilTest(bool enable);
    
    /**
     * @brief 设置视口
     */
    static void setViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height);
    
    /**
     * @brief 设置剪裁区域
     */
    static void setScissor(uint32_t x, uint32_t y, uint32_t width, uint32_t height);
    
    /**
     * @brief 启用或禁用剪裁测试
     */
    static void enableScissorTest(bool enable);
    
    /**
     * @brief 设置深度掩码
     */
    static void setDepthMask(bool enable);
    
    /**
     * @brief 设置颜色掩码
     */
    static void setColorMask(bool r, bool g, bool b, bool a);
    
    /**
     * @brief 设置混合状态
     */
    static void setBlendState(const BlendDesc& desc);
    
    /**
     * @brief 设置深度模板状态
     */
    static void setDepthStencilState(const DepthStencilDesc& desc);
    
    /**
     * @brief 设置光栅化状态
     */
    static void setRasterizerState(const RasterizerDesc& desc);
    
    /**
     * @brief 设置清除颜色
     */
    static void setClearColor(float r, float g, float b, float a);
    
    /**
     * @brief 清除缓冲区
     */
    static void clearBuffers(uint8_t clearMask);
};

} // namespace render
} // namespace hyengine
