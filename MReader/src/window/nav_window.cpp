#include "window/nav_window.h"

namespace MRWindow {
    void createNavWindow(ImVec2 navSize, ImVec2 navPos, int mangaTotal, std::vector<std::string> mangalistobj, int& selected) {

        static char buf[256];

        ImGui::SetNextWindowPos(navPos);
        ImGui::SetNextWindowSize(navSize);

        if (ImGui::Begin("Nav", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_AlwaysAutoResize)) {
            ImGui::InputTextWithHint("", "Search Manga Name :", buf, 256);
            ImGui::SameLine();
            ImGui::Button("Search");
            ImGui::Separator();

            // Create 2 columns
            ImGui::Columns(2, nullptr, false); // 2 columns, no border
            ImGui::SetColumnWidth(0, 150.0f);  // Set column width for labels

            // Centered "Choose Theme" Text
            float columnWidth = ImGui::GetColumnWidth();                 // Get current column width
            float textWidth = ImGui::CalcTextSize("Choose Theme").x;     // Get text width
            float textHeight = ImGui::GetTextLineHeight();               // Get text height
            float xOffset = (columnWidth - textWidth) * 0.5f;            // Calculate horizontal offset
            float yOffset = (ImGui::GetFrameHeight() - textHeight) * 0.5f; // Calculate vertical offset

            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + xOffset);      // Adjust X position
            ImGui::SetCursorPosY(ImGui::GetCursorPosY() + yOffset);      // Adjust Y position
            ImGui::Text("Choose Theme");
            ImGui::NextColumn();

            // Show Theme Selector
            MRTheme::showStyleSelector("##Theme Selector");
            ImGui::NextColumn();

            // Centered "Choose Font" Text
            columnWidth = ImGui::GetColumnWidth();
            textWidth = ImGui::CalcTextSize("Choose Font").x;
            xOffset = (columnWidth - textWidth) * 0.5f;

            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + xOffset);      // Adjust X position
            ImGui::SetCursorPosY(ImGui::GetCursorPosY() + yOffset);      // Adjust Y position
            ImGui::Text("Choose Font");
            ImGui::NextColumn();

            // Show Font Selector
            MRFont::showFontSelector("##Font Selector");
            ImGui::Columns(1); // End columns

            ImGui::Separator();
            for (int n = 0; n < mangaTotal; n++) {
                if (ImGui::Selectable(mangalistobj[n].c_str(), selected == n)) {
                    selected = n;
                }
            }
        }ImGui::End();
    }
}