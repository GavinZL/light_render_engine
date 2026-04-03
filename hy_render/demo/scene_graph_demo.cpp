#include "hy_render/HYScene.hpp"
#include "hy_render/HYMath.hpp"
#include <iostream>
#include <iomanip>

using namespace hyengine::render;
using namespace hyengine::math;

/**
 * @brief 打印实体层级结构
 */
void printEntityHierarchy(std::shared_ptr<Entity> entity, int depth = 0) {
    // 打印缩进
    for (int i = 0; i < depth; ++i) {
        std::cout << "  ";
    }
    
    // 打印实体信息
    std::cout << "- " << entity->getName() << " (ID: " << entity->getID() << ")";
    
    // 打印Transform信息
    auto transform = entity->getTransform();
    if (transform) {
        auto pos = transform->getPosition();
        std::cout << " Pos: (" << std::fixed << std::setprecision(2)
                  << pos.x << ", " << pos.y << ", " << pos.z << ")";
    }
    
    std::cout << std::endl;
    
    // 递归打印子实体
    for (auto& child : entity->getChildren()) {
        printEntityHierarchy(child, depth + 1);
    }
}

/**
 * @brief 演示基本的Entity-Component系统
 */
void demoBasicEntityComponent() {
    std::cout << "\n=== Demo 1: Basic Entity-Component ===" << std::endl;
    
    // 创建场景
    Scene scene("MainScene");
    
    // 创建实体
    auto entity = scene.createEntity("Player");
    
    // 访问Transform组件（默认自带）
    auto transform = entity->getTransform();
    transform->setPosition(Vec3(1.0f, 2.0f, 3.0f));
    transform->setScale(Vec3(2.0f, 2.0f, 2.0f));
    
    // 添加MeshRenderer组件
    auto renderer = entity->addComponent<MeshRendererComponent>();
    renderer->setVisible(true);
    renderer->setCastShadows(true);
    
    // 添加Camera组件
    auto camera = entity->addComponent<CameraComponent>();
    camera->setPerspective(45.0f, 16.0f/9.0f, 0.1f, 100.0f);
    camera->setPrimary(true);
    
    // 打印组件信息
    std::cout << "Entity: " << entity->getName() << std::endl;
    std::cout << "  Components: " << entity->getAllComponents().size() << std::endl;
    
    for (auto& component : entity->getAllComponents()) {
        std::cout << "    - " << component->getTypeName() 
                  << (component->isEnabled() ? " (enabled)" : " (disabled)") << std::endl;
    }
    
    // 测试组件查询
    std::cout << "\n  Component queries:" << std::endl;
    std::cout << "    Has Transform: " << (entity->hasComponent<TransformComponent>() ? "Yes" : "No") << std::endl;
    std::cout << "    Has MeshRenderer: " << (entity->hasComponent<MeshRendererComponent>() ? "Yes" : "No") << std::endl;
    std::cout << "    Has Camera: " << (entity->hasComponent<CameraComponent>() ? "Yes" : "No") << std::endl;
    std::cout << "    Has Light: " << (entity->hasComponent<LightComponent>() ? "Yes" : "No") << std::endl;
}

/**
 * @brief 演示实体层级结构
 */
void demoEntityHierarchy() {
    std::cout << "\n=== Demo 2: Entity Hierarchy ===" << std::endl;
    
    Scene scene("HierarchyScene");
    
    // 创建根实体
    auto root = scene.createEntity("Root");
    root->getTransform()->setPosition(Vec3(0.0f, 0.0f, 0.0f));
    
    // 创建子实体
    auto child1 = scene.createEntity("Child1");
    child1->getTransform()->setPosition(Vec3(1.0f, 0.0f, 0.0f));
    root->addChild(child1);
    
    auto child2 = scene.createEntity("Child2");
    child2->getTransform()->setPosition(Vec3(-1.0f, 0.0f, 0.0f));
    root->addChild(child2);
    
    // 创建孙子实体
    auto grandchild = scene.createEntity("GrandChild");
    grandchild->getTransform()->setPosition(Vec3(0.0f, 1.0f, 0.0f));
    child1->addChild(grandchild);
    
    // 打印层级结构
    std::cout << "\nHierarchy:" << std::endl;
    printEntityHierarchy(root);
    
    // 测试世界坐标
    std::cout << "\nWorld positions:" << std::endl;
    auto rootPos = root->getWorldPosition();
    auto child1Pos = child1->getWorldPosition();
    auto grandchildPos = grandchild->getWorldPosition();
    
    std::cout << "  Root: (" << rootPos.x << ", " << rootPos.y << ", " << rootPos.z << ")" << std::endl;
    std::cout << "  Child1: (" << child1Pos.x << ", " << child1Pos.y << ", " << child1Pos.z << ")" << std::endl;
    std::cout << "  GrandChild: (" << grandchildPos.x << ", " << grandchildPos.y << ", " << grandchildPos.z << ")" << std::endl;
}

/**
 * @brief 演示场景和系统
 */
