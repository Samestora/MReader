#include "nav_window.h"

void createNavWindow(ImVec2 navSize, ImVec2 navPos, int mangaTotal, std::vector<std::string> mangalistobj, int& selected) {

    ImGui::SetNextWindowPos(navPos);
    ImGui::SetNextWindowSize(navSize);

    if (ImGui::Begin("Nav", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_AlwaysAutoResize)) {
        for (int n = 0; n < mangaTotal; n++){
            if (ImGui::Selectable(mangalistobj[n].c_str(), selected == n)) {
                selected = n;
            }
        }
    }ImGui::End();
}