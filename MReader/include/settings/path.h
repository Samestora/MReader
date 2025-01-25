#pragma once
#include <iostream>
#include <filesystem>
#include <regex>
#include <algorithm>
#include <fstream>
#include <cstring>
#include <vector>
#include <cstdio>
#include <tchar.h>
#include <string>
#include <nlohmann/json.hpp>

#ifndef MREADER_MREADER_INCLUDE_SETTINGS_PATH_H_
#define MREADER_MREADER_INCLUDE_SETTINGS_PATH_H_

namespace MRPath {
    std::string getMangaPath();
    std::string getFontPath();
}

#endif // MREADER_MREADER_INCLUDE_SETTINGS_PATH_H_