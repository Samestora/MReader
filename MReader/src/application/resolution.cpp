#include "application/resolution.h"

namespace MRApplication {
    static MRApplication::Resolution bestNativeResolution = getBestNativeResolution();

	MRApplication::Resolution getBestNativeResolution() {
        UINT32 PathArraySize = 0;
        UINT32 ModeArraySize = 0;
        DISPLAYCONFIG_PATH_INFO* PathArray;
        DISPLAYCONFIG_MODE_INFO* ModeArray;
        DISPLAYCONFIG_TOPOLOGY_ID CurrentTopology;
        //Directly query the resolution of the graphics card and get the physical resolution all the time.
        GetDisplayConfigBufferSizes(QDC_ALL_PATHS, &PathArraySize, &ModeArraySize);
        PathArray = (DISPLAYCONFIG_PATH_INFO*)malloc(PathArraySize * sizeof(DISPLAYCONFIG_PATH_INFO));
        memset(PathArray, 0, PathArraySize * sizeof(DISPLAYCONFIG_PATH_INFO));
        ModeArray = (DISPLAYCONFIG_MODE_INFO*)malloc(ModeArraySize * sizeof(DISPLAYCONFIG_MODE_INFO));
        memset(ModeArray, 0, ModeArraySize * sizeof(DISPLAYCONFIG_MODE_INFO));
        LONG ret = QueryDisplayConfig(QDC_DATABASE_CURRENT, &PathArraySize, PathArray, &ModeArraySize, ModeArray, &CurrentTopology);

        int w_DisplayConfigScreen = ModeArray->targetMode.targetVideoSignalInfo.activeSize.cx;
        int h_DisplayConfigScreen = ModeArray->targetMode.targetVideoSignalInfo.activeSize.cy;

        return MRApplication::Resolution{ w_DisplayConfigScreen, h_DisplayConfigScreen };
	}

    MRApplication::Resolution getInitialApplicationSize() {
        // This ratio was developped and tested to looks good for 1920x1080
        // and want to be preserved to all resolution
        float width_ratio = (float)1280 / 1920;
        float height_ratio = (float)800 / 1080;

        return MRApplication::Resolution{ (int)(bestNativeResolution.width * width_ratio), (int)(bestNativeResolution.height * height_ratio) };
    }

    MRApplication::Position getInitialApplicationPosition_Centered(MRApplication::Resolution AppRes) {
        return MRApplication::Position {(bestNativeResolution.width - AppRes.width) / 2, (bestNativeResolution.height - AppRes.height) /2};
    }
}