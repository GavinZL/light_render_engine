#pragma once

#include <cstdint>
#include <memory>
#include <string>

namespace hyengine {
namespace render {

// 前置声明
class HYRenderContext;

/**
 * @brief 渲染API类型枚举
 */
enum class RenderAPI : uint8_t {
    kOpenGL,      // OpenGL 3.3+ Core Profile
    kOpenGLES,    // OpenGL ES 3.0+
    kMetal,       // Metal 2.0+
    kVulkan,      // Vulkan 1.0+
    kUnknown
};

/**
 * @brief 渲染引擎单例
 * 
 * 全局渲染引擎入口，管理渲染API类型和RenderContext的创建
 */
class HYRenderEngine {
public:
    /**
     * @brief 获取单例实例
     */
    static HYRenderEngine& instance();

    /**
     * @brief 设置渲染API类型
     * @param api 渲染API类型
     */
    void setRenderAPI(RenderAPI api);

    /**
     * @brief 获取当前渲染API类型
     * @return 当前使用的渲染API
     */
    RenderAPI getRenderAPI() const;

    /**
     * @brief 获取渲染上下文
     * @return 渲染上下文智能指针
     */
    std::shared_ptr<HYRenderContext> getRenderContext();

    /**
     * @brief 检测平台支持的API
     * @param api 要检测的API
     * @return 是否支持
     */
    bool isAPIAvailable(RenderAPI api) const;

    /**
     * @brief 自动选择最优API
     * @return 选择的API类型
     */
    RenderAPI selectBestAPI();

private:
    HYRenderEngine() = default;
    ~HYRenderEngine() = default;
    HYRenderEngine(const HYRenderEngine&) = delete;
    HYRenderEngine& operator=(const HYRenderEngine&) = delete;

    void createContext();

private:
    RenderAPI mCurrentAPI = RenderAPI::kUnknown;
    std::shared_ptr<HYRenderContext> mRenderContext;
};

} // namespace render
} // namespace hyengine
