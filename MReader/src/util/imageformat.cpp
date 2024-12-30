#include "util/imageformat.h

ImageFormat DetectImageFormat(const void* data, size_t data_size) {
    if (data_size < 12) // Minimum size to check all headers
        return FORMAT_UNKNOWN;

    const unsigned char* bytes = static_cast<const unsigned char*>(data);

    // Check for JPEG
    if (bytes[0] == 0xFF && bytes[1] == 0xD8 && bytes[2] == 0xFF)
        return FORMAT_JPEG;

    // Check for PNG
    if (bytes[0] == 0x89 && bytes[1] == 0x50 && bytes[2] == 0x4E && bytes[3] == 0x47)
        return FORMAT_PNG;

    // Check for HDR
    if (std::memcmp(data, "#?RADIANCE", 10) == 0)
        return FORMAT_HDR;

    // Check for WebP (RIFF + WEBP)
    if (bytes[0] == 'R' && bytes[1] == 'I' && bytes[2] == 'F' && bytes[3] == 'F' &&
        bytes[8] == 'W' && bytes[9] == 'E' && bytes[10] == 'B' && bytes[11] == 'P')
        return FORMAT_WEBP;

    return FORMAT_UNKNOWN;
}
