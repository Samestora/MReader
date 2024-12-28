// MReader main file
// About vendor (additional) library
// Get ImGui v1.91.6 here : https://github.com/ocornut/imgui/
// Get libwebp v1.50-windows-x64 here : https://developers.google.com/speed/webp/download
// The D3D11.lib should be built-in when using Visual Studio 2022 for the package

#define _CRT_SECURE_NO_WARNINGS
#define STB_IMAGE_IMPLEMENTATION

#define APP_WIDTH 1280
#define APP_HEIGHT 800
#define IMAGE_WIDTH 720
#define IMAGE_HEIGHT 1000

// Visual Studio Warnings
#ifdef _MSC_VER
#pragma warning (disable: 4127)     // condition expression is constant
#pragma warning (disable: 4996)     // 'This function or variable may be unsafe': strcpy, strdup, sprintf, vsnprintf, sscanf, fopen
#pragma warning (disable: 26451)    // [Static Analyzer] Arithmetic overflow : Using operator 'xxx' on a 4 byte value and then casting the result to an 8 byte value. Cast the value to the wider type before calling operator 'xxx' to avoid overflow(io.2).
#endif

#include <iostream>
#include <filesystem>
#include <d3d11.h>
#include <imgui/imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_dx11.h>
#include <regex>
#include <algorithm>
#include <fstream>
#include <nlohmann/json.hpp>

// MReader Defined Headers
#include "window/main_window.h"
#include "window/nav_window.h"
#include <stb/stb_image.h>
#include <webp/decode.h>
#include <cstring>
#include <vector>
#include <cstdio>

namespace fs = std::filesystem;
using json = nlohmann::json;

// Data
static ID3D11Device*            g_pd3dDevice            = nullptr;
static ID3D11DeviceContext*     g_pd3dDeviceContext     = nullptr;
static IDXGISwapChain*          g_pSwapChain            = nullptr;
static ID3D11RenderTargetView*  g_mainRenderTargetView  = nullptr;
static bool                     g_SwapChainOccluded     = false;
static UINT                     g_ResizeWidth = 0, g_ResizeHeight = 0;

// Forward declarations of helper functions
bool CreateDeviceD3D(HWND hWnd);
void CleanupDeviceD3D();
void CreateRenderTarget();
void CleanupRenderTarget();
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

std::string getBasePath() {
    std::ifstream configFile("config.json");
    if (configFile.is_open()) {
        json config;
        configFile >> config;
        return config["basePath"].get<std::string>();
    }
    return "./"; // Default base path
}

// All format for images are here could add more in the future
enum ImageFormat {
    FORMAT_UNKNOWN,
    FORMAT_JPEG,
    FORMAT_PNG,
    FORMAT_HDR,
    FORMAT_WEBP
};

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

// Simple helper function to load an image into a DX11 texture with common settings
bool LoadTextureFromMemory(const void* data, size_t data_size, ID3D11ShaderResourceView** out_srv, int* out_width, int* out_height) {
    // Detect the FORMAT from bytes and then store it to format
    ImageFormat format = DetectImageFormat(data, data_size);

    if (format == FORMAT_UNKNOWN)
        return false;

    // Load from disk into a raw RGBA buffer
    int image_width = 0;
    int image_height = 0;

    unsigned char* image_data = NULL;

    if (format == FORMAT_WEBP) {
        image_data = WebPDecodeRGBA(static_cast<const uint8_t*>(data), data_size, &image_width, &image_height);
        if (image_data == NULL) {
            return false;
        }
    }
    else if (format == FORMAT_JPEG || format == FORMAT_PNG) {
        image_data = stbi_load_from_memory((const unsigned char*)data, (int)data_size, &image_width, &image_height, NULL, 4);
        if (image_data == NULL) {
            return false;
        }
    }
    else if (format == FORMAT_HDR) {
        float* hdr_data = stbi_loadf_from_memory(static_cast<const unsigned char*>(data), static_cast<int>(data_size), &image_width, &image_height, nullptr, 4);
        if (hdr_data == NULL) {
            return false;
        }
        // Convert floats to 8-bit unsigned chars
        size_t pixel_count = image_width * image_height * 4;
        image_data = new unsigned char[pixel_count];
        for (size_t i = 0; i < pixel_count; ++i) {
            image_data[i] = static_cast<unsigned char>(hdr_data[i] * 255.0f);
        }
        stbi_image_free(hdr_data);
    }
    // Known but unsupported format, Will be supported in the future
    if (image_data == NULL)
        return false;


    // Create texture
    D3D11_TEXTURE2D_DESC desc;
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

    ID3D11Texture2D* pTexture = NULL;
    D3D11_SUBRESOURCE_DATA subResource;
    subResource.pSysMem = image_data;
    subResource.SysMemPitch = desc.Width * 4;
    subResource.SysMemSlicePitch = 0;
    g_pd3dDevice->CreateTexture2D(&desc, &subResource, &pTexture);

    // Create texture view
    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
    ZeroMemory(&srvDesc, sizeof(srvDesc));
    srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MipLevels = desc.MipLevels;
    srvDesc.Texture2D.MostDetailedMip = 0;
    g_pd3dDevice->CreateShaderResourceView(pTexture, &srvDesc, out_srv);
    pTexture->Release();

    *out_width = image_width;
    *out_height = image_height;
    stbi_image_free(image_data);

    return true;
}

