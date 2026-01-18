#pragma once
// 前置声明
namespace engine::core {
    class Context;  // 前向声明
}
namespace engine::object {
    class GameObject;
}

namespace engine::component {

/**
 * @brief 组件的抽象基类。
 *
 * 所有具体组件都应从此类继承。
 * 定义了组件生命周期中可能调用的通用方法。
 */
class Component {
public:
    Component() = default;
    virtual ~Component() = default;         ///< @brief 虚析构函数确保正确清理派生类
    // 禁止拷贝和移动，组件通常不应被拷贝或移动（更改owner_就相当于移动）
    Component(const Component&) = delete;
    Component& operator=(const Component&) = delete;
    Component(Component&&) = delete;
    Component& operator=(Component&&) = delete;

    void setOwner(engine::object::GameObject* owner) { owner_ = owner; }
    [[nodiscard]] engine::object::GameObject* getOwner() const { return owner_; }        ///< @brief 获取拥有此组件的 GameObject

protected:
    friend class engine::object::GameObject;        // 它需要调用Component的init方法
    engine::object::GameObject* owner_ = nullptr;   ///< @brief 指向拥有此组件的 GameObject
    
    // 关键循环函数，全部设为保护，只有 GameObject 需要（可以）调用 (未来将其中一个改为 = 0 以实现抽象类)
    virtual void init() {}                      ///< @brief 保留两段初始化的机制，GameObject 添加组件时自动调用，不需要外部调用
    virtual void handleInput(engine::core::Context& context) {}               ///< @brief 处理输入
    /**
     * @brief 更新组件状态（纯虚函数，必须实现）
     * @param delta_time 距上一帧的时间间隔（秒）
     * @param context 引擎上下文对象
     */
    virtual void update(float delta_time, engine::core::Context& context) = 0;               ///< @brief 更新
    virtual void render(engine::core::Context& context) {}                    ///< @brief 渲染
    virtual void clean() {}                     ///< @brief 清理
};

} // namespace engine::component