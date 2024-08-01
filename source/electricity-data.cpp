#include <svitlo/electricity-data.h>

#include <stdexcept>

using namespace svitlo;

void ElectricityData::addStatus(int hour, Status status)
{
    validateHour(hour);
    m_hourlyStatus[hour] = status;
}

ElectricityData::Status ElectricityData::getStatus(int hour) const
{
    validateHour(hour);
    auto it = m_hourlyStatus.find(hour);
    if (it != m_hourlyStatus.end())
        return it->second;
    return Status::Unknown;
}

void ElectricityData::validateHour(int hour) const
{
    if (hour < 0 || hour > 23)
        throw std::out_of_range("Hour must be between 0 and 23");
}
