#pragma once

#include "Entity.hpp"
#include "RenderEntity.hpp"
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <functional>
#include <memory>

namespace hyengine {
namespace render {

/**
 * @brief EntityQuery - 实体查询系统
 * 
 * 提供强大的实体查询和过滤功能，支持组件类型、标签、属性等多种查询方式。
 */
class EntityQuery {
public:
    /**
     * @brief 查询结果
     */
    using QueryResult = std::vector<std::shared_ptr<Entity>>;
    
    /**
     * @brief 查询条件函数类型
     */
    using QueryPredicate = std::function<bool(std::shared_ptr<Entity>)>;

    /**
     * @brief 构造函数
     */
    EntityQuery() = default;
    virtual ~EntityQuery() = default;

    // ========================================================================
    // 基础查询
    // ========================================================================

    /**
     * @brief 设置查询的实体列表
     */
    EntityQuery& from(const std::vector<std::shared_ptr<Entity>>& entities) {
        mEntities = entities;
        return *this;
    }

    /**
     * @brief 根据组件类型查询
     */
    template<typename ComponentType>
    EntityQuery& withComponent() {
        mQueryPredicates.emplace_back([](std::shared_ptr<Entity> entity) {
            return entity->hasComponent<ComponentType>();
        });
        return *this;
    }

    /**
     * @brief 查询没有指定组件的实体
     */
    template<typename ComponentType>
    EntityQuery& withoutComponent() {
        mQueryPredicates.emplace_back([](std::shared_ptr<Entity> entity) {
            return !entity->hasComponent<ComponentType>();
        });
        return *this;
    }

    /**
     * @brief 根据名称查询
     */
    EntityQuery& withName(const std::string& name) {
        mQueryPredicates.emplace_back([name](std::shared_ptr<Entity> entity) {
            return entity->getName() == name;
        });
        return *this;
    }

    /**
     * @brief 根据名称前缀查询
     */
    EntityQuery& withNamePrefix(const std::string& prefix) {
        mQueryPredicates.emplace_back([prefix](std::shared_ptr<Entity> entity) {
            return entity->getName().substr(0, prefix.length()) == prefix;
        });
        return *this;
    }

    /**
     * @brief 查询启用的实体
     */
    EntityQuery& enabled(bool isEnabled = true) {
        mQueryPredicates.emplace_back([isEnabled](std::shared_ptr<Entity> entity) {
            return entity->isEnabled() == isEnabled;
        });
        return *this;
    }

    // ========================================================================
    // RenderEntity特化查询
    // ========================================================================

    /**
     * @brief 查询RenderEntity
     */
    EntityQuery& renderEntitiesOnly() {
        mQueryPredicates.emplace_back([](std::shared_ptr<Entity> entity) {
            return std::dynamic_pointer_cast<RenderEntity>(entity) != nullptr;
        });
        return *this;
    }

    /**
     * @brief 根据渲染层级查询
     */
    EntityQuery& withRenderLayer(RenderEntity::RenderLayer layer) {
        mQueryPredicates.emplace_back([layer](std::shared_ptr<Entity> entity) {
            auto renderEntity = std::dynamic_pointer_cast<RenderEntity>(entity);
            return renderEntity && renderEntity->getRenderLayer() == layer;
        });
        return *this;
    }

    /**
     * @brief 根据渲染标签查询
     */
    EntityQuery& withRenderTag(const std::string& tag) {
        mQueryPredicates.emplace_back([tag](std::shared_ptr<Entity> entity) {
            auto renderEntity = std::dynamic_pointer_cast<RenderEntity>(entity);
            return renderEntity && renderEntity->hasRenderTag(tag);
        });
        return *this;
    }

    /**
     * @brief 查询可见的RenderEntity
     */
    EntityQuery& visible() {
        mQueryPredicates.emplace_back([](std::shared_ptr<Entity> entity) {
            auto renderEntity = std::dynamic_pointer_cast<RenderEntity>(entity);
            return renderEntity && renderEntity->isVisible();
        });
        return *this;
    }

    /**
     * @brief 根据LOD级别查询
     */
    EntityQuery& withLODLevel(uint8_t maxLevel) {
        mQueryPredicates.emplace_back([maxLevel](std::shared_ptr<Entity> entity) {
            auto renderEntity = std::dynamic_pointer_cast<RenderEntity>(entity);
            return renderEntity && renderEntity->getLODLevel() <= maxLevel;
        });
        return *this;
    }

    // ========================================================================
    // 空间查询
    // ========================================================================

