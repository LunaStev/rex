#define SDL_MAIN_HANDLED
#include "../Core/Engine.h"
#include "../Lua/LuaScene.h"

#include <filesystem>
#include <iostream>
#include <string>

namespace fs = std::filesystem;

static bool is_dir(const fs::path& p) {
    std::error_code ec;
    return fs::is_directory(p, ec);
}

int main(int argc, char** argv) {
    fs::path entry = "main.lua";
    fs::path workdir = fs::current_path();

    if (argc >= 2) {
        fs::path arg = argv[1];

        if (is_dir(arg)) {
            workdir = fs::absolute(arg);
            entry = "main.lua";
        } else {
            fs::path abs = fs::absolute(arg);
            workdir = abs.parent_path();
            entry = abs.filename();
        }
    }

    std::error_code ec;
    fs::current_path(workdir, ec);
    if (ec) {
        std::cerr << "Failed to set working dir: " << workdir << " (" << ec.message() << ")\n";
        return 1;
    }

    std::string title = "Rex";
    int w = 800, h = 600;

    Engine::run<LuaScene>(title.c_str(), w, h, entry.string());
    return 0;
}