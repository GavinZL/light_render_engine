/**
 * @file metal_cube_demo.mm
 * @brief macOS平台Metal立方体渲染Demo
 * 
 * 使用NSWindow和CAMetalLayer实现立方体渲染，支持鼠标拖动旋转
 */

#import <Cocoa/Cocoa.h>
#import <Metal/Metal.h>
#import <QuartzCore/CAMetalLayer.h>
#include <iostream>
#include <cmath>

#include "hy_render/HYRenderEngine.hpp"
#include "hy_render/HYRenderContext.hpp"
#include "hy_render/HYMath.hpp"

using namespace hyengine::render;
using namespace hyengine::math;

// 顶点结构
struct Vertex {
    Vec3 position;   // 使用 Vec3 而不是 Vec3f
    Vec3 color;
};

// Uniform结构（与shader中的结构对应）
struct Uniforms {
    Mat4 modelMatrix;        // 使用 Mat4 而不是 Mat4f
    Mat4 viewMatrix;
    Mat4 projectionMatrix;
};

// 立方体顶点数据（8个顶点，每个顶点有位置和颜色）
static Vertex cubeVertices[] = {
    // 前面（红色调）
    {{-0.5f, -0.5f,  0.5f}, {1.0f, 0.0f, 0.0f}},  // 0
    {{ 0.5f, -0.5f,  0.5f}, {1.0f, 0.5f, 0.0f}},  // 1
    {{ 0.5f,  0.5f,  0.5f}, {1.0f, 1.0f, 0.0f}},  // 2
    {{-0.5f,  0.5f,  0.5f}, {1.0f, 0.0f, 0.5f}},  // 3
    
    // 后面（蓝色调）
    {{-0.5f, -0.5f, -0.5f}, {0.0f, 0.0f, 1.0f}},  // 4
    {{ 0.5f, -0.5f, -0.5f}, {0.0f, 0.5f, 1.0f}},  // 5
    {{ 0.5f,  0.5f, -0.5f}, {0.5f, 1.0f, 1.0f}},  // 6
    {{-0.5f,  0.5f, -0.5f}, {0.0f, 0.5f, 0.5f}},  // 7
};

// 立方体索引数据（12个三角形，36个索引）
static uint16_t cubeIndices[] = {
    // 前面
    0, 1, 2,  0, 2, 3,
    // 右面
    1, 5, 6,  1, 6, 2,
    // 后面
    5, 4, 7,  5, 7, 6,
    // 左面
    4, 0, 3,  4, 3, 7,
    // 顶面
    3, 2, 6,  3, 6, 7,
    // 底面
    4, 5, 1,  4, 1, 0
};

/**
 * @brief Metal渲染视图
 */
@interface MetalView : NSView
@property (nonatomic, strong) CAMetalLayer* metalLayer;
@property (nonatomic, assign) float rotationX;
@property (nonatomic, assign) float rotationY;
@property (nonatomic, assign) NSPoint lastMousePoint;
@property (nonatomic, assign) BOOL isMouseDown;
@end

@implementation MetalView

- (instancetype)initWithFrame:(NSRect)frame {
    self = [super initWithFrame:frame];
    if (self) {
        self.wantsLayer = YES;
        _rotationX = 0.3f;
        _rotationY = 0.5f;
        _isMouseDown = NO;
    }
    return self;
}

- (CALayer*)makeBackingLayer {
    _metalLayer = [CAMetalLayer layer];
    _metalLayer.pixelFormat = MTLPixelFormatBGRA8Unorm;
    _metalLayer.framebufferOnly = YES;
    return _metalLayer;
}

- (void)mouseDown:(NSEvent*)event {
    _isMouseDown = YES;
    _lastMousePoint = [self convertPoint:[event locationInWindow] fromView:nil];
}

- (void)mouseUp:(NSEvent*)event {
    _isMouseDown = NO;
}

