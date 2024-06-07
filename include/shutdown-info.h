#pragma once

#include <string>
#include <iostream>
#include <algorithm>
#include <sstream>

#include <curl/curl.h>
#include "nlohmann/json.hpp"

class ShutdownInfo
{
public:
    ShutdownInfo();
    ~ShutdownInfo();

public:
    void addHeader(const std::string& header);
    void setPostData(const std::string&);
    std::string send();

    void processRawElectricityData(const std::string& rawData) const;
    void processHour(const nlohmann::json& hourData, bool isToday);
    void formatElectricityData(const std::string& rawData);

public:
    const std::vector<std::pair<int, int>>& getWillBeElectricityToday() const;
    const std::vector<std::pair<int, int>>& getMightBeElectricityToday() const;
    const std::vector<std::pair<int, int>>& getWontBeElectricityToday() const;

    const std::vector<std::pair<int, int>>& getWillBeElectricityTomorrow() const;
    const std::vector<std::pair<int, int>>& getMightBeElectricityTomorrow() const;
    const std::vector<std::pair<int, int>>& getWontBeElectricityTomorrow() const;

    int getQueue() const { return m_queue; }
    int getSubqueue() const { return m_subqueue; }

private:
    static bool s_curlInitialized;
    static curl_slist *s_headers;

    std::string m_url;
    struct curl_slist *m_headers = nullptr;
    std::string m_postData;
    std::string m_readBuffer;

    static size_t m_writeCallback(void* contents, size_t size, size_t nmemb, void* userp);

private:
    int m_queue;
    int m_subqueue;

    std::vector<std::pair<int, int>> m_willBeElectricityToday;
    std::vector<std::pair<int, int>> m_mightBeElectricityToday;
    std::vector<std::pair<int, int>> m_wontBeElectricityToday;

    std::vector<std::pair<int, int>> m_willBeElectricityTomorrow;
    std::vector<std::pair<int, int>> m_mightBeElectricityTomorrow;
    std::vector<std::pair<int, int>> m_wontBeElectricityTomorrow;
};