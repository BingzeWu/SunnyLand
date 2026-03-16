#pragma once
#include "../../engine/scene/scene.h"
#include "../../engine/ui/ui_image.h"
#include <memory>

namespace engine::core {
    class Context;
}
namespace engine::render {
    class Renderer;
    class Camera;
}
namespace engine::input {
    class InputManager;
}
namespace engine::scene {
    class SceneManager;
}

namespace game::scene {
/**
 * @brief 帮助场景类
 * 
 * 显示一张图片，左键退出场景
 */
class HelpsScene final : public engine::scene::Scene {

public:
    HelpsScene(engine::core::Context& context, engine::scene::SceneManager& scene_manager);
    ~HelpsScene() = default;
    // 禁止拷贝和移动
    HelpsScene(const HelpsScene&) = delete;
    HelpsScene& operator=(const HelpsScene&) = delete;
    HelpsScene(HelpsScene&&) = delete;
    HelpsScene& operator=(HelpsScene&&) = delete;

    //核心方法
    void init() override;
    void handleInput() override;
};

} // namespace game::scene
