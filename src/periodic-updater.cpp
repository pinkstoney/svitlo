#include "../include/periodic-updater.h"
#include <chrono>

PeriodicUpdater::PeriodicUpdater(std::chrono::seconds interval)
    : m_interval(interval), m_lastUpdateTime(std::chrono::steady_clock::now()) {}

bool PeriodicUpdater::shouldUpdate()
{
    auto now = std::chrono::steady_clock::now();
    if (now - m_lastUpdateTime >= m_interval)
    {
        m_lastUpdateTime = now;
        return true;
    }
    return false;
}

void PeriodicUpdater::reset()
{
    m_lastUpdateTime = std::chrono::steady_clock::now();
}
