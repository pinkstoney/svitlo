#include <svitlo/consumer.h>
#include <svitlo/database-manager.h>

#include <iostream>
#include <vector>

using namespace svitlo;

void printStatus(ElectricityData::Status status)
{
    switch (status)
    {
        case ElectricityData::Status::Unknown:
            std::cout << "Unknown";
            break;
        case ElectricityData::Status::No:
            std::cout << "No electricity";
            break;
        case ElectricityData::Status::Yes:
            std::cout << "Yes, electricity";
            break;
        case ElectricityData::Status::Maybe:
            std::cout << "Maybe electricity";
            break;
    }
}

int main()
{
    try
    {
        DatabaseManager dbManager("consumers.db");
        dbManager.createTables();

        std::vector<Consumer> consumers;
        consumers.emplace_back(1, 1, "Consumer1");
        consumers.emplace_back(1, 2, "Consumer2");
        consumers.emplace_back(2, 1, "Consumer3");

        for (auto& consumer : consumers)
        {
            consumer.setElectricityStatus(0, ElectricityData::Status::Yes);
            consumer.setElectricityStatus(12, ElectricityData::Status::No);
            dbManager.saveConsumer(consumer);
        }

        for (const auto& consumer : consumers)
        {
            auto loadedConsumer = dbManager.loadConsumer(consumer.getId());
            if (loadedConsumer)
            {
                std::cout << "Consumer ID: " << loadedConsumer->getId() << ", Queue: " << loadedConsumer->getQueue()
                          << ", Subqueue: " << loadedConsumer->getSubqueue() << std::endl;

                for (int hour : {0, 6, 12, 18})
                {
                    std::cout << "  Hour " << hour << ": ";
                    ElectricityData::Status status = loadedConsumer->getElectricityStatus(hour);
                    printStatus(status);
                    std::cout << std::endl;
                }
                std::cout << std::endl;
            }
        }
    }
    catch (const std::exception& e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}