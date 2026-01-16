#pragma once

/**
 * @file HYResourceCommon.hpp
 * @brief 所有平台资源实现的公共定义
 * 
 * 这个文件包含了 OpenGL、OpenGL ES、Metal、Vulkan 等所有平台实现
 * 共同需要的类型定义和基类声明。
 * 
 * 使用说明：
 * - 所有平台的资源类头文件（如 BufferGL.hpp, BufferGLES.hpp）都应该包含此文件
 * - 不要在各个资源类中重复定义这些类型
 * - 新增公共类型时，应该添加到 HYRenderDef.hpp 或此文件中
 */

// 包含公共定义
#include "../../include/hy_render/HYRenderDef.hpp"

// 包含所有资源基类
#include "../../include/hy_render/resources/HYBuffer.hpp"
#include "../../include/hy_render/resources/HYTexture.hpp"
#include "../../include/hy_render/resources/HYShader.hpp"
#include "../../include/hy_render/resources/HYSampler.hpp"
#include "../../include/hy_render/resources/HYFrameBuffer.hpp"
#include "../../include/hy_render/resources/HYPipelineState.hpp"
#include "../../include/hy_render/resources/HYRenderPass.hpp"
#include "../../include/hy_render/resources/HYVertexArray.hpp"
#include "../../include/hy_render/resources/HYFence.hpp"

#include <memory>
#include <string>

namespace hyengine {
namespace render {

// 此文件不需要额外的定义，所有必要的类型都已经包含在上述头文件中

} // namespace render
} // namespace hyengine
