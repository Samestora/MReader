#include "util/textureloader.h"

bool LoadTextureFromMemory(const void* data, size_t data_size, ID3D11ShaderResourceView** out_srv, int* out_width, int* out_height, ID3D11Device* g_pd3dDevice) {
    ImageFormat format = DetectImageFormat(data, data_size);
    if (format == FORMAT_UNKNOWN)
        return false;

    int image_width = 0, image_height = 0;
    unsigned char* image_data = nullptr;

    if (format == FORMAT_WEBP) {
        image_data = WebPDecodeRGBA(static_cast<const uint8_t*>(data), data_size, &image_width, &image_height);
    }
    else if (format == FORMAT_JPEG || format == FORMAT_PNG) {
        image_data = stbi_load_from_memory(static_cast<const unsigned char*>(data), static_cast<int>(data_size), &image_width, &image_height, nullptr, 4);
    }
    else if (format == FORMAT_HDR) {
        float* hdr_data = stbi_loadf_from_memory(static_cast<const unsigned char*>(data), static_cast<int>(data_size), &image_width, &image_height, nullptr, 4);
        if (hdr_data) {
            size_t pixel_count = image_width * image_height * 4;
            image_data = new unsigned char[pixel_count];
            for (size_t i = 0; i < pixel_count; ++i)
                image_data[i] = static_cast<unsigned char>(hdr_data[i] * 255.0f);
            stbi_image_free(hdr_data);
        }
    }

    if (!image_data) return false;

    D3D11_TEXTURE2D_DESC desc = {};
    desc.Width = image_width;
    desc.Height = image_height;
    desc.MipLevels = 1;
    desc.ArraySize = 1;
    desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    desc.SampleDesc.Count = 1;
    desc.Usage = D3D11_USAGE_DEFAULT;
    desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

    ID3D11Texture2D* texture = nullptr;
    D3D11_SUBRESOURCE_DATA subResource = { image_data, static_cast<UINT>(desc.Width * 4), 0 };

    HRESULT hr = g_pd3dDevice->CreateTexture2D(&desc, &subResource, &texture);
    if (FAILED(hr)) {
        stbi_image_free(image_data);
        return false;
    }

    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
    srvDesc.Format = desc.Format;
    srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MostDetailedMip = 0;
    srvDesc.Texture2D.MipLevels = 1;

    hr = g_pd3dDevice->CreateShaderResourceView(texture, &srvDesc, out_srv);
    texture->Release();
    stbi_image_free(image_data);

    if (FAILED(hr)) return false;

    *out_width = image_width;
    *out_height = image_height;

    return true;
}

bool LoadTextureFromFile(const char* file_name, ID3D11ShaderResourceView** out_srv, int* out_width, int* out_height, ID3D11Device* g_pd3dDevice) {
    FILE* file = fopen(file_name, "rb");
    if (!file) return false;

    fseek(file, 0, SEEK_END);
    size_t file_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    void* file_data = malloc(file_size);
    fread(file_data, 1, file_size, file);
    fclose(file);

    bool result = LoadTextureFromMemory(file_data, file_size, out_srv, out_width, out_height,  g_pd3dDevice);
    free(file_data);

    return result;
}
