#pragma once

#include "RenderEntity.hpp"
#include "../HYMath.hpp"
#include <vector>
#include <algorithm>
#include <unordered_map>
#include <memory>

namespace hyengine {
namespace render {

/**
 * @brief RenderQueue - 渲染队列
 * 
 * 管理渲染实体的排序和批处理，优化渲染性能。
 */
class RenderQueue {
public:
    /**
     * @brief 渲染批次
     */
    struct RenderBatch {
        RenderEntity::RenderLayer layer;
        std::vector<std::shared_ptr<RenderEntity>> entities;
        uint32_t estimatedComplexity = 0;
        
        RenderBatch(RenderEntity::RenderLayer l) : layer(l) {}
    };

    /**
     * @brief 渲染命令
     */
    struct RenderCommand {
        std::shared_ptr<RenderEntity> entity;
        uint64_t sortKey;
        float distanceToCamera;
        uint32_t materialID;
        uint32_t geometryID;
        
        RenderCommand(std::shared_ptr<RenderEntity> e) 
            : entity(e)
            , sortKey(e->getRenderSortKey())
            , distanceToCamera(0.0f)
            , materialID(0)
            , geometryID(0)
        {}
    };

    RenderQueue() = default;
    virtual ~RenderQueue() = default;

    // ========================================================================
    // 渲染队列构建
    // ========================================================================

    /**
     * @brief 添加渲染实体到队列
     */
    void addRenderEntity(std::shared_ptr<RenderEntity> entity) {
        if (!entity || !entity->isVisible() || !entity->hasValidRenderResources()) {
            return;
        }
        
        mRenderCommands.emplace_back(entity);
    }

    /**
     * @brief 批量添加渲染实体
     */
    void addRenderEntities(const std::vector<std::shared_ptr<RenderEntity>>& entities) {
        mRenderCommands.reserve(mRenderCommands.size() + entities.size());
        
        for (auto& entity : entities) {
            if (entity && entity->isVisible() && entity->hasValidRenderResources()) {
                mRenderCommands.emplace_back(entity);
            }
        }
    }

    /**
     * @brief 清空渲染队列
     */
    void clear() {
        mRenderCommands.clear();
        mRenderBatches.clear();
    }

    // ========================================================================
    // 视锥体剔除
    // ========================================================================

    /**
     * @brief 执行视锥体剔除
     * @param cameraEntity 相机实体
     * @param cullingMask 剔除掩码
     */
    void performFrustumCulling(std::shared_ptr<Entity> cameraEntity, uint32_t cullingMask = 0xFFFFFFFF) {
        if (!cameraEntity) return;
        
        auto camera = cameraEntity->getComponent<CameraComponent>();
        auto cameraTransform = cameraEntity->getTransform();
        
        if (!camera || !cameraTransform) return;
        
        // 计算相机位置
        hyengine::math::Vec3 cameraPos = cameraTransform->getPosition();
        
        // 移除被剔除的实体
        mRenderCommands.erase(
            std::remove_if(mRenderCommands.begin(), mRenderCommands.end(),
                [cullingMask, cameraPos](RenderCommand& cmd) {
                    // 检查剔除掩码
                    if (cmd.entity->shouldBeCulledBy(cullingMask)) {
                        return true;
                    }
                    
                    // 计算到相机的距离
                    hyengine::math::Vec3 entityPos = cmd.entity->getWorldPosition();
                    cmd.distanceToCamera = (entityPos - cameraPos).length();
                    
                    // 这里可以添加更复杂的视锥体剔除逻辑
                    // 简单的距离剔除
                    float maxDistance = 1000.0f; // 最大渲染距离
                    return cmd.distanceToCamera > maxDistance;
                }),
            mRenderCommands.end()
        );
    }

    // ========================================================================
    // 排序和批处理
    // ========================================================================

    /**
     * @brief 对渲染命令进行排序
     */
    void sortRenderCommands() {
        std::sort(mRenderCommands.begin(), mRenderCommands.end(),
            [](const RenderCommand& a, const RenderCommand& b) {
                // 首先按渲染层级排序
                if (a.entity->getRenderLayer() != b.entity->getRenderLayer()) {
                    return a.entity->getRenderLayer() < b.entity->getRenderLayer();
                }
                
                // 对于不透明物体，从前往后排序（减少overdraw）
                if (a.entity->getRenderLayer() == RenderEntity::RenderLayer::kOpaque) {
                    if (a.distanceToCamera != b.distanceToCamera) {
                        return a.distanceToCamera < b.distanceToCamera;
                    }
                }
                
                // 对于透明物体，从后往前排序（正确的混合）
                if (a.entity->getRenderLayer() == RenderEntity::RenderLayer::kTransparent) {
                    if (a.distanceToCamera != b.distanceToCamera) {
                        return a.distanceToCamera > b.distanceToCamera;
                    }
                }
                
                // 最后按排序顺序排序
                return a.entity->getSortingOrder() < b.entity->getSortingOrder();
            });
    }

