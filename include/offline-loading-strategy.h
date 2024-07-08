#pragma once

#include "data-loading-strategy.h"

class OfflineLoadingStrategy : public DataLoadingStrategy 
{
public:
    void loadData(const std::string& inputInfo, ShutdownInfo& request, DatabaseManager& dbManager) override;
};

