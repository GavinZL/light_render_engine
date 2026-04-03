#include "CommandBufferMtl.hpp"
#include <iostream>

namespace hyengine {
namespace render {

CommandBufferMtl::CommandBufferMtl() {
    setState(CommandBufferState::kReady);
    initializeMetal();
}

CommandBufferMtl::~CommandBufferMtl() {
    cleanupMetal();
}

// ==== 命令缓冲区生命周期管理 ====

void CommandBufferMtl::begin() {
    if (!isValidStateTransition(mState, CommandBufferState::kRecording)) {
        std::cerr << "[CommandBufferMtl] Invalid state transition for begin()" << std::endl;
        return;
    }
    
    setState(CommandBufferState::kRecording);
    mCommands.clear();
    
    // 创建新的Metal命令缓冲区
    if (mUseNativeCommandBuffer && mMtlCommandQueue) {
        // TODO: 实际实现中需要调用Metal API
        // mMtlCommandBuffer = [mMtlCommandQueue commandBuffer];
        std::cout << "[CommandBufferMtl] Created native Metal command buffer" << std::endl;
    }
    
    std::cout << "[CommandBufferMtl] Begin recording commands" << std::endl;
}

void CommandBufferMtl::end() {
    if (mState != CommandBufferState::kRecording) {
        std::cerr << "[CommandBufferMtl] Cannot end: not in recording state" << std::endl;
        return;
    }
    
    setState(CommandBufferState::kExecutable);
    
    std::cout << "[CommandBufferMtl] End recording commands (count=" << mCommands.size() << ")" << std::endl;
}

void CommandBufferMtl::reset() {
    mCommands.clear();
    mCurrentBatchName.clear();
    mCurrentBatchStartIndex = 0;
    
    // 清理Metal命令缓冲区
    if (mMtlCommandBuffer) {
        // TODO: 释放Metal命令缓冲区
        mMtlCommandBuffer = nullptr;
    }
    
    setState(CommandBufferState::kReady);
    
    std::cout << "[CommandBufferMtl] Reset command buffer" << std::endl;
}

void CommandBufferMtl::commit(HYRenderPass* renderPass, bool waitForCompletion) {
    if (mState != CommandBufferState::kExecutable) {
        std::cerr << "[CommandBufferMtl] Cannot commit: buffer is not executable" << std::endl;
        return;
    }
    
    if (!renderPass) {
        std::cerr << "[CommandBufferMtl] Cannot commit: renderPass is null" << std::endl;
        return;
    }
    
    setState(CommandBufferState::kCommitted);
    
    if (mUseNativeCommandBuffer && mMtlCommandBuffer) {
        // Metal原生提交模式
        std::cout << "[CommandBufferMtl] Committing to Metal GPU..." << std::endl;
        
        // TODO: 实际实现中需要调用Metal API
        // [mMtlCommandBuffer commit];
        
        if (waitForCompletion) {
            // TODO: 等待Metal命令缓冲区完成
            // [mMtlCommandBuffer waitUntilCompleted];
            std::cout << "[CommandBufferMtl] Waiting for Metal completion..." << std::endl;
        }
    } else {
        // 模拟模式：执行记录的命令
        std::cout << "[CommandBufferMtl] Executing " << mCommands.size() << " commands..." << std::endl;
        
        for (const auto& command : mCommands) {
            command->execute(renderPass);
        }
    }
    
    setState(CommandBufferState::kCompleted);
    
    std::cout << "[CommandBufferMtl] Command buffer execution completed" << std::endl;
}

// ==== 渲染命令记录接口 ====

void CommandBufferMtl::cmdBeginPass(std::shared_ptr<HYFrameBuffer> framebuffer) {
    if (mState != CommandBufferState::kRecording) {
        std::cerr << "[CommandBufferMtl] Cannot record command: not in recording state" << std::endl;
        return;
    }
    
    if (mUseNativeCommandBuffer) {
        // TODO: 创建Metal渲染编码器
        // MTLRenderPassDescriptor* descriptor = [MTLRenderPassDescriptor renderPassDescriptor];
        // id<MTLRenderCommandEncoder> encoder = [mMtlCommandBuffer renderCommandEncoderWithDescriptor:descriptor];
        std::cout << "[CommandBufferMtl] Begin Metal render pass" << std::endl;
    }
    
    addCommand<BeginPassCommand>(framebuffer);
}

void CommandBufferMtl::cmdEndPass() {
    if (mState != CommandBufferState::kRecording) return;
    
    if (mUseNativeCommandBuffer) {
        // TODO: 结束Metal渲染编码器
        // [renderEncoder endEncoding];
        std::cout << "[CommandBufferMtl] End Metal render pass" << std::endl;
    }
    
    addCommand<EndPassCommand>();
}

void CommandBufferMtl::cmdSetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) {
    if (mState != CommandBufferState::kRecording) return;
    
    if (mUseNativeCommandBuffer) {
        // TODO: 设置Metal视口
        // MTLViewport viewport = {(double)x, (double)y, (double)width, (double)height, 0.0, 1.0};
        // [renderEncoder setViewport:viewport];
        std::cout << "[CommandBufferMtl] Set Metal viewport" << std::endl;
    }
    
    addCommand<SetViewportCommand>(x, y, width, height);
}

void CommandBufferMtl::cmdClearBuffer(uint8_t mask, float r, float g, float b, float a) {
    if (mState != CommandBufferState::kRecording) return;
    
    // Metal的清除操作通常在RenderPassDescriptor中设置
    std::cout << "[CommandBufferMtl] Record clear buffer (Metal uses RenderPassDescriptor)" << std::endl;
    
    addCommand<ClearBufferCommand>(mask, r, g, b, a);
}

void CommandBufferMtl::cmdSetVertexBuffer(std::shared_ptr<HYBuffer> buffer, uint32_t binding) {
    if (mState != CommandBufferState::kRecording) return;
    
    if (mUseNativeCommandBuffer && buffer) {
        // TODO: 绑定Metal顶点缓冲区
        // id<MTLBuffer> mtlBuffer = (__bridge id<MTLBuffer>)buffer->getResourceHandle().ptrHandle;
        // [renderEncoder setVertexBuffer:mtlBuffer offset:0 atIndex:binding];
        std::cout << "[CommandBufferMtl] Set Metal vertex buffer (binding=" << binding << ")" << std::endl;
    }
}

void CommandBufferMtl::cmdSetIndexBuffer(std::shared_ptr<HYBuffer> buffer) {
    if (mState != CommandBufferState::kRecording) return;
    
    if (mUseNativeCommandBuffer && buffer) {
        // TODO: Metal索引缓冲区在绘制时使用
        std::cout << "[CommandBufferMtl] Record Metal index buffer" << std::endl;
    }
}

void CommandBufferMtl::cmdSetShader(std::shared_ptr<HYShader> shader) {
    if (mState != CommandBufferState::kRecording) return;
    
    if (mUseNativeCommandBuffer && shader) {
        // TODO: 设置Metal渲染管线状态
        // id<MTLRenderPipelineState> pipelineState = (__bridge id<MTLRenderPipelineState>)shader->getResourceHandle().ptrHandle;
        // [renderEncoder setRenderPipelineState:pipelineState];
        std::cout << "[CommandBufferMtl] Set Metal render pipeline state" << std::endl;
    }
}

void CommandBufferMtl::cmdSetPipelineState(std::shared_ptr<HYPipelineState> pso) {
    if (mState != CommandBufferState::kRecording) return;
    
    if (mUseNativeCommandBuffer && pso) {
        // TODO: 设置Metal管线状态
        std::cout << "[CommandBufferMtl] Set Metal pipeline state" << std::endl;
    }
}

void CommandBufferMtl::cmdBindTexture(std::shared_ptr<HYTexture> texture, 
                                     uint32_t textureUnit, 
                                     const std::string& samplerName) {
    if (mState != CommandBufferState::kRecording) return;
    
    if (mUseNativeCommandBuffer && texture) {
        // TODO: 绑定Metal纹理
        // id<MTLTexture> mtlTexture = (__bridge id<MTLTexture>)texture->getResourceHandle().ptrHandle;
        // [renderEncoder setFragmentTexture:mtlTexture atIndex:textureUnit];
        std::cout << "[CommandBufferMtl] Bind Metal texture (unit=" << textureUnit << ")" << std::endl;
    }
    
    addCommand<BindTextureCommand>(texture, textureUnit, samplerName);
}

void CommandBufferMtl::cmdBindUniformBuffer(std::shared_ptr<HYBuffer> buffer,
                                           uint32_t bindingPoint,
                                           const std::string& blockName,
                                           uint32_t offset,
                                           uint32_t size) {
    if (mState != CommandBufferState::kRecording) return;
    
    if (mUseNativeCommandBuffer && buffer) {
        // TODO: 绑定Metal uniform缓冲区
        // id<MTLBuffer> mtlBuffer = (__bridge id<MTLBuffer>)buffer->getResourceHandle().ptrHandle;
        // [renderEncoder setVertexBuffer:mtlBuffer offset:offset atIndex:bindingPoint];
        // [renderEncoder setFragmentBuffer:mtlBuffer offset:offset atIndex:bindingPoint];
        std::cout << "[CommandBufferMtl] Bind Metal uniform buffer (binding=" << bindingPoint << ")" << std::endl;
    }
}

void CommandBufferMtl::cmdDraw(const DrawParams& params) {
    if (mState != CommandBufferState::kRecording) return;
    
    if (mUseNativeCommandBuffer) {
        // TODO: Metal绘制调用
        if (params.indexCount > 0) {
            // [renderEncoder drawIndexedPrimitives:...];
            std::cout << "[CommandBufferMtl] Metal drawIndexedPrimitives" << std::endl;
        } else {
            // [renderEncoder drawPrimitives:...];
            std::cout << "[CommandBufferMtl] Metal drawPrimitives" << std::endl;
        }
    }
    
    addCommand<DrawCommand>(params);
}

void CommandBufferMtl::cmdDrawInstanced(const DrawParams& params, 
                                       std::shared_ptr<HYBuffer> instanceData) {
    if (mState != CommandBufferState::kRecording) return;
    
    if (mUseNativeCommandBuffer) {
        // TODO: Metal实例化绘制
        std::cout << "[CommandBufferMtl] Metal instanced draw (instances=" << params.instanceCount << ")" << std::endl;
    }
}

// ==== 高级命令接口 ====

void CommandBufferMtl::cmdBindTextures(const std::vector<TextureBinding>& bindings) {
    if (mState != CommandBufferState::kRecording) return;
    
    for (const auto& binding : bindings) {
        cmdBindTexture(binding.texture, binding.textureUnit, binding.samplerName);
    }
}

void CommandBufferMtl::cmdBindUniformBuffers(const std::vector<UniformBufferBinding>& bindings) {
    if (mState != CommandBufferState::kRecording) return;
    
    for (const auto& binding : bindings) {
        cmdBindUniformBuffer(binding.buffer, binding.bindingPoint, binding.blockName, 
                           binding.offset, binding.size);
    }
}

void CommandBufferMtl::cmdApplyRenderState(const RenderState& state) {
    if (mState != CommandBufferState::kRecording) return;
    std::cout << "[CommandBufferMtl] cmdApplyRenderState" << std::endl;
}

void CommandBufferMtl::cmdCustom(std::function<void(HYRenderPass*)> customFunction) {
    if (mState != CommandBufferState::kRecording) return;
    addCommand<CustomCommand>(customFunction);
}

// ==== 命令批次管理 ====

void CommandBufferMtl::beginBatch(const std::string& batchName) {
    if (mState != CommandBufferState::kRecording) return;
    
    mCurrentBatchName = batchName.empty() ? "Batch_" + std::to_string(mCommands.size()) : batchName;
    mCurrentBatchStartIndex = mCommands.size();
    
    std::cout << "[CommandBufferMtl] Begin batch: " << mCurrentBatchName << std::endl;
}

void CommandBufferMtl::endBatch() {
    if (mState != CommandBufferState::kRecording) return;
    
    size_t batchSize = mCommands.size() - mCurrentBatchStartIndex;
    std::cout << "[CommandBufferMtl] End batch: " << mCurrentBatchName 
              << " (commands=" << batchSize << ")" << std::endl;
    
    mCurrentBatchName.clear();
    mCurrentBatchStartIndex = 0;
}

// ==== 状态查询 ====

size_t CommandBufferMtl::getCommandCount() const {
    return mCommands.size();
}

std::string CommandBufferMtl::getDebugInfo() const {
    std::string info = "[CommandBufferMtl Debug Info]\\n";
    info += "State: " + std::to_string(static_cast<int>(mState)) + "\\n";
    info += "Command Count: " + std::to_string(mCommands.size()) + "\\n";
    info += "Native Command Buffer: " + std::string(mUseNativeCommandBuffer ? "YES" : "NO") + "\\n";
    info += "Current Batch: " + mCurrentBatchName + "\\n";
    return info;
}

ResourceHandle CommandBufferMtl::getResourceHandle() const {
    ResourceHandle handle;
    handle.ptrHandle = mMtlCommandBuffer ? mMtlCommandBuffer : const_cast<CommandBufferMtl*>(this);
    return handle;
}

// ==== 内部方法 ====

bool CommandBufferMtl::isValidStateTransition(CommandBufferState from, CommandBufferState to) const {
    // Metal支持更复杂的状态转换
    switch (from) {
        case CommandBufferState::kReady:
            return to == CommandBufferState::kRecording;
        case CommandBufferState::kRecording:
            return to == CommandBufferState::kExecutable || to == CommandBufferState::kReady;
        case CommandBufferState::kExecutable:
            return to == CommandBufferState::kCommitted || to == CommandBufferState::kReady;
        case CommandBufferState::kCommitted:
            return to == CommandBufferState::kCompleted || to == CommandBufferState::kError;
        case CommandBufferState::kCompleted:
            return to == CommandBufferState::kReady;
        case CommandBufferState::kError:
            return to == CommandBufferState::kReady;
        default:
            return false;
    }
}

void CommandBufferMtl::initializeMetal() {
    // TODO: 实际实现中需要获取Metal设备和命令队列
    // id<MTLDevice> device = MTLCreateSystemDefaultDevice();
    // mMtlCommandQueue = [device newCommandQueue];
    
    std::cout << "[CommandBufferMtl] Initialize Metal resources (simulated)" << std::endl;
}

void CommandBufferMtl::cleanupMetal() {
    if (mMtlCommandBuffer) {
        // TODO: 清理Metal命令缓冲区
        mMtlCommandBuffer = nullptr;
    }
    
    if (mMtlCommandQueue) {
        // TODO: 清理Metal命令队列
        mMtlCommandQueue = nullptr;
    }
    
    std::cout << "[CommandBufferMtl] Cleanup Metal resources" << std::endl;
}

template<typename CommandType, typename... Args>
void CommandBufferMtl::addCommand(Args&&... args) {
    mCommands.push_back(std::make_unique<CommandType>(std::forward<Args>(args)...));
    std::cout << "[CommandBufferMtl] Added command: " << static_cast<int>(mCommands.back()->type) << std::endl;
}

} // namespace render
} // namespace hyengine