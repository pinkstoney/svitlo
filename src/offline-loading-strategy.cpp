#include "../include/offline-loading-strategy.h"

void OfflineLoadingStrategy::loadData(const std::string& inputInfo, ShutdownInfo& request, DatabaseManager& dbManager) 
{
    auto electricityInfo = dbManager.getElectricityInfo(inputInfo);
    for (const auto& info : electricityInfo) 
    {
        int hour = std::get<1>(info);
        int status = std::get<2>(info);
        int queue = std::get<3>(info);
        int subqueue = std::get<4>(info);

        request.setQueue(queue);
        request.setSubqueue(subqueue);

        if (status == 1) 
            request.addWillBeElectricityToday(hour);
        else if (status == 2) 
            request.addMightBeElectricityToday(hour);
        else if (status == 3) 
            request.addWontBeElectricityToday(hour);
    }
}
