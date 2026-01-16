# 使用 HYRenderPass 和 HYCommandBuffer 完成渲染流程

## 概述

本文档说明如何使用 hy_render 的抽象 API（HYRenderPass 和 HYCommandBuffer）替代直接的 Metal API 调用来完成立方体渲染。

## 完整的渲染流程

### 1. 初始化阶段

```cpp
// 初始化渲染引擎
HYRenderEngine::instance().setRenderAPI(RenderAPI::kMetal);
std::shared_ptr<HYRenderContext> context = HYRenderEngine::instance().getRenderContext();

// 创建资源
BufferDesc vertexDesc;
vertexDesc.bindFlags = BIND_VERTEX_BUFFER;
vertexDesc.usage = BufferUsage::USAGE_STATIC_DRAW;
vertexDesc.size = sizeof(cubeVertices);
vertexDesc.initialData = cubeVertices;
auto vertexBuffer = context->createBuffer(vertexDesc);

BufferDesc indexDesc;
indexDesc.bindFlags = BIND_INDEX_BUFFER;
indexDesc.usage = BufferUsage::USAGE_STATIC_DRAW;
indexDesc.size = sizeof(cubeIndices);
indexDesc.initialData = cubeIndices;
auto indexBuffer = context->createBuffer(indexDesc);

BufferDesc uniformDesc;
uniformDesc.bindFlags = BIND_UNIFORM_BUFFER;
uniformDesc.usage = BufferUsage::USAGE_DYNAMIC_DRAW;
uniformDesc.size = sizeof(Uniforms);
auto uniformBuffer = context->createBuffer(uniformDesc);

// 创建RenderPass
auto renderPass = context->createRenderPass();
```

### 2. 每帧渲染流程

```cpp
void render(float rotationX, float rotationY) {
    // ========== 1. 更新Uniform数据 ==========
    Uniforms uniforms;
    
    // 模型矩阵（旋转）
    Mat4 rotX = Mat4::rotateX(rotationX);
    Mat4 rotY = Mat4::rotateY(rotationY);
    uniforms.modelMatrix = rotY * rotX;
    
    // 视图矩阵
    Vec3 eye(0.0f, 0.0f, 3.0f);
    Vec3 center(0.0f, 0.0f, 0.0f);
    Vec3 up(0.0f, 1.0f, 0.0f);
    uniforms.viewMatrix = Mat4::lookAt(eye, center, up);
    
    // 投影矩阵
    float aspect = (float)width / (float)height;
    uniforms.projectionMatrix = Mat4::perspective(45.0f * M_PI / 180.0f, aspect, 0.1f, 100.0f);
    
    // 更新缓冲区
    uniformBuffer->updateData(&uniforms, sizeof(Uniforms), 0);
    
    
    // ========== 2. 开始渲染通道 ==========
    renderPass->beginPass(nullptr);  // nullptr表示使用默认帧缓冲
    
    
    // ========== 3. 设置渲染状态 ==========
    // 设置视口
    renderPass->setViewport(0, 0, width, height);
    
    // 设置清除颜色
    renderPass->setClearColor(0.2f, 0.3f, 0.4f, 1.0f);
    
    // 清除缓冲（颜色+深度）
    renderPass->clearBuffer(0xFF);
    
    
    // ========== 4. 绑定管线状态和着色器 ==========
    renderPass->setPipelineState(pipelineState);
    renderPass->setShader(shader);
    
    
    // ========== 5. 绑定资源 ==========
    // 绑定顶点缓冲（binding point = 0）
    renderPass->setVertexBuffer(vertexBuffer, 0);
    
    // 绑定Uniform缓冲（binding point = 1）
    renderPass->bindUniformBuffer(uniformBuffer, 1, "Uniforms", 0, sizeof(Uniforms));
    
    // 绑定索引缓冲
    renderPass->setIndexBuffer(indexBuffer);
    
    
    // ========== 6. 执行绘制 ==========
    DrawParams drawParams;
    drawParams.primitiveType = PrimitiveType::PRIMITIVE_TRIANGLES;
    drawParams.indexCount = 36;        // 立方体36个索引
    drawParams.indexStart = 0;
    drawParams.vertexCount = 0;        // 使用索引绘制时vertexCount可为0
    drawParams.vertexStart = 0;
    drawParams.instanceCount = 1;
    
    renderPass->draw(drawParams);
    
    
    // ========== 7. 结束渲染通道 ==========
    renderPass->endPass();
}
```

