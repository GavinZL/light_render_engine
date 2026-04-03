#pragma once

#include "Entity.hpp"
#include "../resources/HYShader.hpp"
#include "../resources/HYTexture.hpp"
#include "../resources/HYBuffer.hpp"
#include <unordered_set>
#include <string>

namespace hyengine {
namespace render {

// 前向声明
class Material;

/**
 * @brief RenderEntity - 专门用于渲染的实体
 * 
 * RenderEntity是Entity的特化版本，专门为渲染系统优化，
 * 包含更多渲染相关的功能和属性。
 */
class RenderEntity : public Entity {
public:
    /**
     * @brief 渲染层级枚举
     */
    enum class RenderLayer : uint8_t {
        kBackground = 0,    // 背景层
        kOpaque = 1,        // 不透明物体
        kTransparent = 2,   // 透明物体
        kOverlay = 3,       // 覆盖层
        kUI = 4,           // UI层
        kDebug = 5,        // 调试层
        kMax = 6
    };

    /**
     * @brief 渲染标签
     */
    using RenderTag = std::string;

    RenderEntity(EntityID id, const std::string& name = "RenderEntity")
        : Entity(id, name)
        , mRenderLayer(RenderLayer::kOpaque)
        , mSortingOrder(0)
        , mCastShadows(true)
        , mReceiveShadows(true)
        , mStaticBatching(false)
        , mInstanced(false)
        , mCullingMask(0xFFFFFFFF)
        , mLODLevel(0)
    {
        // RenderEntity默认包含MeshRenderer组件
        if (!hasComponent<MeshRendererComponent>()) {
            addComponent<MeshRendererComponent>();
        }
    }

    virtual ~RenderEntity() = default;

    // ========================================================================
    // 渲染层级和排序
    // ========================================================================

    /**
     * @brief 设置/获取渲染层级
     */
    void setRenderLayer(RenderLayer layer) { mRenderLayer = layer; }
    RenderLayer getRenderLayer() const { return mRenderLayer; }

    /**
     * @brief 设置/获取排序顺序（同层级内的排序）
     */
    void setSortingOrder(int32_t order) { mSortingOrder = order; }
    int32_t getSortingOrder() const { return mSortingOrder; }

    /**
     * @brief 获取最终的渲染排序键
     */
    uint64_t getRenderSortKey() const {
        // 高32位：渲染层级
        // 低32位：排序顺序
        return (static_cast<uint64_t>(mRenderLayer) << 32) | 
               (static_cast<uint64_t>(mSortingOrder) & 0xFFFFFFFF);
    }

    // ========================================================================
    // 阴影设置
    // ========================================================================

    /**
     * @brief 设置是否投射阴影
     */
    void setCastShadows(bool cast) { 
        mCastShadows = cast;
        auto renderer = getComponent<MeshRendererComponent>();
        if (renderer) {
            renderer->setCastShadows(cast);
        }
    }
    bool getCastShadows() const { return mCastShadows; }

    /**
     * @brief 设置是否接收阴影
     */
    void setReceiveShadows(bool receive) { 
        mReceiveShadows = receive;
        auto renderer = getComponent<MeshRendererComponent>();
        if (renderer) {
            renderer->setReceiveShadows(receive);
        }
    }
    bool getReceiveShadows() const { return mReceiveShadows; }

    // ========================================================================
    // 批处理优化
    // ========================================================================

    /**
     * @brief 设置静态批处理标记
     */
    void setStaticBatching(bool batching) { mStaticBatching = batching; }
    bool isStaticBatchingEnabled() const { return mStaticBatching; }

    /**
     * @brief 设置实例化渲染标记
     */
    void setInstanced(bool instanced) { mInstanced = instanced; }
    bool isInstanced() const { return mInstanced; }

    // ========================================================================
    // 视锥体剔除
    // ========================================================================

    /**
     * @brief 设置剔除掩码
     */
    void setCullingMask(uint32_t mask) { mCullingMask = mask; }
    uint32_t getCullingMask() const { return mCullingMask; }

    /**
     * @brief 检查是否应该被指定层剔除
     */
    bool shouldBeCulledBy(uint32_t cullingLayers) const {
        return (mCullingMask & cullingLayers) == 0;
    }

    // ========================================================================
    // LOD系统
    // ========================================================================

    /**
     * @brief 设置LOD级别
     */
    void setLODLevel(uint8_t level) { mLODLevel = level; }
    uint8_t getLODLevel() const { return mLODLevel; }

    // ========================================================================
    // 标签系统
    // ========================================================================

    /**
     * @brief 添加渲染标签
     */
    void addRenderTag(const RenderTag& tag) {
        mRenderTags.insert(tag);
    }

    /**
     * @brief 移除渲染标签
     */
    void removeRenderTag(const RenderTag& tag) {
        mRenderTags.erase(tag);
    }

    /**
     * @brief 检查是否有指定标签
     */
    bool hasRenderTag(const RenderTag& tag) const {
        return mRenderTags.find(tag) != mRenderTags.end();
    }

    /**
     * @brief 获取所有标签
     */
    const std::unordered_set<RenderTag>& getRenderTags() const {
        return mRenderTags;
    }

    /**
     * @brief 清除所有标签
     */
    void clearRenderTags() {
        mRenderTags.clear();
    }

    // ========================================================================
    // 渲染信息查询
    // ========================================================================

    /**
     * @brief 检查实体是否可见
     */
    bool isVisible() const {
        if (!isEnabled()) return false;
        
        auto renderer = getComponent<MeshRendererComponent>();
        return renderer && renderer->isEnabled() && renderer->isVisible();
    }

    /**
     * @brief 获取渲染器组件（便捷方法）
     */
    MeshRendererComponent* getMeshRenderer() const {
        return getComponent<MeshRendererComponent>();
    }

    /**
     * @brief 检查是否有有效的渲染资源
     */
    bool hasValidRenderResources() const {
        auto renderer = getMeshRenderer();
        if (!renderer) return false;
        
        return renderer->getVertexBuffer() != nullptr;
    }

    /**
     * @brief 获取估算的渲染复杂度
     */
    uint32_t getEstimatedRenderComplexity() const {
        // 简单的复杂度估算，实际应用中可以更精确
        uint32_t complexity = 1;
        
        auto renderer = getMeshRenderer();
        if (renderer && renderer->getVertexBuffer()) {
            // 基于顶点数量估算
            complexity += renderer->getVertexBuffer()->getSize() / 32; // 假设每个顶点32字节
        }
        
        // 透明物体需要额外的复杂度
        if (mRenderLayer == RenderLayer::kTransparent) {
            complexity *= 2;
        }
        
        return complexity;
    }

    // ========================================================================
    // 工厂方法
    // ========================================================================

    /**
     * @brief 创建一个RenderEntity
     */
    static std::shared_ptr<RenderEntity> create(const std::string& name = "RenderEntity") {
        static EntityID nextID = 10000; // 从10000开始，避免与普通Entity冲突
        return std::make_shared<RenderEntity>(nextID++, name);
    }

private:
    RenderLayer mRenderLayer;
    int32_t mSortingOrder;
    
    bool mCastShadows;
    bool mReceiveShadows;
    bool mStaticBatching;
    bool mInstanced;
    
    uint32_t mCullingMask;
    uint8_t mLODLevel;
    
    std::unordered_set<RenderTag> mRenderTags;
};

} // namespace render
} // namespace hyengine