void demoSceneAndSystems() {
    std::cout << "\n=== Demo 3: Scene and Systems ===" << std::endl;
    
    Scene scene("GameScene");
    
    // 添加系统
    auto renderSystem = scene.addSystem<RenderSystem>();
    auto cameraSystem = scene.addSystem<CameraSystem>();
    
    std::cout << "Systems added: " << scene.getAllSystems().size() << std::endl;
    for (auto& system : scene.getAllSystems()) {
        std::cout << "  - " << system->getName() 
                  << " (priority: " << system->getPriority() << ")" << std::endl;
    }
    
    // 创建相机实体
    auto cameraEntity = scene.createEntity("MainCamera");
    auto camera = cameraEntity->getComponent<CameraComponent>();
    if (!camera) {
        camera = cameraEntity->addComponent<CameraComponent>();
    }
    camera->setPrimary(true);
    camera->setPerspective(60.0f, 16.0f/9.0f, 0.1f, 1000.0f);
    cameraEntity->getTransform()->setPosition(Vec3(0.0f, 5.0f, 10.0f));
    
    // 创建一些可渲染实体
    for (int i = 0; i < 5; ++i) {
        auto entity = scene.createEntity("Cube_" + std::to_string(i));
        entity->getTransform()->setPosition(Vec3(i * 2.0f, 0.0f, 0.0f));
        entity->addComponent<MeshRendererComponent>();
    }
    
    // 创建光源
    auto lightEntity = scene.createEntity("DirectionalLight");
    auto light = lightEntity->addComponent<LightComponent>();
    light->setType(LightComponent::LightType::kDirectional);
    light->setColor(Vec3(1.0f, 1.0f, 1.0f));
    light->setIntensity(1.0f);
    lightEntity->getTransform()->setRotationFromAxisAngle(
        degToRad(45.0f), Vec3(1.0f, 0.0f, 0.0f)
    );
    
    // 查询信息
    std::cout << "\nScene statistics:" << std::endl;
    std::cout << "  Total entities: " << scene.getEntityCount() << std::endl;
    
    auto activeCam = cameraSystem->getActiveCamera();
    if (activeCam) {
        std::cout << "  Active camera: " << activeCam->getName() << std::endl;
    }
    
    auto renderableEntities = renderSystem->getRenderableEntities();
    std::cout << "  Renderable entities: " << renderableEntities.size() << std::endl;
    for (auto& entity : renderableEntities) {
        std::cout << "    - " << entity->getName() << std::endl;
    }
}

/**
 * @brief 演示Camera组件
 */
void demoCameraComponent() {
    std::cout << "\n=== Demo 4: Camera Component ===" << std::endl;
    
    Scene scene("CameraScene");
    
    // 创建透视相机
    auto perspCamera = scene.createEntity("PerspectiveCamera");
    auto perspCam = perspCamera->addComponent<CameraComponent>();
    perspCam->setPerspective(45.0f, 16.0f/9.0f, 0.1f, 100.0f);
    perspCam->setPrimary(true);
    
    std::cout << "Perspective Camera:" << std::endl;
    std::cout << "  FOV: " << perspCam->getFov() << std::endl;
    std::cout << "  Aspect: " << perspCam->getAspect() << std::endl;
    std::cout << "  Near: " << perspCam->getNearPlane() << std::endl;
    std::cout << "  Far: " << perspCam->getFarPlane() << std::endl;
    std::cout << "  Is Primary: " << (perspCam->isPrimary() ? "Yes" : "No") << std::endl;
    
    // 创建正交相机
    auto orthoCamera = scene.createEntity("OrthographicCamera");
    auto orthoCam = orthoCamera->addComponent<CameraComponent>();
    orthoCam->setOrthographic(10.0f, 16.0f/9.0f, -10.0f, 10.0f);
    
    std::cout << "\nOrthographic Camera:" << std::endl;
    std::cout << "  Type: Orthographic" << std::endl;
    std::cout << "  Near: " << orthoCam->getNearPlane() << std::endl;
    std::cout << "  Far: " << orthoCam->getFarPlane() << std::endl;
}

/**
 * @brief 演示Transform组件
 */
void demoTransformComponent() {
    std::cout << "\n=== Demo 5: Transform Component ===" << std::endl;
    
    Scene scene("TransformScene");
    
    auto entity = scene.createEntity("TransformDemo");
    auto transform = entity->getTransform();
    
    // 设置位置
    transform->setPosition(Vec3(1.0f, 2.0f, 3.0f));
    std::cout << "Position: (" 
              << transform->getPosition().x << ", "
              << transform->getPosition().y << ", "
              << transform->getPosition().z << ")" << std::endl;
    
    // 设置缩放
    transform->setScale(Vec3(2.0f, 2.0f, 2.0f));
    std::cout << "Scale: ("
              << transform->getScale().x << ", "
              << transform->getScale().y << ", "
              << transform->getScale().z << ")" << std::endl;
    
    // 设置旋转
    transform->setRotationFromAxisAngle(degToRad(90.0f), Vec3(0.0f, 1.0f, 0.0f));
    
    // 获取方向向量
    auto forward = transform->getForward();
    auto right = transform->getRight();
    auto up = transform->getUp();
    
    std::cout << "Forward: (" << forward.x << ", " << forward.y << ", " << forward.z << ")" << std::endl;
    std::cout << "Right: (" << right.x << ", " << right.y << ", " << right.z << ")" << std::endl;
    std::cout << "Up: (" << up.x << ", " << up.y << ", " << up.z << ")" << std::endl;
}

int main() {
    std::cout << "==================================================" << std::endl;
    std::cout << "    HY Render Engine - Scene Graph Demo" << std::endl;
    std::cout << "    Version: " << SceneGraphInfo::VERSION_STRING << std::endl;
    std::cout << "==================================================" << std::endl;
    
    try {
        // 运行各个演示
        demoBasicEntityComponent();
        demoEntityHierarchy();
        demoSceneAndSystems();
        demoCameraComponent();
        demoTransformComponent();
        
        std::cout << "\n==================================================" << std::endl;
        std::cout << "All demos completed successfully!" << std::endl;
        std::cout << "==================================================" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
