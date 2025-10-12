#include "RenderContextGLBase.hpp"
#include "GLUtils.hpp"
#include <iostream>
#include <string>
#include <vector>

namespace hyengine {
namespace render {

void RenderContextGLBase::initializeGLFunctions() {
    // TODO: 使用GLAD或类似的GL加载库初始化GL函数指针
    // 这里只做一个示例框架
    
    // gladLoadGL() 或类似调用
    
    std::cout << "[RenderContextGLBase] GL Functions initialized" << std::endl;
    
    // 获取GL版本信息
    // const char* version = (const char*)glGetString(GL_VERSION);
    // if (version) {
    //     std::cout << "OpenGL Version: " << version << std::endl;
    //     // 解析版本号
    //     sscanf(version, "%d.%d", &mMajorVersion, &mMinorVersion);
    // }
    
    // 传统设置（在实际项目中应从实际GL中获取）
    mMajorVersion = 3;
    mMinorVersion = 3;
}

void RenderContextGLBase::checkGLError(const char* operation) {
    GLUtils::checkGLError(operation);
}

bool RenderContextGLBase::hasExtension(const std::string& extName) const {
    // TODO: 实现扩展检查
    // 方法1: 使用glGetString(GL_EXTENSIONS) (老版本)
    // const char* extensions = (const char*)glGetString(GL_EXTENSIONS);
    // if (extensions) {
    //     std::string extStr(extensions);
    //     return extStr.find(extName) != std::string::npos;
    // }
    
    // 方法2: 使用glGetStringi (OpenGL 3.0+)
    // GLint numExtensions = 0;
    // glGetIntegerv(GL_NUM_EXTENSIONS, &numExtensions);
    // for (GLint i = 0; i < numExtensions; ++i) {
    //     const char* ext = (const char*)glGetStringi(GL_EXTENSIONS, i);
    //     if (ext && extName == ext) {
    //         return true;
    //     }
    // }
    
    // 临时返回false，实际中需要真实的检查
    return false;
}

void RenderContextGLBase::detectExtensions() {
    std::cout << "[RenderContextGLBase] Detecting GL extensions..." << std::endl;
    
    // 检测VAO支持
    if (mMajorVersion > 3 || (mMajorVersion == 3 && mMinorVersion >= 0)) {
        // OpenGL 3.0+ 或 OpenGL ES 3.0+ 原生支持VAO
        mHasVAO = true;
    } else {
        mHasVAO = hasExtension("GL_ARB_vertex_array_object") || 
                  hasExtension("GL_OES_vertex_array_object");
    }
    
    // 检测采样器对象支持
    if (mMajorVersion > 3 || (mMajorVersion == 3 && mMinorVersion >= 3)) {
        // OpenGL 3.3+ 支持分离式采样器
        mHasSamplerObjects = true;
    } else {
        mHasSamplerObjects = hasExtension("GL_ARB_sampler_objects");
    }
    
    // 检测DSA支持
    if (mMajorVersion > 4 || (mMajorVersion == 4 && mMinorVersion >= 5)) {
        // OpenGL 4.5+ 支持DSA
        mHasDSA = true;
    } else {
        mHasDSA = hasExtension("GL_ARB_direct_state_access");
    }
    
    // 检测调试输出支持
    if (mMajorVersion > 4 || (mMajorVersion == 4 && mMinorVersion >= 3)) {
        mHasDebugOutput = true;
    } else {
        mHasDebugOutput = hasExtension("GL_ARB_debug_output") ||
                         hasExtension("GL_KHR_debug");
    }
    
    // 输出检测结果
    std::cout << "  VAO Support: " << (mHasVAO ? "YES" : "NO") << std::endl;
    std::cout << "  Sampler Objects: " << (mHasSamplerObjects ? "YES" : "NO") << std::endl;
    std::cout << "  Direct State Access: " << (mHasDSA ? "YES" : "NO") << std::endl;
    std::cout << "  Debug Output: " << (mHasDebugOutput ? "YES" : "NO") << std::endl;
}

std::string RenderContextGLBase::getVersionString() const {
    // TODO: 实际实现中需要调用glGetString(GL_VERSION)
    // const char* version = (const char*)glGetString(GL_VERSION);
    // return version ? std::string(version) : std::string("Unknown");
    return "OpenGL " + std::to_string(mMajorVersion) + "." + std::to_string(mMinorVersion);
}

std::string RenderContextGLBase::getRendererString() const {
    // TODO: 实际实现中需要调用glGetString(GL_RENDERER)
    // const char* renderer = (const char*)glGetString(GL_RENDERER);
    // return renderer ? std::string(renderer) : std::string("Unknown");
    return "Unknown Renderer";
}

std::string RenderContextGLBase::getVendorString() const {
    // TODO: 实际实现中需要调用glGetString(GL_VENDOR)
    // const char* vendor = (const char*)glGetString(GL_VENDOR);
    // return vendor ? std::string(vendor) : std::string("Unknown");
    return "Unknown Vendor";
}

std::vector<std::string> RenderContextGLBase::getSupportedExtensions() const {
    std::vector<std::string> extensions;
    
    // TODO: 实际实现中需要获取真实的扩展列表
    // 方法1: 使用glGetString(GL_EXTENSIONS) (OpenGL 2.x)
    // const char* extensionStr = (const char*)glGetString(GL_EXTENSIONS);
    // if (extensionStr) {
    //     std::stringstream ss(extensionStr);
    //     std::string extension;
    //     while (ss >> extension) {
    //         extensions.push_back(extension);
    //     }
    // }
    
    // 方法2: 使用glGetStringi (OpenGL 3.0+)
    // GLint numExtensions = 0;
    // glGetIntegerv(GL_NUM_EXTENSIONS, &numExtensions);
    // for (GLint i = 0; i < numExtensions; ++i) {
    //     const char* ext = (const char*)glGetStringi(GL_EXTENSIONS, i);
    //     if (ext) {
    //         extensions.push_back(std::string(ext));
    //     }
    // }
    
    // 临时返回一些常见扩展作示例
    if (mHasVAO) {
        extensions.push_back("GL_ARB_vertex_array_object");
    }
    if (mHasSamplerObjects) {
        extensions.push_back("GL_ARB_sampler_objects");
    }
    if (mHasDSA) {
        extensions.push_back("GL_ARB_direct_state_access");
    }
    if (mHasDebugOutput) {
        extensions.push_back("GL_ARB_debug_output");
    }
    
    return extensions;
}

void RenderContextGLBase::initializeDebugContext() {
    if (!mHasDebugOutput) {
        std::cout << "[RenderContextGLBase] Debug output not supported" << std::endl;
        return;
    }
    
    std::cout << "[RenderContextGLBase] Initializing debug context" << std::endl;
    
    // TODO: 实际实现中需要调用GL调试函数
    // glEnable(GL_DEBUG_OUTPUT);
    // glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    
    setupDebugCallback();
}

void RenderContextGLBase::setupDebugCallback() {
    if (!mHasDebugOutput) {
        return;
    }
    
    // TODO: 实际实现中需要设置调试回调
    // glDebugMessageCallback(debugCallback, nullptr);
    // 
    // // 启用所有类型的调试消息
    // glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
    
    std::cout << "[RenderContextGLBase] Debug callback setup completed" << std::endl;
}

bool RenderContextGLBase::validateContext() const {
    // TODO: 实际实现中需要验证GL上下文
    // 检查基本的GL调用是否工作
    // GLenum error = glGetError();
    // if (error != GL_NO_ERROR) {
    //     std::cerr << "GL Error during context validation: 0x" << std::hex << error << std::endl;
    //     return false;
    // }
    
    // 检查版本是否符合要求
    if (mMajorVersion < 2) {
        std::cerr << "OpenGL version too old: " << mMajorVersion << "." << mMinorVersion << std::endl;
        return false;
    }
    
    std::cout << "[RenderContextGLBase] Context validation passed" << std::endl;
    return true;
}

uint32_t RenderContextGLBase::getMaxTextureSize() const {
    // TODO: 实际实现中需要查询GL_MAX_TEXTURE_SIZE
    // GLint maxSize = 0;
    // glGetIntegerv(GL_MAX_TEXTURE_SIZE, &maxSize);
    // return static_cast<uint32_t>(maxSize);
    
    // 返回一个合理的默认值
    return 8192;
}

uint32_t RenderContextGLBase::getMaxCubeMapTextureSize() const {
    // TODO: 实际实现中需要查询GL_MAX_CUBE_MAP_TEXTURE_SIZE
    // GLint maxSize = 0;
    // glGetIntegerv(GL_MAX_CUBE_MAP_TEXTURE_SIZE, &maxSize);
    // return static_cast<uint32_t>(maxSize);
    
    // 返回一个合理的默认值
    return 4096;
}

uint32_t RenderContextGLBase::getMaxColorAttachments() const {
    // TODO: 实际实现中需要查询GL_MAX_COLOR_ATTACHMENTS
    // GLint maxAttachments = 0;
    // glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS, &maxAttachments);
    // return static_cast<uint32_t>(maxAttachments);
    
    // 返回一个合理的默认值 (OpenGL ES 3.0最小要求是4)
    return 8;
}

uint32_t RenderContextGLBase::getMaxVertexAttributes() const {
    // TODO: 实际实现中需要查询GL_MAX_VERTEX_ATTRIBS
    // GLint maxAttribs = 0;
    // glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &maxAttribs);
    // return static_cast<uint32_t>(maxAttribs);
    
    // 返回一个合理的默认值 (OpenGL ES 2.0最小要求是8)
    return 16;
}

} // namespace render
} // namespace hyengine
