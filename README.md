# Light Render Engine - 跨平台轻量级渲染引擎

## 项目简介

基于设计文档实现的跨平台轻量级渲染引擎，支持OpenGL、OpenGL ES、Metal、Vulkan多种图形API。

## 快速导航

- 📖 [项目README](./hy_render/README.md) - 使用指南和快速开始
- 📝 [实现总结](./IMPLEMENTATION_SUMMARY.md) - 当前完成状态和后续计划

## 项目结构

```
light_render_engine/
├── hy_render/                    # 渲染引擎主项目
│   ├── include/                  # 公共头文件
│   ├── src/                      # 源代码实现
│   ├── demo/                     # Demo示例
│   ├── tests/                    # 单元测试
│   ├── CMakeLists.txt            # 构建配置
│   └── README.md                 # 项目文档
├── IMPLEMENTATION_SUMMARY.md     # 实现总结
└── readme.md                     # 本文件
```

## 核心特性

✅ **统一抽象接口** - 单一API，多平台实现  
✅ **现代C++17** - 智能指针，移动语义  
✅ **工厂模式** - 资源统一创建管理  
✅ **轻量级设计** - 最小化依赖，专注核心功能  

## 支持平台

| 平台 | 图形API | 状态 |
|------|---------|------|
| Windows/Linux/macOS | OpenGL 3.3+ | 🚧 框架完成 |
| Android/iOS | OpenGL ES 3.0+ | 🚧 框架完成 |
| macOS/iOS | Metal 2.0+ | 📋 计划中 |
| 跨平台 | Vulkan 1.0+ | 📋 计划中 |

## 快速开始

### 编译要求

- CMake 3.15+
- C++17兼容编译器
- OpenGL开发库

### 编译步骤

```bash
cd hy_render
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build .
./demo/hy_render_demo
```

详细文档请参考 [hy_render/README.md](./hy_render/README.md)

## 开发状态

- ✅ 核心架构设计（100%）
- ✅ 接口定义（100%）
- 🚧 OpenGL实现（60%）
- 🚧 OpenGL ES实现（60%）
- 📋 Metal实现（0%）
- 📋 Vulkan实现（0%）
- ✅ Demo示例（80%）
- ✅ 文档（100%）

详细进度请查看 [IMPLEMENTATION_SUMMARY.md](./IMPLEMENTATION_SUMMARY.md)

## 贡献

欢迎提交Issue和Pull Request！

## 许可证

[待定]
