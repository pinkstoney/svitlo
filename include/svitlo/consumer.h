#pragma once

#include <memory>
#include <string>

#include "electricity-data.h"

namespace svitlo
{
class Consumer
{
public:
    Consumer(int queue, int subqueue, std::string id);

public:
    void setElectricityStatus(int hour, ElectricityData::Status status);
    ElectricityData::Status getElectricityStatus(int hour) const;

    int getQueue() const { return m_queue; }
    int getSubqueue() const { return m_subqueue; }

    const std::string& getId() const { return m_id; }

private:
    std::unique_ptr<ElectricityData> m_electricityData;

    std::string m_id;
    int m_queue;
    int m_subqueue;
};
} // namespace svitlo 
