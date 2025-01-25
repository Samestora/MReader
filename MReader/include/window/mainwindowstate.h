#pragma once

#include <cstddef>
#include <cstring>
#include "graphics/textureloader.h"

#ifndef MREADER_MREADER_INCLUDE_WINDOW_MAINWINDOWSTATE_H_
#define MREADER_MREADER_INCLUDE_WINDOW_MAINWINDOWSTATE_H_

namespace MRWindow {
	struct MainWindowState {
		bool reading = false;
		bool favorite = false;

		int current_page = 0;
		int total_page = 0;

		int selected = -1;
		int last_selected = -1;

		bool next_page = false;
		bool previous_page = false;

		MRGraphics::LOAD_OPTION current_mode = MRGraphics::LOAD_NONE;
	};
}

#endif // MREADER_MREADER_INCLUDE_WINDOW_MAINWINDOWSTATE_H_