- (void)mouseDragged:(NSEvent*)event {
    if (_isMouseDown) {
        NSPoint currentPoint = [self convertPoint:[event locationInWindow] fromView:nil];
        float deltaX = currentPoint.x - _lastMousePoint.x;
        float deltaY = currentPoint.y - _lastMousePoint.y;
        
        _rotationY += deltaX * 0.01f;
        _rotationX += deltaY * 0.01f;
        
        _lastMousePoint = currentPoint;
    }
}

@end

/**
 * @brief Metal渲染器
 */
class MetalCubeRenderer {
public:
    MetalCubeRenderer() = default;
    ~MetalCubeRenderer() = default;
    
    bool initialize(CAMetalLayer* metalLayer);
    void render(float rotationX, float rotationY);
    void resize(int width, int height);
    
private:
    void setupRenderPipeline();
    void createBuffers();
    
private:
    id<MTLDevice> mDevice = nil;
    id<MTLCommandQueue> mCommandQueue = nil;
    id<MTLRenderPipelineState> mPipelineState = nil;
    id<MTLDepthStencilState> mDepthState = nil;
    id<MTLBuffer> mVertexBuffer = nil;
    id<MTLBuffer> mIndexBuffer = nil;
    id<MTLBuffer> mUniformBuffer = nil;
    CAMetalLayer* mMetalLayer = nil;
    
    // hy_render 抽象对象
    std::shared_ptr<HYRenderContext> mContext;
    std::shared_ptr<HYBuffer> mHYVertexBuffer;
    std::shared_ptr<HYBuffer> mHYIndexBuffer;
    std::shared_ptr<HYBuffer> mHYUniformBuffer;
    std::shared_ptr<HYShader> mHYShader;
    std::shared_ptr<HYPipelineState> mHYPipelineState;
    std::shared_ptr<HYRenderPass> mHYRenderPass;
    
    int mWidth = 800;
    int mHeight = 600;
    bool mUseAbstractAPI = true;  // 是否使用抽象API
};

bool MetalCubeRenderer::initialize(CAMetalLayer* metalLayer) {
    mMetalLayer = metalLayer;
    
    // 初始化 hy_render 引擎
    HYRenderEngine::instance().setRenderAPI(RenderAPI::kMetal);
    mContext = HYRenderEngine::instance().getRenderContext();
    
    if (mContext) {
        std::cout << "[使用 hy_render 抽象API模式]" << std::endl;
        mUseAbstractAPI = true;
        
        // 创建 hy_render 资源
        BufferDesc vertexDesc;
        vertexDesc.bindFlags = BIND_VERTEX_BUFFER;
        vertexDesc.usage = BufferUsage::USAGE_STATIC_DRAW;
        vertexDesc.size = sizeof(cubeVertices);
        vertexDesc.initialData = cubeVertices;
        mHYVertexBuffer = mContext->createBuffer(vertexDesc);
        
        BufferDesc indexDesc;
        indexDesc.bindFlags = BIND_INDEX_BUFFER;
        indexDesc.usage = BufferUsage::USAGE_STATIC_DRAW;
        indexDesc.size = sizeof(cubeIndices);
        indexDesc.initialData = cubeIndices;
        mHYIndexBuffer = mContext->createBuffer(indexDesc);
        
        BufferDesc uniformDesc;
        uniformDesc.bindFlags = BIND_UNIFORM_BUFFER;
        uniformDesc.usage = BufferUsage::USAGE_DYNAMIC_DRAW;
        uniformDesc.size = sizeof(Uniforms);
        mHYUniformBuffer = mContext->createBuffer(uniformDesc);
        
        // 创建RenderPass
        mHYRenderPass = mContext->createRenderPass();
        
        std::cout << "hy_render资源创建成功" << std::endl;
    } else {
        std::cout << "[使用原生Metal API模式]" << std::endl;
        mUseAbstractAPI = false;
    }
    
    // 获取Metal设备（两种模式都需要）
    mDevice = MTLCreateSystemDefaultDevice();
    if (!mDevice) {
        std::cerr << "错误: 无法创建Metal设备" << std::endl;
        return false;
    }
    
    std::cout << "Metal设备: " << [[mDevice name] UTF8String] << std::endl;
    
    // 设置Metal Layer
    mMetalLayer.device = mDevice;
    mMetalLayer.pixelFormat = MTLPixelFormatBGRA8Unorm;
    
    // 创建命令队列
    mCommandQueue = [mDevice newCommandQueue];
    if (!mCommandQueue) {
        std::cerr << "错误: 无法创建命令队列" << std::endl;
        return false;
    }
    
    // 创建渲染管线
    setupRenderPipeline();
    
    // 创建缓冲区（原生模式）
    if (!mUseAbstractAPI) {
        createBuffers();
    }
    
    std::cout << "Metal渲染器初始化成功" << std::endl;
    return true;
}

