#pragma once
#include <windows.h>
#include <iostream>

#ifndef MREADER_MREADER_INCLUDE_APPLICATION_RESOLUTION_H_
#define MREADER_MREADER_INCLUDE_APPLICATION_RESOLUTION_H_

namespace MRApplication {
	struct Resolution {
		int width;
		int height;
	};

	struct Position {
		int x;
		int y;
	};

	Resolution getBestNativeResolution();
	Resolution getInitialApplicationSize();
	Position getInitialApplicationPosition_Centered(Resolution AppRes);
}

#endif // MREADER_MREADER_INCLUDE_APPLICATION_RESOLUTION_H_