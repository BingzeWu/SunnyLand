#pragma once
#include <string>

namespace engine::resource {
    class ResourceManager;
}

struct Mix_Chunk;
struct Mix_Music;

namespace engine::audio {

/**
 * @brief 用于控制音频播放的类。
 * 提供播放音效和音乐的方法，使用由 ResourceManager 管理的资源。
 */
class AudioPlayer final{
private:
    engine::resource::ResourceManager* resource_manager_;
    std::string current_music_;

public:
    explicit AudioPlayer(engine::resource::ResourceManager* resource_manager);
    ~AudioPlayer();

    // 删除拷贝和移动构造/赋值函数
    AudioPlayer(const AudioPlayer&) = delete;
    AudioPlayer& operator=(const AudioPlayer&) = delete;
    AudioPlayer(AudioPlayer&&) = delete;
    AudioPlayer& operator=(AudioPlayer&&) = delete;

    // --- 播放控制方法 --- 
    int playSound(const std::string& sound_path, int channel = -1);
    bool playMusic(const std::string& music_path, int loops = -1, int fade_in_ms = 0);
    void stopMusic(int fade_out_ms = 0);
    void pauseMusic();
    void resumeMusic();

    // --- 音量控制方法 ---
    void setSoundVolume(float volume, int channel = -1);
    void setMusicVolume(float volume);
    float getMusicVolume();
    float getSoundVolume(int channel = -1);
};

} // namespace engine::audio