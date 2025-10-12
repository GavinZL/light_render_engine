#pragma once

#include "../HYRenderDef.hpp"
#include <memory>
#include <vector>
#include <functional>

namespace hyengine {
namespace render {

class HYFrameBuffer;
class HYBuffer;
class HYShader;
class HYPipelineState;
class HYTexture;
class HYRenderPass;

/**
 * @brief 命令缓冲状态枚举
 */
enum class CommandBufferState : uint8_t {
    kReady = 0,      // 就绪状态，可以开始记录命令
    kRecording,      // 正在记录命令
    kExecutable,     // 已完成记录，可以执行
    kCommitted,      // 已提交执行
    kCompleted,      // 执行完成
    kError           // 错误状态
};

/**
 * @brief 渲染命令类型枚举
 */
enum class RenderCommandType : uint8_t {
    kBeginPass = 0,
    kEndPass,
    kSetViewport,
    kSetScissor,
    kClearBuffer,
    kSetVertexBuffer,
    kSetIndexBuffer,
    kSetShader,
    kSetPipelineState,
    kBindTexture,
    kBindUniformBuffer,
    kDraw,
    kDrawInstanced,
    kDrawElements,
    kDrawElementsInstanced,
    kSetRenderState,
    kCustomCommand
};

/**
 * @brief 渲染命令基类
 */
struct RenderCommand {
    RenderCommandType type;
    
    RenderCommand(RenderCommandType commandType) : type(commandType) {}
    virtual ~RenderCommand() = default;
    
    /**
     * @brief 在指定RenderPass上执行命令
     */
    virtual void execute(HYRenderPass* renderPass) = 0;
};

/**
 * @brief 具体命令实现（具体实现在.cpp文件中）
 */
struct BeginPassCommand : public RenderCommand {
    std::shared_ptr<HYFrameBuffer> framebuffer;
    
    BeginPassCommand(std::shared_ptr<HYFrameBuffer> fb) 
        : RenderCommand(RenderCommandType::kBeginPass), framebuffer(fb) {}
    
    void execute(HYRenderPass* renderPass) override;
};

struct EndPassCommand : public RenderCommand {
    EndPassCommand() : RenderCommand(RenderCommandType::kEndPass) {}
    
    void execute(HYRenderPass* renderPass) override;
};

struct SetViewportCommand : public RenderCommand {
    uint32_t x, y, width, height;
    
    SetViewportCommand(uint32_t x, uint32_t y, uint32_t w, uint32_t h)
        : RenderCommand(RenderCommandType::kSetViewport), x(x), y(y), width(w), height(h) {}
    
    void execute(HYRenderPass* renderPass) override;
};

struct ClearBufferCommand : public RenderCommand {
    uint8_t mask;
    float clearColor[4];
    
    ClearBufferCommand(uint8_t clearMask, float r = 0.0f, float g = 0.0f, float b = 0.0f, float a = 1.0f)
        : RenderCommand(RenderCommandType::kClearBuffer), mask(clearMask) {
        clearColor[0] = r; clearColor[1] = g; clearColor[2] = b; clearColor[3] = a;
    }
    
    void execute(HYRenderPass* renderPass) override;
};

struct DrawCommand : public RenderCommand {
    DrawParams params;
    
    DrawCommand(const DrawParams& drawParams)
        : RenderCommand(RenderCommandType::kDraw), params(drawParams) {}
    
    void execute(HYRenderPass* renderPass) override;
};

struct BindTextureCommand : public RenderCommand {
    std::shared_ptr<HYTexture> texture;
    uint32_t textureUnit;
    std::string samplerName;
    
    BindTextureCommand(std::shared_ptr<HYTexture> tex, uint32_t unit, const std::string& name)
        : RenderCommand(RenderCommandType::kBindTexture), texture(tex), textureUnit(unit), samplerName(name) {}
    
    void execute(HYRenderPass* renderPass) override;
};

struct CustomCommand : public RenderCommand {
    std::function<void(HYRenderPass*)> customFunction;
    
    CustomCommand(std::function<void(HYRenderPass*)> func)
        : RenderCommand(RenderCommandType::kCustomCommand), customFunction(func) {}
    
    void execute(HYRenderPass* renderPass) override;
};

/**
 * @brief 渲染命令缓冲区抽象
 * 
 * 设计目标：
 * 1. 统一OpenGL即时渲染和Metal命令编码器模式
 * 2. 支持命令记录、重放和优化
 * 3. 提供高级命令编组和批处理能力
 * 4. 支持多线程命令记录（为未来扩展）
 */
class HYCommandBuffer : public HYResource {
public:
    HYCommandBuffer() : HYResource(ResourceType::kCommandBuffer), mState(CommandBufferState::kReady) {}
    virtual ~HYCommandBuffer() = default;

    // ==== 命令缓冲区生命周期管理 ====
    /**
     * @brief 开始记录命令
     */
    virtual void begin() = 0;

    /**
     * @brief 完成命令记录
     */
    virtual void end() = 0;

