#pragma once
#include "../../engine/scene/scene.h"
#include <memory>
#include <glm/vec2.hpp>

// 前置声明
namespace engine::object {
    class GameObject;
}

namespace game::scene {

/**
 * @brief 主要的游戏场景，包含玩家、敌人、关卡元素等。
 */
class GameScene final: public engine::scene::Scene {
    engine::object::GameObject* player_ = nullptr;  ///< @brief 保存玩家对象的指针，方便访问

public:
    GameScene(std::string name, engine::core::Context& context, engine::scene::SceneManager& scene_manager);

    // 覆盖场景基类的核心方法
    void init() override;
    void update(float delta_time) override;
    void render() override;
    void handleInput() override;
    void clean() override;

    // 游戏逻辑处理方法
    void handleObjectCollisions();  ///< @brief 处理场景中游戏对象之间的碰撞
    void handleTileTriggers(); ///< @brief 处理场景中瓦片触发事件
    void playerVSEnemyCollision(engine::object::GameObject* player, engine::object::GameObject* enemy);  ///< @brief 玩家与敌人碰撞处理
    void playerVSItemCollision(engine::object::GameObject* player, engine::object::GameObject* item);    ///< @brief 玩家与道具碰撞处理

    /**
     * @brief 创建一个特效对象（一次性）。
     * @param center_pos 特效中心位置
     * @param tag 特效标签（决定特效类型,例如"enemy","item"）
     */
    void createEffect(const glm::vec2& center_pos, const std::string& tag);

private:
    [[nodiscard]] bool initLevel();               ///< @brief 初始化关卡
    [[nodiscard]] bool initPlayer();              ///< @brief 初始化玩家
    [[nodiscard]] bool initEnemiesAndItems();             ///< @brief 初始化敌人
    void testHealth();                      ///< @brief 测试 HealthComponent 的功能
};

} // namespace game::scene