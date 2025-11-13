#include "../../include/hy_render/resources/HYCommandBuffer.hpp"
#include "../../include/hy_render/resources/HYRenderPass.hpp"
#include "../gl_common/CommandBufferGL.hpp"
#include "../metal/resources/CommandBufferMtl.hpp"
#include <iostream>

namespace hyengine {
namespace render {

// ==== 命令实现 ====

void BeginPassCommand::execute(HYRenderPass* renderPass) {
    renderPass->beginPass(framebuffer);
}

void EndPassCommand::execute(HYRenderPass* renderPass) {
    renderPass->endPass();
}

void SetViewportCommand::execute(HYRenderPass* renderPass) {
    renderPass->setViewport(x, y, width, height);
}

void ClearBufferCommand::execute(HYRenderPass* renderPass) {
    renderPass->setClearColor(clearColor[0], clearColor[1], clearColor[2], clearColor[3]);
    renderPass->clearBuffer(mask);
}

void DrawCommand::execute(HYRenderPass* renderPass) {
    renderPass->draw(params);
}

void BindTextureCommand::execute(HYRenderPass* renderPass) {
    renderPass->bindTexture(texture, textureUnit, samplerName);
}

void CustomCommand::execute(HYRenderPass* renderPass) {
    if (customFunction) {
        customFunction(renderPass);
    }
}

// ==== 命令缓冲区工厂方法 ====

std::shared_ptr<HYCommandBuffer> HYCommandBuffer::create(RenderAPI api) {
    return CommandBufferFactory::createCommandBuffer(api);
}

std::shared_ptr<HYCommandBuffer> CommandBufferFactory::createCommandBuffer(RenderAPI api) {
    switch (api) {
        case RenderAPI::kOpenGL:
            return std::make_shared<CommandBufferGL>();
        case RenderAPI::kOpenGLES:
            return std::make_shared<CommandBufferGLES>();
        case RenderAPI::kMetal:
            return std::make_shared<CommandBufferMtl>();
        default:
            std::cerr << "[CommandBufferFactory] Unsupported API: " << static_cast<int>(api) << std::endl;
            return nullptr;
    }
}

std::vector<std::shared_ptr<HYCommandBuffer>> CommandBufferFactory::createCommandBufferPool(
    RenderAPI api, size_t poolSize) {
    std::vector<std::shared_ptr<HYCommandBuffer>> pool;
    pool.reserve(poolSize);
    
    for (size_t i = 0; i < poolSize; ++i) {
        pool.push_back(createCommandBuffer(api));
    }
    
    return pool;
}

} // namespace render
} // namespace hyengine