#include "util/mangaloader.h"

void LoadTexturesFromDirectory(const std::string& directory, std::vector<ID3D11ShaderResourceView*>& out_srvs, int* out_width, int* out_height, ID3D11Device* g_pd3dDevice) {
    std::vector<std::pair<int, std::string>> files;
    std::regex number_pattern(R"((\d+))");

    try {
        for (const auto& entry : fs::directory_iterator(directory)) {
            std::string file_name = entry.path().filename().string();
            std::smatch match;

            if (std::regex_search(file_name, match, number_pattern)) {
                int num = std::stoi(match.str(1));
                files.emplace_back(num, entry.path().string());
            }
        }

        std::sort(files.begin(), files.end(), [](const auto& a, const auto& b) { return a.first < b.first; });

        for (const auto& [num, file_path] : files) {
            ID3D11ShaderResourceView* texture_srv = nullptr;
            if (LoadTextureFromFile(file_path.c_str(), &texture_srv, out_width, out_height, g_pd3dDevice))
                out_srvs.push_back(texture_srv);
        }
    }
    catch (const fs::filesystem_error& e) {
        std::cerr << "Filesystem error: " << e.what() << std::endl;
    }
}

void LoadFirstMangaCover(const std::string& directory, std::vector<ID3D11ShaderResourceView*>& out_srvs, int* out_width, int* out_height, ID3D11Device* g_pd3dDevice) {
    std::regex number_pattern(R"((\d+))");

    try {
        for (const auto& entry : fs::directory_iterator(directory)) {
            std::string file_name = entry.path().filename().string();
            std::smatch match;

            if (std::regex_search(file_name, match, number_pattern)) {
                ID3D11ShaderResourceView* texture_srv = nullptr;
                if (LoadTextureFromFile(entry.path().string().c_str(), &texture_srv, out_width, out_height, g_pd3dDevice)) {
                    out_srvs.push_back(texture_srv);
                    return;
                }
            }
        }
        std::cerr << "No valid image found in directory: " << directory << std::endl;
    }
    catch (const fs::filesystem_error& e) {
        std::cerr << "Filesystem error: " << e.what() << std::endl;
    }
}

std::vector<std::string> GetMangaList(const std::string& directory) {
    std::vector<std::string> manga_list;
    try {
        // Check if the given path exists and is a directory
        if (fs::exists(directory) && fs::is_directory(directory)) {
            for (const auto& entry : fs::directory_iterator(directory)) {
                // Check if the entry is a directory
                if (fs::is_directory(entry.status())) {
                    manga_list.push_back(entry.path().filename().string());
                }
            }
        }
        else {
            std::cerr << "Path does not exist or is not a directory: " << directory << std::endl;
        }
    }
    catch (const fs::filesystem_error& e) {
        std::cerr << "Filesystem error: " << e.what() << std::endl;
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
    return manga_list;
}