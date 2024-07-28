#include <svitlo/consumer.h>
#include <iostream>
#include <vector>

using namespace svitlo;

void printStatus(ElectricityData::Status status) {
    switch (status) {
        case ElectricityData::Status::Unknown: std::cout << "Unknown"; break;
        case ElectricityData::Status::No: std::cout << "No electricity"; break;
        case ElectricityData::Status::Yes: std::cout << "Yes, electricity"; break;
        case ElectricityData::Status::Maybe: std::cout << "Maybe electricity"; break;
    }
}

int main() {
    std::vector<Consumer> consumers;
    
    // Create some consumers
    consumers.emplace_back(1, 1, "Consumer1");
    consumers.emplace_back(1, 2, "Consumer2");
    consumers.emplace_back(2, 1, "Consumer3");

    // Set electricity status for consumers
    try {
        consumers[0].setElectricityStatus(0, ElectricityData::Status::Yes);
        consumers[0].setElectricityStatus(12, ElectricityData::Status::No);
        
        consumers[1].setElectricityStatus(6, ElectricityData::Status::Maybe);
        consumers[1].setElectricityStatus(18, ElectricityData::Status::Yes);
        
        consumers[2].setElectricityStatus(3, ElectricityData::Status::No);
        consumers[2].setElectricityStatus(15, ElectricityData::Status::Yes);
    } catch (const std::exception& e) {
        std::cerr << "Error setting status: " << e.what() << std::endl;
        return 1;
    }

    // Print consumer information and electricity status
    for (const auto& consumer : consumers) {
        std::cout << "Consumer ID: " << consumer.getId() 
                  << ", Queue: " << consumer.getQueue() 
                  << ", Subqueue: " << consumer.getSubqueue() << std::endl;
        
        for (int hour : {0, 6, 12, 18}) {
            std::cout << "  Hour " << hour << ": ";
            try {
                ElectricityData::Status status = consumer.getElectricityStatus(hour);
                printStatus(status);
            } catch (const std::exception& e) {
                std::cerr << "Error: " << e.what();
            }
            std::cout << std::endl;
        }
        std::cout << std::endl;
    }

    return 0;
}
