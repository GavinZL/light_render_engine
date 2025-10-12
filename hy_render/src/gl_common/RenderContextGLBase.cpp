#include "RenderContextGLBase.hpp"
#include <iostream>

namespace hyengine {
namespace render {

void RenderContextGLBase::initializeGLFunctions() {
    // TODO: 使用GLAD或类似库加载GL函数指针
    // 这里需要根据实际使用的GL加载库来实现
}

void RenderContextGLBase::checkGLError(const char* operation) {
    // TODO: 调用glGetError检查错误
    // GLenum err = glGetError();
    // if (err != GL_NO_ERROR) {
    //     std::cerr << "GL Error after " << operation << ": " << err << std::endl;
    // }
}

bool RenderContextGLBase::hasExtension(const std::string& extName) const {
    // TODO: 使用glGetString(GL_EXTENSIONS)或glGetStringi检查扩展
    return false;
}

void RenderContextGLBase::detectExtensions() {
    // TODO: 检测各种扩展支持
    // mHasVAO = hasExtension("GL_ARB_vertex_array_object");
    // mHasSamplerObjects = hasExtension("GL_ARB_sampler_objects");
    // mHasDSA = hasExtension("GL_ARB_direct_state_access");
}

} // namespace render
} // namespace hyengine