// Open and read a file, then forward to LoadTextureFromMemory()
bool LoadTextureFromFile(const char* file_name, ID3D11ShaderResourceView** out_srv, int* out_width, int* out_height)
{
    FILE* f = fopen(file_name, "rb");
    if (f == NULL)
        return false;
    fseek(f, 0, SEEK_END);
    size_t file_size = (size_t)ftell(f);
    if (file_size == -1)
        return false;
    fseek(f, 0, SEEK_SET);
    void* file_data = IM_ALLOC(file_size);
    fread(file_data, 1, file_size, f);
    bool ret = LoadTextureFromMemory(file_data, file_size, out_srv, out_width, out_height);
    IM_FREE(file_data);
    return ret;
}

// @std::string& directory 
// - Must use absolute path not relative
// - Abide by the Windows standard since MReader is only for windows
// example : "D:\\Manga Collection\\Manga Title\\"
// 
// @std::string file_name 
// - Must correspond be a number as a string starting from 1 not 01
// - Must abide by ImageFormat!
// - Must be a list of file with homogenous format
// example : 1.jpg, 2.jpg,..., etc
void LoadTexturesFromDirectory(const std::string& directory, std::vector<ID3D11ShaderResourceView*>& out_srvs, int* out_width, int* out_height) {
    // Pair of file index (int) and it's path (std::string)
    std::vector<std::pair<int, std::string>> files;

    try {
        std::regex number_pattern(R"((\d+))");

        for (const auto& entry : fs::directory_iterator(directory)) {
            std::string file_name = entry.path().filename().string();

            std::smatch match;
            if (std::regex_search(file_name, match, number_pattern)) {
                // Convert the first matched number to an integer
                int num = std::stoi(match.str(1));
                files.emplace_back(num, entry.path().string());
            }
        }

        // Sort files by the numeric value
        std::sort(files.begin(), files.end(), [](const auto& a, const auto& b) {
            return a.first < b.first;
            });

        for (const auto& [num, file_path] : files) {
            // Check if file exists
            if (!fs::exists(file_path)) {
                std::cerr << "File does not exist: " << file_path << std::endl;
                continue; // Skip this file
            }

            // Load texture from file
            ID3D11ShaderResourceView* texture_srv = nullptr;
            bool ret = LoadTextureFromFile(file_path.c_str(), &texture_srv, out_width, out_height); // Assuming this function exists
            if (!ret || texture_srv == nullptr) {
                std::cerr << "Failed to load texture: " << file_path << std::endl;
                continue; // Skip this file
            }

            // Add the loaded texture to the output vector
            out_srvs.push_back(texture_srv);
        }
    }
    catch (const fs::filesystem_error& e) {
        std::cerr << "Filesystem error: " << e.what() << std::endl;
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
}

std::vector<std::string> GetMangaList(const std::string& directory) {
    std::vector<std::string> manga_list;
    try {
        // Check if the given path exists and is a directory
        if (fs::exists(directory) && fs::is_directory(directory)) {
            for (const auto& entry : fs::directory_iterator(directory)) {
                // Check if the entry is a directory
                if (fs::is_directory(entry.status())) {
                    manga_list.push_back(entry.path().filename().string());
                }
            }
        }
        else {
            std::cerr << "Path does not exist or is not a directory: " << directory << std::endl;
        }
    }
    catch (const fs::filesystem_error& e) {
        std::cerr << "Filesystem error: " << e.what() << std::endl;
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
    return manga_list;
}

void ReleaseTextures(std::vector<ID3D11ShaderResourceView*>& textures){
    // Iterate through the vector and release each texture
    for (auto& texture : textures)
    {
        if (texture)
        {
            texture->Release();  // Release the texture
            texture = nullptr;   // Nullify the pointer to avoid dangling references
        }
    }

    // Clear the vector itself
    textures.clear();
    textures.shrink_to_fit();
}

int main(int, char**) {
    // Create application window
    //ImGui_ImplWin32_EnableDpiAwareness();
    WNDCLASSEXW wc = { sizeof(wc), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle(nullptr), nullptr, nullptr, nullptr, nullptr, L"ImGui Example", nullptr };
    ::RegisterClassExW(&wc);
    
    // 320 x 160 was substracted from 1920-1280 and 1080-800
    HWND hwnd = ::CreateWindowW(wc.lpszClassName, L"MReader", WS_OVERLAPPEDWINDOW, 320, 160, APP_WIDTH, APP_HEIGHT, nullptr, nullptr, wc.hInstance, nullptr);

    // Initialize Direct3D
    if (!CreateDeviceD3D(hwnd))
    {
        CleanupDeviceD3D();
        ::UnregisterClassW(wc.lpszClassName, wc.hInstance);
        return 1;
    }

    // Show the window
    ::ShowWindow(hwnd, SW_SHOWDEFAULT);
    ::UpdateWindow(hwnd);

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsLight();

    // Setup Platform/Renderer backends
    ImGui_ImplWin32_Init(hwnd);
    ImGui_ImplDX11_Init(g_pd3dDevice, g_pd3dDeviceContext);

    // Load Fonts
    // - If no fonts are loaded, dear imgui will use the default font. You can also load multiple fonts and use ImGui::PushFont()/PopFont() to select them.
    // - AddFontFromFileTTF() will return the ImFont* so you can store it if you need to select the font among multiple.
    // - If the file cannot be loaded, the function will return a nullptr. Please handle those errors in your application (e.g. use an assertion, or display an error and quit).
    // - The fonts will be rasterized at a given size (w/ oversampling) and stored into a texture when calling ImFontAtlas::Build()/GetTexDataAsXXXX(), which ImGui_ImplXXXX_NewFrame below will call.
    // - Use '#define IMGUI_ENABLE_FREETYPE' in your imconfig file to use Freetype for higher quality font rendering.
    // - Read 'docs/FONTS.md' for more instructions and details.
    // - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !
    //io.Fonts->AddFontDefault();
    //io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\segoeui.ttf", 18.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf", 16.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf", 16.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf", 15.0f);
    //ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f, nullptr, io.Fonts->GetGlyphRangesJapanese());
    //IM_ASSERT(font != nullptr);

    // Our state
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    std::string mangaBasePath = getBasePath();
    bool done = false;
    bool newlyRead = false;
    bool favorite = false;
    int selected = -1; // Initial selected value
    int lastSelected = -1;

    // Forward declaration, these variables will be populated after LoadTexturesFromFile() being called.
    int image_x = 0;
    int image_y = 0;
    std::vector<ID3D11ShaderResourceView*> textures;

    // Read all from the existing directory
    std::vector<std::string> manga_list = GetMangaList(mangaBasePath);
    
    // Main rendering done here
    while (!done)
    {
        // Poll and handle messages (inputs, window resize, etc.)
        // See the WndProc() function below for our to dispatch events to the Win32 backend.
        MSG msg;
        while (::PeekMessage(&msg, nullptr, 0U, 0U, PM_REMOVE))
        {
            ::TranslateMessage(&msg);
            ::DispatchMessage(&msg);
            if (msg.message == WM_QUIT)
                done = true;
        }
        if (done)
            break;

        // Handle window being minimized or screen locked
        if (g_SwapChainOccluded && g_pSwapChain->Present(0, DXGI_PRESENT_TEST) == DXGI_STATUS_OCCLUDED)
        {
            ::Sleep(10);
            continue;
        }
        g_SwapChainOccluded = false;

        // Handle window resize (we don't resize directly in the WM_SIZE handler)
        if (g_ResizeWidth != 0 && g_ResizeHeight != 0)
        {
            CleanupRenderTarget();
            g_pSwapChain->ResizeBuffers(0, g_ResizeWidth, g_ResizeHeight, DXGI_FORMAT_UNKNOWN, 0);
            g_ResizeWidth = g_ResizeHeight = 0;
            CreateRenderTarget();
        }

        // Start the Dear ImGui frame
        ImGui_ImplDX11_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();
        
        ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImVec2 navSize = ImVec2(viewport->Size.x / 3, viewport->Size.y);
        ImVec2 mainSize = ImVec2(viewport->Size.x / 3 * 2, viewport->Size.y);
        
        createNavWindow(navSize, ImVec2(0,0), manga_list.size(), manga_list, selected);
        // Only load textures if the selection has changed
        if ((selected != lastSelected)) {
            // Clear the vector and free the memory
            ReleaseTextures(textures);
            
            LoadTexturesFromDirectory(mangaBasePath + manga_list[selected], textures, &image_x, &image_y);
            image_x = IMAGE_WIDTH;
            image_y = IMAGE_HEIGHT;

            lastSelected = selected;  // Update the last selected index
        }
        createMainWindow(mainSize, ImVec2(navSize.x, 0), manga_list, textures, image_x, image_y, selected, &newlyRead, &favorite);

        // Rendering
        ImGui::Render();
        const float clear_color_with_alpha[4] = { clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w };
        g_pd3dDeviceContext->OMSetRenderTargets(1, &g_mainRenderTargetView, nullptr);
        g_pd3dDeviceContext->ClearRenderTargetView(g_mainRenderTargetView, clear_color_with_alpha);
        ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

        // Present
        HRESULT hr = g_pSwapChain->Present(1, 0);   // Present with vsync
        //HRESULT hr = g_pSwapChain->Present(0, 0); // Present without vsync
        g_SwapChainOccluded = (hr == DXGI_STATUS_OCCLUDED);
    }

    // Cleanup
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

    CleanupDeviceD3D();
    ::DestroyWindow(hwnd);
    ::UnregisterClassW(wc.lpszClassName, wc.hInstance);

    return 0;
}

// Helper functions

bool CreateDeviceD3D(HWND hWnd)
{
    // Setup swap chain
    DXGI_SWAP_CHAIN_DESC sd;
    ZeroMemory(&sd, sizeof(sd));
    sd.BufferCount = 2;
    sd.BufferDesc.Width = 0;
    sd.BufferDesc.Height = 0;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferDesc.RefreshRate.Numerator = 60;
    sd.BufferDesc.RefreshRate.Denominator = 1;
    sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow = hWnd;
    sd.SampleDesc.Count = 1;
    sd.SampleDesc.Quality = 0;
    sd.Windowed = TRUE;
    sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

    UINT createDeviceFlags = 0;
    //createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
    D3D_FEATURE_LEVEL featureLevel;
    const D3D_FEATURE_LEVEL featureLevelArray[2] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0, };
    HRESULT res = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, createDeviceFlags, featureLevelArray, 2, D3D11_SDK_VERSION, &sd, &g_pSwapChain, &g_pd3dDevice, &featureLevel, &g_pd3dDeviceContext);
    if (res == DXGI_ERROR_UNSUPPORTED) // Try high-performance WARP software driver if hardware is not available.
        res = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_WARP, nullptr, createDeviceFlags, featureLevelArray, 2, D3D11_SDK_VERSION, &sd, &g_pSwapChain, &g_pd3dDevice, &featureLevel, &g_pd3dDeviceContext);
    if (res != S_OK)
        return false;

    CreateRenderTarget();
    return true;
}

