#include "main_window.h"

void createMainWindow(ImVec2 mainSize, ImVec2 mainPos, std::vector<std::string> mangalistobj, std::vector<ID3D11ShaderResourceView*> textures, int image_x, int image_y, int& selected, bool* newlyRead, bool* favorite) {
    
    ImGui::SetNextWindowPos(mainPos);
    ImGui::SetNextWindowSize(mainSize);
    
    if (ImGui::Begin("Main", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_AlwaysAutoResize)) {
        if (selected >= 0) {
            ImGui::Text(mangalistobj[selected].c_str());

            for (size_t i = 0; i < textures.size(); ++i) {
                ImGui::Image((ImTextureID)(intptr_t)textures[i], ImVec2(image_x, image_y));
            }

            // newlyRead latch -> newRead: bool; defaulted to false
            if (!*newlyRead) {
                if (ImGui::Button("Start Reading")) {
                    *newlyRead = true;
                }
            }
            else {
                if (ImGui::Button("Continue Reading")) {
                    *newlyRead = false;
                }
            }

            // favorite latch -> favorite: bool; defaulted to false
            if (!*favorite) {
                if (ImGui::Button("Favorite")) {
                    *favorite = true;
                }
            }
            else {
                if (ImGui::Button("Unfavorite")) {
                    *favorite = false;
                }
            }

            // Remove Content not dirs (avoiding needs of privilege escalation)
            ImGui::Button("Delete");

            // Fetch JSON
            ImGui::Text("Tags : ");
            ImGui::Text("Artists : ");
            ImGui::Text("Languages: : ");
        }
}ImGui::End();
}