#include <iostream>
#include <stdexcept>
#include <svitlo/consumer.h>
#include <svitlo/data-fetcher.h>
#include <svitlo/database-manager.h>
#include <svitlo/json-parser.h>

using namespace svitlo;

void fetchAndSaveConsumerData(const std::string& choice, const std::string& info, DatabaseManager& dbManager)
{
    try
    {
        DataFetcher fetcher;
        fetcher.setPostData(choice, info);
        std::string jsonResponse = fetcher.send();

        Consumer consumer = JsonParser::parseConsumerData(jsonResponse, info);
        dbManager.saveConsumer(consumer);

        std::cout << "Consumer data saved successfully for " << info << std::endl;
    }
    catch (const std::exception& e)
    {
        std::cerr << "Error fetching and saving data for " << info << ": " << e.what() << std::endl;
    }
}

void loadAndPrintConsumerData(const std::string& info, DatabaseManager& dbManager)
{
    try
    {
        auto consumer = dbManager.loadConsumer(info);
        if (consumer)
        {
            std::cout << "Consumer ID (Account/Address): " << consumer->getId() << std::endl;
            std::cout << "Queue: " << consumer->getQueue() << std::endl;
            std::cout << "Subqueue: " << consumer->getSubqueue() << std::endl;

            for (const auto& date : {"today", "tomorrow"})
            {
                std::cout << "\nElectricity Status for " << date << ":" << std::endl;
                std::cout << "Schedule Approved Since: " << consumer->getScheduleApprovedSince(date) << std::endl;
                for (int hour = 0; hour < 24; ++hour)
                {
                    auto status = consumer->getElectricityStatus(date, hour);
                    std::cout << "  Hour " << (hour + 1) << ": "
                              << (status == ElectricityData::Status::Yes  ? "On"
                                  : status == ElectricityData::Status::No ? "Off"
                                                                          : "Unknown")
                              << std::endl;
                }
            }
        }
        else
        {
            std::cout << "Consumer not found for " << info << std::endl;
        }
    }
    catch (const std::exception& e)
    {
        std::cerr << "Error loading data for " << info << ": " << e.what() << std::endl;
    }
}

int main()
{
    try
    {
        DatabaseManager dbManager("consumers.db");
        dbManager.createTables();

        fetchAndSaveConsumerData("accountNumber", "25012345", dbManager);

        fetchAndSaveConsumerData("address", "Калуш,Привокзальна,15", dbManager);

        std::cout << "\n--- Saved Data ---\n" << std::endl;

        loadAndPrintConsumerData("25012345", dbManager);
        std::cout << std::endl;
        loadAndPrintConsumerData("Калуш,Привокзальна,15", dbManager);
    }
    catch (const std::exception& e)
    {
        std::cerr << "Fatal error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
