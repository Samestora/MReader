#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_RESIZE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#define _CRT_SECURE_NO_WARNINGS

// Target dimensions for downscaling
#define TARGET_IMAGE_WIDTH 700
#define TARGET_IMAGE_HEIGHT 980

#define TARGET_COVER_WIDTH 350
#define TARGET_COVER_HEIGHT 490

#include "graphics/textureloader.h"
#include <stb/stb_image_resize.h>
#include <stb/stb_image.h>
#include <stb/stb_image_write.h>
#include <webp/decode.h>

namespace MRGraphics {
    bool LoadTextureFromMemory(const void* data, size_t data_size, ID3D11ShaderResourceView** out_srv, int* out_width, int* out_height, ID3D11Device* g_pd3dDevice) {
        // Dereference if null
        if (!data || !out_srv || !out_width || !out_height || !g_pd3dDevice) {
            return false;
        }

        MRImage::ImageFormat format = MRImage::DetectImageFormat(data, data_size);
        if (format == MRImage::ImageFormat::FORMAT_UNKNOWN) {
            return false;
        }

        int image_width = 0, image_height = 0;
        unsigned char* image_data = nullptr;

        if (format == MRImage::ImageFormat::FORMAT_WEBP) {
            image_data = WebPDecodeRGBA(static_cast<const uint8_t*>(data), data_size, &image_width, &image_height);
        }
        else if (format == MRImage::ImageFormat::FORMAT_JPEG || format == MRImage::ImageFormat::FORMAT_PNG) {
            image_data = stbi_load_from_memory(static_cast<const unsigned char*>(data), static_cast<int>(data_size), &image_width, &image_height, nullptr, 4);
        }
        else if (format == MRImage::ImageFormat::FORMAT_HDR) {
            float* hdr_data = stbi_loadf_from_memory(static_cast<const unsigned char*>(data), static_cast<int>(data_size), &image_width, &image_height, nullptr, 4);
            if (hdr_data) {
                size_t pixel_count = image_width * image_height * 4;
                image_data = new unsigned char[pixel_count];
                for (size_t i = 0; i < pixel_count; ++i)
                    image_data[i] = static_cast<unsigned char>(hdr_data[i] * 255.0f);
                stbi_image_free(hdr_data);
            }
        }
        else if (format == MRImage::ImageFormat::FORMAT_GIF) {
            int image_channel;
            image_data = stbi_load_from_memory(static_cast<const unsigned char*>(data), static_cast<int>(data_size), &image_width, &image_height, &image_channel, 4);
        }

        if (!image_data) return false;

        // Ease GPU and RAM Usage by resizing it, else dont make it bigger
        if (image_width > TARGET_IMAGE_WIDTH || image_height > TARGET_IMAGE_HEIGHT) {
            float ratio = min((float)TARGET_IMAGE_WIDTH / image_width, (float)TARGET_IMAGE_HEIGHT / image_height);

            int width_final = (int)(image_width * ratio);
            int height_final = (int)(image_height * ratio);
            unsigned char* resized_image = new unsigned char[width_final * height_final * 4]; // RGBA
            bool resized = stbir_resize_uint8(image_data, image_width, image_height, 0, resized_image, width_final, height_final, 0, 4);

            // Free original image data and update pointers if resized successfully
            if (resized) {
                if (format == MRImage::ImageFormat::FORMAT_WEBP) {
                    WebPFree(image_data);
                }
                else {
                    stbi_image_free(image_data);
                }

                image_data = resized_image;
                image_height = height_final;
                image_width = width_final;

            }
            else {
                delete[] resized_image; // Cleanup on failure
            }
        }
        D3D11_TEXTURE2D_DESC desc = {};
        ZeroMemory(&desc, sizeof(desc));
        desc.Width = image_width;
        desc.Height = image_height;
        desc.MipLevels = 1;
        desc.ArraySize = 1; 
        desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        desc.SampleDesc.Count = 1;
        desc.Usage = D3D11_USAGE_DEFAULT;
        desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
        desc.CPUAccessFlags = 0;

        ID3D11Texture2D* texture = nullptr;
        D3D11_SUBRESOURCE_DATA subResource = { image_data, static_cast<UINT>(desc.Width * 4), 0 };

        HRESULT hr = g_pd3dDevice->CreateTexture2D(&desc, &subResource, &texture);
        if (FAILED(hr)) {
            if (format == MRImage::ImageFormat::FORMAT_WEBP) {
                WebPFree(image_data);
            }
            else {
                stbi_image_free(image_data);
            }
            return false;
        }

        D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
        ZeroMemory(&srvDesc, sizeof(srvDesc));
        srvDesc.Format = desc.Format;
        srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
        srvDesc.Texture2D.MostDetailedMip = 0;
        srvDesc.Texture2D.MipLevels = desc.MipLevels;

        hr = g_pd3dDevice->CreateShaderResourceView(texture, &srvDesc, out_srv);
        texture->Release();
        if (format == MRImage::ImageFormat::FORMAT_WEBP) {
            WebPFree(image_data);
        }
        else {
            stbi_image_free(image_data);
        }

        if (FAILED(hr)) {
            return false;
        };

        *out_width = image_width;
        *out_height = image_height;
                

        return true;
    }

