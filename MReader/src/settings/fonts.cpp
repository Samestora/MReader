#include "settings/fonts.h"

namespace MRTheme {
	/*
    * set a specific font by it's path
	* font glyph range automatically set to japanese
	* and no config
	*/
	void setFontbyFilepath(std::string fontpath, float fontsize ,ImGuiIO& io) {
		ImFont* font = io.Fonts->AddFontFromFileTTF(fontpath.c_str(), fontsize, nullptr, io.Fonts->GetGlyphRangesJapanese());
		IM_ASSERT(font != nullptr);
	}

	/*
	* Get all
	*/
	std::vector<std::string> getFontList(std::string fontdirpath) {
        std::vector<std::string> fonts;

        try {
            for (const auto& entry : fs::directory_iterator(fontdirpath)) {
                std::string file_name = entry.path().filename().string();
                std::string extension = entry.path().extension().string();

                if (extension == ".ttf" || extension == ".otf") {
                    fonts.emplace_back(entry.path().string());
                }
            }
        }
        catch (const fs::filesystem_error& e) {
            std::cerr << "Filesystem error: " << e.what() << std::endl;
        }
        
        return fonts;
	}
}
