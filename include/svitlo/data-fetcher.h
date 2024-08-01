#pragma once

#include <curl/curl.h>
#include <string>

class DataFetcher
{
public:
    DataFetcher();
    ~DataFetcher();

public:
    void addHeader(const std::string& header);
    void setPostData(const std::string& choice, const std::string& info);

    std::string send();

private:
    static size_t writeCallback(void* contents, size_t size, size_t nmemb, void* userp);
    static size_t headerCallback(char* buffer, size_t size, size_t nitems, void* userdata);

    static bool s_curlInitialized;
    static curl_slist* s_headers;

    const std::string m_url;
    std::string m_postData;
    std::string m_readBuffer;
    std::string m_headerBuffer;
};
