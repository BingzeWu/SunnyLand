#pragma once
#include "../../engine/scene/scene.h"
#include <memory>

namespace engine::object {
    class GameObject;
}

namespace engine::scene {
    class SceneManager;
}

namespace game::data {
    class SessionData;
}

namespace game::scene {
    /**
     * @brief 标题场景，包含开始游戏、退出游戏等按钮。
     */
class TitleScene final: public engine::scene::Scene {
    std::shared_ptr<game::data::SessionData> session_data_; ///< @brief 游戏会话数据，包含玩家状态、得分等, 跨场景共享

public:
    TitleScene(engine::core::Context& context, 
                engine::scene::SceneManager& scene_manager, 
                std::shared_ptr<game::data::SessionData> data = nullptr);
    ~TitleScene() override = default;
    // 禁止拷贝和移动构造
    TitleScene(const TitleScene&) = delete;
    TitleScene(TitleScene&&) = delete;
    TitleScene& operator=(const TitleScene&) = delete;
    TitleScene& operator=(TitleScene&&) = delete;

    // 覆盖场景基类的核心方法
    void init() override;
    void update(float delta_time) override;

private:
    // 初始化 UI 元素
    void createUI();

    // 按钮回调函数
    void onStartGameClick();
    void onLoadGameClick();
    void onHelpsClick();
    void onQuitClick();
};

}// namespace game::scene

