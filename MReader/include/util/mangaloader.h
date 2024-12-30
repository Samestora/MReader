#pragma once

#include "util/textureloader.h"
#include <vector>
#include <string>
#include <filesystem>
#include <regex>
#include <iostream>

namespace fs = std::filesystem;

void LoadTexturesFromDirectory(const std::string& directory, std::vector<ID3D11ShaderResourceView*>& out_srvs, int* out_width, int* out_height, ID3D11Device* g_pd3dDevice);
void LoadFirstMangaCover(const std::string& directory, std::vector<ID3D11ShaderResourceView*>& out_srvs, int* out_width, int* out_height, ID3D11Device* g_pd3dDevice);
std::vector<std::string> GetMangaList(const std::string& directory);
