#include "../include/data-processor.h"

DataProcessor::DataProcessor(DatabaseManager& dbManager)
    : m_dbManager(dbManager), m_isInternetConnected(false) {}

void DataProcessor::processData(const std::string& inputInfo, bool isOnline) {
    m_isInternetConnected = isOnline;
    
    if (isOnline) {
        processDataOnline(inputInfo);
    } else {
        processDataOffline(inputInfo);
    }
}

ShutdownInfo DataProcessor::getProcessedData() const {
    return m_shutdownInfo;
}

bool DataProcessor::isInternetConnected() const {
    return m_isInternetConnected;
}

void DataProcessor::processDataOnline(const std::string& inputInfo) {
    if (m_dbManager.isUserInfoExist(inputInfo)) 
        m_dbManager.deleteUserInfo(inputInfo);
    
    m_dbManager.saveUserInfo(inputInfo);

    std::string response = m_shutdownInfo.send();
    m_shutdownInfo.processRawElectricityData(response);
    m_shutdownInfo.formatElectricityData(response);

    auto now = std::chrono::system_clock::now();
    std::time_t now_c = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&now_c), "%F");

    for (const auto &hour : m_shutdownInfo.getWillBeElectricityToday()) 
        m_dbManager.saveElectricityInfo(inputInfo, ss.str(), hour.first, 1, m_shutdownInfo.getQueue(), m_shutdownInfo.getSubqueue());
    
    for (const auto &hour : m_shutdownInfo.getMightBeElectricityToday()) 
        m_dbManager.saveElectricityInfo(inputInfo, ss.str(), hour.first, 2, m_shutdownInfo.getQueue(), m_shutdownInfo.getSubqueue());
    
    for (const auto &hour : m_shutdownInfo.getWontBeElectricityToday()) 
        m_dbManager.saveElectricityInfo(inputInfo, ss.str(), hour.first, 3, m_shutdownInfo.getQueue(), m_shutdownInfo.getSubqueue());
}

void DataProcessor::processDataOffline(const std::string& inputInfo) {
    auto electricityInfo = m_dbManager.getElectricityInfo(inputInfo);
    if (electricityInfo.empty()) {
        throw std::runtime_error("No saved electricity info found for today");
    }

    for (const auto& info : electricityInfo) {
        int hour = std::get<1>(info);
        int status = std::get<2>(info);
        int queue = std::get<3>(info);
        int subqueue = std::get<4>(info);

        m_shutdownInfo.setQueue(queue);
        m_shutdownInfo.setSubqueue(subqueue);

        if (status == 1) {
            m_shutdownInfo.addWillBeElectricityToday(hour);
        } else if (status == 2) {
            m_shutdownInfo.addMightBeElectricityToday(hour);
        } else if (status == 3) {
            m_shutdownInfo.addWontBeElectricityToday(hour);
        }
    }
}

void DataProcessor::saveProcessedData(const std::string& inputInfo) {
    std::string currentDate = getCurrentDate();

    if (m_dbManager.isUserInfoExist(inputInfo)) {
        m_dbManager.deleteUserInfo(inputInfo);
    }
    m_dbManager.saveUserInfo(inputInfo);

    for (const auto& [hour, _] : m_shutdownInfo.getWillBeElectricityToday()) {
        m_dbManager.saveElectricityInfo(inputInfo, currentDate, hour, 1, m_shutdownInfo.getQueue(), m_shutdownInfo.getSubqueue());
    }
    for (const auto& [hour, _] : m_shutdownInfo.getMightBeElectricityToday()) {
        m_dbManager.saveElectricityInfo(inputInfo, currentDate, hour, 2, m_shutdownInfo.getQueue(), m_shutdownInfo.getSubqueue());
    }
    for (const auto& [hour, _] : m_shutdownInfo.getWontBeElectricityToday()) {
        m_dbManager.saveElectricityInfo(inputInfo, currentDate, hour, 3, m_shutdownInfo.getQueue(), m_shutdownInfo.getSubqueue());
    }
}

std::string DataProcessor::getCurrentDate() const {
    auto now = std::chrono::system_clock::now();
    auto in_time_t = std::chrono::system_clock::to_time_t(now);

    std::stringstream ss;
    ss << std::put_time(std::localtime(&in_time_t), "%Y-%m-%d");
    return ss.str();
}
