// MReader main file
// About vendor (additional) library
// Get ImGui v1.91.6 here : https://github.com/ocornut/imgui/
// Get libwebp v1.50-windows-x64 here : https://developers.google.com/speed/webp/download
//                          -x86 you have to compile it first to x86 using nmake
// The D3D11.lib should be built-in when using Visual Studio 2022 for the package
// Nlohmann's JSON
// OpenDyslexic Font

// Visual Studio Warnings
#ifdef _MSC_VER
#pragma warning (disable: 4127)     // condition expression is constant
#pragma warning (disable: 4996)     // 'This function or variable may be unsafe': strcpy, strdup, sprintf, vsnprintf, sscanf, fopen
#pragma warning (disable: 26451)    // [Static Analyzer] Arithmetic overflow : Using operator 'xxx' on a 4 byte value and then casting the result to an 8 byte value. Cast the value to the wider type before calling operator 'xxx' to avoid overflow(io.2).
#pragma warning (disable: 6387)     // 'pBackBuffer' could be '0':  this does not adhere to the specification for the function 'ID3D11Device::CreateRenderTargetView'. See line 0 for an earlier location where this can occur
#endif

#include <dwmapi.h>
#include <chrono>
#include <thread>

#ifndef DWMWA_USE_IMMERSIVE_DARK_MODE
#define DWMWA_USE_IMMERSIVE_DARK_MODE 20
#endif

// MReader defined headers everything is in the include dirs!
#include "settings/path.h"
#include "settings/themes.h"
#include "fonts/fontloader.h"
#include "graphics/gadgets.h"
#include "resource/resource.h"
#include "application/render.h"
#include "application/resolution.h"

// MReader related data

bool done = false;
MRWindow::MainWindowState state;
std::string fontBasePath;
std::string mangaBasePath;
std::vector<MRImage::Image> image_list;
std::vector<std::string> manga_list;
std::vector<std::string> font_list;
std::vector<ID3D11ShaderResourceView*> textures;
ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

