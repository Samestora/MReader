#include "application/init.h"

namespace MRApplication {
    void Init(
        const std::string& mangaBasePath,
        const std::vector<std::string>& manga_list,
        int& selected,
        int& lastSelected,
        int& image_x,
        int& image_y,
        bool& newlyRead,
        bool& favorite,
        std::vector<ID3D11ShaderResourceView*>& textures,
        const ImVec4& clear_color,
        ID3D11DeviceContext* g_pd3dDeviceContext,
        IDXGISwapChain* g_pSwapChain,
        bool& g_SwapChainOccluded,
        ID3D11RenderTargetView* g_mainRenderTargetView,
        ID3D11Device* g_pd3dDevice
    ) {
        // Start the Dear ImGui frame
        ImGui_ImplDX11_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

        // Setup viewport sizes
        ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImVec2 navSize = ImVec2(viewport->Size.x / 3, viewport->Size.y);
        ImVec2 mainSize = ImVec2(viewport->Size.x / 3 * 2, viewport->Size.y);

        // Create navigation window
        MRWindow::createNavWindow(navSize, ImVec2(0, 0), manga_list.size(), manga_list, selected);

        // Load textures only if the selection changes
        if (selected != lastSelected) {
            // Release current textures
            MRUtil::ReleaseTextures(textures);

            // Load the first manga cover for the new selection
            std::string selectedMangaPath = mangaBasePath + manga_list[selected];
            LoadFirstMangaCover(selectedMangaPath, textures, &image_x, &image_y, g_pd3dDevice);

            // Set image dimensions for rendering
            image_x = IMAGE_WIDTH;
            image_y = IMAGE_HEIGHT;

            // Update last selected index
            lastSelected = selected;
        }

        // Create main window
        MRWindow::createMainWindow(mainSize, ImVec2(navSize.x, 0), manga_list, textures, image_x, image_y, selected, &newlyRead, &favorite);

        // Rendering
        ImGui::Render();
        const float clear_color_with_alpha[4] = {
            clear_color.x * clear_color.w,
            clear_color.y * clear_color.w,
            clear_color.z * clear_color.w,
            clear_color.w
        };
        g_pd3dDeviceContext->OMSetRenderTargets(1, &g_mainRenderTargetView, nullptr);
        g_pd3dDeviceContext->ClearRenderTargetView(g_mainRenderTargetView, clear_color_with_alpha);
        ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

        // Present the frame
        HRESULT hr = g_pSwapChain->Present(1, 0); // Present with vsync
        g_SwapChainOccluded = (hr == DXGI_STATUS_OCCLUDED);
    }

}