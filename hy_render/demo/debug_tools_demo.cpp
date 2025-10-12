#include <iostream>
#include <memory>
#include <thread>
#include <chrono>
#include "../include/hy_render/debug/HYRenderDebugger.hpp"
#include "../include/hy_render/HYRenderDef.hpp"

using namespace hyengine::render;
using namespace hyengine::render::debug;

/**
 * @brief 渲染调试工具功能演示
 * 
 * 这个示例演示了HY渲染引擎调试工具层的主要功能：
 * 1. 不同平台调试器的创建和使用
 * 2. GPU调试标记和性能分析
 * 3. 资源使用监控
 * 4. 错误检测和报告
 * 5. 性能计数器管理
 */
int main() {
    std::cout << "=== HY Render Debug Tools Demo ===" << std::endl;
    
    // 1. 创建不同平台的调试器
    std::cout << "\n1. Creating Debuggers for different APIs:" << std::endl;
    
    auto openglDebugger = DebuggerFactory::createDebugger(RenderAPI::kOpenGL);
    auto metalDebugger = DebuggerFactory::createDebugger(RenderAPI::kMetal);
    
    if (openglDebugger) {
        std::cout << "  ✓ OpenGL Debugger created" << std::endl;
    }
    if (metalDebugger) {
        std::cout << "  ✓ Metal Debugger created" << std::endl;
    }
    
    // 2. 设置全局调试器并启用调试
    std::cout << "\n2. Setting up debugging:" << std::endl;
    
    if (openglDebugger) {
        DebuggerFactory::setGlobalDebugger(openglDebugger);
        openglDebugger->setDebugLevel(DebugLevel::kVerbose);
        openglDebugger->enable();
        
        std::cout << "  Global debugger set to OpenGL" << std::endl;
        std::cout << "  Debug level: " << static_cast<int>(openglDebugger->getDebugLevel()) << std::endl;
    }
    
    // 3. 演示GPU调试标记
    std::cout << "\n3. Demonstrating GPU Debug Markers:" << std::endl;
    
    if (openglDebugger) {
        // 模拟渲染循环
        for (int frame = 0; frame < 3; ++frame) {
            std::cout << "\n  --- Frame " << frame << " ---" << std::endl;
            
            openglDebugger->beginFrame();
            
            // 主渲染通道
            openglDebugger->beginGPUMarker("Main_Render_Pass");
            
            // 几何渲染
            openglDebugger->beginGPUMarker("Geometry_Pass");
            openglDebugger->insertGPUMarker("Draw_Opaque_Objects");
            std::this_thread::sleep_for(std::chrono::milliseconds(2)); // 模拟GPU工作
            
            // 更新性能计数器
            openglDebugger->updateCounter(PerformanceCounterType::kDrawCalls, 15);
            openglDebugger->updateCounter(PerformanceCounterType::kTriangles, 5430);
            openglDebugger->updateCounter(PerformanceCounterType::kVertices, 16290);
            
            openglDebugger->endGPUMarker(); // Geometry_Pass
            
            // 后处理
            openglDebugger->beginGPUMarker("Post_Processing");
            openglDebugger->insertGPUMarker("Apply_Tone_Mapping");
            openglDebugger->insertGPUMarker("Apply_FXAA");
            std::this_thread::sleep_for(std::chrono::milliseconds(1)); // 模拟GPU工作
            openglDebugger->endGPUMarker(); // Post_Processing
            
            openglDebugger->endGPUMarker(); // Main_Render_Pass
            
            openglDebugger->endFrame();
        }
    }
    
    // 4. 演示资源监控
    std::cout << "\n4. Demonstrating Resource Monitoring:" << std::endl;
    
    if (openglDebugger) {
        // 模拟资源创建和销毁
        openglDebugger->registerResourceCreation(ResourceType::kBuffer, 1024 * 1024);  // 1MB
        openglDebugger->registerResourceCreation(ResourceType::kTexture, 2048 * 2048 * 4);  // 16MB
        openglDebugger->registerResourceCreation(ResourceType::kShader, 0);
        openglDebugger->registerResourceCreation(ResourceType::kFrameBuffer, 0);
        
        openglDebugger->registerResourceCreation(ResourceType::kBuffer, 512 * 1024);   // 512KB
        openglDebugger->registerResourceCreation(ResourceType::kTexture, 1024 * 1024 * 4);  // 4MB
        
        std::cout << "  Created various rendering resources" << std::endl;
        
        // 销毁一些资源
        openglDebugger->registerResourceDestruction(ResourceType::kBuffer, 512 * 1024);
        std::cout << "  Destroyed some resources" << std::endl;
    }
    
    // 5. 演示自定义性能计数器
    std::cout << "\n5. Demonstrating Custom Performance Counters:" << std::endl;
    
    if (openglDebugger) {
        openglDebugger->addCustomCounter("Render_Queue_Size", "items");
        openglDebugger->addCustomCounter("Occlusion_Culled_Objects", "objects");
        openglDebugger->addCustomCounter("Shadow_Map_Updates", "updates");
        
        // 更新自定义计数器
        openglDebugger->updateCustomCounter("Render_Queue_Size", 250);
        openglDebugger->updateCustomCounter("Occlusion_Culled_Objects", 45);
        openglDebugger->updateCustomCounter("Shadow_Map_Updates", 3);
        
        std::cout << "  Added and updated custom counters" << std::endl;
    }
    
    // 6. 演示错误和警告报告
    std::cout << "\n6. Demonstrating Error and Warning Reporting:" << std::endl;
    
    if (openglDebugger) {
        // 设置回调函数
        openglDebugger->setErrorCallback([](const std::string& message) {
            std::cout << "  [CALLBACK] Error captured: " << message << std::endl;
        });
        
        openglDebugger->setWarningCallback([](const std::string& message) {
            std::cout << "  [CALLBACK] Warning captured: " << message << std::endl;
        });
        
        // 模拟一些错误和警告
        openglDebugger->reportError("Simulated OpenGL error", "demoFunction", "debug_demo.cpp", 123);
        openglDebugger->reportWarning("Performance warning: too many state changes", "renderFrame", "renderer.cpp", 456);
        
        // GPU错误检查
        bool hasErrors = openglDebugger->checkGPUErrors();
        std::cout << "  GPU Error Check: " << (hasErrors ? "No errors" : "Errors found") << std::endl;
        
        // 渲染状态验证
        bool stateValid = openglDebugger->validateRenderState();
        std::cout << "  Render State Validation: " << (stateValid ? "Valid" : "Invalid") << std::endl;
    }
    
    // 7. 生成报告
    std::cout << "\n7. Generating Debug Reports:" << std::endl;
    
    if (openglDebugger) {
        openglDebugger->printPerformanceReport();
        openglDebugger->printResourceReport();
        
        // 导出调试数据
        bool exported = openglDebugger->exportDebugData("debug_report.txt");
        std::cout << "  Debug data export: " << (exported ? "Success" : "Failed") << std::endl;
    }
    
    // 8. 演示Metal调试器
    if (metalDebugger) {
        std::cout << "\n8. Demonstrating Metal Debugger:" << std::endl;
        
        metalDebugger->setDebugLevel(DebugLevel::kInfo);
        metalDebugger->enable();
        
        metalDebugger->beginFrame();
        metalDebugger->beginGPUMarker("Metal_Render_Pass");
        metalDebugger->updateCounter(PerformanceCounterType::kDrawCalls, 25);
        metalDebugger->updateCounter(PerformanceCounterType::kTriangles, 8760);
        metalDebugger->endGPUMarker();
        metalDebugger->endFrame();
        
        metalDebugger->printPerformanceReport();
    }
    
    // 9. 演示便捷宏
    std::cout << "\n9. Demonstrating Debug Macros:" << std::endl;
    
    // 注意：这些宏只在定义了HY_RENDER_DEBUG时才会执行
    std::cout << "  Using debug macros (simulated):" << std::endl;
    std::cout << "    HY_DEBUG_MARKER_BEGIN(\"Macro_Test\")" << std::endl;
    std::cout << "    HY_DEBUG_COUNTER_UPDATE(kDrawCalls, 10)" << std::endl;
    std::cout << "    HY_DEBUG_RESOURCE_CREATED(kBuffer, 2048)" << std::endl;
    std::cout << "    HY_DEBUG_CHECK_ERRORS()" << std::endl;
    std::cout << "    HY_DEBUG_MARKER_END()" << std::endl;
    
    // 10. 清理
    std::cout << "\n10. Cleanup:" << std::endl;
    
    if (openglDebugger) {
        openglDebugger->disable();
        std::cout << "  OpenGL debugger disabled" << std::endl;
    }
    
    if (metalDebugger) {
        metalDebugger->disable();
        std::cout << "  Metal debugger disabled" << std::endl;
    }
    
    DebuggerFactory::setGlobalDebugger(nullptr);
    
    std::cout << "\n=== Render Debug Tools Demo Completed ===" << std::endl;
    
    return 0;
}