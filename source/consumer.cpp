#include <svitlo/consumer.h>

#include <stdexcept>

using namespace svitlo;

Consumer::Consumer(int queue, int subqueue, std::string id)
    : m_queue(queue)
    , m_subqueue(subqueue)
    , m_id(std::move(id))
    , m_electricityData{ElectricityData::Status::Unknown}
{
    if (m_id.empty())
        throw std::invalid_argument("Consumer ID cannot be empty");
}

void Consumer::setElectricityStatus(int hour, ElectricityData::Status status)
{
    if (hour < 0 || hour > 23)
        throw std::out_of_range("Hour must be between 0 and 23");
    m_electricityData[hour] = status;
}

ElectricityData::Status Consumer::getElectricityStatus(int hour) const
{
    if (hour < 0 || hour > 23)
        throw std::out_of_range("Hour must be between 0 and 23");
    return m_electricityData[hour];
}
