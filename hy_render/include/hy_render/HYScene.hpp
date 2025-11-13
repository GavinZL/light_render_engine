#pragma once

/**
 * @file HYScene.hpp
 * @brief HY渲染引擎场景图系统主头文件
 * 
 * 包含Entity-Component-System (ECS)架构的所有核心类：
 * - Component: 组件基类及核心组件（Transform, MeshRenderer, Camera, Light）
 * - Entity: 场景实体，组件的容器
 * - RenderEntity: 专门用于渲染的特化实体
 * - ComponentSystem: 系统基类及核心系统（RenderSystem, CameraSystem）
 * - Scene: 场景管理器，负责实体和系统的生命周期管理
 * - SceneManager: 多场景管理器
 * - RenderQueue: 渲染队列和批处理管理
 * - EntityQuery: 实体查询系统
 */

#include "scene/Component.hpp"
#include "scene/Entity.hpp"
#include "scene/RenderEntity.hpp"
#include "scene/ComponentSystem.hpp"
#include "scene/Scene.hpp"
#include "scene/SceneManager.hpp"
#include "scene/RenderQueue.hpp"
#include "scene/EntityQuery.hpp"

namespace hyengine {
namespace render {

/**
 * @brief 场景图系统版本信息
 */
struct SceneGraphInfo {
    static constexpr int MAJOR_VERSION = 1;
    static constexpr int MINOR_VERSION = 0;
    static constexpr int PATCH_VERSION = 0;
    
    static constexpr const char* VERSION_STRING = "1.0.0";
};

} // namespace render
} // namespace hyengine
