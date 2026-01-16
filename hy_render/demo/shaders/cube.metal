/**
 * @file cube.metal
 * @brief 立方体渲染的Metal着色器
 * 
 * 包含顶点着色器和片段着色器，支持MVP矩阵变换和顶点颜色
 */

#include <metal_stdlib>
using namespace metal;

/**
 * @brief 顶点输入结构
 */
struct VertexIn {
    float3 position [[attribute(0)]];  // 位置
    float3 color [[attribute(1)]];     // 颜色
};

/**
 * @brief 顶点输出/片段输入结构
 */
struct VertexOut {
    float4 position [[position]];      // 裁剪空间位置
    float3 color;                      // 传递到片段着色器的颜色
};

/**
 * @brief Uniform缓冲区结构
 */
struct Uniforms {
    float4x4 modelMatrix;      // 模型矩阵
    float4x4 viewMatrix;       // 视图矩阵
    float4x4 projectionMatrix; // 投影矩阵
};

/**
 * @brief 顶点着色器
 * 
 * @param vertexIn 输入顶点数据
 * @param uniforms Uniform缓冲区
 * @return 变换后的顶点数据
 */
vertex VertexOut vertex_main(
    VertexIn vertexIn [[stage_in]],
    constant Uniforms& uniforms [[buffer(1)]]
) {
    VertexOut vertexOut;
    
    // 计算MVP矩阵
    float4x4 mvpMatrix = uniforms.projectionMatrix * uniforms.viewMatrix * uniforms.modelMatrix;
    
    // 变换顶点位置
    vertexOut.position = mvpMatrix * float4(vertexIn.position, 1.0);
    
    // 传递颜色
    vertexOut.color = vertexIn.color;
    
    return vertexOut;
}

/**
 * @brief 片段着色器
 * 
 * @param fragmentIn 插值后的片段数据
 * @return 最终颜色
 */
fragment float4 fragment_main(VertexOut fragmentIn [[stage_in]]) {
    // 直接输出顶点颜色
    return float4(fragmentIn.color, 1.0);
}
