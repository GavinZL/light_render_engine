#pragma once

#include "Scene.hpp"
#include "RenderEntity.hpp"
#include <memory>
#include <unordered_map>
#include <vector>
#include <string>
#include <functional>

namespace hyengine {
namespace render {

/**
 * @brief SceneManager - 场景管理器
 * 
 * 负责管理多个场景的生命周期，场景切换，以及全局的场景相关功能。
 */
class SceneManager {
public:
    using SceneID = uint32_t;
    using SceneTransitionCallback = std::function<void(std::shared_ptr<Scene>, std::shared_ptr<Scene>)>;

    SceneManager() 
        : mNextSceneID(1)
        , mActiveScene(nullptr) 
    {}

    virtual ~SceneManager() = default;

    // ========================================================================
    // 场景生命周期管理
    // ========================================================================

    /**
     * @brief 创建新场景
     */
    std::shared_ptr<Scene> createScene(const std::string& name = "Scene") {
        auto scene = std::make_shared<Scene>(name);
        SceneID id = mNextSceneID++;
        
        mScenes[id] = scene;
        mSceneNameToID[name] = id;
        
        // 如果这是第一个场景，设置为活动场景
        if (!mActiveScene) {
            setActiveScene(scene);
        }
        
        return scene;
    }

    /**
     * @brief 加载场景（从文件或数据）
     */
    std::shared_ptr<Scene> loadScene(const std::string& sceneData, const std::string& name = "LoadedScene") {
        auto scene = createScene(name);
        
        // TODO: 实现场景数据的解析和加载
        // 这里可以添加JSON/XML等格式的场景文件解析
        
        return scene;
    }

    /**
     * @brief 卸载场景
     */
    void unloadScene(std::shared_ptr<Scene> scene) {
        if (!scene) return;
        
        SceneID sceneID = 0;
        for (auto& pair : mScenes) {
            if (pair.second == scene) {
                sceneID = pair.first;
                break;
            }
        }
        
        if (sceneID != 0) {
            // 如果是活动场景，清除活动场景
            if (mActiveScene == scene) {
                mActiveScene = nullptr;
            }
            
            // 从映射中移除
            mSceneNameToID.erase(scene->getName());
            mScenes.erase(sceneID);
        }
    }

    /**
     * @brief 根据名称卸载场景
     */
    void unloadScene(const std::string& name) {
        auto scene = findSceneByName(name);
        if (scene) {
            unloadScene(scene);
        }
    }

    // ========================================================================
    // 场景切换
    // ========================================================================

    /**
     * @brief 设置活动场景
     */
    void setActiveScene(std::shared_ptr<Scene> scene) {
        if (mActiveScene == scene) return;
        
        auto oldScene = mActiveScene;
        mActiveScene = scene;
        
        // 调用场景切换回调
        if (mSceneTransitionCallback) {
            mSceneTransitionCallback(oldScene, mActiveScene);
        }
    }

    /**
     * @brief 根据名称设置活动场景
     */
    void setActiveScene(const std::string& name) {
        auto scene = findSceneByName(name);
        if (scene) {
            setActiveScene(scene);
        }
    }

    /**
     * @brief 获取活动场景
     */
    std::shared_ptr<Scene> getActiveScene() const {
        return mActiveScene;
    }

    /**
     * @brief 设置场景切换回调
     */
    void setSceneTransitionCallback(const SceneTransitionCallback& callback) {
        mSceneTransitionCallback = callback;
    }

    // ========================================================================
    // 场景查询
    // ========================================================================

    /**
     * @brief 根据名称查找场景
     */
    std::shared_ptr<Scene> findSceneByName(const std::string& name) const {
        auto it = mSceneNameToID.find(name);
        if (it != mSceneNameToID.end()) {
            auto sceneIt = mScenes.find(it->second);
            if (sceneIt != mScenes.end()) {
                return sceneIt->second;
            }
        }
        return nullptr;
    }

    /**
     * @brief 获取所有场景
     */
    std::vector<std::shared_ptr<Scene>> getAllScenes() const {
        std::vector<std::shared_ptr<Scene>> scenes;
        scenes.reserve(mScenes.size());
        
        for (const auto& pair : mScenes) {
            scenes.push_back(pair.second);
        }
        
        return scenes;
    }

    /**
     * @brief 获取场景数量
     */
    size_t getSceneCount() const {
        return mScenes.size();
    }

