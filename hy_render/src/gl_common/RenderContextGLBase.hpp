#pragma once

#include "../../include/hy_render/HYRenderContext.hpp"
#include <string>
#include <vector>

namespace hyengine {
namespace render {

/**
 * @brief OpenGL/OpenGL ES共享基类
 */
class RenderContextGLBase : public HYRenderContext {
public:
    virtual ~RenderContextGLBase() = default;

protected:
    RenderContextGLBase() = default;

    /**
     * @brief 初始化GL函数
     */
    void initializeGLFunctions();

    /**
     * @brief 检查GL错误
     */
    void checkGLError(const char* operation);

    /**
     * @brief 检查扩展支持
     */
    bool hasExtension(const std::string& extName) const;

    /**
     * @brief 检测扩展
     */
    void detectExtensions();
    
    /**
     * @brief 获取GL版本字符串
     */
    std::string getVersionString() const;
    
    /**
     * @brief 获取渲染器信息
     */
    std::string getRendererString() const;
    
    /**
     * @brief 获取供应商信息
     */
    std::string getVendorString() const;
    
    /**
     * @brief 获取扩展列表
     */
    std::vector<std::string> getSupportedExtensions() const;
    
    /**
     * @brief 初始化调试上下文
     */
    void initializeDebugContext();
    
    /**
     * @brief 设置调试消息回调
     */
    void setupDebugCallback();
    
    /**
     * @brief 验证上下文是否有效
     */
    bool validateContext() const;
    
    /**
     * @brief 获取最大纹理尺寸
     */
    uint32_t getMaxTextureSize() const;
    
    /**
     * @brief 获取最大立方体纹理尺寸
     */
    uint32_t getMaxCubeMapTextureSize() const;
    
    /**
     * @brief 获取最大颜色附件数量
     */
    uint32_t getMaxColorAttachments() const;
    
    /**
     * @brief 获取最大顶点属性数量
     */
    uint32_t getMaxVertexAttributes() const;

protected:
    // GL版本信息
    int mMajorVersion = 0;
    int mMinorVersion = 0;

    // 扩展支持标志
    bool mHasVAO = false;
    bool mHasSamplerObjects = false;
    bool mHasDSA = false;
    bool mHasDebugOutput = false;
    
    // GL限制值（缓存以避免重复查询）
    mutable uint32_t mMaxTextureSize = 0;
    mutable uint32_t mMaxCubeMapTextureSize = 0;
    mutable uint32_t mMaxColorAttachments = 0;
    mutable uint32_t mMaxVertexAttributes = 0;
    mutable bool mLimitsQueried = false;
};

} // namespace render
} // namespace hyengine
