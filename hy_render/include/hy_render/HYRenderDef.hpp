#pragma once

#include <cstdint>
#include <vector>
#include <memory>

namespace hyengine {
namespace render {

/**
 * @brief 资源类型枚举
 */
enum class ResourceType : uint8_t {
    kBuffer,
    kTexture,
    kShader,
    kFrameBuffer,
    kPipelineState,
    kRenderPass,
    kSampler,
    kFence,
    kVertexArray
};

/**
 * @brief 资源句柄联合体
 */
union ResourceHandle {
    void* ptrHandle;
    uint32_t uintHandle;
    uint64_t ulongHandle;
    
    ResourceHandle() : ulongHandle(0) {}
};

/**
 * @brief 渲染资源抽象基类
 */
class HYResource {
public:
    virtual ~HYResource() = default;

    /**
     * @brief 获取资源类型
     */
    ResourceType getResourceType() const { return mResourceType; }

    /**
     * @brief 获取平台原生句柄
     */
    virtual ResourceHandle getResourceHandle() const = 0;

protected:
    explicit HYResource(ResourceType type) : mResourceType(type) {}

protected:
    ResourceType mResourceType;
};

/**
 * @brief 缓冲区绑定标志
 */
enum BufferBindFlag : uint8_t {
    BIND_VERTEX_BUFFER = 1 << 0,
    BIND_INDEX_BUFFER = 1 << 1,
    BIND_UNIFORM_BUFFER = 1 << 2,
};

/**
 * @brief 缓冲区使用模式
 */
enum class BufferUsage : uint8_t {
    USAGE_STATIC_DRAW,   // 数据不常变，只GPU读取
    USAGE_DYNAMIC_DRAW,  // CPU偶尔写入，GPU读取
    USAGE_STREAM_DRAW,   // CPU频繁写入
};

/**
 * @brief 顶点属性格式
 */
enum class VertexFormat : uint8_t {
    kFloat,
    kFloat2,
    kFloat3,
    kFloat4,
    kInt,
    kInt2,
    kInt3,
    kInt4,
    kUInt,
    kUInt2,
    kUInt3,
    kUInt4,
};

/**
 * @brief 顶点布局元素
 */
struct LayoutElement {
    VertexFormat format;
    uint32_t offset;
    uint32_t stride;
    bool normalized = false;
};

/**
 * @brief 缓冲区描述
 */
struct BufferDesc {
    uint8_t bindFlags = 0;
    BufferUsage usage = BufferUsage::USAGE_STATIC_DRAW;
    size_t size = 0;
    std::vector<LayoutElement> layoutElements;
    const void* initialData = nullptr;
};

/**
 * @brief 纹理维度类型
 */
enum class TextureDimension : uint8_t {
    TEXTURE_DIM_TEX_2D,
    TEXTURE_DIM_TEX_CUBE,
    TEXTURE_DIM_TEX_3D,
};

/**
 * @brief 纹理格式
 */
enum class TextureFormat : uint8_t {
    TEX_FORMAT_RGBA8_UNORM,
    TEX_FORMAT_RGBA16_FLOAT,
    TEX_FORMAT_DEPTH24_STENCIL8,
    TEX_FORMAT_DEPTH32_FLOAT,
    TEX_FORMAT_R8_UNORM,
    TEX_FORMAT_RG8_UNORM,
    TEX_FORMAT_RGB8_UNORM,
};

/**
 * @brief 纹理用途
 */
enum TextureUsage : uint8_t {
    TEXTURE_USAGE_SHADER_READ = 1 << 0,
    TEXTURE_USAGE_RENDER_TARGET = 1 << 1,
};

/**
 * @brief 纹理描述
 */
struct TextureDesc {
    uint32_t width = 0;
    uint32_t height = 0;
    TextureDimension type = TextureDimension::TEXTURE_DIM_TEX_2D;
    TextureFormat format = TextureFormat::TEX_FORMAT_RGBA8_UNORM;
    uint8_t usage = TEXTURE_USAGE_SHADER_READ;
    uint32_t mipLevels = 1;
};

/**
 * @brief 纹理区域
 */
struct TextureRegion {
    uint32_t offsetX = 0;
    uint32_t offsetY = 0;
    uint32_t offsetZ = 0;
    uint32_t width = 0;
    uint32_t height = 0;
    uint32_t depth = 1;
};

/**
 * @brief 采样器过滤类型
 */
enum class FilterType : uint8_t {
    FILTER_TYPE_NEAREST,
    FILTER_TYPE_LINEAR,
    FILTER_TYPE_LINEAR_MIPMAP_LINEAR,
};

/**
 * @brief 纹理寻址模式
 */
enum class TextureAddressMode : uint8_t {
    TEXTURE_ADDRESS_REPEAT,
    TEXTURE_ADDRESS_CLAMP_TO_EDGE,
    TEXTURE_ADDRESS_MIRROR_REPEAT,
};

/**
 * @brief 采样器描述
 */
struct SamplerDesc {
    FilterType minFilter = FilterType::FILTER_TYPE_LINEAR;
    FilterType magFilter = FilterType::FILTER_TYPE_LINEAR;
    TextureAddressMode addressModeU = TextureAddressMode::TEXTURE_ADDRESS_REPEAT;
    TextureAddressMode addressModeV = TextureAddressMode::TEXTURE_ADDRESS_REPEAT;
    TextureAddressMode addressModeW = TextureAddressMode::TEXTURE_ADDRESS_REPEAT;
};

/**
 * @brief Shader阶段
 */
enum class ShaderStage : uint8_t {
    kShaderStage_Vertex = 1 << 0,
    kShaderStage_Fragment = 1 << 1,
    kShaderStage_Geometry = 1 << 2,
    kShaderStage_Compute = 1 << 3,
};

/**
 * @brief Shader源语言
 */
enum class ShaderSourceLanguage : uint8_t {
    kShaderSourceLanguage_GLSL,
    kShaderSourceLanguage_SPIRV,
    kShaderSourceLanguage_MSL,
};

/**
 * @brief 着色器代码描述
 */
struct ShaderCodeDesc {
    ShaderStage shaderStage;
    const char* shaderCode = nullptr;
    size_t shaderCodeLength = 0;
    const char* entryFunction = nullptr;
};

/**
 * @brief 着色器描述
 */
struct ShaderDesc {
    std::string name;
    ShaderSourceLanguage sourceLanguage = ShaderSourceLanguage::kShaderSourceLanguage_GLSL;
    std::vector<ShaderCodeDesc> shaderCodeDesc;
};

/**
 * @brief 帧缓冲描述
 */
struct FrameBufferDesc {
    uint32_t width = 0;
    uint32_t height = 0;
    TextureFormat colorAttachmentFmt = TextureFormat::TEX_FORMAT_RGBA8_UNORM;
    TextureFormat depthAttachmentFmt = TextureFormat::TEX_FORMAT_DEPTH24_STENCIL8;
    bool needDepth = true;
    bool isMultSample = false;
    uint32_t multSampleCnt = 1;
};

/**
 * @brief 混合因子
 */
enum class BlendFactor : uint8_t {
    BLEND_FACTOR_ZERO,
    BLEND_FACTOR_ONE,
    BLEND_FACTOR_SRC_ALPHA,
    BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
    BLEND_FACTOR_DST_ALPHA,
    BLEND_FACTOR_ONE_MINUS_DST_ALPHA,
};

/**
 * @brief 混合操作
 */
enum class BlendOperation : uint8_t {
    BLEND_OP_ADD,
    BLEND_OP_SUBTRACT,
    BLEND_OP_REVERSE_SUBTRACT,
};

/**
 * @brief 混合描述
 */
struct BlendDesc {
    bool blendEnable = false;
    BlendFactor srcBlend = BlendFactor::BLEND_FACTOR_ONE;
    BlendFactor dstBlend = BlendFactor::BLEND_FACTOR_ZERO;
    BlendOperation blendOp = BlendOperation::BLEND_OP_ADD;
    BlendFactor srcBlendAlpha = BlendFactor::BLEND_FACTOR_ONE;
    BlendFactor dstBlendAlpha = BlendFactor::BLEND_FACTOR_ZERO;
    BlendOperation blendOpAlpha = BlendOperation::BLEND_OP_ADD;
};

/**
 * @brief 剔除模式
 */
enum class CullMode : uint8_t {
    CULL_MODE_NONE,
    CULL_MODE_FRONT,
    CULL_MODE_BACK,
};

/**
 * @brief 光栅化描述
 */
struct RasterizerDesc {
    CullMode cullMode = CullMode::CULL_MODE_BACK;
    bool frontCCW = false;
    bool scissorEnable = false;
    uint16_t scissorRect[4] = {0, 0, 0, 0};
};

/**
 * @brief 比较函数
 */
enum class CompareFunction : uint8_t {
    COMPARE_FUNC_NEVER,
    COMPARE_FUNC_LESS,
    COMPARE_FUNC_EQUAL,
    COMPARE_FUNC_LESS_EQUAL,
    COMPARE_FUNC_GREATER,
    COMPARE_FUNC_NOT_EQUAL,
    COMPARE_FUNC_GREATER_EQUAL,
    COMPARE_FUNC_ALWAYS,
};

/**
 * @brief 深度模板描述
 */
struct DepthStencilDesc {
    bool depthTestEnable = true;
    bool depthWriteEnable = true;
    CompareFunction depthFunc = CompareFunction::COMPARE_FUNC_LESS;
    bool stencilTestEnable = false;
    uint8_t stencilMask = 0xFF;
};

/**
 * @brief 图元类型
 */
enum class PrimitiveType : uint8_t {
    PRIMITIVE_POINTS,
    PRIMITIVE_LINES,
    PRIMITIVE_TRIANGLES,
    PRIMITIVE_TRIANGLE_STRIP,
};

/**
 * @brief 图元描述
 */
struct PrimitiveDesc {
    PrimitiveType type = PrimitiveType::PRIMITIVE_TRIANGLES;
};

/**
 * @brief 管线状态描述
 */
struct PipelineStateDesc {
    BlendDesc blendDesc;
    RasterizerDesc rasterizerDesc;
    DepthStencilDesc depthStencilDesc;
    PrimitiveDesc primitiveDesc;
};

/**
 * @brief 绘制类型
 */
enum class DrawType : uint8_t {
    DRAW_ARRAYS,
    DRAW_ARRAYS_INSTANCED,
    DRAW_ELEMENTS,
    DRAW_ELEMENTS_INSTANCED,
};

/**
 * @brief 清除掩码
 */
enum ClearMask : uint8_t {
    CLEAR_COLOR_BUFFER = 1 << 0,
    CLEAR_DEPTH_BUFFER = 1 << 1,
    CLEAR_STENCIL_BUFFER = 1 << 2,
};

} // namespace render
} // namespace hyengine
