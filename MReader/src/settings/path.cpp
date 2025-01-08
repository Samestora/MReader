#include "settings/path.h"

namespace MRPath{
    std::string getMangaPath() {
        std::ifstream configFile("config.json");
        if (configFile.is_open()) {
            nlohmann::json config;
            configFile >> config;
            return config["mangaPath"].get<std::string>();
        }
        return "./"; // Default base path
    }

    std::string getFontPath() {
        std::ifstream configFile("config.json");
        if (configFile.is_open()) {
            nlohmann::json config;
            configFile >> config;
            return config["fontPath"].get<std::string>();
        }
        return "./"; // Default base path
    }
}