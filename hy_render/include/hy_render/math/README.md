# HY引擎数学库 (HYMath)

## 概述

HYMath是HY渲染引擎的核心数学库，提供3D图形编程所需的基础数学类型和工具函数。

## 设计特点

### 1. 模板化设计
- 支持float和double类型
- 类型安全的数学运算
- 编译时优化

### 2. 向量类型
- **Vec2/Vec3/Vec4**: 2D/3D/4D向量
- 支持常见运算：加减乘除、点积、叉积、归一化等
- 提供整数和浮点数版本

### 3. 矩阵类型
- **Mat3**: 3x3矩阵（用于2D变换、法线变换）
- **Mat4**: 4x4矩阵（用于3D变换、投影）
- 支持矩阵运算：乘法、转置、求逆、分解等

### 4. 四元数
- **Quaternion**: 用于旋转表示
- 避免万向节死锁问题
- 球面线性插值(SLERP)支持

### 5. 工具函数
- 角度弧度转换
- 插值函数（lerp, smoothstep）
- 数学常量（PI, EPSILON等）
- 坐标系转换

## 目录结构

```
include/hy_render/math/
├── MathDef.hpp          # 数学常量和基础工具函数
├── MathFwd.hpp          # 类型前置声明和别名
├── Vec2.hpp             # 2D向量（待实现）
├── Vec3.hpp             # 3D向量（待实现）
├── Vec4.hpp             # 4D向量（待实现）
├── Mat3.hpp             # 3x3矩阵（待实现）
├── Mat4.hpp             # 4x4矩阵（待实现）
├── Quaternion.hpp       # 四元数（待实现）
└── MathUtils.hpp        # 数学工具函数（待实现）

src/math/
├── Vec2.cpp             # 向量实现（待实现）
├── Vec3.cpp
├── Vec4.cpp
├── Mat3.cpp             # 矩阵实现（待实现）
├── Mat4.cpp
├── Quaternion.cpp       # 四元数实现（待实现）
└── MathUtils.cpp        # 工具函数实现（待实现）
```

## 类型别名

### 浮点数版本（默认）
```cpp
using Vec2 = Vec2T<float>;
using Vec3 = Vec3T<float>;
using Vec4 = Vec4T<float>;
using Mat3 = Mat3T<float>;
using Mat4 = Mat4T<float>;
using Quaternion = QuaternionT<float>;
```

### 整数版本
```cpp
using Vec2i = Vec2T<int32_t>;
using Vec3i = Vec3T<int32_t>;
using Vec4i = Vec4T<int32_t>;
```

### 双精度版本
```cpp
using Vec2d = Vec2T<double>;
using Vec3d = Vec3T<double>;
using Mat4d = Mat4T<double>;
```

## 使用示例

```cpp
#include <hy_render/HYMath.hpp>

using namespace hyengine::math;

// 向量操作
Vec3 position(1.0f, 2.0f, 3.0f);
Vec3 direction(0.0f, 1.0f, 0.0f);
Vec3 result = position + direction * 2.0f;

// 矩阵变换
Mat4 model = Mat4::identity();
model = Mat4::translate(Vec3(1.0f, 0.0f, 0.0f));
model = model * Mat4::rotateY(degToRad(45.0f));

// 四元数旋转
Quaternion rotation = Quaternion::fromAxisAngle(Vec3(0, 1, 0), degToRad(90.0f));
Vec3 rotated = rotation * Vec3(1, 0, 0);
```

## 坐标系

默认使用右手坐标系（OpenGL风格）：
- X轴：向右
- Y轴：向上
- Z轴：向外（指向观察者）

## 性能优化

1. **内联函数**: 小型函数标记为inline，减少函数调用开销
2. **SIMD支持**: 关键运算可选SIMD优化（待实现）
3. **常量表达式**: 编译时计算的constexpr函数
4. **移动语义**: 充分利用C++11移动语义

## 已实现功能

### 已完成组件

