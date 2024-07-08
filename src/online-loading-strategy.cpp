#include "../include/online-loading-strategy.h"
#include "../include/utility.h"

void OnlineLoadingStrategy::loadData(const std::string& inputInfo, ShutdownInfo& request, DatabaseManager& dbManager) 
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
