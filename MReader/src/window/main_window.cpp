#include "window/main_window.h"

namespace MRWindow {
    void createMainWindow(ImVec2 mainSize, ImVec2 mainPos, std::vector<std::string> mangalistobj, std::vector<ID3D11ShaderResourceView*> textures, int image_x, int image_y, MainWindowState* state) {

        ImGui::SetNextWindowPos(mainPos);
        ImGui::SetNextWindowSize(mainSize);

        if (ImGui::Begin("Main", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_AlwaysAutoResize)) {
            if (state -> selected >= 0 && state -> selected < mangalistobj.size()) {
                ImGui::Text(mangalistobj[state->selected].c_str());

                for (size_t i = 0; i < textures.size(); ++i) {
                    ImGui::Image((ImTextureID)(intptr_t)textures[i], ImVec2(image_x, image_y));
                    if (i == 0 && textures.size() > 1) {
                        ImGui::SameLine();
                    }
                }

                if (state -> reading == true) {
                    if (ImGui::Button("Previous Page")) {
                        state->previous_page = true;
                    }

                    ImGui::SameLine();

                    if (ImGui::Button("Next Page")) {
                        state->next_page = true;
                    }
                }

                // reading latch -> newRead: bool; defaulted to false
                if (state -> reading == false) {
                    if (ImGui::Button("Start Reading")) {
                        state -> reading = true;
                    }
                }
                else {
                    if (ImGui::Button("Stop Reading")) {
                        state -> reading = false;
                    }
                }

                // favorite latch -> favorite: bool; defaulted to false
                if (!state -> favorite) {
                    if (ImGui::Button("Favorite")) {
                        state -> favorite = true;
                    }
                }
                else {
                    if (ImGui::Button("Unfavorite")) {
                        state -> favorite = false;
                    }
                }

                // Fetch JSON
                ImGui::Text("Tags : ");
                ImGui::Text("Artists : ");
                ImGui::Text("Language : English");
            }
        }ImGui::End();
    }
}