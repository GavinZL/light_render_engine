#pragma once

#include "Component.hpp"
#include <memory>
#include <vector>
#include <unordered_map>
#include <string>
#include <cassert>

namespace hyengine {
namespace render {

// 前向声明
class Scene;

/**
 * @brief Entity - 场景中的实体对象
 * 
 * Entity是一个容器，用于组合多个Component来定义游戏对象的行为。
 * Entity本身不包含逻辑，所有逻辑都由Component和System处理。
 */
class Entity : public std::enable_shared_from_this<Entity> {
public:
    using EntityID = uint64_t;

    /**
     * @brief 构造函数
     * @param id 实体唯一ID
     * @param name 实体名称
     */
    Entity(EntityID id, const std::string& name = "Entity")
        : mID(id)
        , mName(name)
        , mEnabled(true)
        , mParent(nullptr)
        , mScene(nullptr)
    {
        // 每个Entity默认包含Transform组件
        addComponent<TransformComponent>();
    }

    virtual ~Entity() = default;

    /**
     * @brief 获取实体ID
     */
    EntityID getID() const { return mID; }

    /**
     * @brief 获取/设置实体名称
     */
    const std::string& getName() const { return mName; }
    void setName(const std::string& name) { mName = name; }

    /**
     * @brief 启用/禁用实体
     */
    void setEnabled(bool enabled) { mEnabled = enabled; }
    bool isEnabled() const { return mEnabled; }

    /**
     * @brief 添加组件
     * @tparam T 组件类型
     * @tparam Args 构造函数参数类型
     * @return 添加的组件指针
     */
    template<typename T, typename... Args>
    T* addComponent(Args&&... args) {
        static_assert(std::is_base_of<Component, T>::value, "T must derive from Component");
        
        ComponentTypeID typeID = T::staticTypeID();
        
        // 检查是否已存在该类型组件
        if (hasComponent<T>()) {
            assert(false && "Component already exists on this entity");
            return nullptr;
        }
        
        // 创建组件
        auto component = std::make_shared<T>(std::forward<Args>(args)...);
        component->mOwner = this;
        
        // 添加到组件列表
        mComponents[typeID] = component;
        mComponentList.push_back(component);
        
        // 调用组件的onAttach回调
        component->onAttach();
        
        return component.get();
    }

    /**
     * @brief 获取组件
     * @tparam T 组件类型
     * @return 组件指针，如果不存在返回nullptr
     */
    template<typename T>
    T* getComponent() const {
        static_assert(std::is_base_of<Component, T>::value, "T must derive from Component");
        
        ComponentTypeID typeID = T::staticTypeID();
        auto it = mComponents.find(typeID);
        
        if (it != mComponents.end()) {
            return static_cast<T*>(it->second.get());
        }
        
        return nullptr;
    }

    /**
     * @brief 检查是否有某类型组件
     * @tparam T 组件类型
     * @return true表示有该组件
     */
    template<typename T>
    bool hasComponent() const {
        static_assert(std::is_base_of<Component, T>::value, "T must derive from Component");
        
        ComponentTypeID typeID = T::staticTypeID();
        return mComponents.find(typeID) != mComponents.end();
    }

    /**
     * @brief 移除组件
     * @tparam T 组件类型
     */
    template<typename T>
    void removeComponent() {
        static_assert(std::is_base_of<Component, T>::value, "T must derive from Component");
        
        ComponentTypeID typeID = T::staticTypeID();
        auto it = mComponents.find(typeID);
        
        if (it != mComponents.end()) {
            // 调用组件的onDetach回调
            it->second->onDetach();
            
            // 从列表中移除
            auto listIt = std::find(mComponentList.begin(), mComponentList.end(), it->second);
            if (listIt != mComponentList.end()) {
                mComponentList.erase(listIt);
            }
            
            // 从映射中移除
            mComponents.erase(it);
        }
    }

    /**
     * @brief 获取所有组件
     */
    const std::vector<std::shared_ptr<Component>>& getAllComponents() const {
        return mComponentList;
    }

    /**
     * @brief Transform组件快捷访问
     */
    TransformComponent* getTransform() const {
        return getComponent<TransformComponent>();
    }

    // ========================================================================
    // 层级结构管理
    // ========================================================================

    /**
     * @brief 设置父实体
     */
    void setParent(Entity* parent) {
        if (mParent == parent) return;
        
        // 从旧父节点移除
        if (mParent) {
            auto it = std::find_if(mParent->mChildren.begin(), mParent->mChildren.end(),
                [this](const std::weak_ptr<Entity>& child) {
                    auto ptr = child.lock();
                    return ptr && ptr.get() == this;
                });
            if (it != mParent->mChildren.end()) {
                mParent->mChildren.erase(it);
            }
        }
        
        // 设置新父节点
        mParent = parent;
        
        // 添加到新父节点
        if (mParent) {
            mParent->mChildren.push_back(shared_from_this());
        }
    }

    /**
     * @brief 获取父实体
     */
    Entity* getParent() const { return mParent; }

    /**
     * @brief 添加子实体
     */
    void addChild(std::shared_ptr<Entity> child) {
        if (!child) return;
        child->setParent(this);
    }

    /**
     * @brief 移除子实体
     */
    void removeChild(Entity* child) {
        if (!child) return;
        child->setParent(nullptr);
    }

    /**
     * @brief 获取所有子实体
     */
    std::vector<std::shared_ptr<Entity>> getChildren() const {
        std::vector<std::shared_ptr<Entity>> children;
        children.reserve(mChildren.size());
        
        for (const auto& weakChild : mChildren) {
            if (auto child = weakChild.lock()) {
                children.push_back(child);
            }
        }
        
        return children;
    }

    /**
     * @brief 获取子实体数量
     */
    size_t getChildCount() const {
        return mChildren.size();
    }

    /**
     * @brief 根据名称查找子实体
     */
    std::shared_ptr<Entity> findChildByName(const std::string& name) const {
        for (const auto& weakChild : mChildren) {
            if (auto child = weakChild.lock()) {
                if (child->getName() == name) {
                    return child;
                }
            }
        }
        return nullptr;
    }

    /**
     * @brief 获取世界变换矩阵
     */
    hyengine::math::Mat4 getWorldMatrix() const {
        if (mParent) {
            return mParent->getWorldMatrix() * getTransform()->getLocalMatrix();
        }
        return getTransform()->getLocalMatrix();
    }

    /**
     * @brief 获取世界位置
     */
    hyengine::math::Vec3 getWorldPosition() const {
        if (mParent) {
            auto worldMat = getWorldMatrix();
            return worldMat.getTrans();
        }
        return getTransform()->getPosition();
    }

    /**
     * @brief 设置所属场景
     */
    void setScene(Scene* scene) { mScene = scene; }
    
    /**
     * @brief 获取所属场景
     */
    Scene* getScene() const { return mScene; }

private:
    EntityID mID;
    std::string mName;
    bool mEnabled;
    
    // 组件存储
    std::unordered_map<ComponentTypeID, std::shared_ptr<Component>> mComponents;
    std::vector<std::shared_ptr<Component>> mComponentList;  // 用于顺序遍历
    
    // 层级结构
    Entity* mParent;
    std::vector<std::weak_ptr<Entity>> mChildren;
    
    // 所属场景
    Scene* mScene;
};

} // namespace render
} // namespace hyengine
