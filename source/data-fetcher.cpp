#include <iostream>
#include <stdexcept>
#include <svitlo/data-fetcher.h>

bool DataFetcher::s_curlInitialized = false;
curl_slist* DataFetcher::s_headers = nullptr;

size_t DataFetcher::writeCallback(void* contents, size_t size, size_t nmemb, void* userp)
{
    auto totalSize = size * nmemb;
    static_cast<std::string*>(userp)->append(static_cast<char*>(contents), totalSize);
    return totalSize;
}

size_t DataFetcher::headerCallback(char* buffer, size_t size, size_t nitems, void* userdata)
{
    auto totalSize = size * nitems;
    static_cast<std::string*>(userdata)->append(buffer, totalSize);
    return totalSize;
}

DataFetcher::DataFetcher()
    : m_url("https://svitlo.oe.if.ua/GAVTurnOff/GavGroupByAccountNumber")
{
    if (!s_curlInitialized)
    {
        curl_global_init(CURL_GLOBAL_DEFAULT);
        s_curlInitialized = true;
    }
    addHeader("Accept: */*");
    addHeader("Content-Type: application/x-www-form-urlencoded; charset=UTF-8");
}

DataFetcher::~DataFetcher()
{
    if (s_headers)
    {
        curl_slist_free_all(s_headers);
        s_headers = nullptr;
    }
    if (s_curlInitialized)
    {
        curl_global_cleanup();
        s_curlInitialized = false;
    }
}

void DataFetcher::addHeader(const std::string& header)
{
    s_headers = curl_slist_append(s_headers, header.c_str());
}

void DataFetcher::setPostData(const std::string& choice, const std::string& info)
{
    if (choice == "accountNumber")
    {
        m_postData = "accountNumber=" + info + "&userSearchChoice=pob&address=";
    }
    else if (choice == "address")
    {
        m_postData = "accountNumber=&userSearchChoice=pob&address=" + info;
    }
    else
    {
        throw std::invalid_argument("Invalid choice. Please choose either 'accountNumber' or 'address'.");
    }
}

std::string DataFetcher::send()
{
    CURL* curl = curl_easy_init();
    if (!curl)
        throw std::runtime_error("Failed to init curl");

    curl_easy_setopt(curl, CURLOPT_URL, m_url.c_str());
    curl_easy_setopt(curl, CURLOPT_POST, 1L);
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, s_headers);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, m_postData.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &m_readBuffer);
    curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, headerCallback);
    curl_easy_setopt(curl, CURLOPT_HEADERDATA, &m_headerBuffer);
    curl_easy_setopt(curl, CURLOPT_SSLVERSION, CURL_SSLVERSION_TLSv1_2);

    m_readBuffer.clear();
    m_headerBuffer.clear();

    CURLcode res = curl_easy_perform(curl);
    if (res != CURLE_OK)
    {
        std::string error_msg = "curl_easy_perform() failed: ";
        error_msg += curl_easy_strerror(res);
        curl_easy_cleanup(curl);
        throw std::runtime_error(error_msg);
    }

    std::cout << "Request Headers:\n" << m_headerBuffer << std::endl;
    std::cout << "Response Data:\n" << m_readBuffer << std::endl;

    curl_easy_cleanup(curl);
    return m_readBuffer;
}
