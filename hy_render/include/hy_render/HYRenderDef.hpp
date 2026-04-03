#pragma once

#include <cstdint>
#include <vector>
#include <memory>

namespace hyengine {
namespace render {

// 前向声明
class HYResource;
class HYBuffer;
class HYTexture;
class HYShader;
class HYFrameBuffer;
class HYPipelineState;

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
    kVertexArray,
    kCommandBuffer
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
 * 
 * 根据设计文档5.1.1优化：
 * - layoutElements设为可选项（仅Vertex Buffer需要）
 * - 增加size必填字段
 * - 支持初始化数据
 */
struct BufferDesc {
    uint8_t bindFlags = 0;                            // 绑定类型（必填）
    BufferUsage usage = BufferUsage::USAGE_STATIC_DRAW; // 使用模式（必填）
    size_t size = 0;                                  // 缓冲区大小字节（必填）
    std::vector<LayoutElement> layoutElements;        // 数据布局（仅Vertex Buffer需要）
    const void* initialData = nullptr;               // 初始化数据指针（可选）
};

/**
 * @brief 内存访问模式（用于map/unmap）
 */
enum class MemoryAccess : uint8_t {
    READ_ONLY,
    WRITE_ONLY,
    READ_WRITE
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
 * @brief 纹理读取描述
 * 
 * 根据设计文档5.1.2优化：
 * - 将readTexture的colorRange和colorSpace参数封装到描述结构体
 */
struct TextureReadDesc {
    TextureFormat format = TextureFormat::TEX_FORMAT_RGBA8_UNORM;
    TextureRegion region;
    
    // 颜色范围和色彩空间支持
    enum class ColorRange : uint8_t {
        RANGE_FULL,      // 0-255 或 0.0-1.0
        RANGE_LIMITED    // 16-235 或 0.0625-0.9375
    } colorRange = ColorRange::RANGE_FULL;
    
    enum class ColorSpace : uint8_t {
        SRGB,
        LINEAR,
        REC709,
        REC2020
    } colorSpace = ColorSpace::SRGB;
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
 * @brief Uniform类型
 */
enum class UniformType : uint8_t {
    kFloat,
    kVec2,
    kVec3,
    kVec4,
    kInt,
    kIVec2,
    kIVec3,
    kIVec4,
    kMat3,
    kMat4,
    kSampler2D,
    kSamplerCube
};

/**
 * @brief Uniform信息（5.1.3新增）
 */
struct UniformInfo {
    std::string name;
    UniformType type;
    int32_t location = -1;      // 绑定位置
    uint32_t arraySize = 1;     // 数组大小（1表示非数组）
    uint32_t offset = 0;        // UBO中的偏移
};

/**
 * @brief Attribute信息（5.1.3新增）
 */
struct AttributeInfo {
    std::string name;
    VertexFormat format;
    int32_t location = -1;      // 绑定位置
    uint32_t offset = 0;        // 偏移量
};

/**
 * @brief UniformBlock信息（5.1.3新增）
 */
struct UniformBlockInfo {
    std::string name;
    uint32_t binding = 0;       // 绑定点
    uint32_t size = 0;          // 字节大小
    std::vector<UniformInfo> members; // 成员变量
};

/**
 * @brief Shader反射信息（5.1.3新增）
 */
struct ShaderReflection {
    std::vector<UniformInfo> uniforms;          // Uniform信息
    std::vector<UniformBlockInfo> uniformBlocks; // Uniform Block信息
    std::vector<AttributeInfo> attributes;       // Attribute信息
    
    // 便捷查询方法
    const UniformInfo* findUniform(const std::string& name) const {
        for (const auto& uniform : uniforms) {
            if (uniform.name == name) return &uniform;
        }
        return nullptr;
    }
    
    const AttributeInfo* findAttribute(const std::string& name) const {
        for (const auto& attr : attributes) {
            if (attr.name == name) return &attr;
        }
        return nullptr;
    }
};

/**
 * @brief 附件加载操作（5.1.5新增）
 */
enum class AttachmentLoadOp : uint8_t {
    kLoad,      // 保留附件内容
    kClear,     // 清除附件内容
    kDontCare   // 不关心附件内容
};

/**
 * @brief 附件存储操作（5.1.5新增）
 */
enum class AttachmentStoreOp : uint8_t {
    kStore,     // 存储附件内容
    kDontCare   // 不关心附件内容
};

/**
 * @brief 附件描述（5.1.5新增）
 */
struct AttachmentDesc {
    TextureFormat format = TextureFormat::TEX_FORMAT_RGBA8_UNORM;
    AttachmentLoadOp loadOp = AttachmentLoadOp::kClear;
    AttachmentStoreOp storeOp = AttachmentStoreOp::kStore;
    AttachmentLoadOp stencilLoadOp = AttachmentLoadOp::kDontCare;
    AttachmentStoreOp stencilStoreOp = AttachmentStoreOp::kDontCare;
    uint32_t sampleCount = 1;  // 多重采样数量
    float clearColor[4] = {0.0f, 0.0f, 0.0f, 1.0f};  // 清除颜色
    float clearDepth = 1.0f;    // 清除深度值
    uint32_t clearStencil = 0;  // 清除模板值
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
    
