#pragma once

#include "window/main_window.h"
#include "window/nav_window.h"
#include "settings/themes.h"
#include "images/mangaloader.h"
#include "graphics/release.h"
#include "window/main_window_state.h"
#include "images/imageformat.h"

#include <algorithm>
#include <fstream>
#include <cstring>
#include <cstdio>

namespace MRApplication {
    void Render(
        const std::string& mangaBasePath,
        const std::vector<std::string>& manga_list,
        MRImage::Image& image,
        MRWindow::MainWindowState& state,
        std::vector<ID3D11ShaderResourceView*>& textures,
        const ImVec4& clear_color,
        ID3D11DeviceContext* g_pd3dDeviceContext,
        IDXGISwapChain* g_pSwapChain,
        bool& g_SwapChainOccluded,
        ID3D11RenderTargetView* g_mainRenderTargetView,
        ID3D11Device* g_pd3dDevice
    );
}