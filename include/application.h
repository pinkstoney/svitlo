#pragma once

#include <vector>
#include <tuple>

#include "raygui.h"
#include "raylib.h"

#include "shutdown-info.h"
#include "shutdown-circle.h"
#include "database-manager.h"

const int WINDOW_WIDTH = 1200;
const int WINDOW_HEIGHT = 800;
const int TARGET_FPS = 60;

class Application
{
public:
    Application();
    ~Application();

public:
    void run();

public:
    void handleInput();
    void drawUI();
    void handleListView();
    void handleBackToDataInput();
    void handleSavedUserInfo();
    void handleDeleteUserInfo();
    void handleHomeButtons();
    void handleHideSavedUser();

private:
    void m_initializeWindow() const;
    std::tuple<Font, Font, Font> m_loadFonts() const;

private:
    bool m_handleUserInput(char* info);
    void m_processData(ShutdownInfo& request, const std::string& inputInfo);
    void m_drawCircles(const ShutdownInfo& request, const Font& font) const;

private:
    ShutdownInfo m_request;
    char m_info[256];

private:
    bool m_addressEntered;
    bool m_addressSent;
    bool m_dataProcessed = false;
    bool m_displayListView = false;
    int m_listViewActive = -1;

    std::string m_errorMessage;

private:
    Font m_defaultFont;
    Font m_discoveryFont;
    Font m_lexendFont;

private:
    DatabaseManager m_dbManager;

    std::vector<std::string> m_allUserInfo;
    std::string m_allUserInfoStr;

};