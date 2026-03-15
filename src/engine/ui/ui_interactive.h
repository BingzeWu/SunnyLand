#pragma once
#include "ui_element.h"
#include "../render/sprite.h"
#include "state/ui_state.h"
#include <memory> // 智能指针
#include <string> // std::string
#include <unordered_map> // std::unordered_map\

namespace engine::ui {

class UIInteractive : public UIElement {
protected:
    engine::core::Context& context_; ///< @brief 引用游戏上下文，提供访问资源管理器、输入管理器等功能
    std::unique_ptr<engine::ui::state::UIState> state_; ///< @brief 当前交互状态，用于处理不同状态下的交互逻辑    
    std::unordered_map<std::string, std::unique_ptr<engine::render::Sprite>> sprites_; ///< @brief 存储不同状态下的精灵图，方便切换显示
    std::unordered_map<std::string, std::string> sounds_; ///< @brief 存储不同状态下的音效，方便切换播放
    engine::render::Sprite* current_sprite_ = nullptr; ///< @brief 当前显示的精灵图
    bool interactive_ = true; ///< @brief 是否可交互

public:
    UIInteractive(engine::core::Context& context, 
                   const glm::vec2& position,
                   const glm::vec2& size = {0.0f, 0.0f});
    ~UIInteractive() override;

    virtual void clicked() {}       ///< @brief 如果有点击事件，则重写该方法

    void addSprite(const std::string& name, std::unique_ptr<engine::render::Sprite> sprite);///< @brief 添加精灵
    void setSprite(const std::string& name);                                                ///< @brief 设置当前显示的精灵
    void addSound(const std::string& name, const std::string& path);                        ///< @brief 添加音效
    void playSound(const std::string& name);                                                ///< @brief 播放音效
    // --- Getters and Setters ---
    void setState(std::unique_ptr<engine::ui::state::UIState> state);       ///< @brief 设置当前状态
    engine::ui::state::UIState* getState() const { return state_.get(); }   ///< @brief 获取当前状态

    void setInteractive(bool interactive) { interactive_ = interactive; }   ///< @brief 设置是否可交互
    bool isInteractive() const { return interactive_; }                     ///< @brief 获取是否可交互

    // --- 核心方法 ---
    bool handleInput(engine::core::Context& context) override;
    void render(engine::core::Context& context) override;

};
}// namespace engine::ui
