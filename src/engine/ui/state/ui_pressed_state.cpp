#include "ui_pressed_state.h"
#include "ui_normal_state.h"
#include "ui_hover_state.h"
#include "../ui_interactive.h"
#include "../../input/input_manager.h"
#include "../../core/context.h"
#include <spdlog/spdlog.h>

namespace engine::ui::state {

void UIPressedState::enter()
{
    owner_->setSprite("pressed");
    owner_->playSound("click");
    spdlog::debug("切换到按下状态");
}

std::unique_ptr<UIState> UIPressedState::handleInput(engine::core::Context& context)
{
    auto& input_manager = context.getInputManager();
    auto mouse_pos = input_manager.getLogicalMousePosition();

    if (input_manager.isActionReleased("MouseLeftClick")) { // 如果鼠标释放
        if (owner_->isPointInside(mouse_pos)) {             // 鼠标仍在元素内，触发点击事件
            owner_->clicked();
            return std::make_unique<UIHoverState>(owner_);
        } else {                                            // 鼠标已移出元素外
            return std::make_unique<UINormalState>(owner_);
        }
    }
    return nullptr;
}

} // namespace engine::ui::state
