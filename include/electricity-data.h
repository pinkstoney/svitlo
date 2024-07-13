#pragma once

#include <vector>

class ElectricityData
{
public:
    ElectricityData(std::vector<std::pair<int, int>> willBeElectricity,
                    std::vector<std::pair<int, int>> mightBeElectricity,
                    std::vector<std::pair<int, int>> wontBeElectricity,
                    int queue, int subqueue);

public:
    const std::vector<std::pair<int, int>>& getWillBeElectricity() const;
    const std::vector<std::pair<int, int>>& getMightBeElectricity() const;
    const std::vector<std::pair<int, int>>& getWontBeElectricity() const;

    int getQueue() const;
    int getSubqueue() const;

private:
    std::vector<std::pair<int, int>> m_willBeElectricity;
    std::vector<std::pair<int, int>> m_mightBeElectricity;
    std::vector<std::pair<int, int>> m_wontBeElectricity;

    int m_queue;
    int m_subqueue;
};
