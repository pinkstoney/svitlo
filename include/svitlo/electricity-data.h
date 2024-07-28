#pragma once

#include <map>
#include <cstdint>

namespace svitlo
{
    class ElectricityData
    {
      public:
        enum class Status : std::int8_t
        {
            Unknown = -1,
            No = 0,
            Yes = 1,
            Maybe = 2
        };

        void addStatus(int hour, Status status);
        Status getStatus(int hour) const;

      private:
        std::map<int, Status> m_hourlyStatus;

        static void m_validateHour(int hour);
    };
}

