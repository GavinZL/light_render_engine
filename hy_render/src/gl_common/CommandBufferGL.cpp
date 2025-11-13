#include "CommandBufferGL.hpp"
#include <iostream>

namespace hyengine {
namespace render {

CommandBufferGL::CommandBufferGL() {
    setState(CommandBufferState::kReady);
}

CommandBufferGL::~CommandBufferGL() {
    reset();
}

// ==== 命令缓冲区生命周期管理 ====

void CommandBufferGL::begin() {
    if (!isValidStateTransition(mState, CommandBufferState::kRecording)) {
        std::cerr << "[CommandBufferGL] Invalid state transition for begin()" << std::endl;
        return;
    }
    
    setState(CommandBufferState::kRecording);
    mCommands.clear();
    
    std::cout << "[CommandBufferGL] Begin recording commands" << std::endl;
}

void CommandBufferGL::end() {
    if (mState != CommandBufferState::kRecording) {
        std::cerr << "[CommandBufferGL] Cannot end: not in recording state" << std::endl;
        return;
    }
    
    setState(CommandBufferState::kExecutable);
    
    std::cout << "[CommandBufferGL] End recording commands (count=" << mCommands.size() << ")" << std::endl;
}

void CommandBufferGL::reset() {
    mCommands.clear();
    mCurrentBatchName.clear();
    mCurrentBatchStartIndex = 0;
    setState(CommandBufferState::kReady);
    
    std::cout << "[CommandBufferGL] Reset command buffer" << std::endl;
}

void CommandBufferGL::commit(HYRenderPass* renderPass, bool waitForCompletion) {
    if (mState != CommandBufferState::kExecutable) {
        std::cerr << "[CommandBufferGL] Cannot commit: buffer is not executable" << std::endl;
        return;
    }
    
    if (!renderPass) {
        std::cerr << "[CommandBufferGL] Cannot commit: renderPass is null" << std::endl;
        return;
    }
    
    setState(CommandBufferState::kCommitted);
    
    std::cout << "[CommandBufferGL] Executing " << mCommands.size() << " commands..." << std::endl;
    
    // 执行所有记录的命令
    for (const auto& command : mCommands) {
        command->execute(renderPass);
    }
    
    setState(CommandBufferState::kCompleted);
    
    std::cout << "[CommandBufferGL] Command buffer execution completed" << std::endl;
    
    if (waitForCompletion) {
        // OpenGL是同步执行，所以这里不需要额外等待
        std::cout << "[CommandBufferGL] Wait for completion (immediate in OpenGL)" << std::endl;
    }
}

// ==== 渲染命令记录接口 ====

void CommandBufferGL::cmdBeginPass(std::shared_ptr<HYFrameBuffer> framebuffer) {
    if (mState != CommandBufferState::kRecording) {
        std::cerr << "[CommandBufferGL] Cannot record command: not in recording state" << std::endl;
        return;
    }
    
    addCommand<BeginPassCommand>(framebuffer);
}

void CommandBufferGL::cmdEndPass() {
    if (mState != CommandBufferState::kRecording) return;
    addCommand<EndPassCommand>();
}

void CommandBufferGL::cmdSetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) {
    if (mState != CommandBufferState::kRecording) return;
    addCommand<SetViewportCommand>(x, y, width, height);
}

void CommandBufferGL::cmdClearBuffer(uint8_t mask, float r, float g, float b, float a) {
    if (mState != CommandBufferState::kRecording) return;
    addCommand<ClearBufferCommand>(mask, r, g, b, a);
}

void CommandBufferGL::cmdSetVertexBuffer(std::shared_ptr<HYBuffer> buffer, uint32_t binding) {
    if (mState != CommandBufferState::kRecording) return;
    // TODO: 创建SetVertexBufferCommand
    std::cout << "[CommandBufferGL] cmdSetVertexBuffer (binding=" << binding << ")" << std::endl;
}

void CommandBufferGL::cmdSetIndexBuffer(std::shared_ptr<HYBuffer> buffer) {
    if (mState != CommandBufferState::kRecording) return;
    // TODO: 创建SetIndexBufferCommand
    std::cout << "[CommandBufferGL] cmdSetIndexBuffer" << std::endl;
}

void CommandBufferGL::cmdSetShader(std::shared_ptr<HYShader> shader) {
    if (mState != CommandBufferState::kRecording) return;
    // TODO: 创建SetShaderCommand
    std::cout << "[CommandBufferGL] cmdSetShader" << std::endl;
}

void CommandBufferGL::cmdSetPipelineState(std::shared_ptr<HYPipelineState> pso) {
    if (mState != CommandBufferState::kRecording) return;
    // TODO: 创建SetPipelineStateCommand
    std::cout << "[CommandBufferGL] cmdSetPipelineState" << std::endl;
}

void CommandBufferGL::cmdBindTexture(std::shared_ptr<HYTexture> texture, 
                                   uint32_t textureUnit, 
                                   const std::string& samplerName) {
    if (mState != CommandBufferState::kRecording) return;
    addCommand<BindTextureCommand>(texture, textureUnit, samplerName);
}

void CommandBufferGL::cmdBindUniformBuffer(std::shared_ptr<HYBuffer> buffer,
                                         uint32_t bindingPoint,
                                         const std::string& blockName,
                                         uint32_t offset,
                                         uint32_t size) {
    if (mState != CommandBufferState::kRecording) return;
    // TODO: 创建BindUniformBufferCommand
    std::cout << "[CommandBufferGL] cmdBindUniformBuffer (bindingPoint=" << bindingPoint 
              << ", blockName=" << blockName << ")" << std::endl;
}

void CommandBufferGL::cmdDraw(const DrawParams& params) {
    if (mState != CommandBufferState::kRecording) return;
    addCommand<DrawCommand>(params);
}

void CommandBufferGL::cmdDrawInstanced(const DrawParams& params, 
                                     std::shared_ptr<HYBuffer> instanceData) {
    if (mState != CommandBufferState::kRecording) return;
    // TODO: 创建DrawInstancedCommand
    std::cout << "[CommandBufferGL] cmdDrawInstanced (instanceCount=" << params.instanceCount << ")" << std::endl;
}

// ==== 高级命令接口 ====

void CommandBufferGL::cmdBindTextures(const std::vector<TextureBinding>& bindings) {
    if (mState != CommandBufferState::kRecording) return;
    
    for (const auto& binding : bindings) {
        cmdBindTexture(binding.texture, binding.textureUnit, binding.samplerName);
    }
}

void CommandBufferGL::cmdBindUniformBuffers(const std::vector<UniformBufferBinding>& bindings) {
    if (mState != CommandBufferState::kRecording) return;
    
    for (const auto& binding : bindings) {
        cmdBindUniformBuffer(binding.buffer, binding.bindingPoint, binding.blockName, 
                           binding.offset, binding.size);
    }
}

void CommandBufferGL::cmdApplyRenderState(const RenderState& state) {
    if (mState != CommandBufferState::kRecording) return;
    // TODO: 创建ApplyRenderStateCommand
    std::cout << "[CommandBufferGL] cmdApplyRenderState" << std::endl;
}

void CommandBufferGL::cmdCustom(std::function<void(HYRenderPass*)> customFunction) {
    if (mState != CommandBufferState::kRecording) return;
    addCommand<CustomCommand>(customFunction);
}

// ==== 命令批次管理 ====

void CommandBufferGL::beginBatch(const std::string& batchName) {
    if (mState != CommandBufferState::kRecording) return;
    
    mCurrentBatchName = batchName.empty() ? "Batch_" + std::to_string(mCommands.size()) : batchName;
    mCurrentBatchStartIndex = mCommands.size();
    
    std::cout << "[CommandBufferGL] Begin batch: " << mCurrentBatchName << std::endl;
}

void CommandBufferGL::endBatch() {
    if (mState != CommandBufferState::kRecording) return;
    
    size_t batchSize = mCommands.size() - mCurrentBatchStartIndex;
    std::cout << "[CommandBufferGL] End batch: " << mCurrentBatchName 
              << " (commands=" << batchSize << ")" << std::endl;
    
    mCurrentBatchName.clear();
    mCurrentBatchStartIndex = 0;
}

// ==== 状态查询 ====

size_t CommandBufferGL::getCommandCount() const {
    return mCommands.size();
}

std::string CommandBufferGL::getDebugInfo() const {
    std::string info = "[CommandBufferGL Debug Info]\\n";
    info += "State: " + std::to_string(static_cast<int>(mState)) + "\\n";
    info += "Command Count: " + std::to_string(mCommands.size()) + "\\n";
    info += "Current Batch: " + mCurrentBatchName + "\\n";
    return info;
}

ResourceHandle CommandBufferGL::getResourceHandle() const {
    ResourceHandle handle;
    handle.ptrHandle = const_cast<CommandBufferGL*>(this);
    return handle;
}

// ==== 内部方法 ====

bool CommandBufferGL::isValidStateTransition(CommandBufferState from, CommandBufferState to) const {
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

template<typename CommandType, typename... Args>
void CommandBufferGL::addCommand(Args&&... args) {
    mCommands.push_back(std::make_unique<CommandType>(std::forward<Args>(args)...));
    std::cout << "[CommandBufferGL] Added command: " << static_cast<int>(mCommands.back()->type) << std::endl;
}

} // namespace render
} // namespace hyengine