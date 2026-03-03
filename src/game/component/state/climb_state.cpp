#include "climb_state.h"
#include "idle_state.h"
#include "fall_state.h"
#include "jump_state.h"
#include "../player_component.h"
#include "../../../engine/core/context.h"
#include "../../../engine/input/input_manager.h"
#include "../../../engine/component/physics_component.h"
#include "../../../engine/component/sprite_component.h"
#include "../../../engine/component/animation_component.h"
#include <glm/common.hpp>

namespace game::component::state {
void ClimbState::enter() {
    playAnimation("climb");
    if (auto* physics = player_component_->getPhysicsComponent(); physics) {
        physics->setUseGravity(false); // 关键：禁用重力
    }
}

void ClimbState::exit() {
    if (auto* physics = player_component_->getPhysicsComponent(); physics) {
        physics->setUseGravity(true); // 关键：重新启用重力
    }
}

std::unique_ptr<PlayerState> ClimbState::handleInput(engine::core::Context& context) {
    auto input = context.getInputManager();
    auto physics = player_component_->getPhysicsComponent();
    auto anim = player_component_->getAnimationComponent();
    auto speed = player_component_->getClimbSpeed();
    auto speedx = speed * 0.5f; // 水平爬梯子速度较慢，防止过快导致难以控制

    // 直接根据输入设置速度
    physics->velocity_.y = input.isActionDown("move_up") ? -speed :
                           input.isActionDown("move_down") ? speed : 0.0f;
    physics->velocity_.x = input.isActionDown("move_left") ? -speedx :
                           input.isActionDown("move_right") ? speedx : 0.0f;

    // 如果有移动输入，则播放动画，否则暂停
    (physics->velocity_.y != 0.0f || physics->velocity_.x != 0.0f)
        ? anim->resumeAnimation()
        : anim->stopAnimation();
    
    // 按跳跃键可以主动离开梯子
    if (input.isActionPressed("jump")) {
        return std::make_unique<JumpState>(player_component_);
    }
    return nullptr;
}

std::unique_ptr<PlayerState> ClimbState::update(float, engine::core::Context&) {
    auto physics = player_component_->getPhysicsComponent();
    // 如果接触到地面，切换到站立状态
    if (physics->hasCollidedBelow()) {
        return std::make_unique<IdleState>(player_component_);
    }
    // 如果不再接触梯子（比如从侧面爬出），切换到下落状态
    if (!physics->hasCollidedLadder()) {
        return std::make_unique<FallState>(player_component_);
    }
    return nullptr;
}

} // namespace game::component::state
