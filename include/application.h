#pragma once

#include "raygui.h"

#include "ui-manager.h"
#include "shutdown-info.h"
#include "database-manager.h"

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
    void init() const;

private:
    void handleInput();
    void handleBackToDataInput();
    void handleSavedUserInfo();
    void handleListView();
    void handleHomeButtons();
    void handleHideSavedUser();
    void handleDeleteUserInfo();
    void processData(const std::string& inputInfo);

private:
    bool m_isAddressEntered;
    bool m_isAddressSent;
    bool m_isDataProcessed;
    bool m_isDataListDisplayed;
    int m_DataListCurrentActive;
    char m_info[256];

    std::string m_errorMessage;
    std::vector<std::string> m_allUserInfo;
    std::string m_allUserInfoStr;

private:
    UIManager m_uiManager;
    ShutdownInfo m_request;

    DatabaseManager m_dbManager;

};
