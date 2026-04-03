#include "RenderPassGL.hpp"
#include <iostream>
#include <algorithm>

namespace hyengine {
namespace render {

RenderPassGL::RenderPassGL() {
    std::cout << "[RenderPassGL] Created" << std::endl;
}

RenderPassGL::~RenderPassGL() {
    if (mIsInPass) {
        std::cerr << "[RenderPassGL] Warning: RenderPass destroyed while still in pass!" << std::endl;
    }
    std::cout << "[RenderPassGL] Destroyed" << std::endl;
}

void RenderPassGL::beginPass(std::shared_ptr<HYFrameBuffer> framebuffer) {
    if (mIsInPass) {
        std::cerr << "[RenderPassGL] Error: beginPass called while already in pass!" << std::endl;
        return;
    }
    
    mCurrentFramebuffer = framebuffer;
    mIsInPass = true;
    
    if (framebuffer) {
        std::cout << "[RenderPassGL] beginPass with framebuffer" << std::endl;
        // TODO: 实际实现中这里会调用 glBindFramebuffer
        // auto handle = framebuffer->getResourceHandle();
        // glBindFramebuffer(GL_FRAMEBUFFER, handle.uintHandle);
    } else {
        std::cout << "[RenderPassGL] beginPass with default framebuffer" << std::endl;
        // TODO: 绑定默认帧缓冲
        // glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
}

void RenderPassGL::endPass() {
    if (!mIsInPass) {
        std::cerr << "[RenderPassGL] Error: endPass called without beginPass!" << std::endl;
        return;
    }
    
    std::cout << "[RenderPassGL] endPass" << std::endl;
    
    // 清理状态
    mCurrentFramebuffer.reset();
    mIsInPass = false;
    
    // TODO: 实际实现中这里可能需要解绑帧缓冲
    // glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void RenderPassGL::setViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) {
    mViewport[0] = x;
    mViewport[1] = y;
    mViewport[2] = width;
    mViewport[3] = height;
    
    std::cout << "[RenderPassGL] setViewport(" << x << ", " << y << ", " 
              << width << ", " << height << ")" << std::endl;
    
    // TODO: 实际实现中这里会调用 glViewport
    // glViewport(x, y, width, height);
}

void RenderPassGL::setScissor(uint32_t x, uint32_t y, uint32_t width, uint32_t height) {
    std::cout << "[RenderPassGL] setScissor(" << x << ", " << y << ", " 
              << width << ", " << height << ")" << std::endl;
    
    // TODO: 实际实现中这里会调用 glScissor
    // glEnable(GL_SCISSOR_TEST);
    // glScissor(x, y, width, height);
}

void RenderPassGL::setClearColor(float r, float g, float b, float a) {
    mClearColor[0] = r;
    mClearColor[1] = g;
    mClearColor[2] = b;
    mClearColor[3] = a;
    
    std::cout << "[RenderPassGL] setClearColor(" << r << ", " << g << ", " 
              << b << ", " << a << ")" << std::endl;
    
    // TODO: 实际实现中这里会调用 glClearColor
    // glClearColor(r, g, b, a);
}

void RenderPassGL::clearBuffer(uint8_t mask) {
    std::cout << "[RenderPassGL] clearBuffer(mask=" << static_cast<int>(mask) << ")";
    
    if (mask & ClearMask::CLEAR_COLOR_BUFFER) {
        std::cout << " COLOR";
    }
    if (mask & ClearMask::CLEAR_DEPTH_BUFFER) {
        std::cout << " DEPTH";
    }
    if (mask & ClearMask::CLEAR_STENCIL_BUFFER) {
        std::cout << " STENCIL";
    }
    std::cout << std::endl;
    
    // TODO: 实际实现中这里会调用 glClear
    // GLbitfield glMask = 0;
    // if (mask & ClearMask::CLEAR_COLOR_BUFFER) glMask |= GL_COLOR_BUFFER_BIT;
    // if (mask & ClearMask::CLEAR_DEPTH_BUFFER) glMask |= GL_DEPTH_BUFFER_BIT;
    // if (mask & ClearMask::CLEAR_STENCIL_BUFFER) glMask |= GL_STENCIL_BUFFER_BIT;
    // glClear(glMask);
}

void RenderPassGL::setVertexBuffer(std::shared_ptr<HYBuffer> buffer) {
    mCurrentVertexBuffer = buffer;
    
    if (buffer) {
        std::cout << "[RenderPassGL] setVertexBuffer" << std::endl;
        // TODO: 实际实现中这里会绑定VBO
        // auto handle = buffer->getResourceHandle();
        // glBindBuffer(GL_ARRAY_BUFFER, handle.uintHandle);
    } else {
        std::cout << "[RenderPassGL] setVertexBuffer(null)" << std::endl;
    }
}

void RenderPassGL::setVertexBuffer(std::shared_ptr<HYBuffer> buffer, uint32_t binding) {
    mCurrentVertexBuffer = buffer;
    
    if (buffer) {
        std::cout << "[RenderPassGL] setVertexBuffer (binding " << binding << ")" << std::endl;
        // TODO: 实际实现中这里会绑定VBO到指定绑定点
        // auto handle = buffer->getResourceHandle();
        // glBindVertexBuffer(binding, handle.uintHandle, 0, stride);
    } else {
        std::cout << "[RenderPassGL] setVertexBuffer(null, binding " << binding << ")" << std::endl;
    }
}

void RenderPassGL::setIndexBuffer(std::shared_ptr<HYBuffer> buffer) {
    mCurrentIndexBuffer = buffer;
    
    if (buffer) {
        std::cout << "[RenderPassGL] setIndexBuffer" << std::endl;
        // TODO: 实际实现中这里会绑定IBO
        // auto handle = buffer->getResourceHandle();
        // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, handle.uintHandle);
    } else {
        std::cout << "[RenderPassGL] setIndexBuffer(null)" << std::endl;
    }
}

void RenderPassGL::setShader(std::shared_ptr<HYShader> shader) {
    mCurrentShader = shader;
    
    if (shader) {
        std::cout << "[RenderPassGL] setShader" << std::endl;
        // TODO: 实际实现中这里会使用着色器程序
        // auto handle = shader->getResourceHandle();
        // glUseProgram(handle.uintHandle);
    } else {
        std::cout << "[RenderPassGL] setShader(null)" << std::endl;
    }
}

void RenderPassGL::setPipelineState(std::shared_ptr<HYPipelineState> pso) {
    mCurrentPSO = pso;
    
    if (pso) {
        std::cout << "[RenderPassGL] setPipelineState" << std::endl;
        // TODO: 实际实现中这里会应用管线状态
        // const auto& desc = pso->getDesc();
        // 
        // // 混合状态
        // if (desc.blendDesc.blendEnable) {
        //     glEnable(GL_BLEND);
        //     glBlendFunc(...);
        // } else {
        //     glDisable(GL_BLEND);
        // }
        // 
        // // 深度状态
        // if (desc.depthStencilDesc.depthTestEnable) {
        //     glEnable(GL_DEPTH_TEST);
        //     glDepthFunc(...);
        // } else {
        //     glDisable(GL_DEPTH_TEST);
        // }
        // 
        // // 剔除模式
        // switch (desc.rasterizerDesc.cullMode) {
        //     case CullMode::CULL_MODE_NONE:
        //         glDisable(GL_CULL_FACE);
        //         break;
        //     case CullMode::CULL_MODE_BACK:
        //         glEnable(GL_CULL_FACE);
        //         glCullFace(GL_BACK);
        //         break;
        //     case CullMode::CULL_MODE_FRONT:
        //         glEnable(GL_CULL_FACE);
        //         glCullFace(GL_FRONT);
        //         break;
        // }
    } else {
        std::cout << "[RenderPassGL] setPipelineState(null)" << std::endl;
    }
}

void RenderPassGL::drawArrays(uint32_t vertexStart, uint32_t vertexCount) {
    if (!mIsInPass) {
        std::cerr << "[RenderPassGL] Error: drawArrays called outside of pass!" << std::endl;
        return;
    }
    
    std::cout << "[RenderPassGL] drawArrays(start=" << vertexStart 
              << ", count=" << vertexCount << ")" << std::endl;
    
    // TODO: 实际实现中这里会调用 glDrawArrays
    // GLenum mode = GL_TRIANGLES; // 从PipelineState获取
    // glDrawArrays(mode, vertexStart, vertexCount);
}

void RenderPassGL::drawArraysInstanced(uint32_t vertexStart, uint32_t vertexCount, uint32_t instanceCount) {
    if (!mIsInPass) {
        std::cerr << "[RenderPassGL] Error: drawArraysInstanced called outside of pass!" << std::endl;
        return;
    }
    
    std::cout << "[RenderPassGL] drawArraysInstanced(start=" << vertexStart 
              << ", count=" << vertexCount << ", instances=" << instanceCount << ")" << std::endl;
    
    // TODO: 实际实现中这里会调用 glDrawArraysInstanced
    // GLenum mode = GL_TRIANGLES;
    // glDrawArraysInstanced(mode, vertexStart, vertexCount, instanceCount);
}

void RenderPassGL::drawElements(uint32_t indexCount, uint32_t indexOffset) {
    if (!mIsInPass) {
        std::cerr << "[RenderPassGL] Error: drawElements called outside of pass!" << std::endl;
        return;
    }
    
    std::cout << "[RenderPassGL] drawElements(count=" << indexCount 
              << ", offset=" << indexOffset << ")" << std::endl;
    
    // TODO: 实际实现中这里会调用 glDrawElements
    // GLenum mode = GL_TRIANGLES;
    // GLenum type = GL_UNSIGNED_INT; // 或 GL_UNSIGNED_SHORT
    // const void* indices = reinterpret_cast<const void*>(indexOffset * sizeof(uint32_t));
    // glDrawElements(mode, indexCount, type, indices);
}

void RenderPassGL::drawIndexed(uint32_t indexCount, uint32_t indexStart) {
    if (!mIsInPass) {
        std::cerr << "[RenderPassGL] Error: drawIndexed called outside of pass!" << std::endl;
        return;
    }
    
    std::cout << "[RenderPassGL] drawIndexed(count=" << indexCount 
              << ", start=" << indexStart << ")" << std::endl;
    
    // TODO: 实际实现中这里会调用 glDrawElements
    // GLenum mode = GL_TRIANGLES;
    // GLenum type = GL_UNSIGNED_INT;
    // const void* indices = reinterpret_cast<const void*>(indexStart * sizeof(uint32_t));
    // glDrawElements(mode, indexCount, type, indices);
}

void RenderPassGL::drawElementsInstanced(uint32_t indexCount, uint32_t instanceCount, uint32_t indexOffset) {
    if (!mIsInPass) {
        std::cerr << "[RenderPassGL] Error: drawElementsInstanced called outside of pass!" << std::endl;
        return;
    }
    
    std::cout << "[RenderPassGL] drawElementsInstanced(count=" << indexCount 
              << ", instances=" << instanceCount << ", offset=" << indexOffset << ")" << std::endl;
    
    // TODO: 实际实现中这里会调用 glDrawElementsInstanced
    // GLenum mode = GL_TRIANGLES;
    // GLenum type = GL_UNSIGNED_INT;
    // const void* indices = reinterpret_cast<const void*>(indexOffset * sizeof(uint32_t));
    // glDrawElementsInstanced(mode, indexCount, type, indices, instanceCount);
}

ResourceHandle RenderPassGL::getResourceHandle() const {
    ResourceHandle handle;
    handle.uintHandle = 0; // RenderPass没有特定的GL句柄
    return handle;
}

// 5.1.4 新增：高级资源绑定接口
void RenderPassGL::bindTexture(std::shared_ptr<HYTexture> texture, uint32_t textureUnit, const std::string& samplerName) {
    if (!texture) {
        std::cerr << "[RenderPassGL] Error: bindTexture called with null texture!" << std::endl;
        return;
    }
    
    // 查找是否已存在相同的绑定
    auto it = std::find_if(mTextureBindings.begin(), mTextureBindings.end(),
        [&](const TextureBinding& binding) {
            return binding.samplerName == samplerName;
        });
    
    if (it != mTextureBindings.end()) {
        // 更新现有绑定
        it->texture = texture;
        it->textureUnit = textureUnit;
        it->bindingIndex = textureUnit;
    } else {
        // 添加新绑定
        TextureBinding binding;
        binding.texture = texture;
        binding.textureUnit = textureUnit;
        binding.bindingIndex = textureUnit;
        binding.samplerName = samplerName;
        mTextureBindings.push_back(binding);
    }
    
    // 立即应用绑定
    auto handle = texture->getResourceHandle();
    // TODO: 实际实现
    // glActiveTexture(GL_TEXTURE0 + textureUnit);
    // glBindTexture(GL_TEXTURE_2D, handle.uintHandle);
    
    // 标记状态为脏
    mRenderState.isDirty = true;
    
    std::cout << "[RenderPassGL] bindTexture(unit=" << textureUnit 
              << ", sampler=" << samplerName << ")" << std::endl;
}

void RenderPassGL::bindUniformBuffer(std::shared_ptr<HYBuffer> buffer, uint32_t bindingPoint,
                                   const std::string& blockName, uint32_t offset, uint32_t size) {
    if (!buffer) {
        std::cerr << "[RenderPassGL] Error: bindUniformBuffer called with null buffer!" << std::endl;
        return;
    }
    
    // 查找是否已存在相同的绑定
    auto it = std::find_if(mUniformBufferBindings.begin(), mUniformBufferBindings.end(),
        [&](const UniformBufferBinding& binding) {
            return binding.blockName == blockName;
        });
    
    if (it != mUniformBufferBindings.end()) {
        // 更新现有绑定
        it->buffer = buffer;
        it->bindingPoint = bindingPoint;
        it->offset = offset;
        it->size = size;
    } else {
        // 添加新绑定
        UniformBufferBinding binding;
        binding.buffer = buffer;
        binding.bindingPoint = bindingPoint;
        binding.blockName = blockName;
        binding.offset = offset;
        binding.size = size;
        mUniformBufferBindings.push_back(binding);
    }
    
    // 立即应用绑定
    auto handle = buffer->getResourceHandle();
    // TODO: 实际实现
    // if (size > 0) {
    //     glBindBufferRange(GL_UNIFORM_BUFFER, bindingPoint, handle.uintHandle, offset, size);
    // } else {
    //     glBindBufferBase(GL_UNIFORM_BUFFER, bindingPoint, handle.uintHandle);
    // }
    
    // 标记状态为脏
    mRenderState.isDirty = true;
    
    std::cout << "[RenderPassGL] bindUniformBuffer(point=" << bindingPoint 
              << ", block=" << blockName << ", offset=" << offset 
              << ", size=" << size << ")" << std::endl;
}

void RenderPassGL::bindTextures(const std::vector<TextureBinding>& bindings) {
    for (const auto& binding : bindings) {
        bindTexture(binding.texture, binding.textureUnit, binding.samplerName);
    }
    
    std::cout << "[RenderPassGL] bindTextures: bound " << bindings.size() << " textures" << std::endl;
}

void RenderPassGL::bindUniformBuffers(const std::vector<UniformBufferBinding>& bindings) {
    for (const auto& binding : bindings) {
        bindUniformBuffer(binding.buffer, binding.bindingPoint, binding.blockName, 
                         binding.offset, binding.size);
    }
    
    std::cout << "[RenderPassGL] bindUniformBuffers: bound " << bindings.size() << " UBOs" << std::endl;
}

// 5.1.4 新增：统一绘制接口
void RenderPassGL::draw(const DrawParams& params) {
    if (!mIsInPass) {
        std::cerr << "[RenderPassGL] Error: draw called outside of pass!" << std::endl;
        return;
    }
    
    // 应用渲染状态
    updateStateIfDirty();
    
    std::cout << "[RenderPassGL] draw(primitive=" << static_cast<int>(params.primitiveType)
              << ", vertexCount=" << params.vertexCount
              << ", instanceCount=" << params.instanceCount << ")" << std::endl;
    
    if (params.indexCount > 0) {
        // 索引绘制
        if (params.instanceCount > 1) {
            drawElementsInstanced(params.indexCount, params.instanceCount, params.indexStart);
        } else {
            drawElements(params.indexCount, params.indexStart);
        }
    } else {
        // 顶点数组绘制
        if (params.instanceCount > 1) {
            drawArraysInstanced(params.vertexStart, params.vertexCount, params.instanceCount);
        } else {
            drawArrays(params.vertexStart, params.vertexCount);
        }
    }
}

void RenderPassGL::drawInstanced(const DrawParams& params, std::shared_ptr<HYBuffer> instanceData) {
    if (instanceData) {
        mInstanceBuffer = instanceData;
        // TODO: 绑定实例数据缓冲区
        // auto handle = instanceData->getResourceHandle();
        // glBindBuffer(GL_ARRAY_BUFFER, handle.uintHandle);
    }
    
    draw(params);
}

// 5.1.4 新增：状态管理接口
const RenderState& RenderPassGL::getRenderState() const {
    return mRenderState;
}

void RenderPassGL::applyRenderState(const RenderState& state) {
    mRenderState = state;
    
    // 应用着色器
    if (state.currentShader) {
        setShader(state.currentShader);
    }
    
    // 应用管线状态
    if (state.currentPipelineState) {
        setPipelineState(state.currentPipelineState);
    }
    
    // 应用帧缓冲
    if (state.currentFrameBuffer) {
        // 注意：这里不能直接调用beginPass，因为可能已经在Pass中
        mCurrentFramebuffer = state.currentFrameBuffer;
    }
    
    // 应用顶点缓冲
    for (size_t i = 0; i < state.vertexBuffers.size(); ++i) {
        if (state.vertexBuffers[i]) {
            setVertexBuffer(state.vertexBuffers[i], static_cast<uint32_t>(i));
        }
    }
    
    // 应用纹理绑定
    mTextureBindings = state.textureBindings;
    for (const auto& binding : state.textureBindings) {
        if (binding.texture) {
            auto handle = binding.texture->getResourceHandle();
            // TODO: 实际实现
            // glActiveTexture(GL_TEXTURE0 + binding.textureUnit);
            // glBindTexture(GL_TEXTURE_2D, handle.uintHandle);
        }
    }
    
    // 应用UBO绑定
    mUniformBufferBindings = state.uboBindings;
    for (const auto& binding : state.uboBindings) {
        if (binding.buffer) {
            auto handle = binding.buffer->getResourceHandle();
            // TODO: 实际实现
            // if (binding.size > 0) {
            //     glBindBufferRange(GL_UNIFORM_BUFFER, binding.bindingPoint, 
            //                      handle.uintHandle, binding.offset, binding.size);
            // } else {
            //     glBindBufferBase(GL_UNIFORM_BUFFER, binding.bindingPoint, handle.uintHandle);
            // }
        }
    }
    
    mRenderState.isDirty = false;
    
    std::cout << "[RenderPassGL] applyRenderState completed" << std::endl;
}

void RenderPassGL::flushRenderState() {
    updateStateIfDirty();
    std::cout << "[RenderPassGL] flushRenderState" << std::endl;
}

// 5.1.4 新增：工具方法
void RenderPassGL::autoBindResources(std::shared_ptr<HYShader> shader,
                                    const std::map<std::string, std::shared_ptr<HYTexture>>& textures,
                                    const std::map<std::string, std::shared_ptr<HYBuffer>>& uniformBuffers) {
    if (!shader) {
        std::cerr << "[RenderPassGL] Error: autoBindResources called with null shader!" << std::endl;
        return;
    }
    
    setShader(shader);
    
    // 缓存着色器反射信息
    cacheShaderReflection(shader);
    
    // 自动绑定纹理
    uint32_t textureUnit = 0;
    for (const auto& pair : textures) {
        const std::string& samplerName = pair.first;
        auto texture = pair.second;
        
        if (texture) {
            bindTexture(texture, textureUnit++, samplerName);
        }
    }
    
    // 自动绑定UBO
    uint32_t bindingPoint = 0;
    for (const auto& pair : uniformBuffers) {
        const std::string& blockName = pair.first;
        auto buffer = pair.second;
        
        if (buffer) {
            bindUniformBuffer(buffer, bindingPoint++, blockName);
        }
    }
    
    std::cout << "[RenderPassGL] autoBindResources completed (textures=" 
              << textures.size() << ", ubos=" << uniformBuffers.size() << ")" << std::endl;
}

void RenderPassGL::drawAuto() {
    // 创建默认绘制参数
    DrawParams params;
    params.primitiveType = PrimitiveType::PRIMITIVE_TRIANGLES;
    params.vertexStart = 0;
    params.vertexCount = 3; // 默认绘制三角形
    params.instanceCount = 1;
    
    // 自动刷新状态
    flushRenderState();
    
    // 执行绘制
    draw(params);
}

// 5.1.4 新增：内部状态管理方法
void RenderPassGL::updateStateIfDirty() {
    if (!mRenderState.isDirty) {
        return;
    }
    
    // 更新渲染状态结构
    mRenderState.currentShader = mCurrentShader;
    mRenderState.currentPipelineState = mCurrentPSO;
    mRenderState.currentFrameBuffer = mCurrentFramebuffer;
    
    // 更新顶点缓冲列表
    mRenderState.vertexBuffers.clear();
    if (mCurrentVertexBuffer) {
        mRenderState.vertexBuffers.push_back(mCurrentVertexBuffer);
    }
    
    // 更新纹理绑定列表
    mRenderState.textureBindings = mTextureBindings;
    
    // 更新UBO绑定列表
    mRenderState.uboBindings = mUniformBufferBindings;
    
    mRenderState.isDirty = false;
    
    std::cout << "[RenderPassGL] updateStateIfDirty completed" << std::endl;
}

void RenderPassGL::cacheShaderReflection(std::shared_ptr<HYShader> shader) {
    if (!shader) {
        return;
    }
    
    // 获取着色器反射信息
    const auto& reflection = shader->getReflection();
    
    std::cout << "[RenderPassGL] cacheShaderReflection: uniforms=" << reflection.uniforms.size()
              << ", attributes=" << reflection.attributes.size() << std::endl;
    
    // 这里可以缓存uniform位置等信息以优化性能
    // 实际实现中可以预先查询所有uniform位置并缓存
}

} // namespace render
} // namespace hyengine
