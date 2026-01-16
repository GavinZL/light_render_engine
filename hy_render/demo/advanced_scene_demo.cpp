#include "hy_render/HYScene.hpp"
#include "hy_render/HYMath.hpp"
#include <iostream>
#include <iomanip>
#include <chrono>

using namespace hyengine::render;
using namespace hyengine::math;

/**
 * @brief 打印场景统计信息
 */
void printSceneStats(const SceneManager& sceneManager) {
    auto stats = sceneManager.getSceneStats();
    
    std::cout << "\n--- Scene Statistics ---" << std::endl;
    std::cout << "Total Scenes: " << stats.totalScenes << std::endl;
    std::cout << "Active Scene: " << stats.activeSceneName << std::endl;
    std::cout << "Total Entities: " << stats.totalEntities << std::endl;
    std::cout << "Render Entities: " << stats.totalRenderEntities << std::endl;
    std::cout << "Visible Render Entities: " << stats.visibleRenderEntities << std::endl;
}

/**
 * @brief 打印渲染队列统计信息
 */
void printRenderStats(const RenderQueue& renderQueue) {
    auto stats = renderQueue.getRenderStats();
    
    std::cout << "\n--- Render Queue Statistics ---" << std::endl;
    std::cout << "Total Commands: " << stats.totalCommands << std::endl;
    std::cout << "Total Batches: " << stats.totalBatches << std::endl;
    std::cout << "Total Complexity: " << stats.totalComplexity << std::endl;
    
    const char* layerNames[] = {"Background", "Opaque", "Transparent", "Overlay", "UI", "Debug"};
    for (size_t i = 0; i < static_cast<size_t>(RenderEntity::RenderLayer::kMax); ++i) {
        if (stats.entitiesByLayer[i] > 0) {
            std::cout << "  " << layerNames[i] << ": " << stats.entitiesByLayer[i] << " entities" << std::endl;
        }
    }
}

/**
 * @brief 演示SceneManager基础功能
 */
void demoSceneManager() {
    std::cout << "\n=== Demo 1: SceneManager Basics ===" << std::endl;
    
    SceneManager sceneManager;
    
    // 创建多个场景
    auto mainScene = sceneManager.createScene("MainScene");
    auto menuScene = sceneManager.createScene("MenuScene");
    auto gameScene = sceneManager.createScene("GameScene");
    
    std::cout << "Created 3 scenes" << std::endl;
    std::cout << "Active scene: " << sceneManager.getActiveScene()->getName() << std::endl;
    
    // 场景切换
    sceneManager.setActiveScene("GameScene");
    std::cout << "Switched to: " << sceneManager.getActiveScene()->getName() << std::endl;
    
    // 场景切换回调
    sceneManager.setSceneTransitionCallback([](std::shared_ptr<Scene> from, std::shared_ptr<Scene> to) {
        std::cout << "Scene transition: ";
        if (from) std::cout << from->getName();
        else std::cout << "nullptr";
        std::cout << " -> ";
        if (to) std::cout << to->getName();
        else std::cout << "nullptr";
        std::cout << std::endl;
    });
    
    sceneManager.setActiveScene("MenuScene");
    
    printSceneStats(sceneManager);
}

/**
 * @brief 演示RenderEntity功能
 */
void demoRenderEntity() {
    std::cout << "\n=== Demo 2: RenderEntity Management ===" << std::endl;
    
    SceneManager sceneManager;
    auto scene = sceneManager.createScene("RenderScene");
    
    // 创建不同类型的RenderEntity
    auto backgroundEntity = sceneManager.createRenderEntity("Background");
    backgroundEntity->setRenderLayer(RenderEntity::RenderLayer::kBackground);
    backgroundEntity->setSortingOrder(-100);
    backgroundEntity->addRenderTag("background");
    
    auto playerEntity = sceneManager.createRenderEntity("Player");
    playerEntity->setRenderLayer(RenderEntity::RenderLayer::kOpaque);
    playerEntity->setSortingOrder(0);
    playerEntity->addRenderTag("player");
    playerEntity->addRenderTag("important");
    playerEntity->getTransform()->setPosition(Vec3(0.0f, 0.0f, 0.0f));
    
    auto enemyEntity = sceneManager.createRenderEntity("Enemy");
    enemyEntity->setRenderLayer(RenderEntity::RenderLayer::kOpaque);
    enemyEntity->setSortingOrder(10);
    enemyEntity->addRenderTag("enemy");
    enemyEntity->getTransform()->setPosition(Vec3(5.0f, 0.0f, 0.0f));
    
    auto uiEntity = sceneManager.createRenderEntity("UI_Panel");
    uiEntity->setRenderLayer(RenderEntity::RenderLayer::kUI);
    uiEntity->setSortingOrder(1000);
    uiEntity->addRenderTag("ui");
    
    auto particleEntity = sceneManager.createRenderEntity("Particles");
    particleEntity->setRenderLayer(RenderEntity::RenderLayer::kTransparent);
    particleEntity->setSortingOrder(50);
    particleEntity->addRenderTag("effect");
    particleEntity->setCastShadows(false);
    
    std::cout << "Created 5 RenderEntities with different properties" << std::endl;
    
    // 测试层级查询
    auto opaqueEntities = sceneManager.getRenderEntitiesByLayer(RenderEntity::RenderLayer::kOpaque);
    std::cout << "Opaque entities: " << opaqueEntities.size() << std::endl;
    for (auto& entity : opaqueEntities) {
        std::cout << "  - " << entity->getName() 
                  << " (sort: " << entity->getSortingOrder() << ")" << std::endl;
    }
    
    // 测试标签查询
    auto importantEntities = sceneManager.getRenderEntitiesByTag("important");
    std::cout << "Important entities: " << importantEntities.size() << std::endl;
    for (auto& entity : importantEntities) {
        std::cout << "  - " << entity->getName() << std::endl;
    }
    
    printSceneStats(sceneManager);
}

