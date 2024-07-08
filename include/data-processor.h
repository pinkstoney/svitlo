#pragma once

#include <string>
#include <chrono>
#include <optional>
#include <algorithm>
#include <iomanip>
#include <sstream>

#include "database-manager.h"
#include "shutdown-info.h"

class DataProcessor {
public:
    DataProcessor(DatabaseManager& dbManager);

    void processData(const std::string& inputInfo, bool isOnline);
    ShutdownInfo getProcessedData() const;
    bool isInternetConnected() const;

private:
    void processDataOnline(const std::string& inputInfo);
    void processDataOffline(const std::string& inputInfo);
    void saveProcessedData(const std::string& inputInfo);
    std::string getCurrentDate() const;

    DatabaseManager& m_dbManager;
    ShutdownInfo m_shutdownInfo;
    bool m_isInternetConnected;
};
