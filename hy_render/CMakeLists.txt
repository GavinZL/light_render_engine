cmake_minimum_required(VERSION 3.15)
project(HYRenderEngine VERSION 1.0.0 LANGUAGES CXX)

# C++17标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# 生成编译数据库，用于IDE代码导航和补全
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

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

# 查找依赖包
find_package(PkgConfig)

# GLFW (仅在构建Demo时需要)
if(HY_RENDER_BUILD_DEMO)
    find_package(glfw3 QUIET)
    if(NOT glfw3_FOUND)
        # 如果系统没有安装GLFW，尝试使用pkg-config
        if(PkgConfig_FOUND)
            pkg_check_modules(GLFW QUIET glfw3)
        endif()
        
        # 如果还是找不到，给出提示
        if(NOT GLFW_FOUND)
            message(STATUS "GLFW not found. Demo will use simplified window management.")
            set(HY_RENDER_DEMO_USE_GLFW OFF)
        else()
            set(HY_RENDER_DEMO_USE_GLFW ON)
        endif()
    else()
        set(HY_RENDER_DEMO_USE_GLFW ON)
    endif()
endif()

# OpenGL (桌面版)
if(HY_RENDER_ENABLE_OPENGL)
    find_package(OpenGL QUIET)
    if(NOT OpenGL_FOUND)
        message(STATUS "OpenGL not found, disabling OpenGL support")
        set(HY_RENDER_ENABLE_OPENGL OFF)
    endif()
endif()

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
    src/core/HYCommandBuffer.cpp
    src/core/HYRenderDebugger.cpp
)

# 数学库源文件
set(HY_RENDER_MATH_SOURCES
    src/math/MathUtils.cpp
)

set(HY_RENDER_SOURCES ${HY_RENDER_CORE_SOURCES} ${HY_RENDER_MATH_SOURCES})

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
        src/gl_common/CommandBufferGL.cpp
        src/gl_common/debug/RenderDebuggerGL.cpp
    )
    list(APPEND HY_RENDER_SOURCES ${HY_RENDER_GL_SOURCES})
    add_definitions(-DHY_RENDER_OPENGL)
endif()

# OpenGL ES支持
if(HY_RENDER_ENABLE_OPENGLES)
    # 检查OpenGL ES文件是否存在
    if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/src/opengles)
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
    else()
        message(STATUS "OpenGL ES source files not found, disabling OpenGL ES support")
        set(HY_RENDER_ENABLE_OPENGLES OFF)
    endif()
endif()

# Metal支持
if(HY_RENDER_ENABLE_METAL AND APPLE)
    # 检查Metal文件是否存在
    if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/src/metal)
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
            src/metal/resources/VertexArrayMtl.mm
            src/metal/resources/CommandBufferMtl.mm
            src/metal/debug/RenderDebuggerMtl.mm
        )
        list(APPEND HY_RENDER_SOURCES ${HY_RENDER_METAL_SOURCES})
        add_definitions(-DHY_RENDER_METAL)
    else()
        message(STATUS "Metal source files not found, disabling Metal support")
        set(HY_RENDER_ENABLE_METAL OFF)
    endif()
endif()

# Vulkan支持
if(HY_RENDER_ENABLE_VULKAN)
    # 检查Vulkan文件是否存在
    if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/src/vulkan)
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
    else()
        message(STATUS "Vulkan source files not found, disabling Vulkan support")
        set(HY_RENDER_ENABLE_VULKAN OFF)
    endif()
endif()

# 创建库
add_library(hy_render STATIC ${HY_RENDER_SOURCES})

target_include_directories(hy_render
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
)

# 平台特定链接
if(APPLE AND HY_RENDER_ENABLE_METAL)
    find_library(METAL_LIBRARY Metal)
    find_library(QUARTZCORE_LIBRARY QuartzCore)
    if(METAL_LIBRARY AND QUARTZCORE_LIBRARY)
        target_link_libraries(hy_render PUBLIC ${METAL_LIBRARY} ${QUARTZCORE_LIBRARY})
    else()
        message(STATUS "Metal libraries not found, disabling Metal support")
        set(HY_RENDER_ENABLE_METAL OFF)
    endif()
endif()

if(HY_RENDER_ENABLE_VULKAN)
    find_package(Vulkan QUIET)
    if(Vulkan_FOUND)
        target_link_libraries(hy_render PUBLIC Vulkan::Vulkan)
    else()
        message(STATUS "Vulkan not found, disabling Vulkan support")
        set(HY_RENDER_ENABLE_VULKAN OFF)
    endif()
endif()

if(HY_RENDER_ENABLE_OPENGL AND OpenGL_FOUND)
    target_link_libraries(hy_render PUBLIC OpenGL::GL)
endif()

# 显示配置信息
message(STATUS "HY Render Engine Configuration:")
message(STATUS "  OpenGL Support: ${HY_RENDER_ENABLE_OPENGL}")
message(STATUS "  OpenGL ES Support: ${HY_RENDER_ENABLE_OPENGLES}")
if(APPLE)
    message(STATUS "  Metal Support: ${HY_RENDER_ENABLE_METAL}")
else()
    message(STATUS "  Vulkan Support: ${HY_RENDER_ENABLE_VULKAN}")
endif()
message(STATUS "  Build Demo: ${HY_RENDER_BUILD_DEMO}")
if(HY_RENDER_BUILD_DEMO)
    message(STATUS "  Demo GLFW Support: ${HY_RENDER_DEMO_USE_GLFW}")
endif()

# Demo
if(HY_RENDER_BUILD_DEMO)
    add_subdirectory(demo)
endif()

# Tests
if(HY_RENDER_BUILD_TESTS)
    # 检查tests目录是否存在
    if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/tests)
        enable_testing()
        add_subdirectory(tests)
    else()
        message(STATUS "Tests directory not found, skipping tests")
    endif()
endif()