    bool LoadCoverFromMemory(const void* data, size_t data_size, ID3D11ShaderResourceView** out_srv, int* out_width, int* out_height, ID3D11Device* g_pd3dDevice) {
        // Dereference if null
        if (!data || !out_srv || !out_width || !out_height || !g_pd3dDevice) {
            return false;
        }

        MRImage::ImageFormat format = MRImage::DetectImageFormat(data, data_size);
        if (format == MRImage::ImageFormat::FORMAT_UNKNOWN) {
            return false;
        }

        int image_width = 0, image_height = 0;
        unsigned char* image_data = nullptr;

        if (format == MRImage::ImageFormat::FORMAT_WEBP) {
            image_data = WebPDecodeRGBA(static_cast<const uint8_t*>(data), data_size, &image_width, &image_height);
        }
        else if (format == MRImage::ImageFormat::FORMAT_JPEG || format == MRImage::ImageFormat::FORMAT_PNG) {
            image_data = stbi_load_from_memory(static_cast<const unsigned char*>(data), static_cast<int>(data_size), &image_width, &image_height, nullptr, 4);
        }
        else if (format == MRImage::ImageFormat::FORMAT_HDR) {
            float* hdr_data = stbi_loadf_from_memory(static_cast<const unsigned char*>(data), static_cast<int>(data_size), &image_width, &image_height, nullptr, 4);
            if (hdr_data) {
                size_t pixel_count = image_width * image_height * 4;
                image_data = new unsigned char[pixel_count];
                for (size_t i = 0; i < pixel_count; ++i)
                    image_data[i] = static_cast<unsigned char>(hdr_data[i] * 255.0f);
                stbi_image_free(hdr_data);
            }
        }
        else if (format == MRImage::ImageFormat::FORMAT_GIF) {
            int image_channel;
            image_data = stbi_load_from_memory(static_cast<const unsigned char*>(data), static_cast<int>(data_size), &image_width, &image_height, &image_channel, 4);
        }

        if (!image_data) return false;

        // Ease GPU and RAM Usage by resizing it, else dont make it bigger
        if (image_width > TARGET_COVER_WIDTH || image_height > TARGET_COVER_HEIGHT) {
            float ratio = min((float)TARGET_COVER_WIDTH / image_width, (float)TARGET_COVER_HEIGHT / image_height);

            int width_final = (int)(image_width * ratio);
            int height_final = (int)(image_height * ratio);
            unsigned char* resized_image = new unsigned char[width_final * height_final * 4]; // RGBA
            bool resized = stbir_resize_uint8(image_data, image_width, image_height, 0, resized_image, width_final, height_final, 0, 4);

            // Free original image data and update pointers if resized successfully
            if (resized) {
                if (format == MRImage::ImageFormat::FORMAT_WEBP) {
                    WebPFree(image_data);
                }
                else {
                    stbi_image_free(image_data);
                }

                image_data = resized_image;
                image_height = height_final;
                image_width = width_final;

            }
            else {
                delete[] resized_image; // Cleanup on failure
            }
        }

        D3D11_TEXTURE2D_DESC desc = {};
        ZeroMemory(&desc, sizeof(desc));
        desc.Width = image_width;
        desc.Height = image_height;
        desc.MipLevels = 1;
        desc.ArraySize = 1;
        desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        desc.SampleDesc.Count = 1;
        desc.Usage = D3D11_USAGE_DEFAULT;
        desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
        desc.CPUAccessFlags = 0;

        ID3D11Texture2D* texture = nullptr;
        D3D11_SUBRESOURCE_DATA subResource = { image_data, static_cast<UINT>(desc.Width * 4), 0 };

        HRESULT hr = g_pd3dDevice->CreateTexture2D(&desc, &subResource, &texture);
        if (FAILED(hr)) {
            if (format == MRImage::ImageFormat::FORMAT_WEBP) {
                WebPFree(image_data);
            }
            else {
                stbi_image_free(image_data);
            }
            return false;
        }

        D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
        ZeroMemory(&srvDesc, sizeof(srvDesc));
        srvDesc.Format = desc.Format;
        srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
        srvDesc.Texture2D.MostDetailedMip = 0;
        srvDesc.Texture2D.MipLevels = desc.MipLevels;

        hr = g_pd3dDevice->CreateShaderResourceView(texture, &srvDesc, out_srv);
        texture->Release();
        if (format == MRImage::ImageFormat::FORMAT_WEBP) {
            WebPFree(image_data);
        }
        else {
            stbi_image_free(image_data);
        }

        if (FAILED(hr)) {
            return false;
        };

        *out_width = image_width;
        *out_height = image_height;


        return true;
    }

    bool LoadTextureFromFile(const char* file_name, ID3D11ShaderResourceView** out_srv, int* out_width, int* out_height, ID3D11Device* g_pd3dDevice, LOAD_OPTION load_option) {
        FILE* file = fopen(file_name, "rb");
        if (!file) { 
            std::cerr << "File not found" << std::endl;
            return false; 
        }

        fseek(file, 0, SEEK_END);
        size_t file_size = ftell(file);
        fseek(file, 0, SEEK_SET);

        void* file_data = malloc(file_size);
        fread(file_data, 1, file_size, file);
        fclose(file);

        bool result;
        switch (load_option) {
        case LOAD_NORMAL:
            result = LoadTextureFromMemory(file_data, file_size, out_srv, out_width, out_height, g_pd3dDevice);
            if (!result) {
                std::cerr << "Failed on making texture" << std::endl;
            }
            free(file_data);
            return result;
        case LOAD_COVER:
            result = LoadCoverFromMemory(file_data, file_size, out_srv, out_width, out_height, g_pd3dDevice);
            if (!result) {
                std::cerr << "Failed loading cover" << std::endl;
            }
            free(file_data);
            return result;
        }

        return false;
    }

}