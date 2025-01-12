#pragma once
#define _CRT_SECURE_NO_WARNINGS

#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"
#include <d3d11.h>
#include <tchar.h>
#include <vector>
#include <string>
#include <cstdlib>

#include "fonts/fontloader.h"
#include "settings/themes.h"

namespace MRWindow {
	void createNavWindow(ImVec2 navSize, ImVec2 navPos, int mangaTotal, std::vector<std::string> mangalistobj, int& selected);
}