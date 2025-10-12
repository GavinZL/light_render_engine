// HY Render Engine Demo
// 一个展示渲染引擎基本功能的简单示例

#include <iostream>
#include <cmath>
#include <cstring>

#ifndef USE_GLFW
#define USE_GLFW 0
#endif

#if USE_GLFW
#include <GLFW/glfw3.h>
#endif

// 前向声明和基本类型定义
namespace hyengine {
namespace render {

enum class RenderAPI : uint8_t {
    kOpenGL,
    kOpenGLES,
    kMetal,
    kVulkan,
    kUnknown
};

// 假设的渲染引擎接口
class HYRenderEngine {
public:
    static HYRenderEngine& instance() {
        static HYRenderEngine inst;
        return inst;
    }
    
    void setRenderAPI(RenderAPI api) {
        mCurrentAPI = api;
        std::cout << "[HYRenderEngine] API set to: " << static_cast<int>(api) << std::endl;
    }
    
    RenderAPI getRenderAPI() const { return mCurrentAPI; }
    
    RenderAPI selectBestAPI() {
#ifdef __APPLE__
        return RenderAPI::kMetal;
#elif defined(__ANDROID__) || defined(__linux__)
        return RenderAPI::kVulkan;
#elif defined(_WIN32)
        return RenderAPI::kVulkan;
#else
        return RenderAPI::kOpenGL;
#endif
    }
    
private:
    RenderAPI mCurrentAPI = RenderAPI::kUnknown;
};

} // namespace render
} // namespace hyengine

using namespace hyengine::render;

// 简单的三角形顶点数据
float vertices[] = {
    // 位置          // 颜色
     0.0f,  0.5f,   1.0f, 0.0f, 0.0f,  // 顶点0
    -0.5f, -0.5f,   0.0f, 1.0f, 0.0f,  // 顶点1
     0.5f, -0.5f,   0.0f, 0.0f, 1.0f,  // 顶点2
};

// 顶点着色器（GLSL 3.3）
const char* vertexShaderSource = R"(
#version 330 core
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec3 aColor;

out vec3 vColor;

void main() {
    gl_Position = vec4(aPos, 0.0, 1.0);
    vColor = aColor;
}
)";

// 片段着色器（GLSL 3.3）
const char* fragmentShaderSource = R"(
#version 330 core
in vec3 vColor;
out vec4 FragColor;

void main() {
    FragColor = vec4(vColor, 1.0);
}
)";

#if USE_GLFW
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    // 在实际项目中这里会调用glViewport
    std::cout << "Framebuffer resized to " << width << "x" << height << std::endl;
}

void processInput(GLFWwindow *window) {
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

int runWithGLFW() {
    std::cout << "=== HY Render Engine Demo (GLFW Mode) ===" << std::endl;
    
    // 初始化GLFW
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }
    
    // 配置GLFW
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
    
    // 创建窗口
    GLFWwindow* window = glfwCreateWindow(800, 600, "HY Render Engine Demo", NULL, NULL);
    if (window == NULL) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    
    std::cout << "GLFW window created successfully" << std::endl;
    
    // 初始化渲染引擎
    auto& engine = HYRenderEngine::instance();
    engine.setRenderAPI(RenderAPI::kOpenGL);
    
    std::cout << "Render engine initialized" << std::endl;
    
    // 渲染循环
    int frame = 0;
    while (!glfwWindowShouldClose(window) && frame < 100) { // 限制帧数避免无限循环
        processInput(window);
        
        // 模拟渲染命令
        if (frame % 30 == 0) { // 每30帧打印一次
            std::cout << "Frame " << frame << ": Rendering triangle..." << std::endl;
        }
        
        glfwSwapBuffers(window);
        glfwPollEvents();
        frame++;
    }
    
    glfwTerminate();
    std::cout << "GLFW demo completed successfully after " << frame << " frames" << std::endl;
    return 0;
}
#endif

