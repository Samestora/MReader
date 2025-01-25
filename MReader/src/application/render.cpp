#include "application/render.h"

namespace MRApplication {
    void Render(const std::string& mangaBasePath,const std::vector<std::string>& manga_list, std::vector<MRImage::Image>& image, MRWindow::MainWindowState& state, std::vector<ID3D11ShaderResourceView*>& textures, const ImVec4& clear_color) {
        
        //ImGui::ShowDemoWindow();
        
        // changing manga
        if (state.current_mode == MRGraphics::LOAD_NONE) {
            // Load the first manga cover for the new selection
            if (state.selected > -1) {
                MRGraphics::ReleaseTextures(textures);
                std::string selectedMangaPath = mangaBasePath + manga_list[state.selected];
                MRImage::LoadFirstMangaCover(selectedMangaPath, &state, textures, image, MRGraphics::gadget.g_pd3dDevice);
                state.current_mode = MRGraphics::LOAD_COVER;
                state.last_selected = state.selected;
                state.reading = false;
                state.current_page = 0;
            }
            // if back from loading cover
            else if (state.selected == -1) {
                MRGraphics::ReleaseTextures(textures);
                state.last_selected = state.selected;
                state.reading = false;
                state.current_page = 0;
            }
        }
        // if selected
        else if (state.current_mode == MRGraphics::LOAD_COVER) {
            // Start reading
            if (state.reading) {
                MRGraphics::ReleaseTextures(textures);
                std::string selectedMangaPath = mangaBasePath + manga_list[state.selected];
                MRImage::LoadNormal(selectedMangaPath, &state, textures, image, MRGraphics::gadget.g_pd3dDevice);
                state.current_mode = MRGraphics::LOAD_NORMAL;
            }
            // changing cover while not reading
            else if (state.last_selected != state.selected) {
                MRGraphics::ReleaseTextures(textures);
                std::string selectedMangaPath = mangaBasePath + manga_list[state.selected];
                MRImage::LoadFirstMangaCover(selectedMangaPath, &state, textures, image, MRGraphics::gadget.g_pd3dDevice);
                state.last_selected = state.selected;
            }
        }
        // while reading
        else if(state.current_mode == MRGraphics::LOAD_NORMAL){
            // immediatly change manga
            if (state.next_page || state.previous_page) {
                MRGraphics::ReleaseTextures(textures);
                std::string selectedMangaPath = mangaBasePath + manga_list[state.selected];
                LoadNormal(selectedMangaPath, &state, textures, image, MRGraphics::gadget.g_pd3dDevice);
            }
            // Stop reading or just pressing esc to exit from reading
            else if (!state.reading || state.last_selected != state.selected) {
                MRGraphics::ReleaseTextures(textures);
                std::string selectedMangaPath = mangaBasePath + manga_list[state.selected];
                MRImage::LoadFirstMangaCover(selectedMangaPath, &state, textures, image, MRGraphics::gadget.g_pd3dDevice);
                state.reading = false;
                state.current_page = 0;
                state.last_selected = state.selected;
                state.current_mode = MRGraphics::LOAD_COVER;
            }
        }

        MRWindow::CreateMainWindow(manga_list, textures, image, &state);
    }
}