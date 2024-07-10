#pragma once

#include <string>
#include <memory>

#include "shutdown-info.h"
#include "database-manager.h"
#include "data-loading-strategy.h"
#include "online-loading-strategy.h"
#include "offline-loading-strategy.h"

class DataProcessor
{
public:
    explicit DataProcessor(DatabaseManager& dbManager);

public:
    void reset();

    void processData(const std::string& inputInfo, bool isInternetConnected);
    void setLoadingStrategy(bool isOnline);
    ShutdownInfo getProcessedRequest() const;

    std::string getErrorMessage() const;

private:
    DatabaseManager& m_dbManager;
    std::unique_ptr<DataLoadingStrategy> m_loadingStrategy;
    ShutdownInfo m_request;
    std::string m_errorMessage;
};
