#pragma once
#include <d3d11.h>
#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_dx11.h>

#ifndef MREADER_MREADER_INCLUDE_GRAPHICS_GADGETS_H_
#define MREADER_MREADER_INCLUDE_GRAPHICS_GADGETS_H_

namespace MRGraphics {
	struct Gadget {
		ID3D11Device* g_pd3dDevice = nullptr; // initialize with nullptr
		ID3D11DeviceContext* g_pd3dDeviceContext = nullptr; // initialize with nullptr
		IDXGISwapChain* g_pSwapChain = nullptr; // initialize with nullptr
		ID3D11RenderTargetView* g_mainRenderTargetView = nullptr; // initialize with nullptr
		bool g_SwapChainOccluded = 1; // initialize with 1 or 0, 1 for VSync
		UINT g_ResizeWidth = 0; // initialize with 0
		UINT g_ResizeHeight = 0; // initialize with 0
	};

	extern Gadget gadget;

	bool CreateDeviceD3D(HWND hWnd);
	void CleanupDeviceD3D();
	void CreateRenderTarget();
	void CleanupRenderTarget();
}

#endif // MREADER_MREADER_INCLUDE_GRAPHICS_GADGETS_H_