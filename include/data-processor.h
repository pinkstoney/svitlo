#pragma once

#include "shutdown-info.h"
#include "database-manager.h"
#include "data-loading-strategy.h"
#include "online-loading-strategy.h"
#include "offline-loading-strategy.h"
#include <string>
#include <memory>

class DataProcessor {
public:
    DataProcessor(DatabaseManager& dbManager);

    void reset();

    void processData(const std::string& inputInfo, bool isInternetConnected);
    void setLoadingStrategy(bool isOnline);
    ShutdownInfo getProcessedRequest() const;
    std::string getErrorMessage() const;
    void clearErrorMessage();

private:
    DatabaseManager& m_dbManager;
    std::unique_ptr<DataLoadingStrategy> m_loadingStrategy;
    ShutdownInfo m_request;
    std::string m_errorMessage;
};