/**
 * @brief 演示EntityQuery系统
 */
void demoEntityQuery() {
    std::cout << "\n=== Demo 3: Entity Query System ===" << std::endl;
    
    SceneManager sceneManager;
    auto scene = sceneManager.createScene("QueryScene");
    
    // 创建各种实体用于查询测试
    auto camera = scene->createEntity("MainCamera");
    camera->addComponent<CameraComponent>();
    
    auto light = scene->createEntity("DirectionalLight");
    light->addComponent<LightComponent>();
    
    // 创建一些RenderEntity
    for (int i = 0; i < 10; ++i) {
        auto entity = sceneManager.createRenderEntity("Cube_" + std::to_string(i));
        entity->getTransform()->setPosition(Vec3(i * 2.0f, 0.0f, 0.0f));
        
        if (i % 3 == 0) {
            entity->addRenderTag("special");
        }
        if (i % 2 == 0) {
            entity->setRenderLayer(RenderEntity::RenderLayer::kTransparent);
        }
        if (i < 5) {
            entity->addRenderTag("group_a");
        } else {
            entity->addRenderTag("group_b");
        }
    }
    
    // 执行各种查询
    EntityQuery query;
    auto allEntities = scene->getAllEntities();
    
    // 查询有CameraComponent的实体
    auto cameraEntities = query.from(allEntities)
                               .withComponent<CameraComponent>()
                               .execute();
    std::cout << "Entities with Camera: " << cameraEntities.size() << std::endl;
    
    // 查询RenderEntity
    auto renderEntities = query.reset()
                               .from(allEntities)
                               .renderEntitiesOnly()
                               .execute();
    std::cout << "RenderEntities: " << renderEntities.size() << std::endl;
    
    // 查询透明的RenderEntity
    auto transparentEntities = query.reset()
                                    .from(allEntities)
                                    .renderEntitiesOnly()
                                    .withRenderLayer(RenderEntity::RenderLayer::kTransparent)
                                    .execute();
    std::cout << "Transparent RenderEntities: " << transparentEntities.size() << std::endl;
    
    // 查询有特殊标签的实体
    auto specialEntities = query.reset()
                                .from(allEntities)
                                .renderEntitiesOnly()
                                .withRenderTag("special")
                                .execute();
    std::cout << "Entities with 'special' tag: " << specialEntities.size() << std::endl;
    
    // 空间查询 - 在原点附近的实体
    auto nearOriginEntities = query.reset()
                                   .from(allEntities)
                                   .inSphere(Vec3(0.0f, 0.0f, 0.0f), 5.0f)
                                   .execute();
    std::cout << "Entities near origin (radius 5): " << nearOriginEntities.size() << std::endl;
    
    // 复合查询 - 透明的、有group_a标签的、在指定范围内的实体
    auto complexQuery = query.reset()
                             .from(allEntities)
                             .renderEntitiesOnly()
                             .withRenderLayer(RenderEntity::RenderLayer::kTransparent)
                             .withRenderTag("group_a")
                             .inBox(Vec3(-1.0f, -1.0f, -1.0f), Vec3(10.0f, 1.0f, 1.0f))
                             .execute();
    std::cout << "Complex query result: " << complexQuery.size() << std::endl;
}

/**
 * @brief 演示RenderQueue系统
 */
