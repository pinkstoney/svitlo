#pragma once

#include <iostream>

#include "shutdown-info.h"

enum class AppState
{
    INPUT,
    DISPLAYING_RESULTS,
};

std::ostream& operator<<(std::ostream& os, const AppState& state);

class StateManager
{
public:
    StateManager();

public:
    AppState getCurrentState() const;
    void setCurrentState(AppState newState);

public:
    void reset();

public:
    bool isInternetConnected() const;

    bool isAddressEntered() const;
    void setAddressEntered(bool entered);

    bool isDataProcessed() const;
    void setDataProcessed(bool processed);

    const ShutdownInfo& getShutdownInfo() const;
    void setShutdownInfo(const ShutdownInfo& info);

private:
    AppState m_currentState;
    bool m_isAddressEntered;
    bool m_isDataProcessed;
    ShutdownInfo m_shutdownInfo;
};
        



