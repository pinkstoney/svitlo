#pragma once

#include <map>
#include <string>

#include <svitlo/electricity-data.h>

namespace svitlo
{
class Consumer
{
public:
    Consumer(int queue, int subqueue, std::string id);

    void setElectricityStatus(const std::string& date, int hour, ElectricityData::Status status);
    ElectricityData::Status getElectricityStatus(const std::string& date, int hour) const;

    void setScheduleApprovedSince(const std::string& date, const std::string& time); 
    std::string getScheduleApprovedSince(const std::string& date) const;

    int getQueue() const { return m_queue; }
    int getSubqueue() const { return m_subqueue; }
    const std::string& getId() const { return m_id; }

private:
    int m_queue;
    int m_subqueue;
    std::string m_id;
    std::map<std::string, std::array<ElectricityData::Status, 24>> m_electricityData;
    std::map<std::string, std::string> m_scheduleApprovedSince;
};
} // namespace svitlo
