#pragma once

#include <imgui.h>
#include <string>
#include <vector>
#include <filesystem>
#include <iostream>

#include "fonts/fontformat.h"

#ifndef MREADER_MREADER_INCLUDE_FONTS_FONTLOADER_H_
#define MREADER_MREADER_INCLUDE_FONTS_FONTLOADER_H_

namespace fs = std::filesystem;

namespace MRFont {
	std::vector<std::string> getFontList(std::string fontdirpath); // Return absolute path of those fonts
	
	void setFontbyFilepath(std::string fontpath, float fontsize);
	void setFontbyFilepathSafe(std::string fontpath, float fontsize);
	
	void setDefaultFont();
	FontList getFontListForLabel();

	void selectFont(int index);
	bool showFontSelector(const char* label);

	bool CheckFontFromMemory(const void* file_data, size_t file_size);
	bool LoadFontFromFile(const char* file_name);
}

#endif // MREADER_MREADER_INCLUDE_FONTS_FONTLOADER_H_