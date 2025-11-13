#pragma once

#include "../HYRenderDef.hpp"
#include "../HYMath.hpp"
#include "../resources/HYBuffer.hpp"
#include <memory>
#include <string>
#include <typeinfo>
#include <typeindex>

namespace hyengine {
namespace render {

// 前向声明
class Entity;
class HYVertexArray;

/**
 * @brief 组件类型ID - 用于组件类型识别
 */
using ComponentTypeID = std::type_index;

/**
 * @brief 组件基类
 * 
 * 所有组件都继承自此基类。组件是无状态的数据容器，
 * 负责存储特定类型的数据，由系统进行处理。
 */
class Component {
public:
    Component() : mEnabled(true), mOwner(nullptr) {}
    virtual ~Component() = default;

    /**
     * @brief 获取组件类型ID
     */
    virtual ComponentTypeID getTypeID() const = 0;

    /**
     * @brief 获取组件类型名称
     */
    virtual const char* getTypeName() const = 0;

    /**
     * @brief 组件初始化（当添加到Entity时调用）
     */
    virtual void onAttach() {}

    /**
     * @brief 组件销毁（当从Entity移除时调用）
     */
    virtual void onDetach() {}

    /**
     * @brief 启用/禁用组件
     */
    void setEnabled(bool enabled) { mEnabled = enabled; }
    bool isEnabled() const { return mEnabled; }

    /**
     * @brief 获取所属实体
     */
    Entity* getOwner() const { return mOwner; }

protected:
    friend class Entity;
    
    bool mEnabled;
    Entity* mOwner; // 所属实体
};

/**
 * @brief 组件模板辅助宏 - 简化组件类型定义
 */
#define DECLARE_COMPONENT_TYPE(ComponentClass) \
    ComponentTypeID getTypeID() const override { \
        return std::type_index(typeid(ComponentClass)); \
    } \
    const char* getTypeName() const override { \
        return #ComponentClass; \
    } \
    static ComponentTypeID staticTypeID() { \
        return std::type_index(typeid(ComponentClass)); \
    }

// ============================================================================
// 核心组件定义
// ============================================================================

/**
 * @brief Transform组件 - 存储实体的变换信息
 * 
 * 每个Entity都应该有一个Transform组件来定义其在场景中的位置、旋转和缩放。
 */
class TransformComponent : public Component {
public:
    DECLARE_COMPONENT_TYPE(TransformComponent)

    TransformComponent() 
        : mPosition(0.0f, 0.0f, 0.0f)
        , mRotation(0.0f, 0.0f, 0.0f, 1.0f) // 单位四元数
        , mScale(1.0f, 1.0f, 1.0f)
        , mDirty(true)
    {}

    // 位置
    void setPosition(const hyengine::math::Vec3& position) { 
        mPosition = position; 
        markDirty(); 
    }
    const hyengine::math::Vec3& getPosition() const { return mPosition; }

    // 旋转（使用四元数）
    void setRotation(const hyengine::math::Quat& rotation) { 
        mRotation = rotation; 
        markDirty(); 
    }
    const hyengine::math::Quat& getRotation() const { return mRotation; }

    // 从角度轴设置旋转
    void setRotationFromAxisAngle(float angle, const hyengine::math::Vec3& axis) {
        mRotation.fromAngleAxis(angle, axis);
        markDirty();
    }

    // 缩放
    void setScale(const hyengine::math::Vec3& scale) { 
        mScale = scale; 
        markDirty(); 
    }
    const hyengine::math::Vec3& getScale() const { return mScale; }

    // 获取变换矩阵
    const hyengine::math::Mat4& getLocalMatrix() const {
        if (mDirty) {
            updateMatrix();
        }
        return mLocalMatrix;
    }

    // 方向向量
    hyengine::math::Vec3 getForward() const {
        return mRotation.zAxis(); // Z轴指向前方
    }

    hyengine::math::Vec3 getRight() const {
        return mRotation.xAxis(); // X轴指向右方
    }

    hyengine::math::Vec3 getUp() const {
        return mRotation.yAxis(); // Y轴指向上方
    }

private:
    void markDirty() const { mDirty = true; }
    
    void updateMatrix() const {
        hyengine::math::Mat4 translation = hyengine::math::Mat4::translate(mPosition);
        hyengine::math::Mat4 rotation;
        mRotation.toRotationMatrix(rotation);
        hyengine::math::Mat4 scale = hyengine::math::Mat4::scale(mScale);
        
        mLocalMatrix = translation * rotation * scale;
        mDirty = false;
    }

    hyengine::math::Vec3 mPosition;
    hyengine::math::Quat mRotation;
    hyengine::math::Vec3 mScale;
    
    mutable hyengine::math::Mat4 mLocalMatrix;
    mutable bool mDirty;
};

/**
 * @brief MeshRenderer组件 - 网格渲染器
 * 
 * 存储网格渲染所需的资源引用
 */
class MeshRendererComponent : public Component {
public:
    DECLARE_COMPONENT_TYPE(MeshRendererComponent)

    MeshRendererComponent()
        : mVertexBuffer(nullptr)
        , mIndexBuffer(nullptr)
        , mVisible(true)
        , mCastShadows(true)
        , mReceiveShadows(true)
    {}

    // 顶点缓冲
    void setVertexBuffer(std::shared_ptr<HYBuffer> buffer) {
        mVertexBuffer = buffer;
    }
    std::shared_ptr<HYBuffer> getVertexBuffer() const {
        return mVertexBuffer;
    }

    // 索引缓冲
    void setIndexBuffer(std::shared_ptr<HYBuffer> buffer) {
        mIndexBuffer = buffer;
    }
    std::shared_ptr<HYBuffer> getIndexBuffer() const {
        return mIndexBuffer;
    }