#### 1. 基础定义 (MathDef.hpp)
- ✅ 数学常量: PI, TWO_PI, HALF_PI, DEG_TO_RAD, RAD_TO_DEG, EPSILON
- ✅ 工具函数模板: clamp, lerp, smoothstep, degToRad, radToDeg, floatEqual, saturate
- ✅ 枚举类型: VectorDim, MatrixDim, CoordinateSystem, RotationOrder等

#### 2. 类型前置声明 (MathFwd.hpp)
- ✅ 向量模板前置声明: Vec2T, Vec3T, Vec4T
- ✅ 矩阵模板前置声明: Mat3T, Mat4T
- ✅ 四元数模板前置声明: QuaternionT
- ✅ 常用类型别名: Vec2/Vec3/Vec4, Vec2i/Vec3i/Vec4i, Vec2d/Vec3d/Vec4d等

#### 3. 向量类 (Vec2.hpp, Vec3.hpp, Vec4.hpp)

**Vec2T<T> - 二维向量**
- ✅ union多视图访问: {x,y}, {s,t}, {width,height}, v[2]
- ✅ 完整的运算符重载: +, -, *, /, ==, !=, +=, -=, *=, /=
- ✅ 向量方法: length(), squaredLength(), normalise(), dotProduct(), crossProduct(), reflect()
- ✅ 静态常量: ZERO, UNIT_X, UNIT_Y, NEGATIVE_UNIT_X, NEGATIVE_UNIT_Y, UNIT_SCALE
- ✅ 静态方法: lerp()

**Vec3T<T> - 三维向量**
- ✅ union多视图访问: {x,y,z}, {r,g,b}, v[3]
- ✅ 完整的运算符重载: +, -, *, /, ==, !=, +=, -=, *=, /=
- ✅ 向量方法: length(), normalise(), dotProduct(), crossProduct(), reflect()
- ✅ 静态常量: ZERO, UNIT_X, UNIT_Y, UNIT_Z, NEGATIVE_UNIT_X/Y/Z, UNIT_SCALE
- ✅ 静态方法: lerp()

**Vec4T<T> - 四维向量**
- ✅ union多视图访问: {x,y,z,w}, {r,g,b,a}, {origin,size}, v[4]
- ✅ 完整的运算符重载: +, -, *, /, ==, !=, +=, -=, *=, /=
- ✅ 向量方法: length(), normalise(), dotProduct(), isZero()
- ✅ 静态常量: ZERO, UNIT_SCALE
- ✅ 静态方法: lerp()

#### 4. 矩阵类 (Mat3.hpp, Mat4.hpp)

**Mat3T<T> - 3x3矩阵**
- ✅ union多视图访问: m_mat[3][3], m[9], row[3]
- ✅ 矩阵运算: +, -, *, 转置, 求逆, 行列式
- ✅ 矩阵-向量乘法: mat * vec, vec * mat
- ✅ 静态常量: ZERO, IDENTITY
- ✅ 静态方法: identity(), zero(), scale()

**Mat4T<T> - 4x4矩阵**
- ✅ union多视图访问: m_mat[4][4], m[16], row[4]
- ✅ 矩阵运算: +, -, *, 转置, 求逆, 行列式
- ✅ 矩阵-向量乘法: mat * vec3, mat * vec4, vec4 * mat
- ✅ 变换方法: setTrans(), getTrans(), setScale(), extract3x3Matrix()
- ✅ 静态常量: ZERO, IDENTITY
- ✅ 静态方法: identity(), zero(), translate(), scale(), rotateX/Y/Z()
- ✅ 仿射检查: isAffine()
- ✅ 变换矩阵工具函数:
  - lookAt(eye, target, up) - 构建视图矩阵
  - perspective(fovy, aspect, near, far) - 构建透视投影矩阵
  - ortho(left, right, bottom, top, near, far) - 构建正交投影矩阵

#### 5. 四元数类 (Quaternion.hpp)

