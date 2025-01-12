#pragma once
#include "imgui.h"

namespace MRTheme {
	bool showStyleSelector(const char* label);
	
	void globalStyle();

	void setGlassTheme();
	void setModernColors();
	void setMaterialYouColors();
	void setDarkThemeColors();
	void setFluentUITheme();
	void setFluentUILightTheme();
}