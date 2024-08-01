#pragma once

#include <memory>
#include <string>

#include <svitlo/electricity-data.h>

namespace svitlo
{
class Consumer
{
public:
    Consumer(int queue, int subqueue, std::string id);

    void setElectricityStatus(int hour, ElectricityData::Status status);
    ElectricityData::Status getElectricityStatus(int hour) const;

    int getQueue() const { return m_queue; }
    int getSubqueue() const { return m_subqueue; }
    const std::string& getId() const { return m_id; }

private:
    int m_queue;
    int m_subqueue;
    std::string m_id;
    std::array<ElectricityData::Status, 24> m_electricityData;
};
} // namespace svitlo
