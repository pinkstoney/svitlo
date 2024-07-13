#pragma once

#include "data-fetching-strategy.h"

class OfflineFetchingStrategy: public DataFetchingStrategy
{
public:
    void loadData(const std::string& inputInfo, ShutdownData& request, DatabaseManager& dbManager) override;
};

