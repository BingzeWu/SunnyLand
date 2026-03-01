#pragma once
#include "../utils/math.h"
#include <vector>
#include <utility>  // for std::pair
#include "glm/vec2.hpp"
#include <optional>

namespace engine::component {
    class PhysicsComponent;
    class TileLayerComponent;
    enum class TileType;
}

namespace engine::object {
    class GameObject;
}


namespace engine::physics {

/**
 * @brief 负责管理和模拟物理行为及碰撞检测。
 */
class PhysicsEngine {
private:
    std::vector<engine::component::PhysicsComponent*> components_; ///< @brief 注册的物理组件容器，非拥有指针
    std::vector<engine::component::TileLayerComponent*> collision_tile_layers_; ///< @brief 注册的用于碰撞检测的瓦片图层组件，非拥有指针
    glm::vec2 gravity_ = {0.0f, 980.0f};        ///< @brief 默认重力值 (像素/秒^2, 相当于100像素对应现实1m)
    float max_speed_ = 500.0f;                  ///< @brief 最大速度 (像素/秒)
    std::optional<engine::utils::Rect> world_bounds_;     ///< @brief 世界边界，用于限制物体移动范围
    std::vector<std::pair<engine::object::GameObject*, engine::object::GameObject*>> collision_pairs_;
    /// @brief 检测并处理游戏对象和瓦片层之间的碰撞。
    void resolveTileCollisions(engine::component::PhysicsComponent* pc, float delta_time); 
    /// @brief 处理可移动物体与SOLID物体的碰撞。
    void resolveSolidObjectCollisions(engine::object::GameObject* move_obj, engine::object::GameObject* solid_obj);
    void checkObjectCollisions(); ///<@brief 检测并处理对象之间的碰撞，并记录需要游戏逻辑处理的碰撞对。
    void applyWorldBounds(engine::component::PhysicsComponent* pc);     ///< @brief 应用世界边界，限制物体移动范围


public:
    PhysicsEngine() = default;

    // 禁止拷贝和移动
    PhysicsEngine(const PhysicsEngine&) = delete;
    PhysicsEngine& operator=(const PhysicsEngine&) = delete;
    PhysicsEngine(PhysicsEngine&&) = delete;
    PhysicsEngine& operator=(PhysicsEngine&&) = delete;

    void registerComponent(engine::component::PhysicsComponent* component);     ///< @brief 注册物理组件
    void unregisterComponent(engine::component::PhysicsComponent* component);   ///< @brief 注销物理组件
    void registerCollisionLayer(engine::component::TileLayerComponent* layer);   ///< @brief 注册用于碰撞检测的瓦片图层组件
    void unregisterCollisionLayer(engine::component::TileLayerComponent* layer);   ///< @brief 注销用于碰撞检测的瓦片图层组件


    void update(float delta_time);      ///< @brief 核心循环：更新所有注册的物理组件的状态

    // setters/getters
    void setGravity(const glm::vec2& gravity) { gravity_ = gravity; }   ///< @brief 设置全局重力加速度
    const glm::vec2& getGravity() const { return gravity_; }            ///< @brief 获取当前的全局重力加速度
    void setMaxSpeed(float max_speed) { max_speed_ = max_speed; }       ///< @brief 设置最大速度
    float getMaxSpeed() const { return max_speed_; }                    ///< @brief 获取当前的最大速度
    const auto& getCollisionPairs() const { return collision_pairs_; }; ///< @brief 获取当前的碰撞对容器
    void setWorldBounds(const engine::utils::Rect& world_bounds) { world_bounds_ = world_bounds; }   ///< @brief 设置世界边界
    const std::optional<engine::utils::Rect>& getWorldBounds() const { return world_bounds_; }       ///< @brief 获取世界边界                                        ///< @brief 对所有注册的物理组件应用世界边界
    //斜坡问题核心算法
    float getTileHeightAtWidth(float width, engine::component::TileType type, glm::vec2 tile_size); ///< @brief 根据瓦片类型获取指定位置处的高度（用于斜坡碰撞处理）
};

} // namespace engine::physics