    // 5.1.5 新增：多渲染目标支持
    uint32_t colorAttachmentCount = 1;  // 颜色附件数量（最多8个）
    std::vector<AttachmentDesc> colorAttachments;  // 颜色附件描述列表
    AttachmentDesc depthStencilAttachment;         // 深度模板附件描述
    bool hasDepthStencilAttachment = false;        // 是否有深度模板附件
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

/**
 * @brief 渲染API类型
 */
enum class RenderAPI : uint8_t {
    kOpenGL,
    kOpenGLES,
    kMetal,
    kVulkan
};

/**
 * @brief 纹理类型（扩展）
 */
enum class TextureType : uint8_t {
    k2D,
    k3D,
    kCube,
    k2DArray
};

/**
 * @brief 过滤模式（扩展）
 */
enum class FilterMode : uint8_t {
    kNearest,
    kLinear,
    kNearestMipmapNearest,
    kLinearMipmapNearest,
    kNearestMipmapLinear,
    kLinearMipmapLinear
};

/**
 * @brief 比较函数（扩展）
 */
enum class CompareFunc : uint8_t {
    kNever,
    kLess,
    kEqual,
    kLessEqual,
    kGreater,
    kNotEqual,
    kGreaterEqual,
    kAlways
};

/**
 * @brief 模板操作
 */
enum class StencilOp : uint8_t {
    kKeep,
    kZero,
    kReplace,
    kIncrement,
    kIncrementWrap,
    kDecrement,
    kDecrementWrap,
    kInvert
};

/**
 * @brief 正面朝向
 */
enum class FrontFace : uint8_t {
    kCW,   // 顺时针
    kCCW   // 逆时针
};

/**
 * @brief 混合操作（扩展）
 */
enum class BlendOp : uint8_t {
    kAdd,
    kSubtract,
    kReverseSubtract,
    kMin,
    kMax
};

// ============================================================================
// 5.1.4 RenderPass 接口优化相关结构（新增）
// ============================================================================

/**
 * @brief 纹理绑定描述
 */
struct TextureBinding {
    std::shared_ptr<HYTexture> texture;
    uint32_t textureUnit = 0;     // 纹理单元索引
    uint32_t bindingIndex = 0;    // 绑定点索引
    std::string samplerName;      // 着色器中的sampler名称
};

/**
 * @brief UBO绑定描述
 */
struct UniformBufferBinding {
    std::shared_ptr<HYBuffer> buffer;
    uint32_t bindingPoint = 0;    // 绑定点
    std::string blockName;        // 着色器中的uniform block名称
    uint32_t offset = 0;          // 缓冲区偏移
    uint32_t size = 0;            // 绑定大小（0表示整个缓冲区）
};

/**
 * @brief 绘制参数描述
 */
struct DrawParams {
    uint32_t vertexStart = 0;
    uint32_t vertexCount = 0;
    uint32_t indexStart = 0;
    uint32_t indexCount = 0;
    uint32_t instanceCount = 1;
    PrimitiveType primitiveType = PrimitiveType::PRIMITIVE_TRIANGLES;
};

/**
 * @brief 渲染状态快照（用于状态缓存优化）
 */
struct RenderState {
    std::shared_ptr<HYShader> currentShader;
    std::shared_ptr<HYPipelineState> currentPipelineState;
    std::shared_ptr<HYFrameBuffer> currentFrameBuffer;
    std::vector<std::shared_ptr<HYBuffer>> vertexBuffers;
    std::vector<uint32_t> vertexBufferBindings;
    std::shared_ptr<HYBuffer> indexBuffer;
    std::vector<TextureBinding> textureBindings;
    std::vector<UniformBufferBinding> uboBindings;
    bool isDirty = true;  // 状态是否需要更新
};

} // namespace render
} // namespace hyengine
