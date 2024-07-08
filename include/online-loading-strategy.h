#pragma once

#include "data-loading-strategy.h"
#include "utility.h"

class OnlineLoadingStrategy : public DataLoadingStrategy
{
public:
    void loadData(const std::string& inputInfo, ShutdownInfo& request, DatabaseManager& dbManager) override;
};
