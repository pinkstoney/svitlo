#include "../include/state-manager.h"

std::ostream& operator<<(std::ostream& os, const AppState& state)
{
    switch (state)
    {
        case AppState::INPUT:
            os << "INPUT";
            break;
        case AppState::DISPLAYING_RESULTS:
            os << "DISPLAYING RESULTS";
            break;
        default:
            os << "UNKNOWN";
            break;
    }
    return os;
}

StateManager::StateManager() 
    : m_currentState(AppState::INPUT),
      m_isAddressEntered(false),
      m_isDataProcessed(false) {}

AppState StateManager::getCurrentState() const 
{
    return m_currentState;
}

void StateManager::setCurrentState(AppState newState) 
{
    m_currentState = newState;
}

bool StateManager::isAddressEntered() const 
{
    return m_isAddressEntered;
}

bool StateManager::isInternetConnected() const
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

void StateManager::setAddressEntered(bool entered) 
{
    m_isAddressEntered = entered;
}

bool StateManager::isDataProcessed() const 
{
    return m_isDataProcessed;
}

void StateManager::setDataProcessed(bool processed) 
{
    m_isDataProcessed = processed;
}

const ShutdownData& StateManager::getShutdownData() const 
{
    return m_shutdownInfo;
}

void StateManager::setShutdownData(const ShutdownData& info) 
{
    m_shutdownInfo = info;
}

void StateManager::reset() 
{
    m_currentState = AppState::INPUT;
    m_isAddressEntered = false;
    m_isDataProcessed = false;
    m_shutdownInfo = ShutdownData();
}
