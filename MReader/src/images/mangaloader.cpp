#include "images/mangaloader.h"

void LoadAllPages(const std::string& directory, std::vector<ID3D11ShaderResourceView*>& out_srvs, int* out_width, int* out_height, ID3D11Device* g_pd3dDevice) {
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
            if (MRGraphics::LoadTextureFromFile(file_path.c_str(), &texture_srv, out_width, out_height, g_pd3dDevice))
                out_srvs.push_back(texture_srv);
        }
        out_srvs.shrink_to_fit();
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
                if (MRGraphics::LoadTextureFromFile(entry.path().string().c_str(), &texture_srv, out_width, out_height, g_pd3dDevice)) {
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

// The default would be 2 pages at the same time, kinda like a book
void LoadNormal(const std::string& directory, MRWindow::MainWindowState* state , std::vector<ID3D11ShaderResourceView*>& out_srvs, int* out_width, int* out_height, ID3D11Device* g_pd3dDevice) {
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

        // Update page number to point to the next set of pages
        // next
        if (state->next_page == true && state->current_page < files.size()) {
            for (unsigned int i = state->current_page; i < state->current_page + 2 && i < files.size(); ++i) {
                const auto& [num, file_path] = files[i];
                ID3D11ShaderResourceView* texture_srv = nullptr;
                if (MRGraphics::LoadTextureFromFile(file_path.c_str(), &texture_srv, out_width, out_height, g_pd3dDevice))
                    out_srvs.push_back(texture_srv);
            }
            
            state->current_page += 2;
        }
        // back
        else if (state->previous_page == true && state -> current_page > 2) {
            state->current_page -= 2;
            for (unsigned int i = state->current_page - 2; i < state->current_page; ++i) {
                const auto& [num, file_path] = files[i];
                ID3D11ShaderResourceView* texture_srv = nullptr;
                if (MRGraphics::LoadTextureFromFile(file_path.c_str(), &texture_srv, out_width, out_height, g_pd3dDevice))
                    out_srvs.push_back(texture_srv);
            }
        }
        // Init
        else if (state -> previous_page == false && state -> next_page == false) {
            for (unsigned int i = state->current_page; i < state->current_page + 2 && i < files.size(); ++i) {
                const auto& [num, file_path] = files[i];
                ID3D11ShaderResourceView* texture_srv = nullptr;
                if (MRGraphics::LoadTextureFromFile(file_path.c_str(), &texture_srv, out_width, out_height, g_pd3dDevice))
                    out_srvs.push_back(texture_srv);
            }
            state->current_page += 2;
        }
        else {
            for (unsigned int i = state->current_page - 2; i < state->current_page && i < files.size(); ++i) {
                const auto& [num, file_path] = files[i];
                ID3D11ShaderResourceView* texture_srv = nullptr;
                if (MRGraphics::LoadTextureFromFile(file_path.c_str(), &texture_srv, out_width, out_height, g_pd3dDevice))
                    out_srvs.push_back(texture_srv);
            }
        }
        state->previous_page = false;
        state->next_page = false;

        out_srvs.shrink_to_fit();
    }
    catch (const fs::filesystem_error& e) {
        std::cerr << "Filesystem error: " << e.what() << std::endl;
    }
}

std::vector<std::string> LoadMangaList(const std::string& directory) {
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
    manga_list.shrink_to_fit();
    return manga_list;
}