**QuaternionT<T> - 四元数**
- ✅ union多视图访问: {x,y,z,w}, {v,s}, q[4]
- ✅ 从旋转矩阵构造: fromRotationMatrix(Mat3/Mat4)
- ✅ 转换为旋转矩阵: toRotationMatrix(Mat3/Mat4)
- ✅ 角度轴表示: fromAngleAxis(), toAngleAxis()
- ✅ 轴构造: fromAxes(), toAxes(), xAxis(), yAxis(), zAxis()
- ✅ 四元数运算: +, -, *, 点积, 归一化, 求逆, 共轭
- ✅ 向量旋转: quat * vec3
- ✅ 球面线性插值: sLerp() - 平滑旋转插值
- ✅ 归一化线性插值: nLerp() - 快速插值
- ✅ 静态常量: ZERO, IDENTITY
- ✅ 静态方法: identity()

### 使用示例

```cpp
#include <hy_render/HYMath.hpp>
using namespace hyengine::math;

// Vec2 操作
Vec2 v2a(1.0f, 2.0f);
Vec2 v2b(3.0f, 4.0f);
float dot = v2a.dotProduct(v2b);  // 11.0
Vec2 sum = v2a + v2b;              // (4, 6)

// Vec3 操作
Vec3 v3a(1.0f, 0.0f, 0.0f);
Vec3 v3b(0.0f, 1.0f, 0.0f);
Vec3 cross = v3a.crossProduct(v3b); // (0, 0, 1)

// Vec4 操作
Vec4 color(1.0f, 0.5f, 0.0f, 1.0f);
Vec4 scaled = color * 0.5f;        // (0.5, 0.25, 0, 0.5)

// 类型别名
Vec3i position(100, 200, 300);     // 整数向量
Vec3d precise(1.5, 2.5, 3.5);      // 双精度向量

// Mat3 操作
Mat3 m3 = Mat3::scale(Vec3(2.0f, 2.0f, 2.0f));
Vec3 v = Vec3(1.0f, 1.0f, 1.0f);
Vec3 result = m3 * v;              // (2, 2, 2)

// Mat4 操作
Mat4 transform = Mat4::translate(Vec3(1.0f, 2.0f, 3.0f));
Mat4 rotation = Mat4::rotateZ(degToRad(90.0f));
Mat4 combined = transform * rotation;

// Quaternion 操作
Quaternion qRot;
qRot.fromAngleAxis(degToRad(90.0f), Vec3(0.0f, 0.0f, 1.0f));
Vec3 rotated = qRot * Vec3(1.0f, 0.0f, 0.0f);  // 旋转向量

// 球面线性插值 (SLERP)
Quaternion q1 = Quaternion::IDENTITY;
Quaternion q2;
q2.fromAngleAxis(degToRad(180.0f), Vec3(0.0f, 1.0f, 0.0f));
Quaternion qInterp = Quaternion::sLerp(0.5f, q1, q2);  // 中间旋转

// 变换矩阵工具函数
Mat4 viewMat = Mat4::lookAt(
    Vec3(0.0f, 0.0f, 5.0f),   // 相机位置
    Vec3(0.0f, 0.0f, 0.0f),   // 目标位置
    Vec3(0.0f, 1.0f, 0.0f)    // 上方向
);

Mat4 perspMat = Mat4::perspective(
    degToRad(45.0f),          // 视场角(弧度)
    16.0f / 9.0f,             // 宽高比
    0.1f,                     // 近裁剪面
    100.0f                    // 远裁剪面
);

Mat4 orthoMat = Mat4::ortho(
    -10.0f, 10.0f,            // left, right
    -10.0f, 10.0f,            // bottom, top  
    -1.0f, 1.0f               // near, far
);
```

## 待实现功能

## 待实现功能

- [ ] 边界体（AABB, OBB, Sphere）
- [ ] 射线、平面相交测试
- [ ] SIMD优化版本

## 参考文档

- GLM (OpenGL Mathematics)
- Eigen
- DirectXMath
- 3D Math Primer for Graphics and Game Development
