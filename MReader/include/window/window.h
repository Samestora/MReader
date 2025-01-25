#pragma once
#ifndef MREADER_MREADER_INCLUDE_WINDOW_WINDOW_H_
#define MREADER_MREADER_INCLUDE_WINDOW_WINDOW_H_

#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_dx11.h>
#include <d3d11.h>
#include <tchar.h>
#include <vector>
#include <string>
#include <cstdlib>

#include "fonts/fontloader.h"
#include "settings/themes.h"
#include "settings/preferences.h"
#include "window/mainwindowstate.h"
#include "images/imageformat.h"
#include "graphics/gadgets.h"

namespace MRWindow {
	void CreateMainWindow(std::vector<std::string> mangalistobj, std::vector<ID3D11ShaderResourceView*> textures, std::vector<MRImage::Image> image, MainWindowState* state);
}


#endif // MREADER_MREADER_INCLUDE_WINDOW_WINDOW_H_