    // 可见性
    void setVisible(bool visible) { mVisible = visible; }
    bool isVisible() const { return mVisible; }

    // 阴影设置
    void setCastShadows(bool cast) { mCastShadows = cast; }
    bool getCastShadows() const { return mCastShadows; }
    
    void setReceiveShadows(bool receive) { mReceiveShadows = receive; }
    bool getReceiveShadows() const { return mReceiveShadows; }

private:
    std::shared_ptr<HYBuffer> mVertexBuffer;
    std::shared_ptr<HYBuffer> mIndexBuffer;
    
    bool mVisible;
    bool mCastShadows;
    bool mReceiveShadows;
};

/**
 * @brief Camera组件 - 相机
 * 
 * 定义场景的观察视角
 */
class CameraComponent : public Component {
public:
    DECLARE_COMPONENT_TYPE(CameraComponent)

    enum class ProjectionType {
        kPerspective,
        kOrthographic
    };

    CameraComponent()
        : mProjectionType(ProjectionType::kPerspective)
        , mFov(45.0f)
        , mAspect(16.0f / 9.0f)
        , mNearPlane(0.1f)
        , mFarPlane(1000.0f)
        , mOrthoSize(10.0f)
        , mDirty(true)
        , mIsPrimary(false)
    {}

    // 投影类型
    void setProjectionType(ProjectionType type) {
        mProjectionType = type;
        markDirty();
    }
    ProjectionType getProjectionType() const { return mProjectionType; }

    // 透视投影参数
    void setPerspective(float fov, float aspect, float nearPlane, float farPlane) {
        mFov = fov;
        mAspect = aspect;
        mNearPlane = nearPlane;
        mFarPlane = farPlane;
        mProjectionType = ProjectionType::kPerspective;
        markDirty();
    }

    // 正交投影参数
    void setOrthographic(float orthoSize, float aspect, float nearPlane, float farPlane) {
        mOrthoSize = orthoSize;
        mAspect = aspect;
        mNearPlane = nearPlane;
        mFarPlane = farPlane;
        mProjectionType = ProjectionType::kOrthographic;
        markDirty();
    }

    // 获取投影矩阵
    const hyengine::math::Mat4& getProjectionMatrix() const {
        if (mDirty) {
            updateProjectionMatrix();
        }
        return mProjectionMatrix;
    }

    // 主相机标记
    void setPrimary(bool isPrimary) { mIsPrimary = isPrimary; }
    bool isPrimary() const { return mIsPrimary; }

    // 参数访问
    float getFov() const { return mFov; }
    void setFov(float fov) { mFov = fov; markDirty(); }

    float getAspect() const { return mAspect; }
    void setAspect(float aspect) { mAspect = aspect; markDirty(); }

    float getNearPlane() const { return mNearPlane; }
    void setNearPlane(float nearPlane) { mNearPlane = nearPlane; markDirty(); }

    float getFarPlane() const { return mFarPlane; }
    void setFarPlane(float farPlane) { mFarPlane = farPlane; markDirty(); }

private:
    void markDirty() const { mDirty = true; }

    void updateProjectionMatrix() const {
        if (mProjectionType == ProjectionType::kPerspective) {
            mProjectionMatrix = hyengine::math::Mat4::perspective(
                hyengine::math::degToRad(mFov), mAspect, mNearPlane, mFarPlane
            );
        } else {
            float halfWidth = mOrthoSize * mAspect * 0.5f;
            float halfHeight = mOrthoSize * 0.5f;
            mProjectionMatrix = hyengine::math::Mat4::ortho(
                -halfWidth, halfWidth, -halfHeight, halfHeight, mNearPlane, mFarPlane
            );
        }
        mDirty = false;
    }

    ProjectionType mProjectionType;
    
    // 透视投影参数
    float mFov;
    float mAspect;
    float mNearPlane;
    float mFarPlane;
    
    // 正交投影参数
    float mOrthoSize;
    
    mutable hyengine::math::Mat4 mProjectionMatrix;
    mutable bool mDirty;
    
    bool mIsPrimary; // 是否为主相机
};

/**
 * @brief Light组件 - 光源
 * 
 * 定义场景中的光照
 */
class LightComponent : public Component {
public:
    DECLARE_COMPONENT_TYPE(LightComponent)

    enum class LightType {
        kDirectional,  // 方向光
        kPoint,        // 点光源
        kSpot          // 聚光灯
    };

    LightComponent()
        : mType(LightType::kDirectional)
        , mColor(1.0f, 1.0f, 1.0f)
        , mIntensity(1.0f)
        , mRange(10.0f)
        , mSpotAngle(45.0f)
        , mCastShadows(false)
    {}

    // 光源类型
    void setType(LightType type) { mType = type; }
    LightType getType() const { return mType; }

    // 光源颜色
    void setColor(const hyengine::math::Vec3& color) { mColor = color; }
    const hyengine::math::Vec3& getColor() const { return mColor; }

    // 光照强度
    void setIntensity(float intensity) { mIntensity = intensity; }
    float getIntensity() const { return mIntensity; }

    // 光源范围（点光源和聚光灯）
    void setRange(float range) { mRange = range; }
    float getRange() const { return mRange; }

    // 聚光灯角度
    void setSpotAngle(float angle) { mSpotAngle = angle; }
    float getSpotAngle() const { return mSpotAngle; }

    // 阴影
    void setCastShadows(bool cast) { mCastShadows = cast; }
    bool getCastShadows() const { return mCastShadows; }

private:
    LightType mType;
    hyengine::math::Vec3 mColor;
    float mIntensity;
    float mRange;
    float mSpotAngle;
    bool mCastShadows;
};

} // namespace render
} // namespace hyengine
