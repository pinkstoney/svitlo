#pragma once

#include <algorithm>
#include <string>
#include <chrono>
#include <sstream>
#include <iomanip>

class Utility
{
public:
    static std::string concatenteInfo(const std::string &accumulated, const std::pair<std::string, std::string> &userInfo);
    static float lerp(float a, float b, float f);
    static std::string getCurrentTime();
};