void MetalCubeRenderer::setupRenderPipeline() {
    NSError* error = nil;
    
    // 加载着色器库
    NSString* shaderPath = @"cube.metal";
    id<MTLLibrary> library = nil;
    
    // 尝试从默认库加载
    library = [mDevice newDefaultLibrary];
    if (!library) {
        std::cerr << "警告: 无法加载默认着色器库，使用内联着色器代码" << std::endl;
        
        // 内联Metal着色器代码
        NSString* shaderSource = @R"(
            #include <metal_stdlib>
            using namespace metal;
            
            struct VertexIn {
                float3 position [[attribute(0)]];
                float3 color [[attribute(1)]];
            };
            
            struct VertexOut {
                float4 position [[position]];
                float3 color;
            };
            
            struct Uniforms {
                float4x4 modelMatrix;
                float4x4 viewMatrix;
                float4x4 projectionMatrix;
            };
            
            vertex VertexOut vertex_main(
                VertexIn vertexIn [[stage_in]],
                constant Uniforms& uniforms [[buffer(1)]]
            ) {
                VertexOut vertexOut;
                float4x4 mvpMatrix = uniforms.projectionMatrix * uniforms.viewMatrix * uniforms.modelMatrix;
                vertexOut.position = mvpMatrix * float4(vertexIn.position, 1.0);
                vertexOut.color = vertexIn.color;
                return vertexOut;
            }
            
            fragment float4 fragment_main(VertexOut fragmentIn [[stage_in]]) {
                return float4(fragmentIn.color, 1.0);
            }
        )";
        
        library = [mDevice newLibraryWithSource:shaderSource options:nil error:&error];
        if (!library) {
            std::cerr << "错误: 无法编译着色器: " << [[error localizedDescription] UTF8String] << std::endl;
            return;
        }
    }
    
    id<MTLFunction> vertexFunction = [library newFunctionWithName:@"vertex_main"];
    id<MTLFunction> fragmentFunction = [library newFunctionWithName:@"fragment_main"];
    
    if (!vertexFunction || !fragmentFunction) {
        std::cerr << "错误: 无法加载着色器函数" << std::endl;
        return;
    }
    
    // 创建渲染管线描述符
    MTLRenderPipelineDescriptor* pipelineDesc = [[MTLRenderPipelineDescriptor alloc] init];
    pipelineDesc.vertexFunction = vertexFunction;
    pipelineDesc.fragmentFunction = fragmentFunction;
    pipelineDesc.colorAttachments[0].pixelFormat = MTLPixelFormatBGRA8Unorm;
    pipelineDesc.depthAttachmentPixelFormat = MTLPixelFormatDepth32Float;
    
    // 设置顶点描述符
    MTLVertexDescriptor* vertexDesc = [[MTLVertexDescriptor alloc] init];
    // 位置属性
    vertexDesc.attributes[0].format = MTLVertexFormatFloat3;
    vertexDesc.attributes[0].offset = 0;
    vertexDesc.attributes[0].bufferIndex = 0;
    // 颜色属性
    vertexDesc.attributes[1].format = MTLVertexFormatFloat3;
    vertexDesc.attributes[1].offset = sizeof(float) * 3;
    vertexDesc.attributes[1].bufferIndex = 0;
    // 布局
    vertexDesc.layouts[0].stride = sizeof(Vertex);
    vertexDesc.layouts[0].stepFunction = MTLVertexStepFunctionPerVertex;
    
    pipelineDesc.vertexDescriptor = vertexDesc;
    
    // 创建管线状态
    mPipelineState = [mDevice newRenderPipelineStateWithDescriptor:pipelineDesc error:&error];
    if (!mPipelineState) {
        std::cerr << "错误: 无法创建渲染管线: " << [[error localizedDescription] UTF8String] << std::endl;
        return;
    }
    
    // 创建深度模板状态
    MTLDepthStencilDescriptor* depthDesc = [[MTLDepthStencilDescriptor alloc] init];
    depthDesc.depthCompareFunction = MTLCompareFunctionLess;
    depthDesc.depthWriteEnabled = YES;
    mDepthState = [mDevice newDepthStencilStateWithDescriptor:depthDesc];
    
    std::cout << "Metal渲染管线创建成功" << std::endl;
}