    /**
     * @brief 根据位置范围查询
     */
    EntityQuery& inSphere(const hyengine::math::Vec3& center, float radius) {
        mQueryPredicates.emplace_back([center, radius](std::shared_ptr<Entity> entity) {
            auto transform = entity->getTransform();
            if (!transform) return false;
            
            hyengine::math::Vec3 pos = entity->getWorldPosition();
            float distance = (pos - center).length();
            return distance <= radius;
        });
        return *this;
    }

    /**
     * @brief 根据包围盒查询
     */
    EntityQuery& inBox(const hyengine::math::Vec3& min, const hyengine::math::Vec3& max) {
        mQueryPredicates.emplace_back([min, max](std::shared_ptr<Entity> entity) {
            auto transform = entity->getTransform();
            if (!transform) return false;
            
            hyengine::math::Vec3 pos = entity->getWorldPosition();
            return pos.x >= min.x && pos.x <= max.x &&
                   pos.y >= min.y && pos.y <= max.y &&
                   pos.z >= min.z && pos.z <= max.z;
        });
        return *this;
    }

    /**
     * @brief 根据距离查询
     */
    EntityQuery& nearPoint(const hyengine::math::Vec3& point, float maxDistance) {
        mQueryPredicates.emplace_back([point, maxDistance](std::shared_ptr<Entity> entity) {
            auto transform = entity->getTransform();
            if (!transform) return false;
            
            hyengine::math::Vec3 pos = entity->getWorldPosition();
            float distance = (pos - point).length();
            return distance <= maxDistance;
        });
        return *this;
    }

    // ========================================================================
    // 层级查询
    // ========================================================================

    /**
     * @brief 查询根实体（没有父节点）
     */
    EntityQuery& rootEntities() {
        mQueryPredicates.emplace_back([](std::shared_ptr<Entity> entity) {
            return entity->getParent() == nullptr;
        });
        return *this;
    }

    /**
     * @brief 查询子实体
     */
    EntityQuery& childrenOf(std::shared_ptr<Entity> parent) {
        mQueryPredicates.emplace_back([parent](std::shared_ptr<Entity> entity) {
            return entity->getParent() == parent.get();
        });
        return *this;
    }

    /**
     * @brief 查询后代实体（递归）
     */
    EntityQuery& descendantsOf(std::shared_ptr<Entity> ancestor) {
        mQueryPredicates.emplace_back([ancestor](std::shared_ptr<Entity> entity) {
            Entity* parent = entity->getParent();
            while (parent) {
                if (parent == ancestor.get()) {
                    return true;
                }
                parent = parent->getParent();
            }
            return false;
        });
        return *this;
    }

    // ========================================================================
    // 自定义查询
    // ========================================================================

    /**
     * @brief 添加自定义查询条件
     */
    EntityQuery& where(const QueryPredicate& predicate) {
        mQueryPredicates.emplace_back(predicate);
        return *this;
    }

    // ========================================================================
    // 执行查询
    // ========================================================================

    /**
     * @brief 执行查询并返回结果
     */
    QueryResult execute() const {
        QueryResult results;
        
        for (const auto& entity : mEntities) {
            if (!entity) continue;
            
            bool matches = true;
            for (const auto& predicate : mQueryPredicates) {
                if (!predicate(entity)) {
                    matches = false;
                    break;
                }
            }
            
            if (matches) {
                results.push_back(entity);
            }
        }
        
        return results;
    }

    /**
     * @brief 获取第一个匹配的实体
     */
    std::shared_ptr<Entity> first() const {
        for (const auto& entity : mEntities) {
            if (!entity) continue;
            
            bool matches = true;
            for (const auto& predicate : mQueryPredicates) {
                if (!predicate(entity)) {
                    matches = false;
                    break;
                }
            }
            
            if (matches) {
                return entity;
            }
        }
        
        return nullptr;
    }

    /**
     * @brief 计算匹配实体的数量
     */
    size_t count() const {
        size_t matchCount = 0;
        
        for (const auto& entity : mEntities) {
            if (!entity) continue;
            
            bool matches = true;
            for (const auto& predicate : mQueryPredicates) {
                if (!predicate(entity)) {
                    matches = false;
                    break;
                }
            }
            
            if (matches) {
                matchCount++;
            }
        }
        
        return matchCount;
    }

    /**
     * @brief 检查是否有匹配的实体
     */
    bool any() const {
        return first() != nullptr;
    }

    /**
     * @brief 重置查询条件
     */
    EntityQuery& reset() {
        mQueryPredicates.clear();
        return *this;
    }

private:
    std::vector<std::shared_ptr<Entity>> mEntities;
    std::vector<QueryPredicate> mQueryPredicates;
};

/**
 * @brief TagManager - 标签管理器
 * 
 * 管理实体的标签系统，支持快速的基于标签的查询。
 */
class TagManager {
public:
    using TaggedEntities = std::unordered_set<std::shared_ptr<Entity>>;

