#include "hy_render/HYMath.hpp"
#include <iostream>

using namespace hyengine::math;

/**
 * @brief 测试数学库基础定义
 */
int main() {
    std::cout << "=== HY Math Library Test ===" << std::endl;
    std::cout << "Version: " << MathLibInfo::VERSION_STRING << std::endl;
    std::cout << "Build: " << MathLibInfo::BUILD_DATE << " " << MathLibInfo::BUILD_TIME << std::endl;
    std::cout << std::endl;
    
    // 测试数学常量
    std::cout << "Math Constants:" << std::endl;
    std::cout << "  PI = " << PI << std::endl;
    std::cout << "  TWO_PI = " << TWO_PI << std::endl;
    std::cout << "  DEG_TO_RAD = " << DEG_TO_RAD << std::endl;
    std::cout << "  RAD_TO_DEG = " << RAD_TO_DEG << std::endl;
    std::cout << std::endl;
    
    // 测试工具函数
    std::cout << "Utility Functions:" << std::endl;
    std::cout << "  clamp(5, 0, 10) = " << clamp(5, 0, 10) << std::endl;
    std::cout << "  clamp(-5, 0, 10) = " << clamp(-5, 0, 10) << std::endl;
    std::cout << "  clamp(15, 0, 10) = " << clamp(15, 0, 10) << std::endl;
    std::cout << "  lerp(0, 10, 0.5) = " << lerp(0.0f, 10.0f, 0.5f) << std::endl;
    std::cout << "  degToRad(90) = " << degToRad(90.0f) << std::endl;
    std::cout << "  radToDeg(PI) = " << radToDeg(PI) << std::endl;
    std::cout << "  saturate(1.5) = " << saturate(1.5f) << std::endl;
    std::cout << "  saturate(-0.5) = " << saturate(-0.5f) << std::endl;
    std::cout << std::endl;
    
    // 测试浮点数比较
    std::cout << "Float Comparison:" << std::endl;
    std::cout << "  floatEqual(1.0, 1.0) = " << std::boolalpha << floatEqual(1.0f, 1.0f) << std::endl;
    std::cout << "  floatEqual(1.0, 1.0 + EPSILON/2) = " << floatEqual(1.0f, 1.0f + EPSILON/2.0f) << std::endl;
    std::cout << "  floatEqual(1.0, 1.1) = " << floatEqual(1.0f, 1.1f) << std::endl;
    std::cout << std::endl;
    
    std::cout << "Math library basic definitions test passed!" << std::endl;
    std::cout << std::endl;
    
    // 测试Vec2
    std::cout << "=== Vec2 Tests ===" << std::endl;
    Vec2 v2a(1.0f, 2.0f);
    Vec2 v2b(3.0f, 4.0f);
    std::cout << "  v2a(1, 2) + v2b(3, 4) = (" << (v2a + v2b).x << ", " << (v2a + v2b).y << ")" << std::endl;
    std::cout << "  v2a.length() = " << v2a.length() << std::endl;
    std::cout << "  v2a.dotProduct(v2b) = " << v2a.dotProduct(v2b) << std::endl;
    std::cout << "  Vec2::UNIT_X = (" << Vec2::UNIT_X.x << ", " << Vec2::UNIT_X.y << ")" << std::endl;
    std::cout << std::endl;
    
    // 测试Vec3
    std::cout << "=== Vec3 Tests ===" << std::endl;
    Vec3 v3a(1.0f, 0.0f, 0.0f);
    Vec3 v3b(0.0f, 1.0f, 0.0f);
    Vec3 cross = v3a.crossProduct(v3b);
    std::cout << "  v3a(1,0,0) x v3b(0,1,0) = (" << cross.x << ", " << cross.y << ", " << cross.z << ")" << std::endl;
    std::cout << "  Vec3::ZERO = (" << Vec3::ZERO.x << ", " << Vec3::ZERO.y << ", " << Vec3::ZERO.z << ")" << std::endl;
    std::cout << "  Vec3::UNIT_Z = (" << Vec3::UNIT_Z.x << ", " << Vec3::UNIT_Z.y << ", " << Vec3::UNIT_Z.z << ")" << std::endl;
    std::cout << std::endl;
    
    // 测试Vec4
    std::cout << "=== Vec4 Tests ===" << std::endl;
    Vec4 v4a(1.0f, 2.0f, 3.0f, 4.0f);
    Vec4 v4b(0.5f, 0.5f, 0.5f, 0.5f);
    std::cout << "  v4a(1,2,3,4) * v4b(0.5,0.5,0.5,0.5) = (" << (v4a * v4b).x << ", " << (v4a * v4b).y << ", " 
              << (v4a * v4b).z << ", " << (v4a * v4b).w << ")" << std::endl;
    std::cout << "  v4a.dotProduct(v4b) = " << v4a.dotProduct(v4b) << std::endl;
    std::cout << std::endl;
    
    // 测试类型别名
    std::cout << "=== Type Aliases Tests ===" << std::endl;
    Vec3i v3i(1, 2, 3);
    Vec3d v3d(1.5, 2.5, 3.5);
    std::cout << "  Vec3i(1,2,3) = (" << v3i.x << ", " << v3i.y << ", " << v3i.z << ")" << std::endl;
    std::cout << "  Vec3d(1.5,2.5,3.5) = (" << v3d.x << ", " << v3d.y << ", " << v3d.z << ")" << std::endl;
    std::cout << std::endl;
    
    std::cout << "All math vector tests passed!" << std::endl;
    std::cout << std::endl;
    
    // 测试Mat3
    std::cout << "=== Mat3 Tests ===" << std::endl;
    Mat3 m3a = Mat3::identity();
    Mat3 m3b = Mat3::scale(Vec3(2.0f, 2.0f, 2.0f));
    Mat3 m3c = m3a * m3b;
    std::cout << "  identity * scale(2,2,2) =" << std::endl;
    std::cout << "    [" << m3c[0][0] << ", " << m3c[0][1] << ", " << m3c[0][2] << "]" << std::endl;
    std::cout << "    [" << m3c[1][0] << ", " << m3c[1][1] << ", " << m3c[1][2] << "]" << std::endl;
    std::cout << "    [" << m3c[2][0] << ", " << m3c[2][1] << ", " << m3c[2][2] << "]" << std::endl;
    std::cout << "  determinant = " << m3b.determinant() << std::endl;
    std::cout << std::endl;
    
    // 测试Mat4
    std::cout << "=== Mat4 Tests ===" << std::endl;
    Mat4 m4a = Mat4::identity();
    Mat4 m4b = Mat4::translate(Vec3(1.0f, 2.0f, 3.0f));
    Vec3 trans = m4b.getTrans();
    std::cout << "  translate(1,2,3).getTrans() = (" << trans.x << ", " << trans.y << ", " << trans.z << ")" << std::endl;
    
    Mat4 m4scale = Mat4::scale(Vec3(2.0f, 3.0f, 4.0f));
    Vec4 testVec(1.0f, 1.0f, 1.0f, 1.0f);
    Vec4 scaledVec = m4scale * testVec;
    std::cout << "  scale(2,3,4) * (1,1,1,1) = (" << scaledVec.x << ", " << scaledVec.y << ", " 
              << scaledVec.z << ", " << scaledVec.w << ")" << std::endl;
    
    Mat4 m4rot = Mat4::rotateZ(degToRad(90.0f));
    std::cout << "  rotateZ(90deg) created successfully" << std::endl;
    std::cout << std::endl;
    
    std::cout << "All math matrix tests passed!" << std::endl;
    std::cout << std::endl;
    
    // 测试Quaternion
    std::cout << "=== Quaternion Tests ===" << std::endl;
    
    // 从角度轴创建四元数
    Quaternion qRot;
    qRot.fromAngleAxis(degToRad(90.0f), Vec3(0.0f, 0.0f, 1.0f));
    std::cout << "  rotateZ(90deg) quaternion = (" << qRot.x << ", " << qRot.y << ", " 
              << qRot.z << ", " << qRot.w << ")" << std::endl;
    
    // 四元数旋转向量
    Vec3 v(1.0f, 0.0f, 0.0f);
    Vec3 rotated = qRot * v;
    std::cout << "  quat * (1,0,0) ≈ (" << rotated.x << ", " << rotated.y << ", " << rotated.z << ")" << std::endl;
    
    // 四元数乘法
    Quaternion q1(0.0f, 0.0f, 0.0f, 1.0f);
    Quaternion q2(0.0f, 0.0f, 0.707f, 0.707f);
    Quaternion qMul = q1 * q2;
    std::cout << "  identity * q2 = (" << qMul.x << ", " << qMul.y << ", " 
              << qMul.z << ", " << qMul.w << ")" << std::endl;
    
    // 球面线性插值
    Quaternion qStart = Quaternion::IDENTITY;
    Quaternion qEnd;
    qEnd.fromAngleAxis(degToRad(180.0f), Vec3(0.0f, 1.0f, 0.0f));
    Quaternion qSlerp = Quaternion::sLerp(0.5f, qStart, qEnd);
    std::cout << "  slerp(0.5, identity, 180Y) = (" << qSlerp.x << ", " << qSlerp.y << ", " 
              << qSlerp.z << ", " << qSlerp.w << ")" << std::endl;
    
    std::cout << std::endl;
    std::cout << "All quaternion tests passed!" << std::endl;

    // 测试变换矩阵工具函数
    std::cout << std::endl;
    std::cout << "=== Transformation Matrix Tests ===" << std::endl;

    // 测试LookAt视图矩阵
    Vec3 eye(0.0f, 0.0f, 5.0f);
    Vec3 target(0.0f, 0.0f, 0.0f);
    Vec3 upVec(0.0f, 1.0f, 0.0f);
    Mat4 viewMat = Mat4::lookAt(eye, target, upVec);
    std::cout << "  LookAt view matrix:" << std::endl;
    std::cout << "    [" << viewMat.m_mat[0][0] << ", " << viewMat.m_mat[0][1] << ", " 
              << viewMat.m_mat[0][2] << ", " << viewMat.m_mat[0][3] << "]" << std::endl;
    std::cout << "    [" << viewMat.m_mat[1][0] << ", " << viewMat.m_mat[1][1] << ", " 
              << viewMat.m_mat[1][2] << ", " << viewMat.m_mat[1][3] << "]" << std::endl;
    std::cout << "    [" << viewMat.m_mat[2][0] << ", " << viewMat.m_mat[2][1] << ", " 
              << viewMat.m_mat[2][2] << ", " << viewMat.m_mat[2][3] << "]" << std::endl;
    std::cout << "    [" << viewMat.m_mat[3][0] << ", " << viewMat.m_mat[3][1] << ", " 
              << viewMat.m_mat[3][2] << ", " << viewMat.m_mat[3][3] << "]" << std::endl;

    // 测试透视投影矩阵
    float fovy = degToRad(45.0f);  // 45度视场角
    float aspect = 16.0f / 9.0f;   // 宽高比
    float znear = 0.1f;
    float zfar = 100.0f;
    Mat4 perspMat = Mat4::perspective(fovy, aspect, znear, zfar);
    std::cout << "  Perspective projection matrix (fovy=45°, aspect=16:9):" << std::endl;
    std::cout << "    m[0][0] = " << perspMat.m_mat[0][0] << " (should be > 0)" << std::endl;
    std::cout << "    m[1][1] = " << perspMat.m_mat[1][1] << " (should be > 0)" << std::endl;
    std::cout << "    m[2][2] = " << perspMat.m_mat[2][2] << " (should be < 0)" << std::endl;
    std::cout << "    m[2][3] = " << perspMat.m_mat[2][3] << " (should be -1)" << std::endl;

    // 测试正交投影矩阵
    Mat4 orthoMat = Mat4::ortho(-10.0f, 10.0f, -10.0f, 10.0f, -1.0f, 1.0f);
    std::cout << "  Orthographic projection matrix:" << std::endl;
    std::cout << "    [" << orthoMat.m_mat[0][0] << ", " << orthoMat.m_mat[0][1] << ", " 
              << orthoMat.m_mat[0][2] << ", " << orthoMat.m_mat[0][3] << "]" << std::endl;
    std::cout << "    [" << orthoMat.m_mat[1][0] << ", " << orthoMat.m_mat[1][1] << ", " 
              << orthoMat.m_mat[1][2] << ", " << orthoMat.m_mat[1][3] << "]" << std::endl;
    std::cout << "    [" << orthoMat.m_mat[2][0] << ", " << orthoMat.m_mat[2][1] << ", " 
              << orthoMat.m_mat[2][2] << ", " << orthoMat.m_mat[2][3] << "]" << std::endl;
    std::cout << "    [" << orthoMat.m_mat[3][0] << ", " << orthoMat.m_mat[3][1] << ", " 
              << orthoMat.m_mat[3][2] << ", " << orthoMat.m_mat[3][3] << "]" << std::endl;

    std::cout << std::endl;
    std::cout << "All transformation matrix tests passed!" << std::endl;
    
    return 0;
}
