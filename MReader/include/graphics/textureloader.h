#pragma once
#include "images/imageformat.h"

#include <iostream>
#include <filesystem>
#include <d3d11.h>
#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_dx11.h>
#include <regex>
#include <algorithm>
#include <vector>

#ifndef MREADER_MREADER_INCLUDE_GRAPHICS_TEXTURELOADER_H_
#define MREADER_MREADER_INCLUDE_GRAPHICS_TEXTURELOADER_H_

namespace fs = std::filesystem;

namespace MRGraphics {
	enum LOAD_OPTION {
		LOAD_NONE,
		LOAD_COVER,
		LOAD_NORMAL
	};

	bool LoadTextureFromMemory(const void* data, size_t data_size, ID3D11ShaderResourceView** out_srv, int* out_width, int* out_height, ID3D11Device* g_pd3dDevice);
	bool LoadCoverFromMemory(const void* data, size_t data_size, ID3D11ShaderResourceView** out_srv, int* out_width, int* out_height, ID3D11Device* g_pd3dDevice);
	bool LoadTextureFromFile(const char* file_name, ID3D11ShaderResourceView** out_srv, int* out_width, int* out_height, ID3D11Device* g_pd3dDevice, LOAD_OPTION load_option);
}

#endif //MREADER_MREADER_INCLUDE_GRAPHICS_TEXTURELOADER_H_