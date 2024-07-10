#pragma once

#include <string>

#include "shutdown-info.h"
#include "database-manager.h"

class DataLoadingStrategy 
{
public:
    virtual ~DataLoadingStrategy() = default;
    virtual void loadData(const std::string& inputInfo, ShutdownInfo& request, DatabaseManager& dbManager) = 0;
};

