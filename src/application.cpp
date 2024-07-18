#include "../include/application.h"

#include "../include/state-manager.h"

const ApplicationSpecification appSpec;

Application::Application()
        : m_uiManager(), m_dbManager("user_info.db"),
          m_dataProcessor(m_dbManager), m_userRecordManager(m_dbManager)
{
    SetConfigFlags(FLAG_VSYNC_HINT);
    m_initializeWindow();
    m_uiManager.loadFonts();
    clearUserInput();
}

Application::~Application() 
{
    CloseWindow();
}

void Application::m_initializeWindow() const 
{
    InitWindow(appSpec.WINDOW_WIDTH, appSpec.WINDOW_HEIGHT, "Shutdown Info");
    SetTargetFPS(60);
    GuiLoadStyle("../res/styles/dark/style_dark.rgs");
}

void Application::run() 
{
   m_isInternetConnected = m_stateManager.isInternetConnected();
   // m_isInternetConnected = false;
    m_loadUserHomeInfo();

    while (!WindowShouldClose()) 
    {
        BeginDrawing();
        ClearBackground(RAYWHITE);

        m_processState();
        m_displayErrorMessage();

        EndDrawing();
    }
}

void Application::m_loadUserHomeInfo() 
{
    std::string homeUserInfo = m_dbManager.getHomeUserInfo();
    if (!homeUserInfo.empty())
        processData(homeUserInfo);
}

void Application::m_processState() 
{
    switch (m_stateManager.getCurrentState())
    {
        case AppState::INPUT:
            m_displayInputScreen();
            break;
        case AppState::DISPLAYING_RESULTS:
            m_displayDataScreen();
            break;
    }
}

void Application::m_displayInputScreen() 
{
    m_uiManager.drawInternetStatus(m_isInternetConnected);
    if (m_isInternetConnected)
    {
       m_uiManager.drawInputPrompt();
       m_uiManager.drawInputHint();
        
       if (m_uiManager.textBox({30, static_cast<float>(appSpec.WINDOW_HEIGHT/ 2 - 200), static_cast<float>(appSpec.WINDOW_WIDTH - 60), 30}, m_info, sizeof(m_info), true)) 
           m_stateManager.setAddressEntered(true);

        if (IsKeyPressed(KEY_ENTER) && m_stateManager.isAddressEntered()) 
        {
            processData(std::string(m_info));
            clearUserInput();
        }
    }
    m_processSavedUserInfo();
}

void Application::m_displayDataScreen() 
{
    m_uiManager.drawCircles(m_request, m_uiManager.getFixelBoldFont());

    m_displayDataSavedTime();

    if (m_uiManager.drawBackButton(appSpec.WINDOW_HEIGHT))
        resetApplicationState();
}

void Application::m_displayDataSavedTime()
{
    if (m_isInternetConnected)
        return;

    if (m_DataListCurrentActive >= 0) 
    {
        std::string savedTime = m_userRecordManager.getAllUserInfo()[m_DataListCurrentActive].second;
        m_uiManager.drawDataSavedTime(savedTime);
    }
}

void Application::m_processSavedUserInfo() 
{
    m_userRecordManager.updateAllUserInfo();

    if (m_dbManager.isDatabaseEmpty() && !m_stateManager.isDataProcessed()) 
        return;

    bool shouldToggle = m_uiManager.drawToggleSavedUserInfoButton(appSpec.WINDOW_HEIGHT, m_userRecordManager.isSavedUserInfoDisplayed());
    if (shouldToggle)
        m_userRecordManager.toggleSavedUserInfo(!m_userRecordManager.isSavedUserInfoDisplayed());

    if (m_userRecordManager.isSavedUserInfoDisplayed()) 
        m_displaySavedUserInfo();
}

void Application::m_displaySavedUserInfo()
{
    if (m_userRecordManager.hasUserInfo())
    {
        Rectangle bounds = {60, 390, static_cast<float>(appSpec.WINDOW_WIDTH - 420), 300};
        int scrollIndex = 0;
        int activeIndex = -1;

        m_uiManager.listView(bounds, m_userRecordManager.getAllUserInfoStr().c_str(), &scrollIndex, &activeIndex);

        if (activeIndex >= 0)
        {
            m_userRecordManager.handleUserInfoSelection(activeIndex, [this](const std::string& selectedInfo)
            {
                this->processData(selectedInfo);
            });
        }

        const auto& allUserInfo = m_userRecordManager.getAllUserInfo();
        for (int i = 0; i < allUserInfo.size(); i++) 
        {
            std::string homeUserInfo = m_dbManager.getHomeUserInfo();
            bool isHome = (allUserInfo[i].first == homeUserInfo);

            if (m_uiManager.drawHomeButton(i, isHome)) 
                m_userRecordManager.processHomeButton(i);

            if (m_uiManager.drawDeleteButton(i)) 
                m_userRecordManager.processDeleteButton(i);
        }
    }
}void Application::processData(const std::string& inputInfo)
{
    m_dataProcessor.processData(inputInfo, m_isInternetConnected);
    
    if (m_dataProcessor.getErrorMessage().empty())
    {
        m_stateManager.setCurrentState(AppState::DISPLAYING_RESULTS);
        m_stateManager.setDataProcessed(true);
        m_request = m_dataProcessor.getProcessedRequest();
    }
    else
    {
        m_errorMessage = m_dataProcessor.getErrorMessage();
        resetApplicationState();
    }
}

void Application::resetApplicationState()
{
    m_stateManager.reset();
    m_dataProcessor.reset();
    clearUserInput();
    m_request = ShutdownData();
    m_isSavedUserInfoDisplayed = false;
}

void Application::clearUserInput()
{
    memset(m_info, 0, sizeof(m_info));
}

void Application::m_displayErrorMessage() const
{   
    if (!m_errorMessage.empty()) 
        m_uiManager.drawText(m_errorMessage, 10, appSpec.WINDOW_HEIGHT - 30, 20, RED);
}


