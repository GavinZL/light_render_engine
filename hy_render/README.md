# HY Render Engine - 跨平台轻量级渲染引擎

## 项目概述

HY Render Engine是一个基于现代C++17开发的跨平台轲量级渲染引擎，通过抽象不同图形API的核心概念，提供统一的渲染接口。

### 支持的图形API

- **OpenGL 3.3+ Core Profile** - Windows/Linux/macOS桌面端
- **OpenGL ES 3.0+** - Android/iOS移动端
- **Metal 2.0+** - macOS/iOS (计划实现)
- **Vulkan 1.0+** - 跨平台次世代API (计划实现)

### 核心特性

✅ **统一抽象层** - 单一接口，多平台实现  
✅ **现代C++设计** - C++17标准，智能指针管理  
✅ **工厂模式** - 通过RenderContext创建所有资源  
✅ **轻量级架构** - 最小化依赖，专注核心渲染功能  
✅ **清晰分层** - 核心抽象层/平台实现层/应用层分离  

## 项目结构

```
hy_render/
├── CMakeLists.txt              # 主CMake配置
├── include/                    # 公共头文件
│   └── hy_render/
│       ├── HYRenderEngine.hpp  # 渲染引擎单例
│       ├── HYRenderContext.hpp # 渲染上下文抽象
│       ├── HYRenderDef.hpp     # 通用定义和描述结构
│       └── resources/          # 资源对象头文件
│           ├── HYBuffer.hpp
│           ├── HYTexture.hpp
│           ├── HYShader.hpp
│           ├── HYSampler.hpp
│           ├── HYFrameBuffer.hpp
│           ├── HYPipelineState.hpp
│           ├── HYRenderPass.hpp
│           ├── HYVertexArray.hpp
│           └── HYFence.hpp
├── src/                        # 源代码实现
│   ├── core/                   # 核心抽象层
│   ├── opengl/                 # OpenGL 3.3+实现
│   ├── opengles/               # OpenGL ES 3.0+实现
│   ├── metal/                  # Metal实现（待实现）
│   ├── vulkan/                 # Vulkan实现（待实现）
│   ├── gl_common/              # OpenGL/GLES共享代码
│   └── utils/                  # 工具类
├── demo/                       # 示例程序
│   ├── main.cpp
│   └── shaders/
└── tests/                      # 单元测试
```

## 核心对象架构

### 1. HYRenderEngine（渲染引擎单例）

全局渲染引擎入口，管理API类型和上下文创建。

```cpp
auto& engine = HYRenderEngine::instance();
engine.setRenderAPI(RenderAPI::kOpenGL);
auto context = engine.getRenderContext();
```

### 2. HYRenderContext（渲染上下文工厂）

抽象工厂，创建所有渲染资源对象。

```cpp
auto buffer = context->createBuffer(bufferDesc);
auto texture = context->createTexture(textureDesc);
auto shader = context->createShader(shaderDesc);
auto renderPass = context->createRenderPass();
```

### 3. 核心资源对象

| 资源对象 | 职责 | 对应API概念 |
|---------|------|------------|
| **HYBuffer** | GPU缓冲区 | VBO/IBO/UBO |
| **HYTexture** | 纹理对象 | Texture2D/TextureCube |
| **HYShader** | 着色器程序 | Shader Program |
| **HYSampler** | 采样器状态 | Sampler Object |
| **HYFrameBuffer** | 帧缓冲 | FBO/RenderTarget |
| **HYPipelineState** | 管线状态 | Blend/Depth/Rasterizer |
| **HYRenderPass** | 渲染通道 | 封装绘制流程 |
| **HYVertexArray** | 顶点数组 | VAO (OpenGL) |
| **HYFence** | 同步栅栏 | GPU同步机制 |

## 快速开始

### 编译要求

- **CMake** 3.15+
- **C++17**兼容编译器
  - GCC 7+
  - Clang 5+
  - MSVC 2017+
- **平台依赖**
  - Linux: OpenGL开发库
  - macOS: 系统自带OpenGL框架
  - Windows: OpenGL开发库

### 编译步骤

```bash
# 克隆仓库
git clone <repository-url>
cd light_render_engine/hy_render

# 创建构建目录
mkdir build && cd build

# 配置CMake
cmake .. -DCMAKE_BUILD_TYPE=Release

# 编译
cmake --build . --config Release

# 运行Demo
./demo/hy_render_demo
```

### 基础使用示例