void demoRenderQueue() {
    std::cout << "\n=== Demo 4: Render Queue System ===" << std::endl;
    
    SceneManager sceneManager;
    auto scene = sceneManager.createScene("RenderQueueScene");
    
    // 创建相机
    auto cameraEntity = scene->createEntity("Camera");
    auto camera = cameraEntity->addComponent<CameraComponent>();
    camera->setPrimary(true);
    cameraEntity->getTransform()->setPosition(Vec3(0.0f, 0.0f, 10.0f));
    
    // 创建各种渲染实体用于测试排序
    struct EntityConfig {
        std::string name;
        RenderEntity::RenderLayer layer;
        int32_t sortOrder;
        Vec3 position;
        std::string tag;
    };
    
    std::vector<EntityConfig> configs = {
        {"Background", RenderEntity::RenderLayer::kBackground, 0, Vec3(0, 0, -5), "bg"},
        {"Cube1", RenderEntity::RenderLayer::kOpaque, 10, Vec3(1, 0, 0), "cube"},
        {"Cube2", RenderEntity::RenderLayer::kOpaque, 5, Vec3(2, 0, 0), "cube"},
        {"Cube3", RenderEntity::RenderLayer::kOpaque, 15, Vec3(3, 0, 0), "cube"},
        {"Glass1", RenderEntity::RenderLayer::kTransparent, 20, Vec3(0, 1, -2), "glass"},
        {"Glass2", RenderEntity::RenderLayer::kTransparent, 25, Vec3(0, 1, -8), "glass"},
        {"UI_Button", RenderEntity::RenderLayer::kUI, 100, Vec3(0, 0, 15), "ui"},
        {"Overlay", RenderEntity::RenderLayer::kOverlay, 200, Vec3(0, 0, 20), "overlay"}
    };
    
    std::vector<std::shared_ptr<RenderEntity>> renderEntities;
    for (const auto& config : configs) {
        auto entity = sceneManager.createRenderEntity(config.name);
        entity->setRenderLayer(config.layer);
        entity->setSortingOrder(config.sortOrder);
        entity->getTransform()->setPosition(config.position);
        entity->addRenderTag(config.tag);
        renderEntities.push_back(entity);
    }
    
    std::cout << "Created " << renderEntities.size() << " render entities" << std::endl;
    
    // 创建渲染队列并添加实体
    RenderQueue renderQueue;
    renderQueue.addRenderEntities(renderEntities);
    
    std::cout << "\nBefore sorting:" << std::endl;
    auto commands = renderQueue.getRenderCommands();
    for (const auto& cmd : commands) {
        std::cout << "  " << cmd.entity->getName() 
                  << " (layer: " << static_cast<int>(cmd.entity->getRenderLayer())
                  << ", sort: " << cmd.entity->getSortingOrder() << ")" << std::endl;
    }
    
    // 执行完整的渲染准备
    renderQueue.prepareForRender(cameraEntity);
    
    std::cout << "\nAfter sorting and batching:" << std::endl;
    auto batches = renderQueue.getRenderBatches();
    for (const auto& batch : batches) {
        const char* layerNames[] = {"Background", "Opaque", "Transparent", "Overlay", "UI", "Debug"};
        std::cout << "Batch [" << layerNames[static_cast<int>(batch.layer)] 
                  << "] - " << batch.entities.size() << " entities:" << std::endl;
        
        for (const auto& entity : batch.entities) {
            std::cout << "    " << entity->getName() 
                      << " (sort: " << entity->getSortingOrder() 
                      << ", dist: " << std::fixed << std::setprecision(2)
                      << (entity->getWorldPosition() - cameraEntity->getTransform()->getPosition()).length()
                      << ")" << std::endl;
        }
    }
    
    printRenderStats(renderQueue);
    
    // 测试标签过滤
    std::cout << "\nFiltering by 'cube' tag:" << std::endl;
    renderQueue.clear();
    renderQueue.addRenderEntities(renderEntities);
    renderQueue.filterByTag("cube", true);
    std::cout << "Entities after filtering: " << renderQueue.getRenderCommands().size() << std::endl;
}

/**
 * @brief 演示TagManager系统
 */
