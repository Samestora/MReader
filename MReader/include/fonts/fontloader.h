#pragma once

#include <imgui.h>
#include <string>
#include <vector>
#include <filesystem>
#include <iostream>

namespace fs = std::filesystem;

namespace MRFont {
	std::vector<std::string> getFontList(std::string fontdirpath); // Return absolute path of those fonts
	
	void setFontbyFilepath(std::string fontpath, float fontsize, ImGuiIO& io);
	void setFontbyFilepathSafe(std::string fontpath, float fontsize, ImGuiIO& io);

	bool CheckFontFromMemory(const void* file_data, size_t file_size);
	bool LoadFontFromFile(const char* file_name);
}
