#pragma once

#include <string>
#include <memory>

#include "shutdown-data.h"
#include "database-manager.h"
#include "data-fetching-strategy.h"

class DataProcessor
{
public:
    explicit DataProcessor(DatabaseManager& dbManager);

public:
    void reset();

    void processData(const std::string& inputInfo, bool isInternetConnected);

    void saveTomorrowData(const std::string& inputInfo);

    void setLoadingStrategy(bool isOnline);
    ShutdownData getProcessedRequest() const;

    std::string getErrorMessage() const;

private:
    DatabaseManager& m_dbManager;
    std::unique_ptr<DataFetchingStrategy> m_dataFetchingStrategy;
    ShutdownData m_request;
    std::string m_errorMessage;
};
