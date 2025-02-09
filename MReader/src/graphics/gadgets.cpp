#include "graphics/gadgets.h"
#include <imgui_impl_win32.h>

namespace MRGraphics{
    Gadget gadget;

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
        HRESULT res = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, createDeviceFlags, featureLevelArray, 2, D3D11_SDK_VERSION, &sd, &gadget.g_pSwapChain, &gadget.g_pd3dDevice, &featureLevel, &gadget.g_pd3dDeviceContext);
        if (res == DXGI_ERROR_UNSUPPORTED) // Try high-performance WARP software driver if hardware is not available.
            res = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_WARP, nullptr, createDeviceFlags, featureLevelArray, 2, D3D11_SDK_VERSION, &sd, &gadget.g_pSwapChain, &gadget.g_pd3dDevice, &featureLevel, &gadget.g_pd3dDeviceContext);
        if (res != S_OK)
            return false;

        CreateRenderTarget();
        return true;
    }

    void CleanupDeviceD3D()
    {
        CleanupRenderTarget();
        if (gadget.g_pSwapChain) { gadget.g_pSwapChain->Release(); gadget.g_pSwapChain = nullptr; }
        if (gadget.g_pd3dDeviceContext) { gadget.g_pd3dDeviceContext->Release(); gadget.g_pd3dDeviceContext = nullptr; }
        if (gadget.g_pd3dDevice) { gadget.g_pd3dDevice->Release(); gadget.g_pd3dDevice = nullptr; }
    }

    void CreateRenderTarget()
    {
        ID3D11Texture2D* pBackBuffer;
        gadget.g_pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
        gadget.g_pd3dDevice->CreateRenderTargetView(pBackBuffer, nullptr, &gadget.g_mainRenderTargetView);
        pBackBuffer->Release();
    }

    void CleanupRenderTarget()
    {
        if (gadget.g_mainRenderTargetView) { gadget.g_mainRenderTargetView->Release(); gadget.g_mainRenderTargetView = nullptr; }
    }
}
