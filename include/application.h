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

struct ApplicationSpecification
{
    const int WINDOW_WIDTH = 1200;
    const int WINDOW_HEIGHT = 800;
    const int TARGET_FPS = 60;
};

class Application {
public:
    Application();
    ~Application();

public:
    void run();

private:
    enum { MAX_INPUT_LENGTH = 245 };

    void m_initializeWindow() const;
    void m_loadUserHomeInfo();
    void m_processState();
    void setLoadingStrategy(bool isOnline);

    void m_displayInputScreen();
    void m_displayDataScreen();

    void m_processBackToInputScreen();
    void m_processSavedUserInfo();
    void m_displaySavedUserInfo();
    void m_hideSavedUserInfo();
    void m_displaySavedUserInfoList();

    void m_handleUserInfoSelection();
    void m_processHomeButtons();
    void m_processDeleteUserInfo();
    void processData(const std::string& inputInfo);

    void processDataOffline(const std::string &inputInfo); 
    void processDataOnline(const std::string& inputInfo);

    void resetApplicationState();
    void clearUserInput();
    
    bool m_isSavedUserInfoDisplayed = false;
    void m_toggleSavedUserInfo(bool show);
    void m_displayErrorMessage();
    void m_displayDataSavedTime();
    void m_updateAllUserInfo();
private:
    UIManager m_uiManager;
    DatabaseManager m_dbManager;
    ShutdownInfo m_request;
    StateManager m_stateManager;
    std::unique_ptr<DataLoadingStrategy> m_loadingStrategy;

private:
    int m_DataListCurrentActive = -1;
    bool m_isInternetConnected = false;

private:
    char m_info[256];
    std::string m_errorMessage;
    std::vector<std::pair<std::string, std::string>> m_allUserInfo;;
    std::string m_allUserInfoStr;

    void m_displayNoInternetScreen();
};
