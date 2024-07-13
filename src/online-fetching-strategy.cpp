#include "../include/online-fetching-strategy.h"
#include "../include/utility.h"

void OnlineFetchingStrategy::loadData(const std::string& inputInfo, ShutdownData& request, DatabaseManager& dbManager) 
{
    std::string response = request.send();
    request.processRawElectricityData(response);
    request.formatElectricityData(response);

    auto currentTime = Utility::getCurrentTime();

    dbManager.saveUserInfo(inputInfo);

    for (const auto& hour : request.getWillBeElectricityToday()) 
        dbManager.saveElectricityInfo(inputInfo, currentTime, hour.first, 1, request.getQueue(), request.getSubqueue());
    
    for (const auto& hour : request.getMightBeElectricityToday()) 
        dbManager.saveElectricityInfo(inputInfo, currentTime, hour.first, 2, request.getQueue(), request.getSubqueue());
    
    for (const auto& hour : request.getWontBeElectricityToday()) 
        dbManager.saveElectricityInfo(inputInfo, currentTime, hour.first, 3, request.getQueue(), request.getSubqueue());
}
