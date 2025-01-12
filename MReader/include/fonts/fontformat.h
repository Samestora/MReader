#pragma once

#include <cstddef>
#include <cstring>
#include <vector>

namespace MRFont {
    enum FontFormat {
        FORMAT_UNKNOWN,
        FORMAT_OTF,
        FORMAT_TTF        
    };

    struct FontList {
        int amount;
        std::vector<char> list; // Use vector for dynamic string buffer management
    };

    FontFormat DetectFontFormat(const void* data, size_t data_size);
}