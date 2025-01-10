#pragma once

#include <cstddef>
#include <cstring>

namespace MRFont {
    enum FontFormat {
        FORMAT_UNKNOWN,
        FORMAT_OTF,
        FORMAT_TTF        
    };

    struct Font {
        float size;
    };

    FontFormat DetectFontFormat(const void* data, size_t data_size);
}