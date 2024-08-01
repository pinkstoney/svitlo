#pragma once

#include <string>

#include <nlohmann/json.hpp>
#include <svitlo/consumer.h>

namespace svitlo
{

class JsonParser
{
public:
    static Consumer parseConsumerData(const std::string& jsonStr, const std::string& id);

private:
    static void parseElectricityData(Consumer& consumer, const nlohmann::json& graphData);
};

} // namespace svitlo
