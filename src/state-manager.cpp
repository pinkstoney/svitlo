#include "../include/state-manager.h"

std::ostream& operator<<(std::ostream& os, const AppState& state)
{
    switch (state)
    {
        case AppState::INPUT:
            os << "INPUT";
            break;
        case AppState::DISPLAYING_RESULTS:
            os << "DISPLAYING RESULTS";
            break;
        case AppState::DISPLAYING_SAVED_INFO:
            os << "DISPLAYING_SAVED_INFO";
            break;
        default:
            os << "UNKNOWN";
            break;
    }
    return os;
}

StateManager::StateManager() 
    : m_currentState(AppState::INPUT),
      m_isAddressEntered(false),
      m_isDataProcessed(false) {}

AppState StateManager::getCurrentState() const {
    return m_currentState;
}

void StateManager::setCurrentState(AppState newState) {
    m_currentState = newState;
}

bool StateManager::isAddressEntered() const {
    return m_isAddressEntered;
}

void StateManager::setAddressEntered(bool entered) {
    m_isAddressEntered = entered;
}

bool StateManager::isDataProcessed() const {
    return m_isDataProcessed;
}

void StateManager::setDataProcessed(bool processed) {
    m_isDataProcessed = processed;
}

const ShutdownInfo& StateManager::getShutdownInfo() const {
    return m_shutdownInfo;
}

void StateManager::setShutdownInfo(const ShutdownInfo& info) {
    m_shutdownInfo = info;
}

void StateManager::reset() {
    m_currentState = AppState::INPUT;
    m_isAddressEntered = false;
    m_isDataProcessed = false;
    m_shutdownInfo = ShutdownInfo();
}
