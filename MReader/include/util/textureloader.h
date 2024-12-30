#pragma once

#define _CRT_SECURE_NO_WARNINGS
#define STB_IMAGE_IMPLEMENTATION

#include "util/imageformat.h"
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
#include <cstdlib>
#include <stb/stb_image.h>
#include <webp/decode.h>


bool LoadTextureFromMemory(const void* data, size_t data_size, ID3D11ShaderResourceView** out_srv, int* out_width, int* out_height, ID3D11Device* g_pd3dDevice);
bool LoadTextureFromFile(const char* file_name, ID3D11ShaderResourceView** out_srv, int* out_width, int* out_height, ID3D11Device* g_pd3dDevice);
