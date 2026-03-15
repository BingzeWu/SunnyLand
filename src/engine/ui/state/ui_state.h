#pragma once
#include <memory> // std::unique_ptr

namespace engine::core{
    class Context;
}

namespace engine::ui {
    class UIInteractive;
}

namespace engine::ui::state {
/**
 * @brief UI状态基类，定义了状态的基本接口和生命周期方法。
 *
 * 每个UI元素的交互状态都应该继承自这个类，并实现handleInput方法来处理输入事件。
 * 通过返回一个新的状态对象，可以实现状态之间的切换。
 */
class UIState {
    friend class engine::ui::UIInteractive; // 允许UIInteractive访问UIState的protected成员
protected:
    engine::ui::UIInteractive* owner_ = nullptr; ///< @brief 指向父节点

public:
    UIState(engine::ui::UIInteractive* owner) : owner_(owner) {}
    virtual ~UIState() = default;

    // 删除拷贝和移动构造函数/赋值运算符
    UIState(const UIState&) = delete;
    UIState& operator=(const UIState&) = delete;
    UIState(UIState&&) = delete;
    UIState& operator=(UIState&&) = delete;

protected:
    // 进入该状态时调用
    virtual void enter() {} 
    // 处理输入，并可能返回一个新的状态来替换自己
    virtual std::unique_ptr<UIState> handleInput(engine::core::Context& context) = 0;
};
} // namespace engine::ui::state