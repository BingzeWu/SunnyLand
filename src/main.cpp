#include <spdlog/spdlog.h>

#include "engine/core/game_app.h"

int main(int /* argc */, char* /* argv */[]) {
    spdlog::set_level(spdlog::level::trace);

    engine::core::GameApp app;
    app.run();
    return 0;
}