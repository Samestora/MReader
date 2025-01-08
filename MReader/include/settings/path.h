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

namespace MRPath {
    std::string getMangaPath();
    std::string getFontPath();
}