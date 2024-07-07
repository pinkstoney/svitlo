#include "../include/application.h"
#include "../include/state-manager.h"
#include "../include/utility.h"

const ApplicationSpecification appSpec;

Application::Application()
        : m_uiManager(), m_dbManager("user_info.db"), m_stateManager(),
          m_isInternetConnected(false), m_isSavedUserInfoDisplayed(false), 
          m_DataListCurrentActive(-1)
{
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
//    m_isInternetConnected = m_request.isInternetConnected();
    m_isInternetConnected = true; 
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
    if (m_isInternetConnected)
    {
        m_uiManager.drawText("Enter data:", 10, 30, 20, BLACK);

       if (m_uiManager.textBox({140, 23, 200, 30}, m_info, sizeof(m_info), true)) 
       {
           std::cout << "here";
           m_stateManager.setAddressEntered(true);
       }

        if (IsKeyPressed(KEY_ENTER) && m_stateManager.isAddressEntered()) 
        {
            // m_errorMessage.clear();
            processData(std::string(m_info));
            std::cout << m_errorMessage << "!!!!";
        }
    }
    m_processSavedUserInfo();
}

void Application::m_displayDataScreen() 
{
    m_uiManager.drawCircles(m_request, m_uiManager.getLexendFont());

    m_displayDataSavedTime();

    if (m_uiManager.button({990, static_cast<float>(appSpec.WINDOW_HEIGHT - 60), 200, 30}, "Go back to data input")) 
        resetApplicationState();
}

void Application::m_displayDataSavedTime()
{
    if (m_DataListCurrentActive >= 0) 
    {
        std::string savedTime = m_allUserInfo[m_DataListCurrentActive].second;
        m_uiManager.drawText("Data saved on: " + savedTime, 10, 30, 20, BLACK);
    }
}

void Application::m_processSavedUserInfo() 
{
    if (m_dbManager.isDatabaseEmpty() && !m_stateManager.isDataProcessed())
        return;

    if (m_isSavedUserInfoDisplayed)
        m_displaySavedUserInfo();
    else
        m_hideSavedUserInfo();
}

void Application::m_displaySavedUserInfo()
{
    if (m_stateManager.isDataProcessed() && !m_allUserInfoStr.empty())
        m_displaySavedUserInfoList();

    if (m_uiManager.button({10, static_cast<float>(appSpec.WINDOW_HEIGHT - 60), 200, 30}, "Hide saved user info"))
        m_toggleSavedUserInfo(false);
}

void Application::m_hideSavedUserInfo()
{
    if (m_uiManager.button({10, static_cast<float>(appSpec.WINDOW_HEIGHT - 60), 200, 30}, "Use saved user info")) 
        m_toggleSavedUserInfo(true);
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
        std::string buttonText = (m_allUserInfo[i].first == m_dbManager.getHomeUserInfo()) ? "Remove as Home" : "Set as Home";
        if (m_uiManager.button({330, static_cast<float>(100 + i * 30), 150, 30}, buttonText.c_str())) 
        {
            if (buttonText == "Remove as Home")
                m_dbManager.removeHomeUserInfo();
            else
                m_dbManager.setHomeUserInfo(m_allUserInfo[i].first);
        }
    }
}

void Application::m_processDeleteUserInfo() 
{
    for (int i = 0; i < m_allUserInfo.size(); i++) 
    {
        if (m_uiManager.button({220, static_cast<float>(100 + i * 30), 100, 30}, "Delete")) 
        {
            m_dbManager.deleteUserInfo(m_allUserInfo[i].first);
            m_updateAllUserInfo();
        }
    }
}

void Application::processData(const std::string &inputInfo) 
{
    if(!m_errorMessage.empty() && !inputInfo.empty())
    {
        resetApplicationState();
    }

try 
    {
        bool isNumber = std::all_of(inputInfo.begin(), inputInfo.end(), ::isdigit);
        std::string userChoice = isNumber ? "accountNumber" : "address";
        m_request.setPostData(userChoice, inputInfo);

        if (m_isInternetConnected)
            processDataOnline(inputInfo);
        else
            processDataOffline(inputInfo);

        m_stateManager.setCurrentState(AppState::DISPLAYING_RESULTS);
        m_stateManager.setDataProcessed(true);
    }
    catch (const std::exception &e)
    {
        m_errorMessage = e.what();
        m_stateManager.reset();
    }
}

void Application::processDataOffline(const std::string &inputInfo) 
{
    auto electricityInfo = m_dbManager.getElectricityInfo(inputInfo);
    if (!electricityInfo.empty()) 
    {
        for (const auto &info : electricityInfo) 
        {
            int hour = std::get<1>(info);
            int status = std::get<2>(info);
            int queue = std::get<3>(info);
            int subqueue = std::get<4>(info);

            m_request.setQueue(queue);
            m_request.setSubqueue(subqueue);

            if (status == 1) 
                m_request.addWillBeElectricityToday(hour);
            else if (status == 2) 
                m_request.addMightBeElectricityToday(hour);
            else if (status == 3) 
                m_request.addWontBeElectricityToday(hour);
        }
    }
    else
    {
        throw std::runtime_error("No saved electricity info found for today");
    }
}

void Application::processDataOnline(const std::string& inputInfo)
{
   // m_dbManager.saveUserInfo(inputInfo);

    std::string response = m_request.send();
    m_request.processRawElectricityData(response);
    m_request.formatElectricityData(response);

    auto currentTime = Utility::getCurrentTime();

    m_dbManager.saveUserInfo(inputInfo);

    for (const auto &hour : m_request.getWillBeElectricityToday()) 
        m_dbManager.saveElectricityInfo(inputInfo, currentTime, hour.first, 1, m_request.getQueue(), m_request.getSubqueue());
    
    for (const auto &hour : m_request.getMightBeElectricityToday()) 
        m_dbManager.saveElectricityInfo(inputInfo, currentTime, hour.first, 2, m_request.getQueue(), m_request.getSubqueue());
    
    for (const auto &hour : m_request.getWontBeElectricityToday()) 
        m_dbManager.saveElectricityInfo(inputInfo, currentTime, hour.first, 3, m_request.getQueue(), m_request.getSubqueue());
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
    if(!m_isInternetConnected)
        m_uiManager.drawText("You are not connected to the Internet!", 430, 30, 20, RED);

    if (!m_errorMessage.empty()) 
        m_uiManager.drawText(m_errorMessage, 10, appSpec.WINDOW_HEIGHT - 30, 20, RED);
}


