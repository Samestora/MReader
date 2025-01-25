#define _CRT_SECURE_NO_WARNINGS
#include <rapidfuzz/fuzz.hpp>
#include <optional>
#include <utility>
#include "window/window.h"

namespace MRWindow {
    template <typename Sentence1, typename Iterable, typename Sentence2 = typename Iterable::value_type> std::vector<std::pair<Sentence2, double>> extract(const Sentence1& query, const Iterable& choices, const double score_cutoff = 0.0) {
        std::vector<std::pair<Sentence2, double>> results;

        rapidfuzz::fuzz::CachedRatio<typename Sentence1::value_type> scorer(query);

        for (const auto& choice : choices) {
            double score = rapidfuzz::fuzz::partial_token_set_ratio(choice, query);

            if (score >= score_cutoff) {
                results.emplace_back(choice, score);
            }
        }

        return results;
    }

    void CreateMainWindow(std::vector<std::string> mangalistobj, std::vector<ID3D11ShaderResourceView*> textures, std::vector<MRImage::Image> image, MainWindowState* state) {
        ImGui::SetNextWindowSize(ImGui::GetMainViewport()->Size);
        ImGui::SetNextWindowPos(ImVec2(0, 0));
        
        static char buf[256];
        static std::vector<std::pair<std::string, double>> results;

        if (ImGui::Begin("####MReader", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_AlwaysAutoResize)){
            // Keybinds
            {
                // Global
                {
                    if (ImGui::Shortcut(ImGuiKey_Escape)) {
                        if (state->reading) {
                            state->reading = false;
                        }
                        else {
                            state->current_mode = MRGraphics::LOAD_NONE;
                            state->selected = -1;
                        }
                    }
                }

                // Start reading
                {
                if (ImGui::Shortcut(ImGuiKey_R) && !state->reading) {
                    state->reading = true;
                    state->current_page = 0;
                }
                // Next
                else if ((ImGui::Shortcut(ImGuiKey_LeftArrow) || ImGui::Shortcut(ImGuiKey_MouseX1)) && state->reading) {
                    state->previous_page = true;
                }
                // previous
                else if ((ImGui::Shortcut(ImGuiKey_RightArrow) || ImGui::Shortcut(ImGuiKey_MouseX2)) && state->reading) {
                    state->next_page = true;
                }
                }
            }

            // Left or Nav
            /*if (state->current_mode != MRGraphics::LOAD_NORMAL && user.readmode = MRPref::FULLSCREEN)*/
            {
                ImGui::BeginChild("manga list", ImVec2(ImGui::GetWindowSize().x/5, ImGui::GetWindowSize().y - ImGui::GetStyle().WindowPadding.y * 2), ImGuiChildFlags_Border);
                for (size_t i = 0; i < mangalistobj.size(); i++) {
                    char label[128];

                    if (results.size() > 0) {
                        for (const auto& result : results) {
                            if (result.first == mangalistobj[i].c_str()) {
                                sprintf(label, mangalistobj[i].c_str());
                                if (ImGui::Selectable(label, state->selected == i)) {
                                    state->selected = i;
                                }
                                break;
                            }
                        }
                    }
                    else {
                        snprintf(label, sizeof(label), "%s", mangalistobj[i].c_str());
                        if (ImGui::Selectable(label, state->selected == i)) {
                            state->selected = i;
                        }
                    }
                }
                ImGui::EndChild();
            }
            
            ImGui::SameLine();

            // Right
            {
                // Tutorial window
                if (state->current_mode == MRGraphics::LOAD_NONE){
                    ImGui::BeginGroup();
                    ImGui::BeginChild("tutorial view", ImVec2(ImGui::GetWindowSize().x / 5 * 4 - ImGui::GetStyle().WindowPadding.x * 4, ImGui::GetWindowSize().y - ImGui::GetStyle().WindowPadding.y * 2), ImGuiChildFlags_Border);
                    
                    ImGui::Text("Welcome To MReader!");
                    ImGui::Text("Press ESC to go back, R to start reading and Left/Right Arrow to navigate!");
                    if(ImGui::InputTextWithHint("","Search manga name here :", buf, 256)) {
                        results = extract(std::string(buf), mangalistobj, 60.0);
                    }

                    ImGui::Separator();

                    ImGui::Text("Here you can customize the looks and feel of the app!");
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
                    
                    ImGui::EndChild();
                    ImGui::EndGroup();
                }
                
                // Already selected a manga to load cover
                else if (state->current_mode == MRGraphics::LOAD_COVER) {
                    ImGui::BeginChild("manga view", ImVec2(ImGui::GetWindowSize().x / 5 * 4 - ImGui::GetStyle().WindowPadding.x * 4, ImGui::GetWindowSize().y - ImGui::GetStyle().WindowPadding.y * 2), ImGuiChildFlags_Border);
                    if (ImGui::BeginTabBar("##Tabs", ImGuiTabBarFlags_None))
                    {
                        if (ImGui::BeginTabItem("Overview"))
                        {
                            std::string author = mangalistobj[state->selected];
                            float centered = ImGui::GetContentRegionAvail().x/2 - ImGui::CalcTextSize(author.c_str()).x/2;

                            ImGui::SetCursorPosX(centered);
                            ImGui::Text(author.c_str());

                            centered = ImGui::GetContentRegionAvail().x / 2;
                            
                            ImTextureID tex = (ImTextureID)(intptr_t)textures[0];
                            std::string id = "#image_" + std::to_string(0);
                            ImGui::PushID(0);
                            // removing the padding
                            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
                                
                            ImGui::SetCursorPosX(centered - (float) image[0].width/2);
                            if (ImGui::ImageButton(id.c_str(), tex, ImVec2((float)image[0].width, (float)image[0].height))) {
                                state->reading = true;
                                state->current_page = 0;
                            }

                            ImGui::PopStyleVar();
                            ImGui::PopID();

                            float start_button = 644.0f;
                            
                            ImGui::SetCursorPosY(start_button);
                            if (ImGui::Button("Start Reading")) {
                                state->reading = true;
                                state->current_page = 0;
                            }
                            ImGui::TextWrapped("Manga description goes here");
                            std::string page_count = "Page : " + std::to_string(state->total_page);
                            ImGui::Text(page_count.c_str());
                            ImGui::Text("Author : ");
                            ImGui::Text("Tags : ");
                            ImGui::Text("Languages : ");
                            ImGui::EndTabItem();
                        }
                        if (ImGui::BeginTabItem("Chapters"))
                        {
                            // WIP
                            for (int i = 1; i < 11; i++) {
                                std::string label = "Chapter " + std::to_string(i);
                                if ( ImGui::Selectable(label.c_str()) ){
                                    continue;
                                }
                            }
                            ImGui::EndTabItem();
                        }
                    }
                    ImGui::EndTabBar();
                    ImGui::EndChild();
                }

                // is reading
                else if (state->current_mode == MRGraphics::LOAD_NORMAL) {
                    ImGui::BeginGroup();
                    ImGui::BeginChild("manga view", ImVec2(ImGui::GetWindowSize().x / 5 * 4 - ImGui::GetStyle().WindowPadding.x * 4, ImGui::GetWindowSize().y - ImGui::GetStyle().WindowPadding.y * 2), ImGuiChildFlags_Border);

                    if (textures.size() == 2) {
                        ImGui::SetCursorPosX(ImGui::GetContentRegionAvail().x / 2 - (float)(image[0].width + image[1].width) / 2);
                    }   
                    else {
                        ImGui::SetCursorPosX(ImGui::GetContentRegionAvail().x / 2 - (float)(image[0].width) / 2);
                    }
                    // Making it full screnn no BS just read
                    for (size_t i = 0; i < textures.size(); ++i) {
                        ImTextureID tex = (ImTextureID)(intptr_t)textures[i];
                        std::string id = "#image_" + std::to_string(i);
                        // removing the padding
                        ImGui::PushID(i);
                        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
                        if (ImGui::ImageButton(id.c_str(), tex, ImVec2((float)image[i].width, (float)image[i].height))) {
                            if ((state->reading && i == 0)) {
                                state->previous_page = true;
                            }
                            else if ((state->reading && i == 1)) {
                                state->next_page = true;
                            }
                        }
                        if (textures.size() > 1 && (i % 2 == 0)) {
                            ImGui::SameLine();
                        }
                        ImGui::PopStyleVar();
                        ImGui::PopID();
                    }

                    std::string page_counter = std::to_string(state->current_page) + "/" + std::to_string(state->total_page);
                    ImGui::SetCursorPosX( (ImGui::GetContentRegionAvail().x / 2) - ImGui::CalcTextSize(page_counter.c_str()).x/2 + ImGui::GetStyle().ItemSpacing.x/2);

                    ImGui::Text(page_counter.c_str());

                    ImGui::EndChild();
                    ImGui::EndGroup();
                }
            }
        }
        ImGui::End();
    }
}