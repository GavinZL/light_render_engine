#pragma once

/**
 * @file HYMath.hpp
 * @brief HY引擎数学库主头文件
 * 
 * 包含向量、矩阵、四元数及相关数学工具函数
 * 支持模板化设计，可用于float和double类型
 */

#include "math/MathDef.hpp"
#include "math/MathFwd.hpp"

// 向量类型
#include "math/Vec2.hpp"
#include "math/Vec3.hpp"
#include "math/Vec4.hpp"

// 矩阵类型
#include "math/Mat3.hpp"
#include "math/Mat4.hpp"

// 四元数类型
#include "math/Quaternion.hpp"

namespace hyengine {
namespace math {

/**
 * @brief 数学库版本信息
 */
struct MathLibInfo {
    static constexpr int MAJOR_VERSION = 1;
    static constexpr int MINOR_VERSION = 0;
    static constexpr int PATCH_VERSION = 0;
    
    static constexpr const char* VERSION_STRING = "1.0.0";
    static constexpr const char* BUILD_DATE = __DATE__;
    static constexpr const char* BUILD_TIME = __TIME__;
};

} // namespace math
} // namespace hyengine
