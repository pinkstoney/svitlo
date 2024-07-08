#pragma once

#include "shutdown-info.h"
#include <iostream>

enum class AppState {
    INPUT,
    DISPLAYING_RESULTS,
    DISPLAYING_SAVED_INFO
};

std::ostream& operator<<(std::ostream& os, const AppState& state);

class StateManager {
public:
    StateManager();

    AppState getCurrentState() const;
    void setCurrentState(AppState newState);

    bool isAddressEntered() const;
    void setAddressEntered(bool entered);

    bool isDataProcessed() const;
    void setDataProcessed(bool processed);

    const ShutdownInfo& getShutdownInfo() const;
    void setShutdownInfo(const ShutdownInfo& info);

    void reset();

private:
    AppState m_currentState;
    bool m_isAddressEntered;
    bool m_isDataProcessed;
    ShutdownInfo m_shutdownInfo;
};
        



