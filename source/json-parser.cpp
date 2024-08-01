#include <svitlo/json-parser.h>

using namespace svitlo;

Consumer JsonParser::parseConsumerData(const std::string& jsonStr, const std::string& id)
{
    auto j = nlohmann::json::parse(jsonStr);

    if (!j.contains("current") || !j["current"].contains("queue") || !j["current"].contains("subqueue"))
    {
        throw std::runtime_error("Invalid JSON structure: missing required fields");
    }

    int queue = j["current"]["queue"];
    int subqueue = j["current"]["subqueue"];

    Consumer consumer(queue, subqueue, id);

    if (j.contains("graphs"))
    {
        if (j["graphs"].contains("today"))
            parseElectricityData(consumer, j["graphs"]["today"]);

        if (j["graphs"].contains("tomorrow"))
            parseElectricityData(consumer, j["graphs"]["tomorrow"]);
    }

    return consumer;
}

void JsonParser::parseElectricityData(Consumer& consumer, const nlohmann::json& graphData)
{
    if (!graphData.contains("hoursList"))
    {
        return;
    }

    for (const auto& hourData : graphData["hoursList"])
    {
        if (!hourData.contains("hour") || !hourData.contains("electricity"))
            continue;

        int hour = std::stoi(hourData["hour"].get<std::string>()) - 1; 
        if (hour < 0 || hour > 23)
            continue; 

        ElectricityData::Status status =
            hourData["electricity"].get<int>() == 1 ? ElectricityData::Status::Yes : ElectricityData::Status::No;

        consumer.setElectricityStatus(hour, status);
    }
}
