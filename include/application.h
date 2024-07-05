#pragma once

#include <thread>

#include "raygui.h"

#include "ui-manager.h"
#include "shutdown-info.h"
#include "database-manager.h"
#include "state-manager.h"

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
    void m_initializeWindow() const;
    void m_loadUserHomeInfo();
    void m_processState();

    void m_displayInputScreen();
    void m_displayDataScreen();
   //  void m_processInput();
    void m_processBackToInputScreen();
    void m_processSavedUserInfo();
    void m_displaySavedUserInfoList();
    void m_processHomeButtons();
    void m_processHideSavedUserButton();
    void m_processDeleteUserInfo();
    void processData(const std::string& inputInfo);

    void processDataOffline(const std::string &inputInfo); 
    void processDataOnline(const std::string& inputInfo);

private:
    UIManager m_uiManager;
    DatabaseManager m_dbManager;
    ShutdownInfo m_request;
    StateManager m_stateManager;

private:
    // bool m_isAddressEntered;
    // bool m_isAddressSent;
    // bool m_isDataProcessed;
    // bool m_isDataListDisplayed;
    int m_DataListCurrentActive = -1;
    bool m_isInternetConnected = false;

private:
    char m_info[256];
    std::string m_errorMessage;
    std::vector<std::pair<std::string, std::string>> m_allUserInfo;;
    std::string m_allUserInfoStr;

    void m_displayNoInternetScreen();
};
