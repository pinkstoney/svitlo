#pragma once

#include <string>

class Utility
{
public:
    static std::string concatenteInfo(const std::string &accumulated, const std::pair<std::string, std::string> &userInfo);
    static float lerp(float a, float b, float f);
    static std::string getCurrentTime();
    static std::string getTodayDate();
    static std::string getTomorrowDate();
    static std::string getFormattedDate(const std::chrono::system_clock::time_point& timePoint);
};
