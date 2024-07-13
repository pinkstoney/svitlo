#pragma once

#include <string>

#include "shutdown-data.h"
#include "database-manager.h"

class DataFetchingStrategy 
{
public:
    virtual ~DataFetchingStrategy() = default;
    virtual void loadData(const std::string& inputInfo, ShutdownData& request, DatabaseManager& dbManager) = 0;
};

