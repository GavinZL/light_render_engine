#pragma once

#include "Entity.hpp"
#include "ComponentSystem.hpp"
#include <memory>
#include <vector>
#include <unordered_map>
#include <string>
#include <algorithm>

namespace hyengine {
namespace render {

/**
 * @brief Scene - 场景类
 * 
 * 管理场景中的所有实体和系统。
 * 负责实体的创建、销毁、更新和系统管理。
 */
class Scene {
public:
    Scene(const std::string& name = "Scene")
        : mName(name)
        , mNextEntityID(1)
    {}

    virtual ~Scene() = default;

    /**
     * @brief 获取场景名称
     */
    const std::string& getName() const { return mName; }
    void setName(const std::string& name) { mName = name; }

    // ========================================================================
    // 实体管理
    // ========================================================================

    /**
     * @brief 创建新实体
     * @param name 实体名称
     * @return 新创建的实体
     */
    std::shared_ptr<Entity> createEntity(const std::string& name = "Entity") {
        auto entity = std::make_shared<Entity>(mNextEntityID++, name);
        entity->setScene(this);
        
        mEntities[entity->getID()] = entity;
        mEntityList.push_back(entity);
        
        // 通知所有系统
        for (auto& system : mSystems) {
            if (system->isEnabled()) {
                system->onEntityAdded(entity);
            }
        }
        
        return entity;
    }

    /**
     * @brief 销毁实体
     * @param entity 要销毁的实体
     */
    void destroyEntity(std::shared_ptr<Entity> entity) {
        if (!entity) return;
        
        // 递归销毁所有子实体
        auto children = entity->getChildren();
        for (auto& child : children) {
            destroyEntity(child);
        }
        
        // 通知所有系统
        for (auto& system : mSystems) {
            if (system->isEnabled()) {
                system->onEntityRemoved(entity);
            }
        }
        
        // 从父节点移除
        if (entity->getParent()) {
            entity->setParent(nullptr);
        }
        
        // 从场景中移除
        entity->setScene(nullptr);
        
        auto id = entity->getID();
        mEntities.erase(id);
        
        auto it = std::find_if(mEntityList.begin(), mEntityList.end(),
            [&entity](const std::weak_ptr<Entity>& weakEntity) {
                auto ptr = weakEntity.lock();
                return ptr && ptr == entity;
            });
        
        if (it != mEntityList.end()) {
            mEntityList.erase(it);
        }
    }

    /**
     * @brief 根据ID查找实体
     */
    std::shared_ptr<Entity> findEntityByID(Entity::EntityID id) const {
        auto it = mEntities.find(id);
        if (it != mEntities.end()) {
            return it->second;
        }
        return nullptr;
    }

    /**
     * @brief 根据名称查找实体
     */
    std::shared_ptr<Entity> findEntityByName(const std::string& name) const {
        for (const auto& weakEntity : mEntityList) {
            if (auto entity = weakEntity.lock()) {
                if (entity->getName() == name) {
                    return entity;
                }
            }
        }
        return nullptr;
    }

    /**
     * @brief 获取所有实体
     */
    std::vector<std::shared_ptr<Entity>> getAllEntities() const {
        std::vector<std::shared_ptr<Entity>> entities;
        entities.reserve(mEntityList.size());
        
        for (const auto& weakEntity : mEntityList) {
            if (auto entity = weakEntity.lock()) {
                entities.push_back(entity);
            }
        }
        
        return entities;
    }

    /**
     * @brief 获取实体数量
     */
    size_t getEntityCount() const {
        return mEntities.size();
    }

    /**
     * @brief 清除所有实体
     */
    void clearEntities() {
        // 通知所有系统
        for (const auto& weakEntity : mEntityList) {
            if (auto entity = weakEntity.lock()) {
                for (auto& system : mSystems) {
                    if (system->isEnabled()) {
                        system->onEntityRemoved(entity);
                    }
                }
            }
        }
        
        mEntities.clear();
        mEntityList.clear();
    }

    /**
     * @brief 添加已创建的实体到场景中
     * @param entity 已创建的实体
     */
    void addExistingEntity(std::shared_ptr<Entity> entity) {
        if (!entity) return;
        
        entity->setScene(this);
        
        mEntities[entity->getID()] = entity;
        mEntityList.push_back(entity);
        
        // 通知所有系统
        for (auto& system : mSystems) {
            if (system->isEnabled()) {
                system->onEntityAdded(entity);
            }
        }
    }

    // ========================================================================
    // 系统管理
    // ========================================================================

    /**
     * @brief 添加系统
     * @tparam T 系统类型
     * @tparam Args 构造函数参数类型
     * @return 添加的系统指针
     */
    template<typename T, typename... Args>
    T* addSystem(Args&&... args) {
        static_assert(std::is_base_of<ComponentSystem, T>::value, "T must derive from ComponentSystem");
        
        auto system = std::make_shared<T>(std::forward<Args>(args)...);
        mSystems.push_back(system);
        
        // 按优先级排序系统
        std::sort(mSystems.begin(), mSystems.end(),
            [](const std::shared_ptr<ComponentSystem>& a, const std::shared_ptr<ComponentSystem>& b) {
                return a->getPriority() < b->getPriority();
            });
        
        system->initialize();
        
        // 将现有实体添加到新系统
        for (const auto& weakEntity : mEntityList) {
            if (auto entity = weakEntity.lock()) {
                system->onEntityAdded(entity);
            }
        }
        
        return system.get();
    }

    /**
     * @brief 获取系统
     * @tparam T 系统类型
     * @return 系统指针，如果不存在返回nullptr
     */
    template<typename T>
    T* getSystem() const {
        static_assert(std::is_base_of<ComponentSystem, T>::value, "T must derive from ComponentSystem");
        
        for (const auto& system : mSystems) {
            if (auto casted = dynamic_cast<T*>(system.get())) {
                return casted;
            }
        }
        
        return nullptr;
    }

    /**
     * @brief 移除系统
     * @tparam T 系统类型
     */
    template<typename T>
    void removeSystem() {
        static_assert(std::is_base_of<ComponentSystem, T>::value, "T must derive from ComponentSystem");
        
        auto it = std::find_if(mSystems.begin(), mSystems.end(),
            [](const std::shared_ptr<ComponentSystem>& system) {
                return dynamic_cast<T*>(system.get()) != nullptr;
            });
        
        if (it != mSystems.end()) {
            (*it)->shutdown();
            mSystems.erase(it);
        }
    }

    /**
     * @brief 获取所有系统
     */
    const std::vector<std::shared_ptr<ComponentSystem>>& getAllSystems() const {
        return mSystems;
    }

    // ========================================================================
    // 更新
    // ========================================================================

    /**
     * @brief 更新场景
     * @param deltaTime 帧时间间隔（秒）
     */
    void update(float deltaTime) {
        // 按优先级顺序更新所有系统
        for (auto& system : mSystems) {
            if (system->isEnabled()) {
                system->update(deltaTime);
            }
        }
    }

private:
    std::string mName;
    Entity::EntityID mNextEntityID;
    
    // 实体存储
    std::unordered_map<Entity::EntityID, std::shared_ptr<Entity>> mEntities;
    std::vector<std::weak_ptr<Entity>> mEntityList;
    
    // 系统存储
    std::vector<std::shared_ptr<ComponentSystem>> mSystems;
};

} // namespace render
} // namespace hyengine
