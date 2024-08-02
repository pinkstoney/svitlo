#include <svitlo/consumer.h>

#include <stdexcept>

using namespace svitlo;

Consumer::Consumer(int queue, int subqueue, std::string id)
    : m_queue(queue)
    , m_subqueue(subqueue)
    , m_id(std::move(id))
{
    if (m_id.empty())
        throw std::invalid_argument("Consumer ID cannot be empty");
}

void Consumer::setElectricityStatus(const std::string& date, int hour, ElectricityData::Status status)
{
    if (hour < 0 || hour > 23)
        throw std::out_of_range("Hour must be between 0 and 23");
    m_electricityData[date][hour] = status;
}

ElectricityData::Status Consumer::getElectricityStatus(const std::string& date, int hour) const
{
    if (hour < 0 || hour > 23)
        throw std::out_of_range("Hour must be between 0 and 23");

    auto it = m_electricityData.find(date);
    if (it != m_electricityData.end())
        return it->second[hour];

    return ElectricityData::Status::Unknown;
}

void Consumer::setScheduleApprovedSince(const std::string& date, const std::string& time) { m_scheduleApprovedSince[date] = time; }

std::string Consumer::getScheduleApprovedSince(const std::string& date) const
{
    auto it = m_scheduleApprovedSince.find(date);
    if (it != m_scheduleApprovedSince.end())
        return it->second;

    return "";
}
