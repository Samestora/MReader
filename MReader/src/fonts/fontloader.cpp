#define _CRT_SECURE_NO_WARNINGS

#include "fonts/fontloader.h"
#include "fonts/fontformat.h"

namespace MRFont {
	/*
    * set a specific font by it's path, font glyph range automatically set to japanese, no config
    * USE THIS ONLY FOR AN ARRAY THAT'S ALREADY RUN getFontList()!
	*/
	void setFontbyFilepath(std::string fontpath, float fontsize ,ImGuiIO& io) {
		ImFont* font = io.Fonts->AddFontFromFileTTF(fontpath.c_str(), fontsize, nullptr, io.Fonts->GetGlyphRangesJapanese());
		IM_ASSERT(font != nullptr);
	}

    /*
    * sanitize the input once more in case of being called out-of-the-box
    */
    void setFontbyFilepathSafe(std::string fontpath, float fontsize, ImGuiIO& io) {
        if (LoadFontFromFile(fontpath.c_str())) {
            ImFont* font = io.Fonts->AddFontFromFileTTF(fontpath.c_str(), fontsize, nullptr, io.Fonts->GetGlyphRangesJapanese());
            IM_ASSERT(font != nullptr);
        }
    }

	/*
	* Get all and sanitize it, if clear put it to std::string for font_list array
	*/
	std::vector<std::string> getFontList(std::string fontdirpath) {
        std::vector<std::string> fonts;

        try {
            for (const auto& entry : fs::directory_iterator(fontdirpath)) {
                std::string file_name = entry.path().filename().string();
                std::string extension = entry.path().extension().string();
                if (LoadFontFromFile(entry.path().string().c_str())){
                    fonts.emplace_back(entry.path().string());
                }
            }
        }
        catch (const fs::filesystem_error& e) {
            std::cerr << "Filesystem error: " << e.what() << std::endl;
        }
        
        return fonts;
	}

    bool CheckFontFromMemory(const void* file_data, size_t file_size) {
        // Dereference if null
        if (!file_data) {
            return false;
        }
        MRFont::FontFormat format = MRFont::DetectFontFormat(file_data, file_size);

        if (format == MRFont::FontFormat::FORMAT_UNKNOWN) {
            return false;
        }

        return true;
    }

    bool LoadFontFromFile(const char* file_name) {
        FILE* file = fopen(file_name, "rb");
        if (!file) return false;

        fseek(file, 0, SEEK_END);
        size_t file_size = ftell(file);
        fseek(file, 0, SEEK_SET);

        void* file_data = malloc(file_size);
        fread(file_data, 1, file_size, file);
        fclose(file);

        bool result = CheckFontFromMemory(file_data, file_size);
        free(file_data);

        return result;
    }
}
