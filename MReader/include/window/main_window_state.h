#pragma once

#include <cstddef>
#include <cstring>

namespace MRWindow {
	struct MainWindowState {
		bool reading;
		bool favorite;
		int current_page;

		int selected;
		int last_selected;
		bool isLoaded;

		bool next_page;
		bool previous_page;
	};
}