```cpp
#include "hy_render/HYRenderEngine.hpp"
#include "hy_render/HYRenderContext.hpp"
#include "hy_render/resources/HYBuffer.hpp"
#include "hy_render/resources/HYShader.hpp"
#include "hy_render/resources/HYRenderPass.hpp"

using namespace hyengine::render;

int main() {
    // 1. 初始化引擎
    auto& engine = HYRenderEngine::instance();
    engine.setRenderAPI(RenderAPI::kOpenGL);
    auto context = engine.getRenderContext();

    // 2. 创建顶点缓冲
    float vertices[] = { /* 顶点数据 */ };
    BufferDesc bufferDesc;
    bufferDesc.bindFlags = BIND_VERTEX_BUFFER;
    bufferDesc.size = sizeof(vertices);
    bufferDesc.initialData = vertices;
    auto vertexBuffer = context->createBuffer(bufferDesc);

    // 3. 创建着色器
    ShaderDesc shaderDesc;
    shaderDesc.sourceLanguage = ShaderSourceLanguage::kShaderSourceLanguage_GLSL;
    // ... 填充着色器代码
    auto shader = context->createShader(shaderDesc);

    // 4. 创建管线状态
    PipelineStateDesc psoDesc;
    psoDesc.primitiveDesc.type = PrimitiveType::PRIMITIVE_TRIANGLES;
    auto pipelineState = context->createPipelineState(psoDesc);

    // 5. 渲染循环
    auto renderPass = context->createRenderPass();
    
    while (!shouldClose) {
        renderPass->beginPass();
        renderPass->setViewport(0, 0, 800, 600);
        renderPass->setClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        renderPass->clearBuffer(CLEAR_COLOR_BUFFER);
        
        renderPass->setShader(shader);
        renderPass->setPipelineState(pipelineState);
        renderPass->setVertexBuffer(vertexBuffer);
        
        renderPass->drawArrays(0, 3);
        renderPass->endPass();
    }

    return 0;
}
```

## 渲染流程

### 初始化流程

```
应用层
  └─> HYRenderEngine::setRenderAPI()
      └─> 创建对应平台的RenderContext
          └─> RenderContextGL / RenderContextGLES / RenderContextMtl
```

### 每帧渲染流程

```
beginPass(framebuffer)
  ├─> setViewport(x, y, w, h)
  ├─> setClearColor(r, g, b, a)
  ├─> clearBuffer(COLOR | DEPTH)
  ├─> setShader(shader)
  ├─> setPipelineState(pso)
  ├─> setVertexBuffer(vbo)
  ├─> setIndexBuffer(ibo)
  ├─> drawArrays() / drawElements()
  └─> endPass()
```

## 设计原则

### 1. 分层设计
- **核心抽象层**: 定义统一接口
- **平台实现层**: OpenGL/Metal/Vulkan具体实现
- **业务应用层**: Demo和测试代码

### 2. 设计模式应用
- **工厂模式**: RenderContext创建资源
- **策略模式**: PipelineState封装渲染状态
- **单例模式**: RenderEngine全局唯一
- **适配器模式**: 统一不同平台API

### 3. 资源管理
- 使用`std::shared_ptr`自动管理生命周期
- 资源通过Context工厂创建
- 支持延迟销毁机制（计划实现）

## 平台差异处理

| 特性 | OpenGL 3.3+ | OpenGL ES 3.0 | Metal | Vulkan |
|------|-------------|---------------|-------|--------|
| VAO | 必须 | 可选 | 无 | 无 |
| 采样器 | 分离式 | 内嵌式 | 独立对象 | 独立对象 |
| 深度范围 | double | float | float | float |
| 几何着色器 | ✓ | ✗ | ✗ | ✓ |

## 路线图

### 已完成 ✅
- [x] 核心架构设计
- [x] 抽象接口定义
- [x] OpenGL基础框架
- [x] OpenGL ES基础框架
- [x] Demo示例程序

### 开发中 🚧
- [ ] OpenGL完整实现
- [ ] OpenGL ES完整实现
- [ ] 数学库实现
- [ ] 工具类实现

### 计划中 📋
- [ ] Metal实现
- [ ] Vulkan实现
- [ ] 异步资源加载
- [ ] Shader热重载
- [ ] 场景图系统
- [ ] 材质系统

## 贡献指南

欢迎提交Issue和Pull Request！

### 编码规范
- 遵循Google C++ Style Guide
- 使用C++17特性
- 类名使用PascalCase (HYRenderEngine)
- 函数名使用camelCase (createBuffer)
- 成员变量使用m前缀 (mCurrentAPI)

### 提交规范
- feat: 新功能
- fix: 修复bug
- docs: 文档更新
- refactor: 代码重构
- test: 测试相关

## 许可证

[请根据实际情况添加许可证信息]

## 联系方式

[请添加联系方式]

---

**注意**: 本项目当前处于早期开发阶段，部分功能仍在实现中。
