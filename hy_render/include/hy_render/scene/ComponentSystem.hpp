#pragma once

#include "Entity.hpp"
#include <vector>
#include <memory>
#include <string>

namespace hyengine {
namespace render {

/**
 * @brief Component System - 组件系统基类
 * 
 * 系统负责处理特定类型组件的逻辑。
 * 每个系统可以处理一个或多个组件类型的实体。
 */
class ComponentSystem {
public:
    ComponentSystem(const std::string& name) 
        : mName(name)
        , mEnabled(true)
        , mPriority(0)
    {}
    
    virtual ~ComponentSystem() = default;

    /**
     * @brief 系统初始化
     */
    virtual void initialize() {}

    /**
     * @brief 系统清理
     */
    virtual void shutdown() {}

    /**
     * @brief 系统更新
     * @param deltaTime 帧时间间隔（秒）
     */
    virtual void update(float deltaTime) = 0;

    /**
     * @brief 实体添加到系统时调用
     * @param entity 实体
     */
    virtual void onEntityAdded(std::shared_ptr<Entity> entity) {}

    /**
     * @brief 实体从系统移除时调用
     * @param entity 实体
     */
    virtual void onEntityRemoved(std::shared_ptr<Entity> entity) {}

    /**
     * @brief 获取系统名称
     */
    const std::string& getName() const { return mName; }

    /**
     * @brief 启用/禁用系统
     */
    void setEnabled(bool enabled) { mEnabled = enabled; }
    bool isEnabled() const { return mEnabled; }

    /**
     * @brief 设置系统优先级（数值越小越先执行）
     */
    void setPriority(int priority) { mPriority = priority; }
    int getPriority() const { return mPriority; }

protected:
    std::string mName;
    bool mEnabled;
    int mPriority;
};

/**
 * @brief Render System - 渲染系统
 * 
 * 处理所有具有MeshRendererComponent的实体的渲染
 */
class RenderSystem : public ComponentSystem {
public:
    RenderSystem() : ComponentSystem("RenderSystem") {}

    void update(float deltaTime) override {
        // 渲染系统的更新逻辑
        // 通常在这里收集所有需要渲染的实体，按材质/深度等排序，然后提交给渲染器
    }

    void onEntityAdded(std::shared_ptr<Entity> entity) override {
        // 检查实体是否有MeshRendererComponent
        if (entity->hasComponent<MeshRendererComponent>()) {
            mRenderEntities.push_back(entity);
        }
    }

    void onEntityRemoved(std::shared_ptr<Entity> entity) override {
        auto it = std::find_if(mRenderEntities.begin(), mRenderEntities.end(),
            [&entity](const std::weak_ptr<Entity>& weakEntity) {
                auto ptr = weakEntity.lock();
                return ptr && ptr == entity;
            });
        
        if (it != mRenderEntities.end()) {
            mRenderEntities.erase(it);
        }
    }

    /**
     * @brief 获取所有可渲染实体
     */
    std::vector<std::shared_ptr<Entity>> getRenderableEntities() const {
        std::vector<std::shared_ptr<Entity>> entities;
        entities.reserve(mRenderEntities.size());
        
        for (const auto& weakEntity : mRenderEntities) {
            if (auto entity = weakEntity.lock()) {
                if (entity->isEnabled()) {
                    auto renderer = entity->getComponent<MeshRendererComponent>();
                    if (renderer && renderer->isEnabled() && renderer->isVisible()) {
                        entities.push_back(entity);
                    }
                }
            }
        }
        
        return entities;
    }

private:
    std::vector<std::weak_ptr<Entity>> mRenderEntities;
};

/**
 * @brief Camera System - 相机系统
 * 
 * 管理场景中的相机
 */
class CameraSystem : public ComponentSystem {
public:
    CameraSystem() : ComponentSystem("CameraSystem") {}

    void update(float deltaTime) override {
        // 相机系统的更新逻辑
    }

    void onEntityAdded(std::shared_ptr<Entity> entity) override {
        if (entity->hasComponent<CameraComponent>()) {
            mCameraEntities.push_back(entity);
            
            // 如果这是第一个相机或者是主相机，设置为当前激活相机
            auto camera = entity->getComponent<CameraComponent>();
            auto activePtr = mActiveCamera.lock();
            if (!activePtr || camera->isPrimary()) {
                mActiveCamera = entity;
            }
        }
    }

    void onEntityRemoved(std::shared_ptr<Entity> entity) override {
        auto it = std::find_if(mCameraEntities.begin(), mCameraEntities.end(),
            [&entity](const std::weak_ptr<Entity>& weakEntity) {
                auto ptr = weakEntity.lock();
                return ptr && ptr == entity;
            });
        
        if (it != mCameraEntities.end()) {
            mCameraEntities.erase(it);
        }
        
        // 如果移除的是激活相机，切换到第一个可用相机
        if (auto active = mActiveCamera.lock()) {
            if (active == entity) {
                mActiveCamera.reset();
                if (!mCameraEntities.empty()) {
                    mActiveCamera = mCameraEntities[0];
                }
            }
        }
    }

    /**
     * @brief 获取激活的相机实体
     */
    std::shared_ptr<Entity> getActiveCamera() const {
        return mActiveCamera.lock();
    }

    /**
     * @brief 设置激活的相机
     */
    void setActiveCamera(std::shared_ptr<Entity> camera) {
        if (camera && camera->hasComponent<CameraComponent>()) {
            mActiveCamera = camera;
        }
    }

    /**
     * @brief 获取所有相机实体
     */
    std::vector<std::shared_ptr<Entity>> getAllCameras() const {
        std::vector<std::shared_ptr<Entity>> cameras;
        cameras.reserve(mCameraEntities.size());
        
        for (const auto& weakCamera : mCameraEntities) {
            if (auto camera = weakCamera.lock()) {
                cameras.push_back(camera);
            }
        }
        
        return cameras;
    }

private:
    std::vector<std::weak_ptr<Entity>> mCameraEntities;
    std::weak_ptr<Entity> mActiveCamera;
};

} // namespace render
} // namespace hyengine
