#include <chrono>
#include <functional>

class PeriodicUpdater 
{
public:
    PeriodicUpdater(std::chrono::seconds interval);

public:
    bool shouldUpdate();
    void reset();

private:
    std::chrono::seconds m_interval;
    std::chrono::steady_clock::time_point m_lastUpdateTime;
};
