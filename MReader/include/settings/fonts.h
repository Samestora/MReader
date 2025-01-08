#pragma once

#include <imgui.h>
#include <string>
#include <vector>
#include <filesystem>
#include <iostream>

namespace fs = std::filesystem;

namespace MRTheme {
	std::vector<std::string> getFontList(std::string fontdirpath); // Return abs path of those fonts
	void setFontbyFilepath(std::string fontpath, float fontsize, ImGuiIO& io);
}
