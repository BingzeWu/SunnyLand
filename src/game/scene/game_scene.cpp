#include "game_scene.h"
#include "../component/player_component.h"
#include "../component/ai_component.h"
#include "../component/ai/patrol_behavior.h"
#include "../component/ai/updown_behavior.h"
#include "../component/ai/jump_behavior.h"
#include "../../engine/scene/level_loader.h"
#include "../../engine/input/input_manager.h"
#include "../../engine/render/camera.h"
#include "../../engine/render/animation.h"
#include "../../engine/physics/physics_engine.h"
#include "../../engine/core/context.h"
#include "../../engine/object/game_object.h"
#include "../../engine/audio/audio_player.h"
#include "../../engine/component/transform_component.h"
#include "../../engine/component/sprite_component.h"
#include "../../engine/component/physics_component.h"
#include "../../engine/component/collider_component.h"
#include "../../engine/component/tilelayer_component.h"
#include "../../engine/component/animation_component.h"
#include "../../engine/component/health_component.h"
#include <spdlog/spdlog.h>
#include <SDL3/SDL_rect.h>

namespace game::scene {

// 构造函数：调用基类构造函数
GameScene::GameScene(std::string name, engine::core::Context& context, engine::scene::SceneManager& scene_manager)
    : Scene(name, context, scene_manager) {
    spdlog::trace("GameScene 构造完成。");
}

void GameScene::init() {
    if (is_initialized_) {
        spdlog::warn("GameScene 已经初始化过了，重复调用 init()。");
        return;
    }
    spdlog::trace("GameScene 初始化开始。");
   
    if (!initLevel()) {
        spdlog::error("关卡初始化失败，无法继续。");
        context_.getInputManager().setShouldQuit(true);
        return;
    }
    
    if (!initPlayer()) {
        spdlog::error("玩家初始化失败，无法继续。");
        context_.getInputManager().setShouldQuit(true);
        return;
    }
    if (!initEnemiesAndItems()) {
        spdlog::error("敌人和物品初始化失败，无法继续。");
        context_.getInputManager().setShouldQuit(true);
        return;
    }
    //设置音量
    context_.getAudioPlayer().setMusicVolume(0.2f);
    context_.getAudioPlayer().setSoundVolume(0.5f);
    //播放背景音乐
    if (!context_.getAudioPlayer().playMusic("assets/audio/hurry_up_and_run.ogg", true, 1000)) {
        spdlog::error("无法播放背景音乐");
    }

    Scene::init();
    spdlog::trace("GameScene 初始化完成。");
}

void GameScene::update(float delta_time) {
    Scene::update(delta_time);
    handleObjectCollisions();// 处理对象层碰撞
    handleTileTriggers();    // 处理瓦片触发事件
}

void GameScene::render() {
    Scene::render();
}

void GameScene::handleInput() {
    Scene::handleInput();
    testHealth();  // 测试生命值组件    
}

void GameScene::clean() {
    Scene::clean();
}

bool GameScene::initLevel()
{
    // 加载关卡（level_loader通常加载完成后即可销毁，因此不存为成员变量）
    engine::scene::LevelLoader level_loader;
    if (!level_loader.loadLevel("assets/maps/level1.tmj", *this)){
        spdlog::error("关卡加载失败");
        return false;
    }

    // 注册"main"层到物理引擎
    auto* main_layer = findGameObjectByName("main");
    if (!main_layer) {
        spdlog::error("未找到\"main\"层");
        return false;
    }
    auto* tile_layer = main_layer->getComponent<engine::component::TileLayerComponent>();
    if (!tile_layer) {
        spdlog::error("\"main\"层没有 TileLayerComponent 组件");
        return false;
    }
    context_.getPhysicsEngine().registerCollisionLayer(tile_layer);
    spdlog::info("注册\"main\"层到物理引擎");
    
    // 设置相机边界
    auto world_size = main_layer->getComponent<engine::component::TileLayerComponent>()->getWorldSize();
    context_.getCamera().setLimitBounds(engine::utils::Rect(glm::vec2(0.0f), world_size));

    // 设置世界边界
    context_.getPhysicsEngine().setWorldBounds(engine::utils::Rect(glm::vec2(0.0f), world_size));

    spdlog::trace("关卡初始化完成。");
    return true;
}

bool GameScene::initPlayer()
{
    // 获取玩家对象
    player_ = findGameObjectByName("player");
    if (!player_) {
        spdlog::error("未找到玩家对象");
        return false;
    }

    // 添加PlayerComponent到玩家对象
    auto* player_component = player_->addComponent<game::component::PlayerComponent>();
    if (!player_component) {
        spdlog::error("无法添加 PlayerComponent 到玩家对象");
        return false;
    }

    // 相机跟随玩家
    auto* player_transform = player_->getComponent<engine::component::TransformComponent>();
    if (!player_transform) {
        spdlog::error("玩家对象没有 TransformComponent 组件, 无法设置相机目标");
        return false;
    }
    context_.getCamera().setTarget(player_transform);
    spdlog::trace("Player初始化完成。");
    return true;
}

bool GameScene::initEnemiesAndItems()
{
    bool success = true;
    for (auto& game_object : game_objects_){
        if (game_object->getName() == "eagle"){
            if (auto* ai_component = game_object->addComponent<game::component::AIComponent>(); ai_component){
                auto y_max = game_object->getComponent<engine::component::TransformComponent>()->getPosition().y;
                auto y_min = y_max - 80.0f;  // 让鹰的飞行范围 (当前位置与上方80像素 的区域)
                ai_component->setBehavior(std::make_unique<game::component::ai::UpDownBehavior>(y_min, y_max));
            } else { spdlog::error("Eagle 无法添加 AIComponent 组件"); success = false; }
        }
        if (game_object->getName() == "frog"){
            if (auto* ai_component = game_object->addComponent<game::component::AIComponent>(); ai_component){
                auto x_max = game_object->getComponent<engine::component::TransformComponent>()->getPosition().x - 10.0f;
                auto x_min = x_max - 90.0f;
                ai_component->setBehavior(std::make_unique<game::component::ai::JumpBehavior>(x_min, x_max));
            } else { spdlog::error("Frog 无法添加 AIComponent 组件"); success = false; }
        }
        if (game_object->getName() == "opossum"){
            if (auto* ai_component = game_object->addComponent<game::component::AIComponent>(); ai_component){
                auto x_max = game_object->getComponent<engine::component::TransformComponent>()->getPosition().x;
                auto x_min = x_max - 100.0f;
                ai_component->setBehavior(std::make_unique<game::component::ai::PatrolBehavior>(x_min, x_max));
            } else { spdlog::error("Opossum 无法添加 AIComponent 组件"); success = false; }
        }
        if (game_object->getName() == "fruit"){
            if (auto* ac = game_object->getComponent<engine::component::AnimationComponent>(); ac){
                ac->playAnimation("idle");
            } else { spdlog::error("Fruit 没有 AnimationComponent 组件"); success = false; }
        }
        if (game_object->getName() == "gem"){
            if (auto* ac = game_object->getComponent<engine::component::AnimationComponent>(); ac){
                ac->playAnimation("idle");
            } else { spdlog::error("Gem 没有 AnimationComponent 组件"); success = false; }
        }
    }
    return success;
}

void GameScene::handleObjectCollisions()
{
    // 从物理引擎获取当前发生碰撞的对象对列表
    auto collision_pairs = context_.getPhysicsEngine().getCollisionPairs();
    for (const auto& pair : collision_pairs) {
        auto* objA = pair.first;
        auto* objB = pair.second;

        // 确保对象有效
        if (!objA || !objB) continue;

        // 玩家与敌人碰撞
        if ((objA->getName() == "player" && objB->getTag() == "enemy")) {
            playerVSEnemyCollision(objA, objB);
        } else if (objB->getName() == "player" && objA->getTag() == "enemy") {
            playerVSEnemyCollision(objB, objA);
        }
        // 玩家与道具碰撞
        else if ((objA->getName() == "player" && objB->getTag() == "item")) {
            playerVSItemCollision(objA, objB);
        } else if (objB->getName() == "player" && objA->getTag() == "item") {
            playerVSItemCollision(objB, objA);
        }
        // 玩家与危险瓦片碰撞
        else if ((objA->getName() == "player" && objB->getTag() == "hazard")) {
            objA->getComponent<game::component::PlayerComponent>()->takeDamage(1); // 玩家受伤，造成1点伤害
        } else if (objB->getName() == "player" && objA->getTag() == "hazard") {
            objB->getComponent<game::component::PlayerComponent>()->takeDamage(1); // 玩家受伤，造成1点伤害
        }
    }

}

void GameScene::handleTileTriggers()
{
    const auto& tile_trigger_events = context_.getPhysicsEngine().getTileTriggerEvents();
    for (const auto& event : tile_trigger_events) {
        auto* obj = event.first;      // 触发事件的对象
        auto tile_type = event.second;  // 瓦片类型
        if (tile_type == engine::component::TileType::HAZARD) {
            // 如果是玩家碰到了危险瓦片，就受伤
            if (obj->getName() == "player") {           
                obj->getComponent<game::component::PlayerComponent>()->takeDamage(1);
            } 
            // TODO: 可以在这里添加其他交互的逻辑
        }
    }
}

void GameScene::playerVSEnemyCollision(engine::object::GameObject *player, engine::object::GameObject *enemy)
{
    // --- 踩踏判断逻辑：1. 玩家中心点在敌人上方    2. 重叠区域：overlap.x > overlap.y
    auto player_aabb = player->getComponent<engine::component::ColliderComponent>()->getWorldAABB();
    auto enemy_aabb = enemy->getComponent<engine::component::ColliderComponent>()->getWorldAABB();
    auto player_center = player_aabb.position + player_aabb.size / 2.0f;
    auto enemy_center = enemy_aabb.position + enemy_aabb.size / 2.0f;
    auto overlap = glm::vec2(player_aabb.size / 2.0f + enemy_aabb.size / 2.0f) - glm::abs(player_center - enemy_center);

    // 踩踏判断成功，敌人受伤
    if (overlap.x > overlap.y && player_center.y < enemy_center.y) {    
        spdlog::info("玩家 {} 踩踏了敌人 {}", player->getName(), enemy->getName());
        auto enemy_health = enemy->getComponent<engine::component::HealthComponent>();
        if (!enemy_health) { /* ... */ return; }

        enemy_health->takeDamage(1);  // 造成1点伤害
        if (!enemy_health->isAlive()) {
            enemy->setNeedRemove(true);  // 标记敌人为待删除状态
            createEffect(enemy_center, enemy->getTag());  // 创建（死亡）特效
        }
        // 玩家跳起效果
        player->getComponent<engine::component::PhysicsComponent>()->velocity_.y = -300.0f;
    }
    // 踩踏判断失败，玩家受伤
    else {
        player->getComponent<game::component::PlayerComponent>()->takeDamage(1);
    }
}

void GameScene::playerVSItemCollision(engine::object::GameObject *player, engine::object::GameObject *item)
{
    if (item->getName() == "fruit") {
        player->getComponent<engine::component::HealthComponent>()->heal(1);  // 加血
    } else if (item->getName() == "gem") {
        //TODO: 加分
    }
    item->setNeedRemove(true);  // 标记道具为待删除状态
    auto item_aabb = item->getComponent<engine::component::ColliderComponent>()->getWorldAABB();
    createEffect(item_aabb.position + item_aabb.size / 2.0f, item->getTag());  // 创建特效
}

void GameScene::createEffect(const glm::vec2 &center_pos, const std::string &tag)
{
    auto effect_obj = std::make_unique<engine::object::GameObject>("effect_" + tag);
    effect_obj->addComponent<engine::component::TransformComponent>(center_pos);

    // 根据标签创建不同的精灵组件和动画
    auto animation = std::make_unique<engine::render::Animation>("effect", false); // false表示不循环
    if (tag == "enemy") {
        effect_obj->addComponent<engine::component::SpriteComponent>("assets/textures/FX/enemy-deadth.png", 
                                                                    context_.getResourceManager(),
                                                                    engine::utils::Alignment::CENTER);
        for (auto i = 0; i < 5; ++i) {
            animation->addFrame({static_cast<float>(i * 40), 0.0f, 40.0f, 41.0f}, 0.1f);
        }
    } else if (tag == "item") {
        effect_obj->addComponent<engine::component::SpriteComponent>("assets/textures/FX/item-feedback.png", 
                                                                    context_.getResourceManager(),
                                                                    engine::utils::Alignment::CENTER);
        for (auto i = 0; i < 4; ++i) {
            animation->addFrame({static_cast<float>(i * 32), 0.0f, 32.0f, 32.0f}, 0.1f);
        }
    } else { return; }

    // 添加动画组件，并设置为单次播放后自动移除
    auto* animation_component = effect_obj->addComponent<engine::component::AnimationComponent>();
    animation_component->addAnimation(std::move(animation));
    animation_component->setOneShotRemoval(true); // 核心！确保特效播放完成后自动移除
    animation_component->playAnimation("effect");
    
    safeAddGameObject(std::move(effect_obj));  // 安全添加特效对象
}

void GameScene::testHealth()
{
    auto input_manager = context_.getInputManager();
    if (input_manager.isActionPressed("attack")) {
        player_->getComponent<game::component::PlayerComponent>()->takeDamage(1);
    }
}

} // namespace game::scene