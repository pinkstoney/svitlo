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
    std::time_t now_c = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&now_c), "%F %T");
    return ss.str();
}
