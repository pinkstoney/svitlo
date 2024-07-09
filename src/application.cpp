#include "../include/application.h"
#include "../include/state-manager.h"
#include "../include/utility.h"

const ApplicationSpecification appSpec;

Application::Application()
        : m_uiManager(), m_dbManager("user_info.db"), m_stateManager(),
          m_isInternetConnected(false), m_isSavedUserInfoDisplayed(false), 
          m_DataListCurrentActive(-1)
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
    GuiLoadStyle("../res/styles/candy/style_candy.rgs");
}

void Application::run() 
{
    m_isInternetConnected = m_stateManager.isInternetConnected();
    // m_isInternetConnected = true; 
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
    m_uiManager.drawInternetStatus(m_stateManager.isInternetConnected());
    if (m_isInternetConnected)
    {
       m_uiManager.drawInputPrompt(); 
        
       if (m_uiManager.textBox({140, 23, 200, 30}, m_info, sizeof(m_info), true)) 
           m_stateManager.setAddressEntered(true);

        if (IsKeyPressed(KEY_ENTER) && m_stateManager.isAddressEntered()) 
            processData(std::string(m_info));
    }
    m_processSavedUserInfo();
}

void Application::m_displayDataScreen() 
{
    m_uiManager.drawCircles(m_request, m_uiManager.getLexendFont());

    m_displayDataSavedTime();

    if (m_uiManager.drawBackButton(appSpec.WINDOW_HEIGHT))
        resetApplicationState();
}

void Application::m_displayDataSavedTime()
{
    if (m_DataListCurrentActive >= 0) 
    {
        std::string savedTime = m_allUserInfo[m_DataListCurrentActive].second;
        m_uiManager.drawDataSavedTime(savedTime);
    }
}

void Application::m_processSavedUserInfo() 
{
    if (m_dbManager.isDatabaseEmpty() && !m_stateManager.isDataProcessed())
        return;

    if (m_uiManager.drawToggleSavedUserInfoButton(appSpec.WINDOW_HEIGHT, m_isSavedUserInfoDisplayed))
        m_toggleSavedUserInfo(!m_isSavedUserInfoDisplayed);

    if (m_isSavedUserInfoDisplayed)
        m_displaySavedUserInfo();
}

void Application::m_displaySavedUserInfo()
{
    if (m_stateManager.isDataProcessed() && !m_allUserInfoStr.empty())
        m_displaySavedUserInfoList();

}

void Application::m_toggleSavedUserInfo(bool show)
{
    m_isSavedUserInfoDisplayed = show;
    m_stateManager.setDataProcessed(show);

    if (show)
    {
        m_DataListCurrentActive = -1;
        m_updateAllUserInfo();
    }
}

void Application::m_updateAllUserInfo()
{
    m_allUserInfo = m_dbManager.getAllUserInfo();
    m_allUserInfoStr = std::accumulate(m_allUserInfo.begin(), m_allUserInfo.end(), std::string(), Utility::concatenteInfo);
}

void Application::m_displaySavedUserInfoList() 
{
    Rectangle bounds = {10, 100, 200, 300};
    int scrollIndex = 0;

    m_uiManager.listView(bounds, m_allUserInfoStr.c_str(), &scrollIndex, &m_DataListCurrentActive);

    m_handleUserInfoSelection();
    m_processHomeButtons();
    m_processDeleteUserInfo();
}

void Application::m_handleUserInfoSelection()
{
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && m_DataListCurrentActive >= 0) 
    {
        std::string selectedUserInfo = m_allUserInfo[m_DataListCurrentActive].first;
        processData(selectedUserInfo);
        m_stateManager.setDataProcessed(false);
    }
}

void Application::m_processHomeButtons()
{

    for (int i = 0; i < m_allUserInfo.size(); i++) 
    {
        std::string homeUserInfo = m_dbManager.getHomeUserInfo();
        bool isHome = (m_allUserInfo[i].first == homeUserInfo);

        if (m_uiManager.drawHomeButton(i, isHome)) 
        {
            if (isHome)
                m_dbManager.removeHomeUserInfo();
            else
                m_dbManager.setHomeUserInfo(m_allUserInfo[i].first);
            
//            homeUserInfo = m_dbManager.getHomeUserInfo();
        }
    }
}

void Application::m_processDeleteUserInfo() 
{
    for (int i = 0; i < m_allUserInfo.size(); i++) 
    {
        if (m_uiManager.drawDeleteButton(i)) 
        {
            m_dbManager.deleteUserInfo(m_allUserInfo[i].first);
            m_updateAllUserInfo();
            break;  
        }
    }
}

void Application::setLoadingStrategy(bool isOnline) 
{
    if (isOnline)
        m_loadingStrategy = std::make_unique<OnlineLoadingStrategy>();
    else 
        m_loadingStrategy = std::make_unique<OfflineLoadingStrategy>();
    
}

void Application::processData(const std::string& inputInfo)
{
    if (!m_errorMessage.empty() && !inputInfo.empty())
        resetApplicationState();

    try
    {
        bool isNumber = std::all_of(inputInfo.begin(), inputInfo.end(), ::isdigit);
        std::string userChoice = isNumber ? "accountNumber" : "address";
        m_request.setPostData(userChoice, inputInfo);

        setLoadingStrategy(m_isInternetConnected);
        m_loadingStrategy->loadData(inputInfo, m_request, m_dbManager);

        m_stateManager.setCurrentState(AppState::DISPLAYING_RESULTS);
        m_stateManager.setDataProcessed(true);
    }
    catch (const std::exception& e)
    {
        m_errorMessage = e.what();
        m_stateManager.reset();
    }
}

void Application::resetApplicationState()
{
    m_stateManager.reset();
    m_errorMessage.clear();
    clearUserInput();
    m_stateManager.setShutdownInfo(m_request);
    m_request = ShutdownInfo();
    m_isSavedUserInfoDisplayed = false;
    
}

void Application::clearUserInput()
{
    memset(m_info, 0, sizeof(m_info));
}

void Application::m_displayErrorMessage()
{   
    if (!m_errorMessage.empty()) 
        m_uiManager.drawText(m_errorMessage, 10, appSpec.WINDOW_HEIGHT - 30, 20, RED);
}


