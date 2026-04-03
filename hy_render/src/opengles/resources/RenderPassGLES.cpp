#include "RenderPassGLES.hpp"
#include "RenderPassGLES.hpp"
#include "../../gl_common/GLUtils.hpp"
#include <iostream>
#include <algorithm>

namespace hyengine {
namespace render {

// ============================================================================
// RenderPassGLES 实现
// ============================================================================

RenderPassGLES::RenderPassGLES() {
    std::cout << "[RenderPassGLES] 创建渲染通道" << std::endl;
}

void RenderPassGLES::beginPass(std::shared_ptr<HYFrameBuffer> framebuffer) {
    if (mIsInPass) {
        std::cerr << "[RenderPassGLES] Error: beginPass called while already in pass!" << std::endl;
        return;
    }
    
    mCurrentFramebuffer = framebuffer;
    mIsInPass = true;
    
    std::cout << "[RenderPassGLES] Begin render pass" << std::endl;
    
    // TODO:
    // if (framebuffer) {
    //     GLuint fbo = static_cast<GLuint>(framebuffer->getResourceHandle().uintHandle);
    //     glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    // } else {
    //     // 绑定默认帧缓冲（屏幕）
    //     glBindFramebuffer(GL_FRAMEBUFFER, 0);
    // }
    //
    // // 清除缓冲
    // glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
}

void RenderPassGLES::endPass() {
    if (!mIsInPass) {
        std::cerr << "[RenderPassGLES] Error: endPass called without beginPass!" << std::endl;
        return;
    }
    
    std::cout << "[RenderPassGLES] End render pass" << std::endl;
    
    // TODO:
    // glBindFramebuffer(GL_FRAMEBUFFER, 0);
    
    mCurrentFramebuffer = nullptr;
    mIsInPass = false;
}

void RenderPassGLES::setShader(std::shared_ptr<HYShader> shader) {
    mCurrentShader = shader;
    std::cout << "[RenderPassGLES] Set shader" << std::endl;
    
    // TODO:
    // if (shader) {
    //     GLuint program = static_cast<GLuint>(shader->getResourceHandle().uintHandle);
    //     glUseProgram(program);
    // }
}

void RenderPassGLES::setPipelineState(std::shared_ptr<HYPipelineState> state) {
    mCurrentPipelineState = state;
    std::cout << "[RenderPassGLES] Set pipeline state" << std::endl;
    
    // TODO: 应用管线状态（混合、深度测试、剔除等）
    // 在 OpenGL ES 中，这些状态通过 glEnable/glDisable 等函数设置
}

void RenderPassGLES::setVertexBuffer(std::shared_ptr<HYBuffer> buffer, uint32_t binding) {
    if (!mIsInPass) {
        std::cerr << "[RenderPassGLES] Error: setVertexBuffer called outside of pass!" << std::endl;
        return;
    }
    
    std::cout << "[RenderPassGLES] Set vertex buffer (binding " << binding << ")" << std::endl;
    
    // TODO:
    // GLuint vbo = static_cast<GLuint>(buffer->getResourceHandle().uintHandle);
    // glBindBuffer(GL_ARRAY_BUFFER, vbo);
}

void RenderPassGLES::setVertexBuffer(std::shared_ptr<HYBuffer> buffer) {
    setVertexBuffer(buffer, 0);
}

void RenderPassGLES::setClearColor(float r, float g, float b, float a) {
    std::cout << "[RenderPassGLES] setClearColor(" << r << ", " << g << ", " << b << ", " << a << ")" << std::endl;
    // TODO: glClearColor(r, g, b, a);
}

void RenderPassGLES::clearBuffer(uint8_t mask) {
    std::cout << "[RenderPassGLES] clearBuffer(mask=" << static_cast<int>(mask) << ")" << std::endl;
    
    // TODO:
    // GLbitfield glMask = 0;
    // if (mask & ClearMask::CLEAR_COLOR_BUFFER) glMask |= GL_COLOR_BUFFER_BIT;
    // if (mask & ClearMask::CLEAR_DEPTH_BUFFER) glMask |= GL_DEPTH_BUFFER_BIT;
    // if (mask & ClearMask::CLEAR_STENCIL_BUFFER) glMask |= GL_STENCIL_BUFFER_BIT;
    // glClear(glMask);
}

void RenderPassGLES::drawArraysInstanced(uint32_t vertexStart, uint32_t vertexCount, uint32_t instanceCount) {
    if (!mIsInPass) {
        std::cerr << "[RenderPassGLES] Error: drawArraysInstanced called outside of pass!" << std::endl;
        return;
    }
    
    std::cout << "[RenderPassGLES] drawArraysInstanced(start=" << vertexStart 
              << ", count=" << vertexCount << ", instances=" << instanceCount << ")" << std::endl;
    
    // TODO:
    // GLenum mode = GL_TRIANGLES;
    // glDrawArraysInstanced(mode, vertexStart, vertexCount, instanceCount);
}

void RenderPassGLES::drawElements(uint32_t indexCount, uint32_t indexOffset) {
    if (!mIsInPass) {
        std::cerr << "[RenderPassGLES] Error: drawElements called outside of pass!" << std::endl;
        return;
    }
    
    std::cout << "[RenderPassGLES] drawElements(count=" << indexCount 
              << ", offset=" << indexOffset << ")" << std::endl;
    
    // TODO:
    // GLenum mode = GL_TRIANGLES;
    // GLenum type = GL_UNSIGNED_INT;
    // const void* indices = reinterpret_cast<const void*>(indexOffset * sizeof(uint32_t));
    // glDrawElements(mode, indexCount, type, indices);
}

void RenderPassGLES::drawElementsInstanced(uint32_t indexCount, uint32_t instanceCount, uint32_t indexOffset) {
    if (!mIsInPass) {
        std::cerr << "[RenderPassGLES] Error: drawElementsInstanced called outside of pass!" << std::endl;
        return;
    }
    
    std::cout << "[RenderPassGLES] drawElementsInstanced(count=" << indexCount 
              << ", instances=" << instanceCount << ", offset=" << indexOffset << ")" << std::endl;
    
    // TODO:
    // GLenum mode = GL_TRIANGLES;
    // GLenum type = GL_UNSIGNED_INT;
    // const void* indices = reinterpret_cast<const void*>(indexOffset * sizeof(uint32_t));
    // glDrawElementsInstanced(mode, indexCount, type, indices, instanceCount);
}

void RenderPassGLES::setIndexBuffer(std::shared_ptr<HYBuffer> buffer) {
    if (!mIsInPass) {
        std::cerr << "[RenderPassGLES] Error: setIndexBuffer called outside of pass!" << std::endl;
        return;
    }
    
    std::cout << "[RenderPassGLES] Set index buffer" << std::endl;
    
    // TODO:
    // GLuint ibo = static_cast<GLuint>(buffer->getResourceHandle().uintHandle);
    // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
}

void RenderPassGLES::setViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) {
    if (!mIsInPass) {
        std::cerr << "[RenderPassGLES] Error: setViewport called outside of pass!" << std::endl;
        return;
    }
    
    std::cout << "[RenderPassGLES] Set viewport (" << x << "," << y 
              << " " << width << "x" << height << ")" << std::endl;
    
    // TODO:
    // glViewport(x, y, width, height);
}

void RenderPassGLES::setScissor(uint32_t x, uint32_t y, uint32_t width, uint32_t height) {
    if (!mIsInPass) {
        std::cerr << "[RenderPassGLES] Error: setScissor called outside of pass!" << std::endl;
        return;
    }
    
    std::cout << "[RenderPassGLES] Set scissor (" << x << "," << y 
              << " " << width << "x" << height << ")" << std::endl;
    
    // TODO:
    // glEnable(GL_SCISSOR_TEST);
    // glScissor(x, y, width, height);
}

void RenderPassGLES::drawArrays(uint32_t vertexStart, uint32_t vertexCount) {
    if (!mIsInPass) {
        std::cerr << "[RenderPassGLES] Error: drawArrays called outside of pass!" << std::endl;
        return;
    }
    
    std::cout << "[RenderPassGLES] drawArrays(start=" << vertexStart 
              << ", count=" << vertexCount << ")" << std::endl;
    
    // TODO:
    // GLenum mode = GL_TRIANGLES; // 根据 PrimitiveType 转换
    // glDrawArrays(mode, vertexStart, vertexCount);
}

void RenderPassGLES::drawIndexed(uint32_t indexCount, uint32_t indexStart) {
    if (!mIsInPass) {
        std::cerr << "[RenderPassGLES] Error: drawIndexed called outside of pass!" << std::endl;
        return;
    }
    
    std::cout << "[RenderPassGLES] drawIndexed(count=" << indexCount 
              << ", start=" << indexStart << ")" << std::endl;
    
    // TODO:
    // GLenum mode = GL_TRIANGLES; // 根据 PrimitiveType 转换
    // GLenum type = GL_UNSIGNED_INT; // 或 GL_UNSIGNED_SHORT
    // const void* indices = reinterpret_cast<const void*>(indexStart * sizeof(uint32_t));
    // glDrawElements(mode, indexCount, type, indices);
}

ResourceHandle RenderPassGLES::getResourceHandle() const {
    ResourceHandle handle;
    handle.uintHandle = 0; // 渲染通道没有独立的GL对象
    return handle;
}

// 5.1.4 新增：高级资源绑定接口
void RenderPassGLES::bindTexture(std::shared_ptr<HYTexture> texture, uint32_t textureUnit, const std::string& samplerName) {
    if (!texture) {
        std::cerr << "[RenderPassGLES] Error: bindTexture called with null texture!" << std::endl;
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
    
    std::cout << "[RenderPassGLES] bindTexture(unit=" << textureUnit 
              << ", sampler=" << samplerName << ")" << std::endl;
}

void RenderPassGLES::bindUniformBuffer(std::shared_ptr<HYBuffer> buffer, uint32_t bindingPoint,
                                      const std::string& blockName, uint32_t offset, uint32_t size) {
    if (!buffer) {
        std::cerr << "[RenderPassGLES] Error: bindUniformBuffer called with null buffer!" << std::endl;
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
    
    std::cout << "[RenderPassGLES] bindUniformBuffer(point=" << bindingPoint 
              << ", block=" << blockName << ", offset=" << offset 
              << ", size=" << size << ")" << std::endl;
}

void RenderPassGLES::bindTextures(const std::vector<TextureBinding>& bindings) {
    for (const auto& binding : bindings) {
        bindTexture(binding.texture, binding.textureUnit, binding.samplerName);
    }
    
    std::cout << "[RenderPassGLES] bindTextures: bound " << bindings.size() << " textures" << std::endl;
}

void RenderPassGLES::bindUniformBuffers(const std::vector<UniformBufferBinding>& bindings) {
    for (const auto& binding : bindings) {
        bindUniformBuffer(binding.buffer, binding.bindingPoint, binding.blockName, 
                         binding.offset, binding.size);
    }
    
    std::cout << "[RenderPassGLES] bindUniformBuffers: bound " << bindings.size() << " UBOs" << std::endl;
}

// 5.1.4 新增：统一绘制接口
void RenderPassGLES::draw(const DrawParams& params) {
    if (!mIsInPass) {
        std::cerr << "[RenderPassGLES] Error: draw called outside of pass!" << std::endl;
        return;
    }
    
    // 应用渲染状态
    updateStateIfDirty();
    
    std::cout << "[RenderPassGLES] draw(primitive=" << static_cast<int>(params.primitiveType)
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

void RenderPassGLES::drawInstanced(const DrawParams& params, std::shared_ptr<HYBuffer> instanceData) {
    if (instanceData) {
        mInstanceBuffer = instanceData;
        // TODO: 绑定实例数据缓冲区
        // auto handle = instanceData->getResourceHandle();
        // glBindBuffer(GL_ARRAY_BUFFER, handle.uintHandle);
    }
    
    draw(params);
}

// 5.1.4 新增：状态管理接口
const RenderState& RenderPassGLES::getRenderState() const {
    return mRenderState;
}

void RenderPassGLES::applyRenderState(const RenderState& state) {
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
    
    std::cout << "[RenderPassGLES] applyRenderState completed" << std::endl;
}

void RenderPassGLES::flushRenderState() {
    updateStateIfDirty();
    std::cout << "[RenderPassGLES] flushRenderState" << std::endl;
}

// 5.1.4 新增：工具方法
void RenderPassGLES::autoBindResources(std::shared_ptr<HYShader> shader,
                                      const std::map<std::string, std::shared_ptr<HYTexture>>& textures,
                                      const std::map<std::string, std::shared_ptr<HYBuffer>>& uniformBuffers) {
    if (!shader) {
        std::cerr << "[RenderPassGLES] Error: autoBindResources called with null shader!" << std::endl;
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
    
    std::cout << "[RenderPassGLES] autoBindResources completed (textures=" 
              << textures.size() << ", ubos=" << uniformBuffers.size() << ")" << std::endl;
}

void RenderPassGLES::drawAuto() {
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
void RenderPassGLES::updateStateIfDirty() {
    if (!mRenderState.isDirty) {
        return;
    }
    
    // 更新渲染状态结构
    mRenderState.currentShader = mCurrentShader;
    mRenderState.currentPipelineState = mCurrentPipelineState;
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
    
    std::cout << "[RenderPassGLES] updateStateIfDirty completed" << std::endl;
}

void RenderPassGLES::cacheShaderReflection(std::shared_ptr<HYShader> shader) {
    if (!shader) {
        return;
    }
    
    // 获取着色器反射信息
    const auto& reflection = shader->getReflection();
    
    std::cout << "[RenderPassGLES] cacheShaderReflection: uniforms=" << reflection.uniforms.size()
              << ", attributes=" << reflection.attributes.size() << std::endl;
    
    // 这里可以缓存uniform位置等信息以优化性能
    // 实际实现中可以预先查询所有uniform位置并缓存
}

} // namespace render
} // namespace hyengine
