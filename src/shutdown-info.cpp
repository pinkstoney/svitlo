#include "../include/shutdown-info.h"

size_t ShutdownInfo::m_writeCallback(void* contents, size_t size, size_t nmemb, void* userp)
{
    size_t totalSize = size * nmemb;
    auto s = static_cast<std::string*>(userp);
    s->append(static_cast<char*>(contents), totalSize);
    return totalSize;
}

bool ShutdownInfo::s_curlInitialized = false;

curl_slist* ShutdownInfo::s_headers = nullptr;

ShutdownInfo::ShutdownInfo()
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

ShutdownInfo::~ShutdownInfo()
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

void ShutdownInfo::addHeader(const std::string& header)
{
    s_headers = curl_slist_append(s_headers, header.c_str());
}

void ShutdownInfo::setPostData(const std::string& choice, const std::string& info)
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

std::string ShutdownInfo::send() {
    CURL *curl;
    CURLcode res;

    curl = curl_easy_init();
    if (!curl) {
        throw std::runtime_error("Failed to init curl");
    }

    curl_easy_setopt(curl, CURLOPT_URL, m_url.c_str());
    curl_easy_setopt(curl, CURLOPT_POST, 1L);
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, s_headers);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, m_postData.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, m_writeCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &m_readBuffer);
    curl_easy_setopt(curl, CURLOPT_SSLVERSION, CURL_SSLVERSION_TLSv1_2);

    res = curl_easy_perform(curl);

    if (res != CURLE_OK) {
        std::string error_msg = "curl_easy_perform() failed: ";
        error_msg += curl_easy_strerror(res);
        curl_easy_cleanup(curl);
        throw std::runtime_error(error_msg);
    }
    curl_easy_cleanup(curl);

    return m_readBuffer;
}
void ShutdownInfo::processRawElectricityData(const std::string& rawData) const
{
    std::cout << "raw data: " << rawData << std::endl;
}

void ShutdownInfo::processHour(const nlohmann::json& hourData, bool isToday) {
    int electricity = hourData["electricity"];
    int hour = std::stoi(hourData["hour"].get<std::string>());
    int previousHour = (hour == 1) ? 24 : hour - 1;

    auto &willBeElectricity = isToday ? m_willBeElectricityToday : m_willBeElectricityTomorrow;
    auto &mightBeElectricity = isToday ? m_mightBeElectricityToday : m_mightBeElectricityTomorrow;
    auto &wontBeElectricity = isToday ? m_wontBeElectricityToday : m_wontBeElectricityTomorrow;

    if (electricity == 1)
        wontBeElectricity.emplace_back(previousHour, hour);
    else if (electricity == 2)
        mightBeElectricity.emplace_back(previousHour, hour);
    else
        willBeElectricity.emplace_back(previousHour, hour);
}

void ShutdownInfo::formatElectricityData(const std::string& rawData)
{
    if (rawData.empty())
        throw std::runtime_error("Received empty data. Cannot parse as JSON.");
    nlohmann::json data;
    try
    {
        data = nlohmann::json::parse(rawData);
    }
    catch (nlohmann::json::parse_error& e)
    {
        throw std::runtime_error("Invalid JSON data. Please check the correctness of your input.");
    }

    if (!data.contains("graphs"))
        throw std::runtime_error("The data does not contain information for today/tomorrow.");

    m_queue = data["current"]["queue"];
    m_subqueue = data["current"]["subqueue"];

    m_willBeElectricityToday.clear();
    m_mightBeElectricityToday.clear();
    m_wontBeElectricityToday.clear();

    m_willBeElectricityTomorrow.clear();
    m_mightBeElectricityTomorrow.clear();
    m_wontBeElectricityTomorrow.clear();

    auto todayHoursList = data["graphs"]["today"]["hoursList"];
    for (const auto& hourData : todayHoursList)
        processHour(hourData, true);

    if (data["graphs"].contains("tomorrow"))
    {
        auto tomorrowHoursList = data["graphs"]["tomorrow"]["hoursList"];
        for (const auto& hourData : tomorrowHoursList)
            processHour(hourData, false);
    }
}

const std::vector<std::pair<int, int>>& ShutdownInfo::getWillBeElectricityToday() const
{
    return m_willBeElectricityToday;
}

const std::vector<std::pair<int, int>>& ShutdownInfo::getMightBeElectricityToday() const
{
    return m_mightBeElectricityToday;
}

const std::vector<std::pair<int, int>>& ShutdownInfo::getWontBeElectricityToday() const
{
    return m_wontBeElectricityToday;
}

const std::vector<std::pair<int, int>>& ShutdownInfo::getWillBeElectricityTomorrow() const
{
    return m_willBeElectricityTomorrow;
}

const std::vector<std::pair<int, int>>& ShutdownInfo::getMightBeElectricityTomorrow() const
{
    return m_mightBeElectricityTomorrow;
}

const std::vector<std::pair<int, int>>& ShutdownInfo::getWontBeElectricityTomorrow() const
{
    return m_wontBeElectricityTomorrow;
}

void ShutdownInfo::addWillBeElectricityToday(int hour)
{
    m_willBeElectricityToday.emplace_back(hour, hour + 1);
}

void ShutdownInfo::addMightBeElectricityToday(int hour)
{
    m_mightBeElectricityToday.emplace_back(hour, hour + 1);
}

void ShutdownInfo::addWontBeElectricityToday(int hour)
{
    m_wontBeElectricityToday.emplace_back(hour, hour + 1);
}

bool ShutdownInfo::isInternetConnected() const
{
    CURL* curl;
    CURLcode res;
    curl = curl_easy_init();
    if (curl)
    {
        curl_easy_setopt(curl, CURLOPT_URL, "http://www.google.com");
                curl_easy_setopt(curl, CURLOPT_NOBODY, 1);
        curl_easy_setopt(curl, CURLOPT_FAILONERROR, 1);
        curl_easy_setopt(curl, CURLOPT_TIMEOUT, 10L);
        res = curl_easy_perform(curl);
        curl_easy_cleanup(curl);
        return (res == CURLE_OK);
    }
    return false;
}

void ShutdownInfo::setQueue(int queue)
{
    m_queue = queue;
}

void ShutdownInfo::setSubqueue(int subqueue)
{
    m_subqueue = subqueue;
}
