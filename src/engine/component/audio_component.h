#pragma once
#include "component.h"
#include <string>
#include <unordered_map>

namespace engine::audio {
    class AudioPlayer;
}

namespace engine::render {
    class Camera;
}

namespace engine::component {
    class TransformComponent;
/**
 * @brief 音频组件，用于处理音频播放和管理。
 */
class AudioComponent final: public Component {
    friend class engine::object::GameObject;
private:
    engine::audio::AudioPlayer* audio_player_;
    engine::render::Camera* camera_;
    engine::component::TransformComponent* transform_ = nullptr;

    // 音效id 到 路径的映射表
    std::unordered_map<std::string, std::string> sound_id_to_path_; 

public:
    AudioComponent(engine::audio::AudioPlayer* audio_player, engine::render::Camera* camera);
    
    void playSound(const std::string& sound_id, int channel = -1, bool use_spatial = false);
    void addSound(const std::string& sound_id, const std::string& sound_path);

private:
    void init() override;
    void update(float, engine::core::Context&) override {}
};
} // namespace engine::component