## 使用 HYCommandBuffer 的高级用法

对于需要记录和重放命令的场景，可以使用 HYCommandBuffer：

```cpp
// 创建命令缓冲区（需要RenderContext支持）
auto commandBuffer = context->createCommandBuffer();

// 开始记录命令
commandBuffer->begin();

// 记录渲染通道开始
commandBuffer->cmdBeginPass(nullptr);

// 记录视口设置
commandBuffer->cmdSetViewport(0, 0, width, height);

// 记录清除操作
commandBuffer->cmdClearBuffer(0xFF, 0.2f, 0.3f, 0.4f, 1.0f);

// 记录设置顶点缓冲
commandBuffer->cmdSetVertexBuffer(vertexBuffer, 0);

// 记录设置着色器
commandBuffer->cmdSetShader(shader);

// 记录设置管线状态
commandBuffer->cmdSetPipelineState(pipelineState);

// 记录绑定Uniform缓冲
commandBuffer->cmdBindUniformBuffer(uniformBuffer, 1, "Uniforms", 0, sizeof(Uniforms));

// 记录绘制命令
DrawParams drawParams;
drawParams.primitiveType = PrimitiveType::PRIMITIVE_TRIANGLES;
drawParams.indexCount = 36;
drawParams.instanceCount = 1;
commandBuffer->cmdDraw(drawParams);

// 记录渲染通道结束
commandBuffer->cmdEndPass();

// 结束命令记录
commandBuffer->end();

// 提交执行
commandBuffer->commit(renderPass.get(), false);  // false表示异步执行
```

## Metal 特定实现注意事项

### RenderPassMtl 实现要点

在 Metal 后端，`RenderPassMtl::beginPass()` 应该：

1. 创建 `MTLCommandBuffer`
2. 创建 `MTLRenderPassDescriptor`
3. 创建 `MTLRenderCommandEncoder`
4. 保存这些对象供后续操作使用

```cpp
void RenderPassMtl::beginPass(std::shared_ptr<HYFrameBuffer> framebuffer) {
    id<MTLDevice> device = (__bridge id<MTLDevice>)mDevice;
    id<MTLCommandQueue> queue = (__bridge id<MTLCommandQueue>)mCommandQueue;
    
    // 创建命令缓冲
    mCommandBuffer = [queue commandBuffer];
    
    // 创建渲染通道描述符
    MTLRenderPassDescriptor* desc = [MTLRenderPassDescriptor renderPassDescriptor];
    
    // 配置颜色附件
    desc.colorAttachments[0].texture = /* 从drawable或framebuffer获取 */;
    desc.colorAttachments[0].loadAction = MTLLoadActionClear;
    desc.colorAttachments[0].clearColor = MTLClearColorMake(r, g, b, a);
    desc.colorAttachments[0].storeAction = MTLStoreActionStore;
    
    // 创建渲染编码器
    mRenderEncoder = [mCommandBuffer renderCommandEncoderWithDescriptor:desc];
    
    mIsInPass = true;
}
```

### 资源绑定实现

```cpp
void RenderPassMtl::setVertexBuffer(std::shared_ptr<HYBuffer> buffer, uint32_t binding) {
    if (!mIsInPass || !buffer) return;
    
    id<MTLRenderCommandEncoder> encoder = (__bridge id<MTLRenderCommandEncoder>)mRenderEncoder;
    id<MTLBuffer> mtlBuffer = (__bridge id<MTLBuffer>)buffer->getResourceHandle().ptrHandle;
    
    [encoder setVertexBuffer:mtlBuffer offset:0 atIndex:binding];
}

void RenderPassMtl::bindUniformBuffer(std::shared_ptr<HYBuffer> buffer,
                                      uint32_t bindingPoint,
                                      const std::string& blockName,
                                      uint32_t offset,
                                      uint32_t size) {
    if (!mIsInPass || !buffer) return;
    
    id<MTLRenderCommandEncoder> encoder = (__bridge id<MTLRenderCommandEncoder>)mRenderEncoder;
    id<MTLBuffer> mtlBuffer = (__bridge id<MTLBuffer>)buffer->getResourceHandle().ptrHandle;
    
    // Metal中Uniform缓冲需要同时绑定到顶点和片段着色器
    [encoder setVertexBuffer:mtlBuffer offset:offset atIndex:bindingPoint];
    [encoder setFragmentBuffer:mtlBuffer offset:offset atIndex:bindingPoint];
}
```

