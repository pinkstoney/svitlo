#include "../include/offline-fetching-strategy.h"

void OfflineFetchingStrategy::loadData(const std::string& inputInfo, ShutdownData& request, DatabaseManager& dbManager)
{
    auto todayData = dbManager.getElectricityInfo(inputInfo, false);

    for (const auto& [date, hour, status, queue, subqueue] : todayData)
    {
        if (status == 0) 
        {
            request.addWillBeElectricityToday(hour);
        } 
        else if (status == 2)
        {
            request.addMightBeElectricityToday(hour);
        }
        else if (status == 1)
        {
            request.addWontBeElectricityToday(hour);
        }

        request.setQueue(queue);
        request.setSubqueue(subqueue);
    }

    auto tomorrowData = dbManager.getElectricityInfo(inputInfo, true);

    for (const auto& [date, hour, status, queue, subqueue] : tomorrowData) 
    {
        if (status == 0) 
        {
            request.addWillBeElectricityTomorrow(hour);
        }
        else if (status == 2) 
        {
            request.addMightBeElectricityTomorrow(hour);
        } 
        else if (status == 1)
        {
            request.addWontBeElectricityTomorrow(hour);
        }
    }
}
