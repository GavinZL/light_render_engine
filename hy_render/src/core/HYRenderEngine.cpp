#include "hy_render/HYRenderEngine.hpp"
#include "hy_render/HYRenderContext.hpp"

#ifdef HY_RENDER_OPENGL
#include "../opengl/RenderContextGL.hpp"
#endif

#ifdef HY_RENDER_OPENGLES
#include "../opengles/RenderContextGLES.hpp"
#endif

#ifdef HY_RENDER_METAL
#include "../metal/RenderContextMtl.hpp"
#endif

#ifdef HY_RENDER_VULKAN
#include "../vulkan/RenderContextVk.hpp"
#endif

namespace hyengine {
namespace render {

HYRenderEngine& HYRenderEngine::instance() {
    static HYRenderEngine instance;
    return instance;
}

void HYRenderEngine::setRenderAPI(RenderAPI api) {
    if (mCurrentAPI != api) {
        mCurrentAPI = api;
        mRenderContext.reset();
        createContext();
    }
}

RenderAPI HYRenderEngine::getRenderAPI() const {
    return mCurrentAPI;
}

std::shared_ptr<HYRenderContext> HYRenderEngine::getRenderContext() {
    if (!mRenderContext) {
        createContext();
    }
    return mRenderContext;
}

bool HYRenderEngine::isAPIAvailable(RenderAPI api) const {
    switch (api) {
#ifdef HY_RENDER_OPENGL
        case RenderAPI::kOpenGL:
            return true;
#endif
#ifdef HY_RENDER_OPENGLES
        case RenderAPI::kOpenGLES:
            return true;
#endif
#ifdef HY_RENDER_METAL
        case RenderAPI::kMetal:
            return true;
#endif
#ifdef HY_RENDER_VULKAN
        case RenderAPI::kVulkan:
            return true;
#endif
        default:
            return false;
    }
}

RenderAPI HYRenderEngine::selectBestAPI() {
#ifdef __APPLE__
    #ifdef HY_RENDER_METAL
        return RenderAPI::kMetal;
    #endif
    #ifdef HY_RENDER_OPENGL
        return RenderAPI::kOpenGL;
    #endif
#elif defined(__ANDROID__) || defined(__linux__)
    #ifdef HY_RENDER_VULKAN
        return RenderAPI::kVulkan;
    #endif
    #ifdef HY_RENDER_OPENGLES
        return RenderAPI::kOpenGLES;
    #endif
    #ifdef HY_RENDER_OPENGL
        return RenderAPI::kOpenGL;
    #endif
#elif defined(_WIN32)
    #ifdef HY_RENDER_VULKAN
        return RenderAPI::kVulkan;
    #endif
    #ifdef HY_RENDER_OPENGL
        return RenderAPI::kOpenGL;
    #endif
#endif
    return RenderAPI::kUnknown;
}

void HYRenderEngine::createContext() {
    switch (mCurrentAPI) {
#ifdef HY_RENDER_OPENGL
        case RenderAPI::kOpenGL:
            mRenderContext = std::make_shared<RenderContextGL>();
            break;
#endif
#ifdef HY_RENDER_OPENGLES
        case RenderAPI::kOpenGLES:
            mRenderContext = std::make_shared<RenderContextGLES>();
            break;
#endif
#ifdef HY_RENDER_METAL
        case RenderAPI::kMetal:
            mRenderContext = std::make_shared<RenderContextMtl>();
            break;
#endif
#ifdef HY_RENDER_VULKAN
        case RenderAPI::kVulkan:
            mRenderContext = std::make_shared<RenderContextVk>();
            break;
#endif
        default:
            break;
    }
}

} // namespace render
} // namespace hyengine