### 绘制实现

```cpp
void RenderPassMtl::draw(const DrawParams& params) {
    if (!mIsInPass) return;
    
    id<MTLRenderCommandEncoder> encoder = (__bridge id<MTLRenderCommandEncoder>)mRenderEncoder;
    
    if (params.indexCount > 0) {
        // 索引绘制
        id<MTLBuffer> indexBuffer = (__bridge id<MTLBuffer>)mIndexBuffer->getResourceHandle().ptrHandle;
        
        MTLPrimitiveType primitiveType = convertPrimitiveType(params.primitiveType);
        MTLIndexType indexType = MTLIndexTypeUInt16;  // 或根据实际情况选择
        
        [encoder drawIndexedPrimitives:primitiveType
                            indexCount:params.indexCount
                             indexType:indexType
                           indexBuffer:indexBuffer
                     indexBufferOffset:params.indexStart * sizeof(uint16_t)];
    } else {
        // 顶点绘制
        MTLPrimitiveType primitiveType = convertPrimitiveType(params.primitiveType);
        
        [encoder drawPrimitives:primitiveType
                    vertexStart:params.vertexStart
                    vertexCount:params.vertexCount];
    }
}
```

### 结束渲染通道

```cpp
void RenderPassMtl::endPass() {
    if (!mIsInPass) return;
    
    id<MTLRenderCommandEncoder> encoder = (__bridge id<MTLRenderCommandEncoder>)mRenderEncoder;
    id<MTLCommandBuffer> commandBuffer = (__bridge id<MTLCommandBuffer>)mCommandBuffer;
    
    // 结束编码
    [encoder endEncoding];
    
    // 提交命令缓冲
    // 注意：drawable的present需要在外部处理，或在这里根据情况处理
    [commandBuffer commit];
    
    mIsInPass = false;
    mRenderEncoder = nil;
    mCommandBuffer = nil;
}
```

## 优势

使用 HYRenderPass 和 HYCommandBuffer 抽象层的优势：

1. **跨平台一致性**：相同的代码可以在 OpenGL、Metal、Vulkan 等不同后端运行
2. **代码简洁**：隐藏了平台特定的复杂性
3. **易于维护**：渲染逻辑与平台实现分离
4. **命令记录**：支持记录和重放渲染命令，便于优化
5. **状态管理**：自动管理渲染状态，减少错误

## 与原生 Metal API 的对比

### 原生 Metal API
```objc
id<MTLCommandBuffer> commandBuffer = [queue commandBuffer];
MTLRenderPassDescriptor* desc = [MTLRenderPassDescriptor renderPassDescriptor];
// ... 配置描述符 ...
id<MTLRenderCommandEncoder> encoder = [commandBuffer renderCommandEncoderWithDescriptor:desc];

[encoder setRenderPipelineState:pipelineState];
[encoder setVertexBuffer:vertexBuffer offset:0 atIndex:0];
[encoder setVertexBuffer:uniformBuffer offset:0 atIndex:1];
[encoder drawIndexedPrimitives:MTLPrimitiveTypeTriangle
                    indexCount:36
                     indexType:MTLIndexTypeUInt16
                   indexBuffer:indexBuffer
             indexBufferOffset:0];

[encoder endEncoding];
[commandBuffer commit];
```

### HYRenderPass 抽象API
```cpp
renderPass->beginPass(nullptr);
renderPass->setViewport(0, 0, width, height);
renderPass->setClearColor(0.2f, 0.3f, 0.4f, 1.0f);
renderPass->setPipelineState(pipelineState);
renderPass->setVertexBuffer(vertexBuffer, 0);
renderPass->bindUniformBuffer(uniformBuffer, 1, "Uniforms");
renderPass->setIndexBuffer(indexBuffer);

DrawParams params;
params.primitiveType = PrimitiveType::PRIMITIVE_TRIANGLES;
params.indexCount = 36;
renderPass->draw(params);

renderPass->endPass();
```

## 总结

HYRenderPass 和 HYCommandBuffer 提供了统一的渲染接口，使得渲染代码可以在不同的图形API之间无缝切换。通过合理实现Metal后端，可以获得原生Metal的性能，同时保持代码的可维护性和可移植性。