    TagManager() = default;
    virtual ~TagManager() = default;

    /**
     * @brief 为实体添加标签
     */
    void addTag(std::shared_ptr<Entity> entity, const std::string& tag) {
        if (!entity) return;
        
        mTagToEntities[tag].insert(entity);
        mEntityToTags[entity].insert(tag);
    }

    /**
     * @brief 从实体移除标签
     */
    void removeTag(std::shared_ptr<Entity> entity, const std::string& tag) {
        if (!entity) return;
        
        auto tagIt = mTagToEntities.find(tag);
        if (tagIt != mTagToEntities.end()) {
            tagIt->second.erase(entity);
            if (tagIt->second.empty()) {
                mTagToEntities.erase(tagIt);
            }
        }
        
        auto entityIt = mEntityToTags.find(entity);
        if (entityIt != mEntityToTags.end()) {
            entityIt->second.erase(tag);
            if (entityIt->second.empty()) {
                mEntityToTags.erase(entityIt);
            }
        }
    }

    /**
     * @brief 移除实体的所有标签
     */
    void removeAllTags(std::shared_ptr<Entity> entity) {
        if (!entity) return;
        
        auto entityIt = mEntityToTags.find(entity);
        if (entityIt != mEntityToTags.end()) {
            for (const auto& tag : entityIt->second) {
                auto tagIt = mTagToEntities.find(tag);
                if (tagIt != mTagToEntities.end()) {
                    tagIt->second.erase(entity);
                    if (tagIt->second.empty()) {
                        mTagToEntities.erase(tagIt);
                    }
                }
            }
            mEntityToTags.erase(entityIt);
        }
    }

    /**
     * @brief 检查实体是否有指定标签
     */
    bool hasTag(std::shared_ptr<Entity> entity, const std::string& tag) const {
        auto entityIt = mEntityToTags.find(entity);
        if (entityIt != mEntityToTags.end()) {
            return entityIt->second.find(tag) != entityIt->second.end();
        }
        return false;
    }

    /**
     * @brief 获取带有指定标签的所有实体
     */
    std::vector<std::shared_ptr<Entity>> getEntitiesWithTag(const std::string& tag) const {
        std::vector<std::shared_ptr<Entity>> entities;
        
        auto it = mTagToEntities.find(tag);
        if (it != mTagToEntities.end()) {
            entities.reserve(it->second.size());
            for (const auto& entity : it->second) {
                entities.push_back(entity);
            }
        }
        
        return entities;
    }

    /**
     * @brief 获取实体的所有标签
     */
    std::vector<std::string> getEntityTags(std::shared_ptr<Entity> entity) const {
        std::vector<std::string> tags;
        
        auto it = mEntityToTags.find(entity);
        if (it != mEntityToTags.end()) {
            tags.reserve(it->second.size());
            for (const auto& tag : it->second) {
                tags.push_back(tag);
            }
        }
        
        return tags;
    }

    /**
     * @brief 获取所有标签
     */
    std::vector<std::string> getAllTags() const {
        std::vector<std::string> tags;
        tags.reserve(mTagToEntities.size());
        
        for (const auto& pair : mTagToEntities) {
            tags.push_back(pair.first);
        }
        
        return tags;
    }

    /**
     * @brief 清理无效的实体引用
     */
    void cleanup() {
        // 移除已经销毁的实体引用
        for (auto tagIt = mTagToEntities.begin(); tagIt != mTagToEntities.end();) {
            auto& entities = tagIt->second;
            for (auto entityIt = entities.begin(); entityIt != entities.end();) {
                if (entityIt->use_count() == 0) {
                    entityIt = entities.erase(entityIt);
                } else {
                    ++entityIt;
                }
            }
            
            if (entities.empty()) {
                tagIt = mTagToEntities.erase(tagIt);
            } else {
                ++tagIt;
            }
        }
        
        // 清理实体到标签的映射
        for (auto entityIt = mEntityToTags.begin(); entityIt != mEntityToTags.end();) {
            if (entityIt->first.use_count() == 0) {
                entityIt = mEntityToTags.erase(entityIt);
            } else {
                ++entityIt;
            }
        }
    }

private:
    std::unordered_map<std::string, TaggedEntities> mTagToEntities;
    std::unordered_map<std::shared_ptr<Entity>, std::unordered_set<std::string>> mEntityToTags;
};

} // namespace render
} // namespace hyengine