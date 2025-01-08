#pragma once

#include <cstddef>
#include <cstring>

namespace MRImage {
    enum ImageFormat {
        FORMAT_UNKNOWN,
        FORMAT_JPEG,
        FORMAT_PNG,
        FORMAT_HDR,
        FORMAT_WEBP
    };

    struct Image {
        int width;
        int height;
    };

    ImageFormat DetectImageFormat(const void* data, size_t data_size);
}