int runWithoutGLFW() {
    std::cout << "=== HY Render Engine Demo (Console Mode) ===" << std::endl;

    // 1. 初始化渲染引擎
    auto& engine = HYRenderEngine::instance();
    
    // 选择最优API
    auto api = engine.selectBestAPI();
    std::cout << "Selected API: ";
    switch(api) {
        case RenderAPI::kOpenGL:
            std::cout << "OpenGL" << std::endl;
            break;
        case RenderAPI::kOpenGLES:
            std::cout << "OpenGL ES" << std::endl;
            break;
        case RenderAPI::kMetal:
            std::cout << "Metal" << std::endl;
            break;
        case RenderAPI::kVulkan:
            std::cout << "Vulkan" << std::endl;
            break;
        default:
            std::cout << "Unknown" << std::endl;
            break;
    }
    
    if (api == RenderAPI::kUnknown) {
        std::cerr << "No supported render API found!" << std::endl;
        return -1;
    }
    
    engine.setRenderAPI(api);

    // 2. 模拟渲染资源创建
    std::cout << "\nCreating render resources..." << std::endl;
    
    // 顶点缓冲
    size_t vertexBufferSize = sizeof(vertices);
    std::cout << "Created vertex buffer: " << vertexBufferSize << " bytes" << std::endl;
    std::cout << "  Vertex count: " << sizeof(vertices) / (5 * sizeof(float)) << std::endl;
    
    // 着色器
    std::cout << "Created shader program:" << std::endl;
    std::cout << "  Vertex shader: " << strlen(vertexShaderSource) << " chars" << std::endl;
    std::cout << "  Fragment shader: " << strlen(fragmentShaderSource) << " chars" << std::endl;
    
    // 管线状态
    std::cout << "Created pipeline state: no blending, no depth test, triangles" << std::endl;

    // 3. 模拟渲染循环
    std::cout << "\n=== Simulating Render Loop ===" << std::endl;
    
    for (int frame = 0; frame < 5; ++frame) {
        std::cout << "\n--- Frame " << frame << " ---" << std::endl;
        
        // 开始渲染通道
        std::cout << "  beginPass(defaultFramebuffer)" << std::endl;
        
        // 设置视口
        std::cout << "  setViewport(0, 0, 800, 600)" << std::endl;
        
        // 清除颜色缓冲
        std::cout << "  setClearColor(0.2, 0.3, 0.3, 1.0)" << std::endl;
        std::cout << "  clearBuffer(COLOR_BUFFER)" << std::endl;
        
        // 设置着色器和管线状态
        std::cout << "  setShader(BasicTriangleShader)" << std::endl;
        std::cout << "  setPipelineState(trianglePSO)" << std::endl;
        
        // 绑定顶点缓冲
        std::cout << "  setVertexBuffer(triangleVBO)" << std::endl;
        
        // 绘制三角形
        std::cout << "  drawArrays(0, 3)" << std::endl;
        
        // 结束渲染通道
        std::cout << "  endPass()" << std::endl;
        
        // 模拟帧间间隔
        std::cout << "  [Present and swap buffers]" << std::endl;
    }

    std::cout << "\n=== Demo Completed Successfully ===" << std::endl;
    std::cout << "\nNote: This is a simulation. In a real implementation with OpenGL context," << std::endl;
    std::cout << "      the triangle would be rendered to the screen." << std::endl;
    std::cout << "\nTo see actual rendering, you would need to:" << std::endl;
    std::cout << "1. Link with OpenGL/GLAD libraries" << std::endl;
    std::cout << "2. Create a proper OpenGL context (e.g. with GLFW)" << std::endl;
    std::cout << "3. Implement the actual GL calls in the resource classes" << std::endl;
    return 0;
}

int main() {
#if USE_GLFW
    return runWithGLFW();
#else
    return runWithoutGLFW();
#endif
}