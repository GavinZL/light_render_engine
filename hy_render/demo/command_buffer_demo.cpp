#include <iostream>
#include <memory>
#include "../include/hy_render/resources/HYCommandBuffer.hpp"
#include "../include/hy_render/HYRenderDef.hpp"

using namespace hyengine::render;

/**
 * @brief 命令缓冲区功能测试
 * 
 * 这个示例演示了如何使用命令缓冲区抽象层：
 * 1. 创建命令缓冲区
 * 2. 记录渲染命令
 * 3. 提交执行命令
 */
int main() {
    std::cout << "=== HY Command Buffer Demo ===" << std::endl;
    
    // 1. 创建不同平台的命令缓冲区
    std::cout << "\n1. Creating Command Buffers for different APIs:" << std::endl;
    
    auto openglCmdBuffer = CommandBufferFactory::createCommandBuffer(RenderAPI::kOpenGL);
    auto openglesCmdBuffer = CommandBufferFactory::createCommandBuffer(RenderAPI::kOpenGLES);
    auto metalCmdBuffer = CommandBufferFactory::createCommandBuffer(RenderAPI::kMetal);
    
    if (openglCmdBuffer) {
        std::cout << "  ✓ OpenGL Command Buffer created" << std::endl;
    }
    if (openglesCmdBuffer) {
        std::cout << "  ✓ OpenGL ES Command Buffer created" << std::endl;
    }
    if (metalCmdBuffer) {
        std::cout << "  ✓ Metal Command Buffer created" << std::endl;
    }
    
    // 2. 使用OpenGL命令缓冲区进行演示
    if (openglCmdBuffer) {
        std::cout << "\n2. Recording commands with OpenGL Command Buffer:" << std::endl;
        
        // 开始记录命令
        openglCmdBuffer->begin();
        
        // 记录一系列渲染命令
        openglCmdBuffer->cmdBeginPass(nullptr); // 默认帧缓冲
        openglCmdBuffer->cmdSetViewport(0, 0, 800, 600);
        openglCmdBuffer->cmdClearBuffer(0x01, 0.2f, 0.3f, 0.3f, 1.0f); // 清除颜色缓冲
        
        // 记录绘制命令
        DrawParams drawParams;
        drawParams.vertexCount = 3;
        drawParams.primitiveType = PrimitiveType::PRIMITIVE_TRIANGLES;
        openglCmdBuffer->cmdDraw(drawParams);
        
        openglCmdBuffer->cmdEndPass();
        
        // 完成命令记录
        openglCmdBuffer->end();
        
        std::cout << "  Commands recorded: " << openglCmdBuffer->getCommandCount() << std::endl;
        std::cout << "  State: " << static_cast<int>(openglCmdBuffer->getState()) << std::endl;
        
        // 显示调试信息
        std::cout << "\n  Debug Info:" << std::endl;
        std::cout << openglCmdBuffer->getDebugInfo() << std::endl;
    }
    
    // 3. 演示批次管理
    if (metalCmdBuffer) {
        std::cout << "\n3. Demonstrating batch management with Metal Command Buffer:" << std::endl;
        
        metalCmdBuffer->begin();
        
        // 第一个批次：设置渲染状态
        metalCmdBuffer->beginBatch("Setup");
        metalCmdBuffer->cmdBeginPass(nullptr);
        metalCmdBuffer->cmdSetViewport(0, 0, 1024, 768);
        metalCmdBuffer->cmdClearBuffer(0x01, 0.1f, 0.1f, 0.2f, 1.0f);
        metalCmdBuffer->endBatch();
        
        // 第二个批次：绘制操作
        metalCmdBuffer->beginBatch("Draw_Objects");
        DrawParams params1;
        params1.vertexCount = 6;
        params1.primitiveType = PrimitiveType::PRIMITIVE_TRIANGLES;
        metalCmdBuffer->cmdDraw(params1);
        
        DrawParams params2;
        params2.vertexCount = 4;
        params2.indexCount = 6;
        params2.primitiveType = PrimitiveType::PRIMITIVE_TRIANGLES;
        metalCmdBuffer->cmdDraw(params2);
        metalCmdBuffer->endBatch();
        
        // 第三个批次：清理
        metalCmdBuffer->beginBatch("Cleanup");
        metalCmdBuffer->cmdEndPass();
        metalCmdBuffer->endBatch();
        
        metalCmdBuffer->end();
        
        std::cout << "  Total commands recorded: " << metalCmdBuffer->getCommandCount() << std::endl;
        std::cout << "  Debug Info:" << std::endl;
        std::cout << metalCmdBuffer->getDebugInfo() << std::endl;
    }
    
    // 4. 演示自定义命令
    if (openglCmdBuffer && openglCmdBuffer->getState() == CommandBufferState::kCompleted) {
        std::cout << "\n4. Demonstrating custom commands:" << std::endl;
        
        openglCmdBuffer->reset();
        openglCmdBuffer->begin();
        
        // 添加自定义命令
        openglCmdBuffer->cmdCustom([](HYRenderPass* renderPass) {
            std::cout << "    Executing custom command: Setting up special render state" << std::endl;
            // 这里可以执行任何自定义的渲染操作
        });
        
        openglCmdBuffer->cmdCustom([](HYRenderPass* renderPass) {
            std::cout << "    Executing custom command: GPU performance marker" << std::endl;
        });
        
        openglCmdBuffer->end();
        
        std::cout << "  Custom commands recorded: " << openglCmdBuffer->getCommandCount() << std::endl;
    }
    
    // 5. 演示命令缓冲区池
    std::cout << "\n5. Creating Command Buffer Pool:" << std::endl;
    auto cmdBufferPool = CommandBufferFactory::createCommandBufferPool(RenderAPI::kOpenGL, 4);
    std::cout << "  Created pool with " << cmdBufferPool.size() << " command buffers" << std::endl;
    
    for (size_t i = 0; i < cmdBufferPool.size(); ++i) {
        if (cmdBufferPool[i]) {
            std::cout << "    Buffer " << i << ": Ready (State=" 
                      << static_cast<int>(cmdBufferPool[i]->getState()) << ")" << std::endl;
        }
    }
    
    std::cout << "\n=== Command Buffer Demo Completed ===" << std::endl;
    
    return 0;
}