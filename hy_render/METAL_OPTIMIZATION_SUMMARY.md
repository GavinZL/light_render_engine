# Metal渲染流程优化总结

## 已完成工作

### 1. 完善 Metal 着色器实现 (ShaderMtl)

**文件**: `src/metal/resources/ShaderMtl.mm`

**改进内容**:
- ✅ 实现真实的 Metal 着色器编译功能
- ✅ 支持从 MSL 源代码编译
- ✅ 正确使用 `ShaderCodeDesc` 结构处理顶点和片段着色器
- ✅ 添加析构函数释放 Metal 资源
- ✅ 使用 `__bridge_retained` 管理对象生命周期

**关键代码**:
```cpp
bool ShaderMtl::compile() {
    // 从 ShaderDesc 获取源代码
    NSString* source = [NSString stringWithUTF8String:firstShader.shaderCode];
    
    // 编译 Metal Library
    library = [device newLibraryWithSource:source options:nil error:&error];
    
    // 获取着色器函数
    vertexFunc = [library newFunctionWithName:vertexFuncName];
    fragmentFunc = [library newFunctionWithName:fragmentFuncName];
    
    // 保存句柄
    mLibrary = (__bridge_retained void*)library;
    mVertexFunction = (__bridge_retained void*)vertexFunc;
    mFragmentFunction = (__bridge_retained void*)fragmentFunc;
}
```

### 2. 完善 Metal 管线状态实现 (PipelineStateMtl)

**文件**: `src/metal/resources/PipelineStateMtl.mm`

**改进内容**:
- ✅ 新增 `build()` 方法创建真实的 MTLRenderPipelineState
- ✅ 创建 MTLDepthStencilState 对象
- ✅ 正确配置颜色附件、深度附件、混合状态
- ✅ 添加析构函数释放资源

**关键代码**:
```cpp
bool PipelineStateMtl::build(void* shader, void* vertexDesc) {
    // 创建管线描述符
    MTLRenderPipelineDescriptor* pipelineDesc = [[MTLRenderPipelineDescriptor alloc] init];
    pipelineDesc.vertexFunction = vertexFunc;
    pipelineDesc.fragmentFunction = fragmentFunc;
    pipelineDesc.vertexDescriptor = vertexDescriptor;
    pipelineDesc.colorAttachments[0].pixelFormat = MTLPixelFormatBGRA8Unorm;
    
    // 创建管线状态
    id<MTLRenderPipelineState> pipelineState = 
        [device newRenderPipelineStateWithDescriptor:pipelineDesc error:&error];
    
    // 创建深度模板状态
    MTLDepthStencilDescriptor* depthDesc = [[MTLDepthStencilDescriptor alloc] init];
    depthDesc.depthCompareFunction = MTLCompareFunctionLess;
    depthDesc.depthWriteEnabled = YES;
    id<MTLDepthStencilState> depthState = 
        [device newDepthStencilStateWithDescriptor:depthDesc];
}
```

### 3. 优化 Metal 缓冲区实现 (BufferMtl)

**文件**: `src/metal/resources/BufferMtl.mm`

**改进内容**:
- ✅ 使用真实的 Metal API 创建缓冲区
- ✅ 正确处理 Shared/Managed 存储模式
- ✅ 实现数据更新和映射功能

### 4. HYRenderPass 和 HYCommandBuffer 使用文档

**文件**: `demo/HY_RENDER_PASS_USAGE.md`

创建了完整的使用文档，说明如何使用抽象API完成渲染流程。

## 使用 HYRenderPass 的渲染流程

### 简化版本（当前可用）

