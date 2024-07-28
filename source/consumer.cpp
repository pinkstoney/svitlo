#include <svitlo/consumer.h>

using namespace svitlo;

Consumer::Consumer(int queue, int subqueue, std::string id)
    : m_queue(queue)
    , m_subqueue(subqueue)
    , m_id(std::move(id))
    , m_electricityData(std::make_unique<ElectricityData>())
{
}

void Consumer::setElectricityStatus(int hour, ElectricityData::Status status)
{
    m_electricityData->addStatus(hour, status);
}

ElectricityData::Status Consumer::getElectricityStatus(int hour) const
{
    return m_electricityData->getStatus(hour);
}
