#pragma once

#include <thread>

#include "raygui.h"

#include "ui-manager.h"
#include "shutdown-info.h"
#include "database-manager.h"
#include "state-manager.h"
#include "data-loading-strategy.h"
#include "online-loading-strategy.h"
#include "offline-loading-strategy.h"
#include "data-processor.h"
#include "saved-user-info-manager.h"

struct ApplicationSpecification
{
    const int WINDOW_WIDTH = 1200;
    const int WINDOW_HEIGHT = 800;
    const int TARGET_FPS = 60;
};

class Application
{
public:
    Application();
    ~Application();

public:
    void run();

private:
    void m_initializeWindow() const;
    void m_loadUserHomeInfo();
    void m_processState();

    void m_displayInputScreen();
    void m_displayDataScreen();

    void m_processSavedUserInfo();
    void m_displaySavedUserInfo();

    void processData(const std::string& inputInfo);

    void resetApplicationState();
    void clearUserInput();

    void m_displayErrorMessage() const;
    void m_displayDataSavedTime();

private:
    UIManager m_uiManager;
    DatabaseManager m_dbManager;
    ShutdownInfo m_request;
    StateManager m_stateManager;
    std::unique_ptr<DataLoadingStrategy> m_loadingStrategy;
    DataProcessor m_dataProcessor;
    SavedUserInfoManager m_savedUserInfoManager;

private:
    int m_DataListCurrentActive = -1;
    bool m_isInternetConnected = false;
    bool m_isSavedUserInfoDisplayed = false;

private:
    char m_info[256];
    std::string m_errorMessage;
    std::vector<std::pair<std::string, std::string>> m_allUserInfo;;
    std::string m_allUserInfoStr;

}; 
