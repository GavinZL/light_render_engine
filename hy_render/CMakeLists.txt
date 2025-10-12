cmake_minimum_required(VERSION 3.15)
project(HYRenderEngine VERSION 1.0.0 LANGUAGES CXX)

# C++17标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# 编译选项
if(MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# 平台检测
if(APPLE)
    option(HY_RENDER_ENABLE_METAL "Enable Metal support" ON)
    option(HY_RENDER_ENABLE_OPENGL "Enable OpenGL support" ON)
else()
    option(HY_RENDER_ENABLE_VULKAN "Enable Vulkan support" ON)
    option(HY_RENDER_ENABLE_OPENGL "Enable OpenGL support" ON)
endif()

option(HY_RENDER_ENABLE_OPENGLES "Enable OpenGL ES support" ON)
option(HY_RENDER_BUILD_DEMO "Build demo" ON)
option(HY_RENDER_BUILD_TESTS "Build tests" ON)

# 源文件
set(HY_RENDER_CORE_SOURCES
    src/core/HYRenderEngine.cpp
    src/core/HYRenderContext.cpp
    src/core/resources/HYBuffer.cpp
    src/core/resources/HYTexture.cpp
    src/core/resources/HYShader.cpp
    src/core/resources/HYFrameBuffer.cpp
    src/core/resources/HYPipelineState.cpp
    src/core/resources/HYRenderPass.cpp
    src/core/resources/HYSampler.cpp
    src/core/resources/HYVertexArray.cpp
    src/core/resources/HYFence.cpp
)

set(HY_RENDER_SOURCES ${HY_RENDER_CORE_SOURCES})

# OpenGL支持
if(HY_RENDER_ENABLE_OPENGL)
    set(HY_RENDER_GL_SOURCES
        src/opengl/RenderContextGL.cpp
        src/opengl/resources/BufferGL.cpp
        src/opengl/resources/TextureGL.cpp
        src/opengl/resources/ShaderGL.cpp
        src/opengl/resources/FrameBufferGL.cpp
        src/opengl/resources/PipelineStateGL.cpp
        src/opengl/resources/RenderPassGL.cpp
        src/opengl/resources/SamplerGL.cpp
        src/opengl/resources/VertexArrayGL.cpp
        src/opengl/resources/FenceGL.cpp
        src/gl_common/RenderContextGLBase.cpp
        src/gl_common/GLUtils.cpp
    )
    list(APPEND HY_RENDER_SOURCES ${HY_RENDER_GL_SOURCES})
    add_definitions(-DHY_RENDER_OPENGL)
endif()

# OpenGL ES支持
if(HY_RENDER_ENABLE_OPENGLES)
    set(HY_RENDER_GLES_SOURCES
        src/opengles/RenderContextGLES.cpp
        src/opengles/resources/BufferGLES.cpp
        src/opengles/resources/TextureGLES.cpp
        src/opengles/resources/ShaderGLES.cpp
        src/opengles/resources/FrameBufferGLES.cpp
        src/opengles/resources/PipelineStateGLES.cpp
        src/opengles/resources/RenderPassGLES.cpp
        src/opengles/resources/SamplerGLES.cpp
        src/opengles/resources/VertexArrayGLES.cpp
        src/opengles/resources/FenceGLES.cpp
        src/gl_common/RenderContextGLBase.cpp
        src/gl_common/GLUtils.cpp
    )
    list(APPEND HY_RENDER_SOURCES ${HY_RENDER_GLES_SOURCES})
    add_definitions(-DHY_RENDER_OPENGLES)
endif()

# Metal支持
if(HY_RENDER_ENABLE_METAL AND APPLE)
    set(HY_RENDER_METAL_SOURCES
        src/metal/RenderContextMtl.mm
        src/metal/resources/BufferMtl.mm
        src/metal/resources/TextureMtl.mm
        src/metal/resources/ShaderMtl.mm
        src/metal/resources/FrameBufferMtl.mm
        src/metal/resources/PipelineStateMtl.mm
        src/metal/resources/RenderPassMtl.mm
        src/metal/resources/SamplerMtl.mm
        src/metal/resources/FenceMtl.mm
    )
    list(APPEND HY_RENDER_SOURCES ${HY_RENDER_METAL_SOURCES})
    add_definitions(-DHY_RENDER_METAL)
endif()

# Vulkan支持
if(HY_RENDER_ENABLE_VULKAN)
    set(HY_RENDER_VULKAN_SOURCES
        src/vulkan/RenderContextVk.cpp
        src/vulkan/resources/BufferVk.cpp
        src/vulkan/resources/TextureVk.cpp
        src/vulkan/resources/ShaderVk.cpp
        src/vulkan/resources/FrameBufferVk.cpp
        src/vulkan/resources/PipelineStateVk.cpp
        src/vulkan/resources/RenderPassVk.cpp
        src/vulkan/resources/SamplerVk.cpp
        src/vulkan/resources/FenceVk.cpp
    )
    list(APPEND HY_RENDER_SOURCES ${HY_RENDER_VULKAN_SOURCES})
    add_definitions(-DHY_RENDER_VULKAN)
endif()

# 创建库
add_library(hy_render STATIC ${HY_RENDER_SOURCES})

target_include_directories(hy_render
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# 平台特定链接
if(APPLE AND HY_RENDER_ENABLE_METAL)
    find_library(METAL_LIBRARY Metal)
    find_library(QUARTZCORE_LIBRARY QuartzCore)
    target_link_libraries(hy_render PUBLIC ${METAL_LIBRARY} ${QUARTZCORE_LIBRARY})
endif()

if(HY_RENDER_ENABLE_VULKAN)
    find_package(Vulkan REQUIRED)
    target_link_libraries(hy_render PUBLIC Vulkan::Vulkan)
endif()

# Demo
if(HY_RENDER_BUILD_DEMO)
    add_subdirectory(demo)
endif()

# Tests
if(HY_RENDER_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()
