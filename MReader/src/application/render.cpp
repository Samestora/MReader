#include "application/render.h"

namespace MRApplication {
    void Render(const std::string& mangaBasePath,const std::vector<std::string>& manga_list, MRImage::Image& image, MRWindow::MainWindowState& state, std::vector<ID3D11ShaderResourceView*>& textures, const ImVec4& clear_color, ID3D11DeviceContext* g_pd3dDeviceContext, IDXGISwapChain* g_pSwapChain, bool& g_SwapChainOccluded, ID3D11RenderTargetView* g_mainRenderTargetView, ID3D11Device* g_pd3dDevice) {
        // Makes the window sizing dynamic
        ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImVec2 navSize = ImVec2(viewport->Size.x / 3, viewport->Size.y);
        ImVec2 mainSize = ImVec2(viewport->Size.x / 3 * 2, viewport->Size.y);

        // Create navigation window
        if (state.reading == false) {
            MRWindow::createNavWindow(navSize, ImVec2(0, 0), manga_list.size(), manga_list, state.selected);
        }

        // changing manga
        if (state.selected != state.last_selected) {
            // Release current textures
            MRGraphics::ReleaseTextures(textures);
            // Set path
            std::string selectedMangaPath = mangaBasePath + manga_list[state.selected];
            
            // Load the first manga cover for the new selection
            LoadFirstMangaCover(selectedMangaPath, textures, &image.width, &image.height, g_pd3dDevice);

            // Update last selected index
            state.last_selected = state.selected;
            state.isLoaded = true;
            state.reading = false;
            state.current_page = 0;
        }
        // if selected
        else if (state.selected == state.last_selected && state.selected >= 0) {
            // Start reading
            if (state.reading && !state.isLoaded) {
                MRGraphics::ReleaseTextures(textures);
                std::string selectedMangaPath = mangaBasePath + manga_list[state.selected];
                LoadNormal(selectedMangaPath, &state, textures, &image.width, &image.height, g_pd3dDevice);
                state.isLoaded = true;
            }
            // Stop reading
            else if (!state.reading && state.isLoaded) {
                MRGraphics::ReleaseTextures(textures);
                std::string selectedMangaPath = mangaBasePath + manga_list[state.selected];
                LoadFirstMangaCover(selectedMangaPath, textures, &image.width, &image.height, g_pd3dDevice);
                state.isLoaded = false;
            }
            // Changing
            else if (state.next_page || state.previous_page) {
                MRGraphics::ReleaseTextures(textures);
                std::string selectedMangaPath = mangaBasePath + manga_list[state.selected];
                LoadNormal(selectedMangaPath, &state, textures, &image.width, &image.height, g_pd3dDevice);
                state.isLoaded = true;
            }
        }

        // Create main window
        // Not yet reading browse with the nav_window
        if (!state.reading) {
            MRWindow::createMainWindow(mainSize, ImVec2(navSize.x, 0), manga_list, textures, image.width, image.height, &state);
        }
        // Full screen view
        else {
            MRWindow::createMainWindow(viewport->Size, viewport->Pos, manga_list, textures, image.width, image.height, &state);
        }
    }
}