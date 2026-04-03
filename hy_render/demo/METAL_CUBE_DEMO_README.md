# Metal Cube Demo

## 概述

这是一个在 macOS 平台上使用 Metal 图形 API 渲染立方体的演示程序。该 demo 展示了：

- Metal 渲染管线的完整实现
- 立方体几何体的渲染
- MVP（模型-视图-投影）矩阵变换
- 鼠标交互式旋转控制

## 功能特性

### 1. 渲染特性

- **图形 API**: Metal 2.0+（Apple 原生图形 API）
- **着色器**: 使用 Metal Shading Language (MSL)
- **几何体**: 8 顶点立方体，带有颜色插值
- **变换**: 完整的 MVP 矩阵变换链

### 2. 交互功能

- **鼠标拖动旋转**: 左键按住拖动可旋转立方体
  - 水平拖动：绕 Y 轴旋转
  - 垂直拖动：绕 X 轴旋转
- **实时渲染**: 60 FPS 渲染循环

### 3. 技术实现

#### 窗口系统
- 使用 Cocoa (NSWindow) 创建原生 macOS 窗口
- CAMetalLayer 作为 Metal 渲染表面

#### 渲染管线
- 内联 MSL 着色器代码
- 顶点属性：位置 + 颜色
- 深度测试和背面剔除

#### 缓冲区管理
- 顶点缓冲区（Shared 存储模式）
- 索引缓冲区（用于绘制优化）
- Uniform 缓冲区（存储 MVP 矩阵）

## 编译运行

### 前置要求

- macOS 10.15+
- Xcode 命令行工具
- CMake 3.15+
- 支持 Metal 的 Mac 设备

### 编译步骤

```bash
cd hy_render
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make metal_cube_demo
```

### 运行

```bash
./bin/metal_cube_demo
```

## 文件结构

```
demo/
├── metal_cube_demo.mm          # 主程序文件
└── shaders/
    └── cube.metal              # Metal 着色器（未使用，采用内联代码）
```

## 代码说明

### 主要类

#### MetalView
- 继承自 NSView
- 管理 CAMetalLayer
- 处理鼠标事件
- 维护旋转角度状态

#### MetalCubeRenderer
- 初始化 Metal 设备和命令队列
- 创建渲染管线状态
- 管理顶点、索引和 Uniform 缓冲区
- 执行每帧渲染

#### AppDelegate
- 应用程序生命周期管理
- 创建窗口和视图
- 管理渲染循环定时器

### 立方体数据

**顶点格式**:
```cpp
struct Vertex {
    Vec3 position;  // 3D 位置
    Vec3 color;     // RGB 颜色
};
```

**立方体顶点**: 8 个顶点，每个顶点带有唯一颜色
**立方体索引**: 36 个索引组成 12 个三角形

### 着色器

**顶点着色器** (`vertex_main`):
- 接收顶点位置和颜色
- 通过 Uniform 缓冲区接收 MVP 矩阵
- 变换顶点到裁剪空间
- 传递颜色到片段着色器

**片段着色器** (`fragment_main`):
- 接收插值后的颜色
- 直接输出为最终颜色

### 矩阵变换

```cpp
// 模型矩阵（旋转）
Mat4 rotX = Mat4::rotateX(rotationX);
Mat4 rotY = Mat4::rotateY(rotationY);
modelMatrix = rotY * rotX;

// 视图矩阵（相机位置）
viewMatrix = Mat4::lookAt(eye, center, up);

// 投影矩阵（透视投影）
projectionMatrix = Mat4::perspective(fov, aspect, near, far);
```

## HY Render Engine 集成

该 demo 使用了 hy_render 引擎的以下组件：

1. **数学库** (`HYMath.hpp`)
   - `Vec3`: 3D 向量
   - `Mat4`: 4x4 矩阵
   - 矩阵变换函数：`lookAt`, `perspective`, `rotateX`, `rotateY`

2. **Metal 渲染上下文** (`RenderContextMtl`)
   - Metal 设备管理
   - 命令队列创建

3. **Metal 资源实现**
   - `BufferMtl`: Metal 缓冲区封装
   - 其他 Metal 资源类（未在此 demo 中直接使用）

## 优化建议

### 当前实现
- ✅ 使用内联着色器代码（避免外部文件依赖）
- ✅ Shared 存储模式（便于 CPU-GPU 数据传输）
- ✅ 深度测试和背面剔除

### 可能的改进
- 🔄 添加深度缓冲区（当前使用深度状态但未创建深度纹理）
- 🔄 支持窗口大小调整
- 🔄 添加更多几何体和材质
- 🔄 实现光照计算

## 常见问题

### Q: 窗口打开但是黑屏？
A: 检查 Metal 着色器编译是否成功，查看控制台输出的错误信息。

### Q: 鼠标拖动没有反应？
A: 确保点击在窗口内容区域，并且鼠标事件正常触发。

### Q: 编译时找不到 Metal 框架？
A: 确保使用的是 macOS 系统，Metal 仅在 Apple 平台可用。

## 性能

- **设备**: Apple M4
- **分辨率**: 800x600
- **帧率**: 60 FPS
- **三角形数**: 12（立方体的 6 个面）

## 许可证

与 HY Render Engine 主项目保持一致。

## 作者

HY Render Engine Team

---

**享受你的 Metal 渲染之旅！** 🎨✨
