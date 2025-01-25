#pragma once

#ifndef MREADER_MREADER_INCLUDE_APPLICATION_RENDER_H_
#define MREADER_MREADER_INCLUDE_APPLICATION_RENDER_H_

#include "window/window.h"
#include "settings/themes.h"
#include "images/mangaloader.h"
#include "graphics/release.h"
#include "window/mainwindowstate.h"
#include "images/imageformat.h"
#include "graphics/gadgets.h"

#include <algorithm>
#include <fstream>
#include <cstring>
#include <cstdio>

namespace MRApplication {
    void Render(
        const std::string& mangaBasePath,
        const std::vector<std::string>& manga_list,
        std::vector<MRImage::Image>& image,
        MRWindow::MainWindowState& state,
        std::vector<ID3D11ShaderResourceView*>& textures,
        const ImVec4& clear_color
    );
}

#endif // MREADER_MREADER_INCLUDE_APPLICATION_RENDER_H_