    // ========================================================================
    // 全局更新
    // ========================================================================

    /**
     * @brief 更新活动场景
     */
    void update(float deltaTime) {
        if (mActiveScene) {
            mActiveScene->update(deltaTime);
        }
        
        // 处理延迟的场景操作
        processDelayedOperations();
    }

    /**
     * @brief 更新所有场景（用于特殊情况）
     */
    void updateAllScenes(float deltaTime) {
        for (auto& pair : mScenes) {
            if (pair.second) {
                pair.second->update(deltaTime);
            }
        }
        
        processDelayedOperations();
    }

    // ========================================================================
    // RenderEntity专用功能
    // ========================================================================

    /**
     * @brief 创建RenderEntity并添加到活动场景
     */
    std::shared_ptr<RenderEntity> createRenderEntity(const std::string& name = "RenderEntity") {
        if (!mActiveScene) {
            throw std::runtime_error("No active scene to create RenderEntity");
        }
        
        // 使用RenderEntity的工厂方法创建
        auto renderEntity = RenderEntity::create(name);
        
        // 将其作为普通Entity添加到场景（因为RenderEntity继承自Entity）
        mActiveScene->addExistingEntity(renderEntity);
        
        return renderEntity;
    }

    /**
     * @brief 获取活动场景中的所有RenderEntity
     */
    std::vector<std::shared_ptr<RenderEntity>> getAllRenderEntities() const {
        std::vector<std::shared_ptr<RenderEntity>> renderEntities;
        
        if (mActiveScene) {
            auto allEntities = mActiveScene->getAllEntities();
            for (auto& entity : allEntities) {
                auto renderEntity = std::dynamic_pointer_cast<RenderEntity>(entity);
                if (renderEntity) {
                    renderEntities.push_back(renderEntity);
                }
            }
        }
        
        return renderEntities;
    }

    /**
     * @brief 根据渲染层级获取RenderEntity
     */
    std::vector<std::shared_ptr<RenderEntity>> getRenderEntitiesByLayer(RenderEntity::RenderLayer layer) const {
        std::vector<std::shared_ptr<RenderEntity>> entities;
        
        auto allRenderEntities = getAllRenderEntities();
        for (auto& entity : allRenderEntities) {
            if (entity->getRenderLayer() == layer) {
                entities.push_back(entity);
            }
        }
        
        return entities;
    }

    /**
     * @brief 根据标签获取RenderEntity
     */
    std::vector<std::shared_ptr<RenderEntity>> getRenderEntitiesByTag(const std::string& tag) const {
        std::vector<std::shared_ptr<RenderEntity>> entities;
        
        auto allRenderEntities = getAllRenderEntities();
        for (auto& entity : allRenderEntities) {
            if (entity->hasRenderTag(tag)) {
                entities.push_back(entity);
            }
        }
        
        return entities;
    }

    // ========================================================================
    // 场景统计信息
    // ========================================================================

    /**
     * @brief 获取场景统计信息
     */
    struct SceneStats {
        size_t totalScenes = 0;
        size_t totalEntities = 0;
        size_t totalRenderEntities = 0;
        size_t visibleRenderEntities = 0;
        std::string activeSceneName;
    };

    SceneStats getSceneStats() const {
        SceneStats stats;
        stats.totalScenes = mScenes.size();
        
        if (mActiveScene) {
            stats.activeSceneName = mActiveScene->getName();
            stats.totalEntities = mActiveScene->getEntityCount();
            
            auto renderEntities = getAllRenderEntities();
            stats.totalRenderEntities = renderEntities.size();
            
            for (auto& entity : renderEntities) {
                if (entity->isVisible()) {
                    stats.visibleRenderEntities++;
                }
            }
        }
        
        return stats;
    }

    // ========================================================================
    // 延迟操作处理
    // ========================================================================

private:
    void processDelayedOperations() {
        // 处理延迟的场景切换、实体创建/销毁等操作
        // 避免在更新循环中直接修改场景结构
    }

private:
    SceneID mNextSceneID;
    std::unordered_map<SceneID, std::shared_ptr<Scene>> mScenes;
    std::unordered_map<std::string, SceneID> mSceneNameToID;
    std::shared_ptr<Scene> mActiveScene;
    
    SceneTransitionCallback mSceneTransitionCallback;
};

} // namespace render
} // namespace hyengine