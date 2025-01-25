#pragma once

#include "graphics/textureloader.h"
#include "window/mainwindowstate.h"
#include <string>
#include <future>

#ifndef MREADER_MREADER_INCLUDE_IMAGES_MANGALOADER_H_
#define MREADER_MREADER_INCLUDE_IMAGES_MANGALOADER_H_

namespace MRImage {
	void LoadAllPages(const std::string& directory, std::vector<ID3D11ShaderResourceView*>& out_srvs, std::vector<MRImage::Image>& image, ID3D11Device* g_pd3dDevice);
	void LoadNormal(const std::string& directory, MRWindow::MainWindowState* state, std::vector<ID3D11ShaderResourceView*>& out_srvs, std::vector<MRImage::Image>& image, ID3D11Device* g_pd3dDevice);
	void LoadFirstMangaCover(const std::string& directory, MRWindow::MainWindowState* state, std::vector<ID3D11ShaderResourceView*>& out_srvs, std::vector<MRImage::Image>& image, ID3D11Device* g_pd3dDevice);
	std::vector<std::string> LoadMangaList(const std::string& directory);
}

#endif //MREADER_MREADER_INCLUDE_IMAGES_MANGALOADER_H_