    /**
     * @brief 重置命令缓冲区到初始状态
     */
    virtual void reset() = 0;

    /**
     * @brief 提交命令缓冲区执行
     * @param renderPass 执行命令的RenderPass
     * @param waitForCompletion 是否等待执行完成
     */
    virtual void commit(HYRenderPass* renderPass, bool waitForCompletion = false) = 0;

    // ==== 渲染命令记录接口 ====
    /**
     * @brief 记录开始渲染通道命令
     */
    virtual void cmdBeginPass(std::shared_ptr<HYFrameBuffer> framebuffer = nullptr) = 0;

    /**
     * @brief 记录结束渲染通道命令
     */
    virtual void cmdEndPass() = 0;

    /**
     * @brief 记录设置视口命令
     */
    virtual void cmdSetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) = 0;

    /**
     * @brief 记录清除缓冲区命令
     */
    virtual void cmdClearBuffer(uint8_t mask, float r = 0.0f, float g = 0.0f, float b = 0.0f, float a = 1.0f) = 0;

    /**
     * @brief 记录设置顶点缓冲命令
     */
    virtual void cmdSetVertexBuffer(std::shared_ptr<HYBuffer> buffer, uint32_t binding = 0) = 0;

    /**
     * @brief 记录设置索引缓冲命令
     */
    virtual void cmdSetIndexBuffer(std::shared_ptr<HYBuffer> buffer) = 0;

    /**
     * @brief 记录设置着色器命令
     */
    virtual void cmdSetShader(std::shared_ptr<HYShader> shader) = 0;

    /**
     * @brief 记录设置管线状态命令
     */
    virtual void cmdSetPipelineState(std::shared_ptr<HYPipelineState> pso) = 0;

    /**
     * @brief 记录绑定纹理命令
     */
    virtual void cmdBindTexture(std::shared_ptr<HYTexture> texture, 
                               uint32_t textureUnit, 
                               const std::string& samplerName) = 0;

    /**
     * @brief 记录绑定UBO命令
     */
    virtual void cmdBindUniformBuffer(std::shared_ptr<HYBuffer> buffer,
                                     uint32_t bindingPoint,
                                     const std::string& blockName,
                                     uint32_t offset = 0,
                                     uint32_t size = 0) = 0;

    /**
     * @brief 记录绘制命令
     */
    virtual void cmdDraw(const DrawParams& params) = 0;

    /**
     * @brief 记录实例化绘制命令
     */
    virtual void cmdDrawInstanced(const DrawParams& params, 
                                 std::shared_ptr<HYBuffer> instanceData = nullptr) = 0;

    // ==== 高级命令接口 ====
    /**
     * @brief 记录批量纹理绑定命令
     */
    virtual void cmdBindTextures(const std::vector<TextureBinding>& bindings) = 0;

    /**
     * @brief 记录批量UBO绑定命令
     */
    virtual void cmdBindUniformBuffers(const std::vector<UniformBufferBinding>& bindings) = 0;

    /**
     * @brief 记录应用渲染状态快照命令
     */
    virtual void cmdApplyRenderState(const RenderState& state) = 0;

    /**
     * @brief 记录自定义命令
     */
    virtual void cmdCustom(std::function<void(HYRenderPass*)> customFunction) = 0;

    // ==== 命令批次管理 ====
    /**
     * @brief 开始一个命令批次（用于性能优化）
     */
    virtual void beginBatch(const std::string& batchName = "") = 0;

    /**
     * @brief 结束当前命令批次
     */
    virtual void endBatch() = 0;

    // ==== 状态查询 ====
    /**
     * @brief 获取命令缓冲区状态
     */
    CommandBufferState getState() const { return mState; }

    /**
     * @brief 获取记录的命令数量
     */
    virtual size_t getCommandCount() const = 0;

    /**
     * @brief 获取命令缓冲区调试信息
     */
    virtual std::string getDebugInfo() const = 0;

    // ==== 工厂方法 ====
    /**
     * @brief 创建平台相关的命令缓冲区实例
     */
    static std::shared_ptr<HYCommandBuffer> create(RenderAPI api);

protected:
    CommandBufferState mState;

    // ==== 内部辅助方法 ====
    /**
     * @brief 设置命令缓冲区状态
     */
    void setState(CommandBufferState newState) { mState = newState; }

    /**
     * @brief 验证状态转换是否有效
     */
    virtual bool isValidStateTransition(CommandBufferState from, CommandBufferState to) const = 0;
};

/**
 * @brief 命令缓冲区工厂类
 */
class CommandBufferFactory {
public:
    /**
     * @brief 创建指定API的命令缓冲区
     */
    static std::shared_ptr<HYCommandBuffer> createCommandBuffer(RenderAPI api);

    /**
     * @brief 创建命令缓冲区池（用于高性能场景）
     */
    static std::vector<std::shared_ptr<HYCommandBuffer>> createCommandBufferPool(
        RenderAPI api, size_t poolSize);
};

} // namespace render
} // namespace hyengine