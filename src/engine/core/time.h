#pragma once
#include <SDL3/SDL_stdinc.h>    // 用于 Uint64

namespace engine::core {
/**
 * @brief 管理游戏循环中的时间，计算帧间时间差 (DeltaTime)。
 *
 * 使用 SDL 的高精度性能计数器来确保时间测量的准确性。
 * 提供获取缩放和未缩放 DeltaTime 的方法，以及设置时间缩放因子的能力。
 */
class Time final{
private:
    Uint64 last_time_ = 0;         ///< @brief 上一帧的时间戳 (用于计算 delta)
    Uint64 frame_start_time_ = 0;  ///< @brief 当前帧开始的时间戳 (用于帧率限制)
    double delta_time_ = 0.0;      ///< @brief 未缩放的帧间时间差 (秒)
    double time_scale_ = 1.0;      ///< @brief 时间缩放因子

    // 帧率限制相关
    int target_fps_ = 0;             ///< @brief 目标 FPS (0 表示不限制)
    double target_frame_time_ = 0.0; ///< @brief 目标每帧时间 (秒)

public:
    /**
     * @brief 构造函数，初始化时间相关变量。
     */
    Time();
    // 删除拷贝、移动和赋值操作符
    Time(const Time&) = delete;
    Time(Time&&) = delete;
    Time& operator=(const Time&) = delete;
    Time& operator=(Time&&) = delete;

     /**
     * @brief 更新时间相关变量，计算当前帧的时间差。
     */
    void update();
     /**
     * @brief 获取当前帧的时间差（秒）。
     * @return float 当前帧的时间差（秒）。
     */
    float getDeltaTime() const;
     /**
     * @brief 获取未经过时间缩放的原始帧间时间差。
     * @return float 未经过时间缩放的原始帧间时间差（秒）。
     */
    float getUnscaledDelta_time() const;
     /**
     * @brief 设置时间缩放因子。
     * @param scale 时间缩放因子，1.0 表示正常速度，0.5 表示一半速度，2.0 表示两倍速度。
     */
    void setTimeScale(float scale);
     /**
     * @brief 获取当前的时间缩放因子。
     * @return float 当前的时间缩放因子。
     */
    float getTimeScale() const;
     /**
     * @brief 设置目标帧率。
     * @param fps 目标帧率，0 表示不限制。
     */
    void setTargetFps(int fps);
     /**
     * @brief 获取当前的目标帧率。
     * @return int 当前的目标帧率。
     */
    int getTargetFps() const;

private:
    /**
     * @brief update 中调用，用于限制帧率。如果设置了 target_fps_ > 0，且当前帧执行时间小于目标帧时间，则会调用 SDL_DelayNS() 来等待剩余时间。
     * 
     * @param current_delta_time 当前帧的执行时间（秒）
     */
    void limitFrameRate(float current_delta_time);

};
}