#include "../include/online-fetching-strategy.h"
#include "../include/utility.h"

void OnlineFetchingStrategy::loadData(const std::string& inputInfo, ShutdownData& request, DatabaseManager& dbManager)
{
    std::string rawData = request.send();
    request.formatElectricityData(rawData);
    
    std::string todayDate = Utility::getTodayDate();

    for (int hour = 0; hour <= 24; ++hour) 
    {
        int status = 0;
        if (std::find_if(request.getWillBeElectricityToday().begin(), request.getWillBeElectricityToday().end(),
                         [hour](const auto& p) { return p.first == hour; }) != request.getWillBeElectricityToday().end()) {
            status = 0;
        } else if (std::find_if(request.getMightBeElectricityToday().begin(), request.getMightBeElectricityToday().end(),
                                [hour](const auto& p) { return p.first == hour; }) != request.getMightBeElectricityToday().end()) {
            status = 2;
        } else if (std::find_if(request.getWontBeElectricityToday().begin(), request.getWontBeElectricityToday().end(),
                                [hour](const auto& p) { return p.first == hour; }) != request.getWontBeElectricityToday().end()) {
            status = 1;
        }

        dbManager.saveElectricityInfo(inputInfo, todayDate, hour, status, request.getQueue(), request.getSubqueue(), false);
    }

    std::string tomorrowDate = Utility::getTomorrowDate();

    if (!request.getWillBeElectricityTomorrow().empty() ||
        !request.getMightBeElectricityTomorrow().empty() ||
        !request.getWontBeElectricityTomorrow().empty()) {
        
        for (int hour = 0; hour <= 24; ++hour) 
        {
            int status = 0;
            if (std::find_if(request.getWillBeElectricityTomorrow().begin(), request.getWillBeElectricityTomorrow().end(),
                             [hour](const auto& p) { return p.first == hour; }) != request.getWillBeElectricityTomorrow().end()) {
                status = 0;
            } else if (std::find_if(request.getMightBeElectricityTomorrow().begin(), request.getMightBeElectricityTomorrow().end(),
                                    [hour](const auto& p) { return p.first == hour; }) != request.getMightBeElectricityTomorrow().end()) {
                status = 2;
            } else if (std::find_if(request.getWontBeElectricityTomorrow().begin(), request.getWontBeElectricityTomorrow().end(),
                                    [hour](const auto& p) { return p.first == hour; }) != request.getWontBeElectricityTomorrow().end()) {
                status = 1;
            }

            dbManager.saveElectricityInfo(inputInfo, tomorrowDate, hour, status, request.getQueue(), request.getSubqueue(), true);
        }
    }

    dbManager.saveUserInfo(inputInfo);
}