int main(int, char**) {

    // Starting size for the app width, height
    MRApplication::Resolution AppRes = MRApplication::getInitialApplicationSize();
    // Starting position for the app x,y wise
    MRApplication::Position AppPos = MRApplication::getInitialApplicationPosition_Centered(AppRes);

    ImGui_ImplWin32_EnableDpiAwareness();

    WNDCLASSEXW wc = {
        sizeof(wc), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle(nullptr), nullptr, nullptr, nullptr, nullptr, L"MReader", nullptr
    };

    wc.hIcon = LoadIcon(wc.hInstance, MAKEINTRESOURCE(MREADER_ICON));
    wc.hbrBackground = CreateSolidBrush(RGB(26, 27, 28));

    ::RegisterClassExW(&wc); 
    
    HWND hwnd = ::CreateWindowW(wc.lpszClassName, L"MReader",  WS_OVERLAPPEDWINDOW, AppPos.x, AppPos.y, AppRes.width, AppRes.height, nullptr, nullptr, wc.hInstance, nullptr);
    BOOL value = TRUE;
    HRESULT hr = ::DwmSetWindowAttribute(hwnd, DWMWA_USE_IMMERSIVE_DARK_MODE, &value, sizeof(value));
    
    if (!MRGraphics::CreateDeviceD3D(hwnd))
    {
        MRGraphics::CleanupDeviceD3D();
        ::UnregisterClassW(wc.lpszClassName, wc.hInstance);
        return 1;
    }

    ::ShowWindow(hwnd, SW_MAXIMIZE);
    ::UpdateWindow(hwnd);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Platform/Renderer backends
    ImGui_ImplWin32_Init(hwnd);
    ImGui_ImplDX11_Init(MRGraphics::gadget.g_pd3dDevice, MRGraphics::gadget.g_pd3dDeviceContext);

    // Init data
    mangaBasePath = MRPath::getMangaPath();
    fontBasePath = MRPath::getFontPath();

    // Read all from the existing directory
    manga_list = MRImage::LoadMangaList(mangaBasePath);
    font_list = MRFont::getFontList(fontBasePath);

    // Setup style
    // Theme on start
    MRTheme::setFluentUITheme();
    MRTheme::globalStyle();

    // Font on start
    MRFont::setDefaultFont();
    for (auto& font : font_list) {
        MRFont::setFontbyFilepath(font, 20.0);
    }

    const int targetFPS = 60;
    const float targetFrameTime = 1.0f / targetFPS;

    using clock = std::chrono::high_resolution_clock;
    auto lastFrameTime = clock::now();
    bool renderNeeded = false;
    bool inputActive = false;
    bool stateChanged = false;
    bool startup = false;

    // Main rendering done here
    while (!done)
    {
        auto currentFrameTime = clock::now();
        std::chrono::duration<float> elapsed = currentFrameTime - lastFrameTime;
        if (elapsed.count() >= targetFrameTime) {
            lastFrameTime = currentFrameTime;

            // Poll and handle messages (inputs, window resize, etc.)
            // See the WndProc() function below for our to dispatch events to the Win32 backend.
            MSG msg;
            while (::PeekMessage(&msg, nullptr, 0U, 0U, PM_REMOVE))
            {
                ::TranslateMessage(&msg);
                ::DispatchMessage(&msg);
                if (msg.message == WM_QUIT) {
                    done = true;
                }
                
                if (msg.message == WM_MOUSEMOVE  || msg.message == WM_LBUTTONDOWN || msg.message == WM_KEYDOWN) {
                    inputActive = true;
                }
                else {
                    inputActive = false;
                }
            }

            if (done)
                break;

            // Handle window being minimized or screen locked
            if (MRGraphics::gadget.g_SwapChainOccluded && MRGraphics::gadget.g_pSwapChain->Present(0, DXGI_PRESENT_TEST) == DXGI_STATUS_OCCLUDED)
            {
                ::Sleep(10);
                continue;
            }
            MRGraphics::gadget.g_SwapChainOccluded = false;

            // Handle window resize (we don't resize directly in the WM_SIZE handler)
            if (MRGraphics::gadget.g_ResizeWidth != 0 && MRGraphics::gadget.g_ResizeHeight != 0) {
                MRGraphics::CleanupRenderTarget();
                MRGraphics::gadget.g_pSwapChain->ResizeBuffers(0, MRGraphics::gadget.g_ResizeWidth, MRGraphics::gadget.g_ResizeHeight, DXGI_FORMAT_UNKNOWN, 0);
                MRGraphics::gadget.g_ResizeWidth = MRGraphics::gadget.g_ResizeHeight = 0;
                MRGraphics::CreateRenderTarget();
            }
            if (ImGui::IsAnyItemActive() || ImGui::IsAnyItemHovered()) {
                stateChanged = true;
            }
            else {
                stateChanged = false;
            }

            renderNeeded = stateChanged || inputActive;
            
            if (!renderNeeded && startup) {
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
                continue;
            }
            startup = true;
            // Start the Dear ImGui frame
            ImGui_ImplDX11_NewFrame();
            ImGui_ImplWin32_NewFrame();
            ImGui::NewFrame();

            MRApplication::Render(mangaBasePath, manga_list, image_list, state, textures, clear_color);

            // Rendering
            ImGui::Render();
            const float clear_color_with_alpha[4] = {
                clear_color.x * clear_color.w,
                clear_color.y * clear_color.w,
                clear_color.z * clear_color.w,
                clear_color.w
            };
            MRGraphics::gadget.g_pd3dDeviceContext->OMSetRenderTargets(1, &MRGraphics::gadget.g_mainRenderTargetView, nullptr);
            MRGraphics::gadget.g_pd3dDeviceContext->ClearRenderTargetView(MRGraphics::gadget.g_mainRenderTargetView, clear_color_with_alpha);
            ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

            // Present the frame
            HRESULT hr = MRGraphics::gadget.g_pSwapChain->Present(1, 0); // Present with vsync
            MRGraphics::gadget.g_SwapChainOccluded = (hr == DXGI_STATUS_OCCLUDED);
        }
        else {
            // Sleep to prevent spinning
            std::this_thread::sleep_for(
                std::chrono::milliseconds(1)
            );
        }
    }

    // Cleanup
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

    MRGraphics::CleanupDeviceD3D();
    ::DestroyWindow(hwnd);
    ::UnregisterClassW(wc.lpszClassName, wc.hInstance);

    return 0;
}


// Helper functions

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

    switch (msg) {
    case WM_SIZE:
        if (wParam == SIZE_MINIMIZED)
            return 0;
        MRGraphics::gadget.g_ResizeWidth = (UINT)LOWORD(lParam); // Queue resize
        MRGraphics::gadget.g_ResizeHeight = (UINT)HIWORD(lParam);
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
