#include "helps_scene.h"
#include "../../engine/core/context.h"
#include "../../engine/input/input_manager.h"
#include "../../engine/scene/scene_manager.h"
#include "../../engine/ui/ui_manager.h"
#include "../../engine/ui/ui_image.h"
#include <glm/vec2.hpp> // glm::vec2
#include <spdlog/spdlog.h>

namespace game::scene {

HelpsScene::HelpsScene(engine::core::Context& context, engine::scene::SceneManager& scene_manager):
    engine::scene::Scene("HelpsScene",context, scene_manager)
{
    spdlog::trace("HelpsScene 创建");
}

void HelpsScene::init()
{
    if (is_initialized_) {
        spdlog::warn("HelpsScene 已初始化，重复初始化");
        return;
    }
    auto window_size = glm::vec2(640.0f, 360.0f); 
    auto help_image = std::make_unique<engine::ui::UIImage>(
        "assets/textures/UI/instructions.png",
        glm::vec2(0.0f, 0.0f),
        window_size
    );

    ui_manager_->addElement(std::move(help_image));

    Scene::init();
    spdlog::trace("HelpsScene 初始化完成.");
}

void HelpsScene::handleInput()
{
    if(!is_initialized_) {
        spdlog::warn("HelpsScene 未初始化，无法处理输入");
        return;
    }
    // 处理退出场景, 点击鼠标左键退出场景
    if(context_.getInputManager().isActionPressed("MouseLeftClick")) {
        scene_manager_.requestPopScene();
        spdlog::debug("退出 HelpsScene");
    }
}

}// namespace game::scene