```cpp
// 1. 初始化
HYRenderEngine::instance().setRenderAPI(RenderAPI::kMetal);
auto context = HYRenderEngine::instance().getRenderContext();
auto renderPass = context->createRenderPass();

// 2. 每帧渲染
void render() {
    // 更新uniform数据
    uniformBuffer->updateData(&uniforms, sizeof(Uniforms), 0);
    
    // 开始渲染通道
    renderPass->beginPass(nullptr);
    
    // 设置视口和清除
    renderPass->setViewport(0, 0, width, height);
    renderPass->setClearColor(0.2f, 0.3f, 0.4f, 1.0f);
    renderPass->clearBuffer(0xFF);
    
    // 绑定资源
    renderPass->setPipelineState(pipelineState);
    renderPass->setShader(shader);
    renderPass->setVertexBuffer(vertexBuffer, 0);
    renderPass->bindUniformBuffer(uniformBuffer, 1, "Uniforms");
    renderPass->setIndexBuffer(indexBuffer);
    
    // 执行绘制
    DrawParams params;
    params.primitiveType = PrimitiveType::PRIMITIVE_TRIANGLES;
    params.indexCount = 36;
    renderPass->draw(params);
    
    // 结束渲染通道
    renderPass->endPass();
}
```

## RenderPassMtl 需要进一步完善的部分

### 1. beginPass 实现

```cpp
void RenderPassMtl::beginPass(std::shared_ptr<HYFrameBuffer> framebuffer) {
    // TODO: 创建Metal命令缓冲和渲染编码器
    id<MTLCommandQueue> queue = (__bridge id<MTLCommandQueue>)mCommandQueue;
    mCommandBuffer = [queue commandBuffer];
    
    // 配置渲染通道描述符
    MTLRenderPassDescriptor* desc = [MTLRenderPassDescriptor renderPassDescriptor];
    desc.colorAttachments[0].texture = /* 从drawable获取 */;
    desc.colorAttachments[0].loadAction = MTLLoadActionClear;
    
    // 创建渲染编码器
    mRenderEncoder = [mCommandBuffer renderCommandEncoderWithDescriptor:desc];
}
```

### 2. 资源绑定实现

```cpp
void RenderPassMtl::setVertexBuffer(std::shared_ptr<HYBuffer> buffer, uint32_t binding) {
    id<MTLRenderCommandEncoder> encoder = (__bridge id<MTLRenderCommandEncoder>)mRenderEncoder;
    id<MTLBuffer> mtlBuffer = (__bridge id<MTLBuffer>)buffer->getResourceHandle().ptrHandle;
    [encoder setVertexBuffer:mtlBuffer offset:0 atIndex:binding];
}

void RenderPassMtl::bindUniformBuffer(std::shared_ptr<HYBuffer> buffer,
                                      uint32_t bindingPoint,
                                      const std::string& blockName,
                                      uint32_t offset,
                                      uint32_t size) {
    id<MTLRenderCommandEncoder> encoder = (__bridge id<MTLRenderCommandEncoder>)mRenderEncoder;
    id<MTLBuffer> mtlBuffer = (__bridge id<MTLBuffer>)buffer->getResourceHandle().ptrHandle;
    
    // Uniform缓冲需要绑定到顶点和片段着色器
    [encoder setVertexBuffer:mtlBuffer offset:offset atIndex:bindingPoint];
    [encoder setFragmentBuffer:mtlBuffer offset:offset atIndex:bindingPoint];
}
```

### 3. 绘制实现

```cpp
void RenderPassMtl::draw(const DrawParams& params) {
    id<MTLRenderCommandEncoder> encoder = (__bridge id<MTLRenderCommandEncoder>)mRenderEncoder;
    id<MTLBuffer> indexBuffer = (__bridge id<MTLBuffer>)mIndexBuffer->getResourceHandle().ptrHandle;
    
    if (params.indexCount > 0) {
        [encoder drawIndexedPrimitives:MTLPrimitiveTypeTriangle
                            indexCount:params.indexCount
                             indexType:MTLIndexTypeUInt16
                           indexBuffer:indexBuffer
                     indexBufferOffset:params.indexStart * sizeof(uint16_t)];
    } else {
        [encoder drawPrimitives:MTLPrimitiveTypeTriangle
                    vertexStart:params.vertexStart
                    vertexCount:params.vertexCount];
    }
}
```

### 4. endPass 实现

