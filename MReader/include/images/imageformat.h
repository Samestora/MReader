#pragma once

#include <cstddef>
#include <cstring>

#ifndef MREADER_MREADER_INCLUDE_IMAGES_IMAGEFORMAT_H_
#define MREADER_MREADER_INCLUDE_IMAGES_IMAGEFORMAT_H_

namespace MRImage {
    enum ImageFormat {
        FORMAT_UNKNOWN,
        FORMAT_JPEG,
        FORMAT_PNG,
        FORMAT_HDR,
        FORMAT_WEBP,
        FORMAT_GIF,
        FORMAT_ZIP
    };

    struct Image {
        int width = 0;
        int height = 0;
    };

    ImageFormat DetectImageFormat(const void* data, size_t data_size);
}

#endif //MREADER_MREADER_INCLUDE_IMAGES_IMAGEFORMAT_H_