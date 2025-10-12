#include "RenderPassMtl.hpp"
#include "RenderPassMtl.hpp"
#include <iostream>
#include <algorithm>

namespace hyengine {
namespace render {

RenderPassMtl::RenderPassMtl(void* commandQueue) : mCommandQueue(commandQueue) {
    std::cout << "[RenderPassMtl] 创建Metal渲染通道" << std::endl;
}

void RenderPassMtl::beginPass(std::shared_ptr<HYFrameBuffer> framebuffer) {
    mIsInPass = true;
    mCurrentFramebuffer = framebuffer;
    mCommandBuffer = reinterpret_cast<void*>(0x600);
    mRenderCommandEncoder = reinterpret_cast<void*>(0x601);
    std::cout << "[RenderPassMtl] 开始渲染通道" << std::endl;
}

void RenderPassMtl::endPass() {
    mIsInPass = false;
    std::cout << "[RenderPassMtl] 结束渲染通道" << std::endl;
}

void RenderPassMtl::setShader(std::shared_ptr<HYShader> shader) { mCurrentShader = shader; }
void RenderPassMtl::setPipelineState(std::shared_ptr<HYPipelineState> state) { mCurrentPipelineState = state; }
void RenderPassMtl::setVertexBuffer(std::shared_ptr<HYBuffer> buffer, uint32_t binding) {}
void RenderPassMtl::setVertexBuffer(std::shared_ptr<HYBuffer> buffer) {}
void RenderPassMtl::setClearColor(float r, float g, float b, float a) {}
void RenderPassMtl::clearBuffer(uint8_t mask) {}
void RenderPassMtl::setIndexBuffer(std::shared_ptr<HYBuffer> buffer) {}
void RenderPassMtl::setViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) {}
void RenderPassMtl::setScissor(uint32_t x, uint32_t y, uint32_t width, uint32_t height) {}

void RenderPassMtl::drawArrays(uint32_t vertexStart, uint32_t vertexCount) {
    std::cout << "[RenderPassMtl] drawArrays(" << vertexStart << ", " << vertexCount << ")" << std::endl;
}

void RenderPassMtl::drawArraysInstanced(uint32_t vertexStart, uint32_t vertexCount, uint32_t instanceCount) {
    std::cout << "[RenderPassMtl] drawArraysInstanced" << std::endl;
}

void RenderPassMtl::drawElements(uint32_t indexCount, uint32_t indexOffset) {
    std::cout << "[RenderPassMtl] drawElements" << std::endl;
}

void RenderPassMtl::drawElementsInstanced(uint32_t indexCount, uint32_t instanceCount, uint32_t indexOffset) {
    std::cout << "[RenderPassMtl] drawElementsInstanced" << std::endl;
}

void RenderPassMtl::drawIndexed(uint32_t indexCount, uint32_t indexStart) {
    std::cout << "[RenderPassMtl] drawIndexed(" << indexCount << ", " << indexStart << ")" << std::endl;
}

ResourceHandle RenderPassMtl::getResourceHandle() const {
    ResourceHandle handle;
    handle.ptrHandle = mRenderCommandEncoder;
    return handle;
}

// 5.1.4 新增：高级资源绑定接口
void RenderPassMtl::bindTexture(std::shared_ptr<HYTexture> texture, uint32_t textureUnit, const std::string& samplerName) {
    if (!texture) {
        std::cerr << "[RenderPassMtl] Error: bindTexture called with null texture!" << std::endl;
        return;
    }
    
    // 查找是否已存在相同的绑定
    auto it = std::find_if(mTextureBindings.begin(), mTextureBindings.end(),
        [&](const TextureBinding& binding) {
            return binding.samplerName == samplerName;
        });
    
    if (it != mTextureBindings.end()) {
        it->texture = texture;
        it->textureUnit = textureUnit;
        it->bindingIndex = textureUnit;
    } else {
        TextureBinding binding;
        binding.texture = texture;
        binding.textureUnit = textureUnit;
        binding.bindingIndex = textureUnit;
        binding.samplerName = samplerName;
        mTextureBindings.push_back(binding);
    }
    
    // TODO: Metal实际实现
    // id<MTLRenderCommandEncoder> encoder = (__bridge id<MTLRenderCommandEncoder>)mRenderCommandEncoder;
    // id<MTLTexture> mtlTexture = (__bridge id<MTLTexture>)texture->getResourceHandle().ptrHandle;
    // [encoder setFragmentTexture:mtlTexture atIndex:textureUnit];
    
    mRenderState.isDirty = true;
    std::cout << "[RenderPassMtl] bindTexture(unit=" << textureUnit << ", sampler=" << samplerName << ")" << std::endl;
}

void RenderPassMtl::bindUniformBuffer(std::shared_ptr<HYBuffer> buffer, uint32_t bindingPoint,
                                     const std::string& blockName, uint32_t offset, uint32_t size) {
    if (!buffer) {
        std::cerr << "[RenderPassMtl] Error: bindUniformBuffer called with null buffer!" << std::endl;
        return;
    }
    
    auto it = std::find_if(mUniformBufferBindings.begin(), mUniformBufferBindings.end(),
        [&](const UniformBufferBinding& binding) {
            return binding.blockName == blockName;
        });
    
    if (it != mUniformBufferBindings.end()) {
        it->buffer = buffer;
        it->bindingPoint = bindingPoint;
        it->offset = offset;
        it->size = size;
    } else {
        UniformBufferBinding binding;
        binding.buffer = buffer;
        binding.bindingPoint = bindingPoint;
        binding.blockName = blockName;
        binding.offset = offset;
        binding.size = size;
        mUniformBufferBindings.push_back(binding);
    }
    
    // TODO: Metal实际实现
    // id<MTLRenderCommandEncoder> encoder = (__bridge id<MTLRenderCommandEncoder>)mRenderCommandEncoder;
    // id<MTLBuffer> mtlBuffer = (__bridge id<MTLBuffer>)buffer->getResourceHandle().ptrHandle;
    // [encoder setVertexBuffer:mtlBuffer offset:offset atIndex:bindingPoint];
    
    mRenderState.isDirty = true;
    std::cout << "[RenderPassMtl] bindUniformBuffer(point=" << bindingPoint << ", block=" << blockName << ")" << std::endl;
}

void RenderPassMtl::bindTextures(const std::vector<TextureBinding>& bindings) {
    for (const auto& binding : bindings) {
        bindTexture(binding.texture, binding.textureUnit, binding.samplerName);
    }
    std::cout << "[RenderPassMtl] bindTextures: bound " << bindings.size() << " textures" << std::endl;
}

void RenderPassMtl::bindUniformBuffers(const std::vector<UniformBufferBinding>& bindings) {
    for (const auto& binding : bindings) {
        bindUniformBuffer(binding.buffer, binding.bindingPoint, binding.blockName, 
                         binding.offset, binding.size);
    }
    std::cout << "[RenderPassMtl] bindUniformBuffers: bound " << bindings.size() << " UBOs" << std::endl;
}

// 5.1.4 新增：统一绘制接口
void RenderPassMtl::draw(const DrawParams& params) {
    if (!mIsInPass) {
        std::cerr << "[RenderPassMtl] Error: draw called outside of pass!" << std::endl;
        return;
    }
    
    updateStateIfDirty();
    
    std::cout << "[RenderPassMtl] draw(primitive=" << static_cast<int>(params.primitiveType)
              << ", vertexCount=" << params.vertexCount << ")" << std::endl;
    
    if (params.indexCount > 0) {
        if (params.instanceCount > 1) {
            drawElementsInstanced(params.indexCount, params.instanceCount, params.indexStart);
        } else {
            drawElements(params.indexCount, params.indexStart);
        }
    } else {
        if (params.instanceCount > 1) {
            drawArraysInstanced(params.vertexStart, params.vertexCount, params.instanceCount);
        } else {
            drawArrays(params.vertexStart, params.vertexCount);
        }
    }
}

void RenderPassMtl::drawInstanced(const DrawParams& params, std::shared_ptr<HYBuffer> instanceData) {
    if (instanceData) {
        mInstanceBuffer = instanceData;
    }
    draw(params);
}

// 5.1.4 新增：状态管理接口
const RenderState& RenderPassMtl::getRenderState() const {
    return mRenderState;
}

void RenderPassMtl::applyRenderState(const RenderState& state) {
    mRenderState = state;
    
    if (state.currentShader) {
        setShader(state.currentShader);
    }
    
    if (state.currentPipelineState) {
        setPipelineState(state.currentPipelineState);
    }
    
    if (state.currentFrameBuffer) {
        mCurrentFramebuffer = state.currentFrameBuffer;
    }
    
    for (size_t i = 0; i < state.vertexBuffers.size(); ++i) {
        if (state.vertexBuffers[i]) {
            setVertexBuffer(state.vertexBuffers[i], static_cast<uint32_t>(i));
        }
    }
    
    mTextureBindings = state.textureBindings;
    mUniformBufferBindings = state.uboBindings;
    
    mRenderState.isDirty = false;
    std::cout << "[RenderPassMtl] applyRenderState completed" << std::endl;
}

void RenderPassMtl::flushRenderState() {
    updateStateIfDirty();
    std::cout << "[RenderPassMtl] flushRenderState" << std::endl;
}

// 5.1.4 新增：工具方法
void RenderPassMtl::autoBindResources(std::shared_ptr<HYShader> shader,
                                     const std::map<std::string, std::shared_ptr<HYTexture>>& textures,
                                     const std::map<std::string, std::shared_ptr<HYBuffer>>& uniformBuffers) {
    if (!shader) {
        std::cerr << "[RenderPassMtl] Error: autoBindResources called with null shader!" << std::endl;
        return;
    }
    
    setShader(shader);
    cacheShaderReflection(shader);
    
    uint32_t textureUnit = 0;
    for (const auto& pair : textures) {
        if (pair.second) {
            bindTexture(pair.second, textureUnit++, pair.first);
        }
    }
    
    uint32_t bindingPoint = 0;
    for (const auto& pair : uniformBuffers) {
        if (pair.second) {
            bindUniformBuffer(pair.second, bindingPoint++, pair.first);
        }
    }
    
    std::cout << "[RenderPassMtl] autoBindResources completed" << std::endl;
}

void RenderPassMtl::drawAuto() {
    DrawParams params;
    params.primitiveType = PrimitiveType::PRIMITIVE_TRIANGLES;
    params.vertexStart = 0;
    params.vertexCount = 3;
    params.instanceCount = 1;
    
    flushRenderState();
    draw(params);
}

// 5.1.4 新增：内部状态管理方法
void RenderPassMtl::updateStateIfDirty() {
    if (!mRenderState.isDirty) {
        return;
    }
    
    mRenderState.currentShader = mCurrentShader;
    mRenderState.currentPipelineState = mCurrentPipelineState;
    mRenderState.currentFrameBuffer = mCurrentFramebuffer;
    
    mRenderState.vertexBuffers.clear();
    if (mCurrentVertexBuffer) {
        mRenderState.vertexBuffers.push_back(mCurrentVertexBuffer);
    }
    
    mRenderState.textureBindings = mTextureBindings;
    mRenderState.uboBindings = mUniformBufferBindings;
    
    mRenderState.isDirty = false;
}

void RenderPassMtl::cacheShaderReflection(std::shared_ptr<HYShader> shader) {
    if (!shader) {
        return;
    }
    
    const auto& reflection = shader->getReflection();
    std::cout << "[RenderPassMtl] cacheShaderReflection: uniforms=" << reflection.uniforms.size()
              << ", attributes=" << reflection.attributes.size() << std::endl;
}

} // namespace render
} // namespace hyengine