void demoTagManager() {
    std::cout << "\n=== Demo 5: Tag Manager System ===" << std::endl;
    
    SceneManager sceneManager;
    auto scene = sceneManager.createScene("TagScene");
    TagManager tagManager;
    
    // 创建一些实体
    auto player = scene->createEntity("Player");
    auto enemy1 = scene->createEntity("Enemy1");
    auto enemy2 = scene->createEntity("Enemy2");
    auto powerup = scene->createEntity("PowerUp");
    
    // 添加标签
    tagManager.addTag(player, "player");
    tagManager.addTag(player, "important");
    tagManager.addTag(player, "controllable");
    
    tagManager.addTag(enemy1, "enemy");
    tagManager.addTag(enemy1, "ai");
    tagManager.addTag(enemy2, "enemy");
    tagManager.addTag(enemy2, "ai");
    tagManager.addTag(enemy2, "boss");
    
    tagManager.addTag(powerup, "pickup");
    tagManager.addTag(powerup, "temporary");
    
    // 查询标签
    std::cout << "Entities with 'enemy' tag:" << std::endl;
    auto enemies = tagManager.getEntitiesWithTag("enemy");
    for (auto& entity : enemies) {
        std::cout << "  - " << entity->getName() << std::endl;
    }
    
    std::cout << "\nPlayer tags:" << std::endl;
    auto playerTags = tagManager.getEntityTags(player);
    for (const auto& tag : playerTags) {
        std::cout << "  - " << tag << std::endl;
    }
    
    std::cout << "\nAll tags in system:" << std::endl;
    auto allTags = tagManager.getAllTags();
    for (const auto& tag : allTags) {
        auto entities = tagManager.getEntitiesWithTag(tag);
        std::cout << "  " << tag << ": " << entities.size() << " entities" << std::endl;
    }
    
    // 移除标签
    tagManager.removeTag(enemy2, "boss");
    std::cout << "\nAfter removing 'boss' tag from Enemy2:" << std::endl;
    auto bossEntities = tagManager.getEntitiesWithTag("boss");
    std::cout << "Entities with 'boss' tag: " << bossEntities.size() << std::endl;
}

/**
 * @brief 性能测试
 */
void performanceTest() {
    std::cout << "\n=== Demo 6: Performance Test ===" << std::endl;
    
    SceneManager sceneManager;
    auto scene = sceneManager.createScene("PerformanceScene");
    
    const size_t entityCount = 1000;
    std::cout << "Creating " << entityCount << " RenderEntities..." << std::endl;
    
    auto start = std::chrono::high_resolution_clock::now();
    
    // 创建大量实体
    std::vector<std::shared_ptr<RenderEntity>> entities;
    entities.reserve(entityCount);
    
    for (size_t i = 0; i < entityCount; ++i) {
        auto entity = sceneManager.createRenderEntity("Entity_" + std::to_string(i));
        entity->getTransform()->setPosition(Vec3(
            static_cast<float>(i % 100), 
            static_cast<float>((i / 100) % 100), 
            static_cast<float>(i / 10000)
        ));
        
        entity->setRenderLayer(static_cast<RenderEntity::RenderLayer>(i % 3));
        entity->setSortingOrder(static_cast<int32_t>(i % 1000));
        
        if (i % 10 == 0) entity->addRenderTag("special");
        if (i % 5 == 0) entity->addRenderTag("group_a");
        if (i % 7 == 0) entity->addRenderTag("group_b");
        
        entities.push_back(entity);
    }
    
    auto createTime = std::chrono::high_resolution_clock::now();
    auto createDuration = std::chrono::duration_cast<std::chrono::milliseconds>(createTime - start);
    std::cout << "Entity creation time: " << createDuration.count() << "ms" << std::endl;
    
    // 测试渲染队列性能
    RenderQueue renderQueue;
    renderQueue.addRenderEntities(entities);
    
    auto camera = scene->createEntity("Camera");
    camera->addComponent<CameraComponent>();
    camera->getTransform()->setPosition(Vec3(50.0f, 50.0f, 50.0f));
    
    auto sortStart = std::chrono::high_resolution_clock::now();
    renderQueue.prepareForRender(camera);
    auto sortEnd = std::chrono::high_resolution_clock::now();
    auto sortDuration = std::chrono::duration_cast<std::chrono::microseconds>(sortEnd - sortStart);
    
    std::cout << "Render queue preparation time: " << sortDuration.count() << "μs" << std::endl;
    
    // 测试查询性能
    EntityQuery query;
    auto allEntities = scene->getAllEntities();
    
    auto queryStart = std::chrono::high_resolution_clock::now();
    auto specialEntities = query.from(allEntities)
                                .renderEntitiesOnly()
                                .withRenderTag("special")
                                .execute();
    auto queryEnd = std::chrono::high_resolution_clock::now();
    auto queryDuration = std::chrono::duration_cast<std::chrono::microseconds>(queryEnd - queryStart);
    
    std::cout << "Query time (" << specialEntities.size() << " results): " 
              << queryDuration.count() << "μs" << std::endl;
    
    printSceneStats(sceneManager);
    printRenderStats(renderQueue);
}

int main() {
    std::cout << "==================================================" << std::endl;
    std::cout << "    HY Render Engine - Advanced Scene Demo" << std::endl;
    std::cout << "    Version: " << SceneGraphInfo::VERSION_STRING << std::endl;
    std::cout << "==================================================" << std::endl;
    
    try {
        // 运行各个演示
        demoSceneManager();
        demoRenderEntity();
        demoEntityQuery();
        demoRenderQueue();
        demoTagManager();
        performanceTest();
        
        std::cout << "\n==================================================" << std::endl;
        std::cout << "All advanced scene demos completed successfully!" << std::endl;
        std::cout << "==================================================" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}