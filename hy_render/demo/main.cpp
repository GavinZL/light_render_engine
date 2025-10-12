#include "hy_render/HYRenderEngine.hpp"
#include "hy_render/HYRenderContext.hpp"
#include "hy_render/resources/HYBuffer.hpp"
#include "hy_render/resources/HYShader.hpp"
#include "hy_render/resources/HYRenderPass.hpp"
#include "hy_render/resources/HYPipelineState.hpp"

#include <iostream>
#include <cmath>

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

int main() {
    std::cout << "=== HY Render Engine Demo ===" << std::endl;

    // 1. 初始化渲染引擎
    auto& engine = HYRenderEngine::instance();
    
    // 选择最优API
    auto api = engine.selectBestAPI();
    std::cout << "Selected API: " << static_cast<int>(api) << std::endl;
    
    if (api == RenderAPI::kUnknown) {
        std::cerr << "No supported render API found!" << std::endl;
        return -1;
    }
    
    engine.setRenderAPI(api);
    
    // 2. 获取渲染上下文
    auto context = engine.getRenderContext();
    if (!context) {
        std::cerr << "Failed to create render context!" << std::endl;
        return -1;
    }
    
    std::cout << "Render context created successfully" << std::endl;

    // 3. 创建顶点缓冲
    BufferDesc bufferDesc;
    bufferDesc.bindFlags = BIND_VERTEX_BUFFER;
    bufferDesc.usage = BufferUsage::USAGE_STATIC_DRAW;
    bufferDesc.size = sizeof(vertices);
    bufferDesc.initialData = vertices;
    
    LayoutElement layout;
    layout.format = VertexFormat::kFloat2;  // 位置
    layout.offset = 0;
    layout.stride = 5 * sizeof(float);
    bufferDesc.layoutElements.push_back(layout);
    
    layout.format = VertexFormat::kFloat3;  // 颜色
    layout.offset = 2 * sizeof(float);
    bufferDesc.layoutElements.push_back(layout);
    
    auto vertexBuffer = context->createBuffer(bufferDesc);
    if (!vertexBuffer) {
        std::cerr << "Failed to create vertex buffer!" << std::endl;
        return -1;
    }
    
    std::cout << "Vertex buffer created: " << vertexBuffer->getSize() << " bytes" << std::endl;

    // 4. 创建着色器
    ShaderDesc shaderDesc;
    shaderDesc.name = "BasicShader";
    shaderDesc.sourceLanguage = ShaderSourceLanguage::kShaderSourceLanguage_GLSL;
    
    ShaderCodeDesc vertCode;
    vertCode.shaderStage = ShaderStage::kShaderStage_Vertex;
    vertCode.shaderCode = vertexShaderSource;
    vertCode.shaderCodeLength = std::strlen(vertexShaderSource);
    shaderDesc.shaderCodeDesc.push_back(vertCode);
    
    ShaderCodeDesc fragCode;
    fragCode.shaderStage = ShaderStage::kShaderStage_Fragment;
    fragCode.shaderCode = fragmentShaderSource;
    fragCode.shaderCodeLength = std::strlen(fragmentShaderSource);
    shaderDesc.shaderCodeDesc.push_back(fragCode);
    
    auto shader = context->createShader(shaderDesc);
    if (!shader) {
        std::cerr << "Failed to create shader!" << std::endl;
        return -1;
    }
    
    std::cout << "Shader created successfully" << std::endl;

    // 5. 创建管线状态
    PipelineStateDesc psoDesc;
    psoDesc.blendDesc.blendEnable = false;
    psoDesc.depthStencilDesc.depthTestEnable = false;
    psoDesc.rasterizerDesc.cullMode = CullMode::CULL_MODE_NONE;
    psoDesc.primitiveDesc.type = PrimitiveType::PRIMITIVE_TRIANGLES;
    
    auto pipelineState = context->createPipelineState(psoDesc);
    if (!pipelineState) {
        std::cerr << "Failed to create pipeline state!" << std::endl;
        return -1;
    }
    
    std::cout << "Pipeline state created successfully" << std::endl;

    // 6. 创建渲染通道
    auto renderPass = context->createRenderPass();
    if (!renderPass) {
        std::cerr << "Failed to create render pass!" << std::endl;
        return -1;
    }
    
    std::cout << "Render pass created successfully" << std::endl;

    // 模拟渲染循环（仅打印信息，不实际渲染）
    std::cout << "\n=== Simulating Render Loop ===" << std::endl;
    
    for (int frame = 0; frame < 3; ++frame) {
        std::cout << "\n--- Frame " << frame << " ---" << std::endl;
        
        // 开始渲染通道
        std::cout << "  beginPass()" << std::endl;
        // renderPass->beginPass(nullptr);
        
        // 设置视口
        std::cout << "  setViewport(0, 0, 800, 600)" << std::endl;
        // renderPass->setViewport(0, 0, 800, 600);
        
        // 清除颜色缓冲
        std::cout << "  setClearColor(0.2, 0.3, 0.3, 1.0)" << std::endl;
        std::cout << "  clearBuffer(COLOR)" << std::endl;
        // renderPass->setClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        // renderPass->clearBuffer(CLEAR_COLOR_BUFFER);
        
        // 设置着色器和管线状态
        std::cout << "  setShader()" << std::endl;
        std::cout << "  setPipelineState()" << std::endl;
        // renderPass->setShader(shader);
        // renderPass->setPipelineState(pipelineState);
        
        // 绑定顶点缓冲
        std::cout << "  setVertexBuffer()" << std::endl;
        // renderPass->setVertexBuffer(vertexBuffer);
        
        // 绘制
        std::cout << "  drawArrays(0, 3)" << std::endl;
        // renderPass->drawArrays(0, 3);
        
        // 结束渲染通道
        std::cout << "  endPass()" << std::endl;
        // renderPass->endPass();
    }

    std::cout << "\n=== Demo Completed Successfully ===" << std::endl;
    return 0;
}
