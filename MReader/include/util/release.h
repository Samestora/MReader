#pragma once
#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"
#include <d3d11.h>
#include <tchar.h>
#include <vector>
#include <string>

namespace MRUtil {
	void ReleaseTextures(std::vector<ID3D11ShaderResourceView*>& textures);
}