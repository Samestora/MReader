#pragma once
#include "imgui.h"

#ifndef MREADER_MREADER_INCLUDE_SETTINGS_THEMES_H_
#define MREADER_MREADER_INCLUDE_SETTINGS_THEMES_H_
namespace MRTheme {
	bool showStyleSelector(const char* label);
	
	void globalStyle();

	void setGlassTheme();
	void setModernColors();
	void setMaterialYouColors();
	void setFluentUITheme();
	void setFluentUILightTheme();
}

#endif // MREADER_MREADER_INCLUDE_SETTINGS_THEMES_H_