#pragma once

#include <map>

namespace svitlo
{
class ElectricityData
{
public:
    enum class Status
    {
        Unknown,
        No,
        Yes,
        Maybe
    };

    void addStatus(int hour, Status status);
    Status getStatus(int hour) const;

private:
    void validateHour(int hour) const;

    std::map<int, Status> m_hourlyStatus;
};
} // namespace svitlo
