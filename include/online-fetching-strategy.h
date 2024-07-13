#pragma once

#include "data-fetching-strategy.h"
#include "utility.h"

class OnlineFetchingStrategy : public DataFetchingStrategy 
{
public:
    void loadData(const std::string& inputInfo, ShutdownData& request, DatabaseManager& dbManager) override;
};