void CleanupDeviceD3D()
{
    CleanupRenderTarget();
    if (g_pSwapChain) { g_pSwapChain->Release(); g_pSwapChain = nullptr; }
    if (g_pd3dDeviceContext) { g_pd3dDeviceContext->Release(); g_pd3dDeviceContext = nullptr; }
    if (g_pd3dDevice) { g_pd3dDevice->Release(); g_pd3dDevice = nullptr; }
}

void CreateRenderTarget()
{
    ID3D11Texture2D* pBackBuffer;
    g_pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
    g_pd3dDevice->CreateRenderTargetView(pBackBuffer, nullptr, &g_mainRenderTargetView);
    pBackBuffer->Release();
}

void CleanupRenderTarget()
{
    if (g_mainRenderTargetView) { g_mainRenderTargetView->Release(); g_mainRenderTargetView = nullptr; }
}

// Forward declare message handler from imgui_impl_win32.cpp
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

// Win32 message handler
// You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
// - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application, or clear/overwrite your copy of the mouse data.
// - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application, or clear/overwrite your copy of the keyboard data.
// Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
        return true;

    switch (msg)
    {
    case WM_SIZE:
        if (wParam == SIZE_MINIMIZED)
            return 0;
        g_ResizeWidth = (UINT)LOWORD(lParam); // Queue resize
        g_ResizeHeight = (UINT)HIWORD(lParam);
        return 0;
    case WM_SYSCOMMAND:
        if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
            return 0;
        break;
    case WM_DESTROY:
        ::PostQuitMessage(0);
        return 0;
    }
    return ::DefWindowProcW(hWnd, msg, wParam, lParam);
}
