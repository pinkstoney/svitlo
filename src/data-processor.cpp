#include "../include/data-processor.h"

#include "../include/online-fetching-strategy.h"
#include "../include/offline-fetching-strategy.h"

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
        m_dataFetchingStrategy->loadData(inputInfo, m_request, m_dbManager);
    }
    catch (const std::exception& e)
    {
        m_errorMessage = e.what();
    }
}

void DataProcessor::setLoadingStrategy(bool isOnline)
{
    if (isOnline)
        m_dataFetchingStrategy = std::make_unique<OnlineFetchingStrategy>();
    else 
        m_dataFetchingStrategy = std::make_unique<OfflineFetchingStrategy>();
}

ShutdownData DataProcessor::getProcessedRequest() const
{
    return m_request;
}

std::string DataProcessor::getErrorMessage() const
{
    return m_errorMessage;
}

void DataProcessor::reset()
{
    m_request = ShutdownData();
    m_errorMessage.clear();
}
