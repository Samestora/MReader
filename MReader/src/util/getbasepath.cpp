#include "util/getbasepath.h"

namespace MRUtil{
    std::string getBasePath() {
        std::ifstream configFile("config.json");
        if (configFile.is_open()) {
            nlohmann::json config;
            configFile >> config;
            return config["basePath"].get<std::string>();
        }
        return "./"; // Default base path
    }
}