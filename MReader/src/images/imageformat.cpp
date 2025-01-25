#include "images/imageformat.h"

namespace MRImage {
    MRImage::ImageFormat DetectImageFormat(const void* data, size_t data_size) {
        if (data_size < 12) // Minimum size to check all headers
            return MRImage::ImageFormat::FORMAT_UNKNOWN;

        const unsigned char* bytes = static_cast<const unsigned char*>(data);

        /* 
         * Check if the file is a JPEG
         * JPEG file signature varies.. and these are the first 3 bytes that are all the same
         * check here : https://en.wikipedia.org/wiki/List_of_file_signatures
         */
        if (bytes[0] == 0xFF && bytes[1] == 0xD8 && bytes[2] == 0xFF)
            return MRImage::ImageFormat::FORMAT_JPEG;

        // Check if the file is a PNG
        else if (bytes[0] == 0x89 && bytes[1] == 0x50 && bytes[2] == 0x4E && bytes[3] == 0x47 && bytes[4] == 0x0D && bytes[5] == 0x0A && bytes[6] == 0x1A && bytes[7] == 0x0A)
            return MRImage::ImageFormat::FORMAT_PNG;

        // Check if the file is a HDR
        else if (std::memcmp(data, "#?RADIANCE", 10) == 0)
            return MRImage::ImageFormat::FORMAT_HDR;

        // Check if the file is a WebP (RIFF + WEBP)
        else if (bytes[0] == 'R' && bytes[1] == 'I' && bytes[2] == 'F' && bytes[3] == 'F' &&
            bytes[8] == 'W' && bytes[9] == 'E' && bytes[10] == 'B' && bytes[11] == 'P')
            return MRImage::ImageFormat::FORMAT_WEBP;

        // GIF Format signature
        else if (bytes[0] == 0x47 && bytes[1] == 0x49 && bytes[2] == 0x46 && bytes[3] == 0x38)
            return MRImage::ImageFormat::FORMAT_GIF;
        
        // Zip file signature also works with CBZ
        else if (bytes[0] == 0x4C && bytes[1] == 0x5A && bytes[2] == 0x49 && bytes[3] == 0x50) {
            return MRImage::ImageFormat::FORMAT_ZIP;
        }

        return MRImage::ImageFormat::FORMAT_UNKNOWN;
    }
}
