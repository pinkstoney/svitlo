#include "../include/utility.h"

#include <chrono>
#include <sstream>
#include <iomanip>

std::string Utility::concatenteInfo(const std::string &accumulated, const std::pair<std::string, std::string> &userInfo) 
{
    std::string separator = accumulated.empty() ? "" : ";";
    return accumulated + separator + userInfo.first;
}

float Utility::lerp(float a, float b, float f)
{
    return a + f * (b - a);
}

std::string Utility::getCurrentTime() 
{
    auto now = std::chrono::system_clock::now();
    return getFormattedDate(now);
}

std::string Utility::getTodayDate()
{
    auto now = std::chrono::system_clock::now();
    return getFormattedDate(now);
}

std::string Utility::getTomorrowDate()
{
    auto now = std::chrono::system_clock::now();
    auto tomorrow = now + std::chrono::hours(24);
    return getFormattedDate(tomorrow);
}

std::string Utility::getFormattedDate(const std::chrono::system_clock::time_point& timePoint)
{
    std::time_t time = std::chrono::system_clock::to_time_t(timePoint);
    std::tm tm = {};
    localtime_r(&time, &tm);
    std::stringstream ss;
    ss << std::put_time(&tm, "%F");  // format as YYYY-MM-DD
    return ss.str();
}
