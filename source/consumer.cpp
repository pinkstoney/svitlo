#include <svitlo/consumer.h>

#include <utility>

using namespace svitlo;

Consumer::Consumer(int queue, int subqueue, std::string id)
    : m_queue(queue)
    , m_subqueue(subqueue)
    , m_id(std::move(id))
    , m_electricityData(std::make_unique<ElectricityData>())
{
    if (m_id.empty())
        throw std::invalid_argument("Consumer ID cannot be empty");
}

void Consumer::setElectricityStatus(int hour, ElectricityData::Status status)
{
    if (!m_electricityData)
        m_electricityData = std::make_unique<ElectricityData>();

    m_electricityData->addStatus(hour, status);
}

ElectricityData::Status Consumer::getElectricityStatus(int hour) const
{
    if (!m_electricityData)
        return ElectricityData::Status::Unknown;

    return m_electricityData->getStatus(hour);
}
