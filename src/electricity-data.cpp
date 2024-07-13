#include "../include/electricity-data.h"

ElectricityData::ElectricityData(std::vector<std::pair<int, int>> willBeElectricity,
                                 std::vector<std::pair<int, int>> mightBeElectricity,
                                 std::vector<std::pair<int, int>> wontBeElectricity,
                                 int queue, int subqueue)
        : m_willBeElectricity(std::move(willBeElectricity)),
          m_mightBeElectricity(std::move(mightBeElectricity)),
          m_wontBeElectricity(std::move(wontBeElectricity)),
          m_queue(queue),
          m_subqueue(subqueue)
{
}

const std::vector<std::pair<int, int>>& ElectricityData::getWillBeElectricity() const
{
    return m_willBeElectricity;
}

const std::vector<std::pair<int, int>>& ElectricityData::getMightBeElectricity() const
{
    return m_mightBeElectricity;
}

const std::vector<std::pair<int, int>>& ElectricityData::getWontBeElectricity() const
{
    return m_wontBeElectricity;
}

int ElectricityData::getQueue() const
{
    return m_queue;
}

int ElectricityData::getSubqueue() const
{
    return m_subqueue;
}
