#pragma once

#define IMAGE_WIDTH 514 // 720
#define IMAGE_HEIGHT 714 // 1000

#include <iostream>
#include <filesystem>
#include <d3d11.h>
#include <imgui/imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_dx11.h>
#include <regex>
#include <algorithm>
#include <fstream>
#include <cstring>
#include <vector>
#include <cstdio>

#include "window/main_window.h"
#include "window/nav_window.h"
#include "settings/themes.h"
#include "util/mangaloader.h"
#include "util/release.h"

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
    );
}