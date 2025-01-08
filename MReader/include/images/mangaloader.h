#pragma once

#include "graphics/textureloader.h"
#include "window/main_window_state.h"
#include <string>

void LoadAllPages(const std::string& directory, std::vector<ID3D11ShaderResourceView*>& out_srvs, int* out_width, int* out_height, ID3D11Device* g_pd3dDevice);
void LoadNormal(const std::string& directory, MRWindow::MainWindowState* state, std::vector<ID3D11ShaderResourceView*>& out_srvs, int* out_width, int* out_height, ID3D11Device* g_pd3dDevice);
void LoadFirstMangaCover(const std::string& directory, std::vector<ID3D11ShaderResourceView*>& out_srvs, int* out_width, int* out_height, ID3D11Device* g_pd3dDevice);
std::vector<std::string> LoadMangaList(const std::string& directory);