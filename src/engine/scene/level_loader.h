#pragma once
#include <string>
#include <glm/vec2.hpp>
#include <nlohmann/json.hpp>
#include <map>


namespace engine::component { struct TileInfo; }

namespace engine::scene {
class Scene;

/**
 * @brief 负责加载 Tiled JSON 格式的地图文件到游戏场景中。
 */
class LevelLoader final {
    std::string map_path_;      ///< @brief 地图路径（拼接路径时需要）
    glm::ivec2 tile_size_;      ///< @brief 单个瓦片尺寸（像素）
    glm::ivec2 map_size_;       ///< @brief 地图尺寸（瓦片数）
    std::map<int,nlohmann::json> tileset_data_; ///< @brief 所有瓦片集的信息（包括类型、属性等）
public:
    LevelLoader() = default;

    /**
     * @brief 加载关卡数据到指定的 Scene 对象中。
     * @param map_path Tiled JSON 地图文件的路径。
     * @param scene 要加载数据的目标 Scene 对象。
     * @return bool 是否加载成功。
     */
    bool loadLevel(const std::string& map_path, Scene& scene);

private:
    void loadImageLayer(const nlohmann::json& layer_json, Scene& scene);    ///< @brief 加载图片图层
    
    void loadTileLayer(const nlohmann::json& layer_json, Scene& scene);     ///< @brief 加载瓦片图层
    engine::component::TileInfo getTileInfoByGid(int gid); ///< @brief 根据 GID 获取对应的 TileInfo
    void loadTileset(const std::string& tileset_path, int first_gid); ///< @brief 加载单个瓦片集
    
    void loadObjectLayer(const nlohmann::json& layer_json, Scene& scene);   ///< @brief 加载对象图层
    

    /**
     * @brief 解析图片路径，合并地图路径和相对路径。例如：
     * 1. 地图路径："assets/maps/level1.tmj"
     * 2. 相对路径："../textures/Layers/back.png"
     * 3. 最终路径："assets/textures/Layers/back.png"
     * @param image_path （图片）相对路径
     * @return std::string 解析后的完整路径。
     */
    std::string resolvePath(const std::string& relative_path, const std::string& file_path);
};

} // namespace engine::scene