#include "../include/data-processor.h"
#include "../include/utility.h"
#include <algorithm>

DataProcessor::DataProcessor(DatabaseManager& dbManager) : m_dbManager(dbManager) {}

void DataProcessor::processData(const std::string& inputInfo, bool isInternetConnected)
{
    reset();
    try
    {
        bool isNumber = std::all_of(inputInfo.begin(), inputInfo.end(), ::isdigit);
        std::string userChoice = isNumber ? "accountNumber" : "address";
        m_request.setPostData(userChoice, inputInfo);

        setLoadingStrategy(isInternetConnected); 
        m_loadingStrategy->loadData(inputInfo, m_request, m_dbManager);
    }
    catch (const std::exception& e)
    {
        m_errorMessage = e.what();
    }
}

void DataProcessor::setLoadingStrategy(bool isOnline)
{
    if (isOnline)
        m_loadingStrategy = std::make_unique<OnlineLoadingStrategy>();
    else 
        m_loadingStrategy = std::make_unique<OfflineLoadingStrategy>();
}

ShutdownInfo DataProcessor::getProcessedRequest() const
{
    return m_request;
}

std::string DataProcessor::getErrorMessage() const
{
    return m_errorMessage;
}

void DataProcessor::reset()
{
    m_request = ShutdownInfo();
    m_errorMessage.clear();
}
