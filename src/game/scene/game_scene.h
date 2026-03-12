#pragma once
#include "../../engine/scene/scene.h"
#include <memory>
#include <glm/vec2.hpp>

// 前置声明
namespace engine::object {
    class GameObject;
}

namespace game::data {
    class SessionData;
}

namespace engine::ui {
class UIlabel;
class UIPanel;
class UIImage;
class UILabel;
}

namespace game::scene {

/**
 * @brief 主要的游戏场景，包含玩家、敌人、关卡元素等。
 */
class GameScene final: public engine::scene::Scene {
    // 场景间共享数据，因此用shared_ptr
    std::shared_ptr<game::data::SessionData> game_session_data_; ///< @brief 游戏会话数据，包含玩家状态、得分等, 跨场景共享
    engine::object::GameObject* player_ = nullptr;  ///< @brief 保存玩家对象的指针，方便访问
    // UI元素生命周期由UIManager管理，因此使用裸指针缓存
    engine::ui::UIPanel* health_panel_ = nullptr;   ///< @brief 显示玩家生命值的标签
    engine::ui::UILabel* score_label_ = nullptr;    ///< @brief 显示玩家得分的标签

public:
    GameScene(engine::core::Context& context, 
              engine::scene::SceneManager& scene_manager, 
              std::shared_ptr<game::data::SessionData> data = nullptr);

    // 覆盖场景基类的核心方法
    void init() override;
    void update(float delta_time) override;
    void render() override;
    void handleInput() override;
    void clean() override;

    // 游戏逻辑处理方法
    void handleObjectCollisions();  ///< @brief 处理场景中游戏对象之间的碰撞
    void handleTileTriggers(); ///< @brief 处理场景中瓦片触发事件
    void handlePlayerDamage(int damage); ///< @brief 处理玩家受伤逻辑
    void playerVSEnemyCollision(engine::object::GameObject* player, engine::object::GameObject* enemy);  ///< @brief 玩家与敌人碰撞处理
    void playerVSItemCollision(engine::object::GameObject* player, engine::object::GameObject* item);    ///< @brief 玩家与道具碰撞处理
    bool isPlayerDropOut(engine::object::GameObject* player); ///< @brief 处理玩家掉落出场景的逻辑

    /**
     * @brief 创建一个特效对象（一次性）。
     * @param center_pos 特效中心位置
     * @param tag 特效标签（决定特效类型,例如"enemy","item"）
     */
    void createEffect(const glm::vec2& center_pos, const std::string& tag);

private:
    [[nodiscard]] bool initLevel();               ///< @brief 初始化关卡
    [[nodiscard]] bool initPlayer();              ///< @brief 初始化玩家
    [[nodiscard]] bool initEnemiesAndItems();             ///< @brief 初始化敌人和道具
    [[nodiscard]] bool initUI(); ///< @brief 初始化用户界面
    /// @brief 根据关卡名称获取对应的地图文件路径
    std::string levelNameToPath(const std::string& level_name) const { 
        return "assets/maps/" + level_name + ".tmj"; 
    }
    bool toNextLevel(engine::object::GameObject* trigger); ///< @brief 处理进入下一关的逻辑
    bool loadLevel(const std::string& map_path); ///< @brief 加载关卡地图
    // ui相关方法
    void createHealthUI(); ///< @brief 创建玩家生命值UI
    void createScoreUI(); ///< @brief 创建玩家得分UI
    void addScoreWithUI(int score); ///< @brief 增加分数并更新UI显示
    void updateHealthWithUI(); ///< @brief 更新玩家生命值UI显示

};

} // namespace game::scene