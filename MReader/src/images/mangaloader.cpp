    #include "images/mangaloader.h"

namespace MRImage {
    void LoadAllPages(const std::string& directory, std::vector<ID3D11ShaderResourceView*>& out_srvs, std::vector<MRImage::Image>& image, ID3D11Device* g_pd3dDevice) {
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
                int out_width = 0, out_height = 0;
                if (MRGraphics::LoadTextureFromFile(file_path.c_str(), &texture_srv, &out_width, &out_height, g_pd3dDevice, MRGraphics::LOAD_NORMAL)) {
                    out_srvs.push_back(texture_srv);
                    image[num] = MRImage::Image{ out_width, out_height };
                }
            }
            out_srvs.shrink_to_fit();
        }
        catch (const fs::filesystem_error& e) {
            std::cerr << "Filesystem error: " << e.what() << std::endl;
        }
    }

    void LoadFirstMangaCover(const std::string& directory, MRWindow::MainWindowState* state, std::vector<ID3D11ShaderResourceView*>& out_srvs, std::vector<MRImage::Image>& image, ID3D11Device* g_pd3dDevice) {
        std::regex number_pattern(R"((\d+))");
        image.clear();
        try {
            std::vector<std::pair<int, std::string>> files;
            for (const auto& entry : fs::directory_iterator(directory)) {
                std::string file_name = entry.path().filename().string();
                std::smatch match;
                if (std::regex_search(file_name, match, number_pattern)) {
                    int num = std::stoi(match.str(1));
                    files.emplace_back(num, entry.path().string());
                }
            }
            state->total_page = files.size();
            for (const auto& entry : fs::directory_iterator(directory)) {
                std::string file_name = entry.path().filename().string();
                std::smatch match;

                if (std::regex_search(file_name, match, number_pattern)) {
                    ID3D11ShaderResourceView* texture_srv = nullptr;
                    int out_width = 0, out_height = 0;
                    if (MRGraphics::LoadTextureFromFile(entry.path().string().c_str(), &texture_srv, &out_width, &out_height, g_pd3dDevice, MRGraphics::LOAD_COVER)) {
                        out_srvs.push_back(texture_srv);
                        image.push_back(MRImage::Image{ out_width, out_height });
                        return;
                    }
                    else {
                        out_srvs.push_back(nullptr);
                        image.push_back(MRImage::Image{ 0,0 });
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
    void LoadNormal(const std::string& directory, MRWindow::MainWindowState* state, std::vector<ID3D11ShaderResourceView*>& out_srvs, std::vector<MRImage::Image>& image, ID3D11Device* g_pd3dDevice) {
        std::vector<std::pair<int, std::string>> files;
        std::regex number_pattern(R"((\d+))");
        image.clear();
        try {
            // Collect all files in the directory
            for (const auto& entry : fs::directory_iterator(directory)) {
                std::string file_name = entry.path().filename().string();
                std::smatch match;
                if (std::regex_search(file_name, match, number_pattern)) {
                    int num = std::stoi(match.str(1));
                    files.emplace_back(num, entry.path().string());
                }
            }
            state->total_page = files.size();

            // Sort the files based on the number extracted from the file name
            std::sort(files.begin(), files.end(), [](const auto& a, const auto& b) { return a.first < b.first; });

            // Load textures based on page navigation
            if (state->next_page && (unsigned int) state->current_page < files.size()) {
                for (unsigned int i = state->current_page; i < (unsigned int) state->current_page + 2 && i < files.size(); ++i) {
                    const auto& [num, file_path] = files[i];
                    ID3D11ShaderResourceView* texture_srv = nullptr;
                    int width = 0, height = 0;
                    if (MRGraphics::LoadTextureFromFile(file_path.c_str(), &texture_srv, &width, &height, g_pd3dDevice, MRGraphics::LOAD_NORMAL)) {
                        out_srvs.push_back(texture_srv);
                        image.push_back(MRImage::Image{ width, height });
                    }
                    else {
                        out_srvs.push_back(texture_srv);
                        image.push_back(MRImage::Image{ 0,0 });
                        return;
                    }
                }
                state->current_page += 2;
            }
            else if (state->previous_page && (unsigned int) state->current_page > 2) {
                state->current_page -= 2;

                for (unsigned int i = (unsigned int) state->current_page - 2; i < (unsigned int) state->current_page; ++i) {
                    const auto& [num, file_path] = files[i];

                    ID3D11ShaderResourceView* texture_srv = nullptr;
                    int width = 0, height = 0;
                    if (MRGraphics::LoadTextureFromFile(file_path.c_str(), &texture_srv, &width, &height, g_pd3dDevice, MRGraphics::LOAD_NORMAL)) {
                        out_srvs.push_back(texture_srv);
                        image.push_back(MRImage::Image{ width, height });
                    }
                }
            }
            else if (!state->previous_page && !state->next_page) {
                for (unsigned int i = (unsigned int)state->current_page; i < (unsigned int)state->current_page + 2 && i < files.size(); ++i) {
                    const auto& [num, file_path] = files[i];
                    int width = 0, height = 0;

                    ID3D11ShaderResourceView* texture_srv = nullptr;
                    if (MRGraphics::LoadTextureFromFile(file_path.c_str(), &texture_srv, &width, &height, g_pd3dDevice, MRGraphics::LOAD_NORMAL)) {
                        out_srvs.push_back(texture_srv);
                        image.push_back(MRImage::Image{ width, height });
                    }
                }
                state->current_page += 2;
            }
            else {
                for (unsigned int i = (unsigned int) state->current_page - 2; i < (unsigned int) state->current_page && i < files.size(); ++i) {
                    const auto& [num, file_path] = files[i];
                    int width = 0, height = 0;
                    ID3D11ShaderResourceView* texture_srv = nullptr;

                    if (MRGraphics::LoadTextureFromFile(file_path.c_str(), &texture_srv, &width, &height, g_pd3dDevice, MRGraphics::LOAD_NORMAL)) {
                        out_srvs.push_back(texture_srv);
                        image.push_back(MRImage::Image{ width, height });
                    }
                }
            }

            state->previous_page = false;
            state->next_page = false;

            image.shrink_to_fit();
            out_srvs.shrink_to_fit();  // Free unused capacity in the vector
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
                        try {
                            manga_list.push_back(entry.path().filename().string());
                        }
                        // ignore non UTF-8 (i gave up on making this shi unicode firendly fr fr)
                        catch (const std::exception& e) {
                            std::cerr << e.what() << std::endl;
                            continue;
                        }
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
}