void MetalCubeRenderer::createBuffers() {
    // 创建顶点缓冲区
    mVertexBuffer = [mDevice newBufferWithBytes:cubeVertices
                                         length:sizeof(cubeVertices)
                                        options:MTLResourceStorageModeShared];
    
    // 创建索引缓冲区
    mIndexBuffer = [mDevice newBufferWithBytes:cubeIndices
                                        length:sizeof(cubeIndices)
                                       options:MTLResourceStorageModeShared];
    
    // 创建Uniform缓冲区
    mUniformBuffer = [mDevice newBufferWithLength:sizeof(Uniforms)
                                          options:MTLResourceStorageModeShared];
    
    std::cout << "缓冲区创建成功" << std::endl;
}

void MetalCubeRenderer::resize(int width, int height) {
    mWidth = width;
    mHeight = height;
    mMetalLayer.drawableSize = CGSizeMake(width, height);
}

void MetalCubeRenderer::render(float rotationX, float rotationY) {
    @autoreleasepool {
        // 获取可绘制对象
        id<CAMetalDrawable> drawable = [mMetalLayer nextDrawable];
        if (!drawable) {
            return;
        }
        
        // 更新Uniform
        Uniforms* uniforms = (Uniforms*)[mUniformBuffer contents];
        
        // 模型矩阵（旋转）
        Mat4 rotX = Mat4::rotateX(rotationX);
        Mat4 rotY = Mat4::rotateY(rotationY);
        uniforms->modelMatrix = rotY * rotX;
        
        // 视图矩阵（相机位置）
        Vec3 eye(0.0f, 0.0f, 3.0f);
        Vec3 center(0.0f, 0.0f, 0.0f);
        Vec3 up(0.0f, 1.0f, 0.0f);
        uniforms->viewMatrix = Mat4::lookAt(eye, center, up);
        
        // 投影矩阵
        float aspect = (float)mWidth / (float)mHeight;
        uniforms->projectionMatrix = Mat4::perspective(45.0f * M_PI / 180.0f, aspect, 0.1f, 100.0f);
        
        // 创建命令缓冲区
        id<MTLCommandBuffer> commandBuffer = [mCommandQueue commandBuffer];
        
        // 创建渲染通道描述符
        MTLRenderPassDescriptor* renderPassDesc = [MTLRenderPassDescriptor renderPassDescriptor];
        renderPassDesc.colorAttachments[0].texture = drawable.texture;
        renderPassDesc.colorAttachments[0].loadAction = MTLLoadActionClear;
        renderPassDesc.colorAttachments[0].clearColor = MTLClearColorMake(0.2, 0.3, 0.4, 1.0);
        renderPassDesc.colorAttachments[0].storeAction = MTLStoreActionStore;
        
        // 创建渲染编码器
        id<MTLRenderCommandEncoder> renderEncoder = [commandBuffer renderCommandEncoderWithDescriptor:renderPassDesc];
        
        [renderEncoder setRenderPipelineState:mPipelineState];
        [renderEncoder setDepthStencilState:mDepthState];
        [renderEncoder setFrontFacingWinding:MTLWindingCounterClockwise];
        [renderEncoder setCullMode:MTLCullModeBack];
        
        // 绑定缓冲区
        [renderEncoder setVertexBuffer:mVertexBuffer offset:0 atIndex:0];
        [renderEncoder setVertexBuffer:mUniformBuffer offset:0 atIndex:1];
        
        // 绘制
        [renderEncoder drawIndexedPrimitives:MTLPrimitiveTypeTriangle
                                  indexCount:sizeof(cubeIndices) / sizeof(uint16_t)
                                   indexType:MTLIndexTypeUInt16
                                 indexBuffer:mIndexBuffer
                           indexBufferOffset:0];
        
        [renderEncoder endEncoding];
        
        // 提交
        [commandBuffer presentDrawable:drawable];
        [commandBuffer commit];
    }
}