```cpp
void RenderPassMtl::endPass() {
    id<MTLRenderCommandEncoder> encoder = (__bridge id<MTLRenderCommandEncoder>)mRenderEncoder;
    id<MTLCommandBuffer> commandBuffer = (__bridge id<MTLCommandBuffer>)mCommandBuffer;
    
    [encoder endEncoding];
    [commandBuffer commit];
    
    mIsInPass = false;
}
```

## 与 CAMetalLayer 的集成

要在 macOS 窗口应用中使用 HYRenderPass，需要：

1. **在 beginPass 中获取 drawable**:
```cpp
void RenderPassMtl::beginPass(std::shared_ptr<HYFrameBuffer> framebuffer) {
    // 从外部传入的metalLayer获取drawable
    id<CAMetalDrawable> drawable = [mMetalLayer nextDrawable];
    
    MTLRenderPassDescriptor* desc = [MTLRenderPassDescriptor renderPassDescriptor];
    desc.colorAttachments[0].texture = drawable.texture;
    
    // ... 创建渲染编码器
}
```

2. **在 endPass 中 present drawable**:
```cpp
void RenderPassMtl::endPass() {
    [mRenderEncoder endEncoding];
    [mCommandBuffer presentDrawable:mDrawable];
    [mCommandBuffer commit];
}
```

## 当前 demo 的工作状态

### metal_cube_demo.mm
- ✅ 使用原生 Metal API
- ✅ 正常工作，可以渲染立方体
- ✅ 支持鼠标拖动旋转

### metal_cube_refactored.mm
- ⚠️  使用 hy_render 抽象API（部分实现）
- ⚠️  存在头文件依赖问题
- 📝 作为参考示例，说明如何使用抽象API

## 下一步工作建议

1. **完善 RenderPassMtl 实现**
   - 实现真实的 Metal 命令编码器
   - 处理 drawable 的获取和 present
   - 实现所有资源绑定方法

2. **集成 CAMetalLayer**
   - 在 RenderContextMtl 中保存 metalLayer 引用
   - 在 RenderPass 中访问 drawable

3. **实现 CommandBufferMtl**
   - 支持命令记录和重放
   - 优化命令提交流程

4. **添加深度缓冲支持**
   - 创建深度纹理
   - 配置深度附件

5. **性能优化**
   - 实现命令缓冲池
   - 减少状态切换
   - 批量绘制支持

## 文件清单

```
hy_render/
├── src/metal/resources/
│   ├── ShaderMtl.mm              ✅ 已完善
│   ├── ShaderMtl.hpp             ✅ 已完善
│   ├── PipelineStateMtl.mm       ✅ 已完善
│   ├── PipelineStateMtl.hpp      ✅ 已完善
│   ├── BufferMtl.mm              ✅ 已完善
│   ├── RenderPassMtl.mm          ⚠️  需进一步完善
│   └── CommandBufferMtl.mm       ⚠️  需进一步完善
│
├── demo/
│   ├── metal_cube_demo.mm        ✅ 工作正常
│   ├── metal_cube_refactored.mm  📝 参考示例
│   ├── HY_RENDER_PASS_USAGE.md   📚 使用文档
│   └── shaders/
│       └── cube.metal            ✅ Metal着色器
│
└── include/hy_render/
    └── resources/
        ├── HYRenderPass.hpp      📖 接口定义
        └── HYCommandBuffer.hpp   📖 接口定义
```

## 总结

本次优化完成了 Metal 渲染管线的核心部分：

1. ✅ **ShaderMtl**: 真实编译和管理 Metal 着色器
2. ✅ **PipelineStateMtl**: 创建真实的渲染管线状态
3. ✅ **BufferMtl**: 管理 Metal 缓冲区资源
4. 📝 **使用文档**: 详细说明如何使用抽象API

**当前状态**:
- `metal_cube_demo.mm` 使用原生API，**完全可用** ✅
- Metal 底层实现（Shader、Pipeline、Buffer）已**完善** ✅
- RenderPass 抽象层**部分实现**，可作为参考 📝

**后续工作**:
- 完善 RenderPassMtl 的 Metal 实现
- 集成 CAMetalLayer 到渲染流程
- 实现完整的 CommandBuffer 支持
