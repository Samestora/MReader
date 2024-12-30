#pragma once

#include <cstddef>
#include <cstring>

enum ImageFormat {
    FORMAT_UNKNOWN,
    FORMAT_JPEG,
    FORMAT_PNG,
    FORMAT_HDR,
    FORMAT_WEBP
};

ImageFormat DetectImageFormat(const void* data, size_t data_size);
