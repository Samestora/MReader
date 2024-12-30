#pragma once
#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"
#include <d3d11.h>
#include <tchar.h>
#include <vector>
#include <string>

namespace MRWindow {
	void createMainWindow(ImVec2 mainSize, ImVec2 mainPos, std::vector<std::string> mangalistobj, std::vector<ID3D11ShaderResourceView*> textures, int image_x, int image_y, int& selected, bool* newlyRead, bool* favorite);
}