    /**
     * @brief 创建渲染批次
     */
    void createRenderBatches() {
        mRenderBatches.clear();
        
        if (mRenderCommands.empty()) return;
        
        // 按层级分组
        std::unordered_map<RenderEntity::RenderLayer, std::vector<std::shared_ptr<RenderEntity>>> layerGroups;
        
        for (const auto& cmd : mRenderCommands) {
            layerGroups[cmd.entity->getRenderLayer()].push_back(cmd.entity);
        }
        
        // 为每个层级创建批次
        for (auto& pair : layerGroups) {
            RenderBatch batch(pair.first);
            batch.entities = std::move(pair.second);
            
            // 计算批次复杂度
            for (const auto& entity : batch.entities) {
                batch.estimatedComplexity += entity->getEstimatedRenderComplexity();
            }
            
            mRenderBatches.push_back(std::move(batch));
        }
        
        // 按层级排序批次
        std::sort(mRenderBatches.begin(), mRenderBatches.end(),
            [](const RenderBatch& a, const RenderBatch& b) {
                return a.layer < b.layer;
            });
    }

    // ========================================================================
// 查询接口
    // ========================================================================

    /**
     * @brief 获取渲染命令
     */
    const std::vector<RenderCommand>& getRenderCommands() const {
        return mRenderCommands;
    }

    /**
     * @brief 获取渲染批次
     */
    const std::vector<RenderBatch>& getRenderBatches() const {
        return mRenderBatches;
    }

    /**
     * @brief 获取指定层级的实体
     */
    std::vector<std::shared_ptr<RenderEntity>> getEntitiesByLayer(RenderEntity::RenderLayer layer) const {
        std::vector<std::shared_ptr<RenderEntity>> entities;
        
        for (const auto& cmd : mRenderCommands) {
            if (cmd.entity->getRenderLayer() == layer) {
                entities.push_back(cmd.entity);
            }
        }
        
        return entities;
    }

    /**
     * @brief 获取统计信息
     */
    struct RenderStats {
        size_t totalCommands = 0;
        size_t totalBatches = 0;
        uint32_t totalComplexity = 0;
        size_t entitiesByLayer[static_cast<size_t>(RenderEntity::RenderLayer::kMax)] = {0};
    };

    RenderStats getRenderStats() const {
        RenderStats stats;
        stats.totalCommands = mRenderCommands.size();
        stats.totalBatches = mRenderBatches.size();
        
        for (const auto& batch : mRenderBatches) {
            stats.totalComplexity += batch.estimatedComplexity;
            stats.entitiesByLayer[static_cast<size_t>(batch.layer)] += batch.entities.size();
        }
        
        return stats;
    }

    // ========================================================================
    // 高级功能
    // ========================================================================

    /**
     * @brief 执行完整的渲染准备流程
     */
    void prepareForRender(std::shared_ptr<Entity> cameraEntity, uint32_t cullingMask = 0xFFFFFFFF) {
        // 1. 视锥体剔除
        performFrustumCulling(cameraEntity, cullingMask);
        
        // 2. 排序
        sortRenderCommands();
        
        // 3. 创建批次
        createRenderBatches();
    }

    /**
     * @brief 根据标签过滤实体
     */
    void filterByTag(const std::string& tag, bool include = true) {
        mRenderCommands.erase(
            std::remove_if(mRenderCommands.begin(), mRenderCommands.end(),
                [&tag, include](const RenderCommand& cmd) {
                    bool hasTag = cmd.entity->hasRenderTag(tag);
                    return include ? !hasTag : hasTag;
                }),
            mRenderCommands.end()
        );
    }

    /**
     * @brief 根据LOD级别过滤实体
     */
    void filterByLOD(uint8_t maxLODLevel) {
        mRenderCommands.erase(
            std::remove_if(mRenderCommands.begin(), mRenderCommands.end(),
                [maxLODLevel](const RenderCommand& cmd) {
                    return cmd.entity->getLODLevel() > maxLODLevel;
                }),
            mRenderCommands.end()
        );
    }

private:
    std::vector<RenderCommand> mRenderCommands;
    std::vector<RenderBatch> mRenderBatches;
};

} // namespace render
} // namespace hyengine