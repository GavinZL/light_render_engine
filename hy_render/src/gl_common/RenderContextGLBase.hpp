#pragma once

#include "hy_render/HYRenderContext.hpp"
#include <string>

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

protected:
    // GL版本信息
    int mMajorVersion = 0;
    int mMinorVersion = 0;

    // 扩展支持标志
    bool mHasVAO = false;
    bool mHasSamplerObjects = false;
    bool mHasDSA = false;
    bool mHasDebugOutput = false;
};

} // namespace render
} // namespace hyengine