/**
 * @brief 应用委托
 */
@interface AppDelegate : NSObject <NSApplicationDelegate>
@property (nonatomic, strong) NSWindow* window;
@property (nonatomic, strong) MetalView* metalView;
@property (nonatomic, assign) MetalCubeRenderer* renderer;
@property (nonatomic, strong) NSTimer* renderTimer;
@end

@implementation AppDelegate

- (void)applicationDidFinishLaunching:(NSNotification*)notification {
    // 创建窗口
    NSRect frame = NSMakeRect(0, 0, 800, 600);
    _window = [[NSWindow alloc] initWithContentRect:frame
                                         styleMask:(NSWindowStyleMaskTitled |
                                                   NSWindowStyleMaskClosable |
                                                   NSWindowStyleMaskResizable)
                                           backing:NSBackingStoreBuffered
                                             defer:NO];
    [_window setTitle:@"HY Render - Metal Cube Demo"];
    [_window center];
    
    // 创建Metal视图
    _metalView = [[MetalView alloc] initWithFrame:frame];
    [_window setContentView:_metalView];
    
    // 初始化渲染器
    _renderer = new MetalCubeRenderer();
    if (!_renderer->initialize(_metalView.metalLayer)) {
        std::cerr << "渲染器初始化失败" << std::endl;
        [NSApp terminate:nil];
        return;
    }
    
    // 启动渲染循环
    _renderTimer = [NSTimer scheduledTimerWithTimeInterval:1.0/60.0
                                                    target:self
                                                  selector:@selector(renderFrame)
                                                  userInfo:nil
                                                   repeats:YES];
    
    [_window makeKeyAndOrderFront:nil];
    
    std::cout << "应用启动成功" << std::endl;
    std::cout << "使用鼠标拖动旋转立方体" << std::endl;
}

- (void)renderFrame {
    _renderer->render(_metalView.rotationX, _metalView.rotationY);
}

- (void)applicationWillTerminate:(NSNotification*)notification {
    [_renderTimer invalidate];
    if (_renderer) {
        delete _renderer;
        _renderer = nullptr;
    }
}

- (BOOL)applicationShouldTerminateAfterLastWindowClosed:(NSApplication*)sender {
    return YES;
}

@end

/**
 * @brief 主函数
 */
int main(int argc, const char* argv[]) {
    @autoreleasepool {
        NSApplication* app = [NSApplication sharedApplication];
        AppDelegate* delegate = [[AppDelegate alloc] init];
        [app setDelegate:delegate];
        [app run];
    }
    return 0;
}
