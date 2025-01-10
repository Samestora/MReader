#include "fonts/fontformat.h"

namespace MRFont {
	FontFormat DetectFontFormat(const void* data, size_t data_size) {
        if (data_size < 12) // Minimum size to check all headers
            return MRFont::FontFormat::FORMAT_UNKNOWN;

        const unsigned char* bytes = static_cast<const unsigned char*>(data);

        // Check for JPEG
        if (bytes[0] == 0x4F && bytes[1] == 0x54 && bytes[2] == 0x54 && bytes[3] == 0x4F)
            return MRFont::FontFormat::FORMAT_OTF;

        // Check for PNG
        if (bytes[0] == 0x00 && bytes[1] == 0x01 && bytes[2] == 0x00 && bytes[3] == 0x00 && bytes[4] == 0x00)
            return MRFont::FontFormat::FORMAT_TTF;

        return MRFont::FontFormat::FORMAT_UNKNOWN;
	}
}
