#include "../include/application.h"
#include "../include/state-manager.h"

const ApplicationSpecification appSpec;

Application::Application()
        : m_uiManager(), m_dbManager("user_info.db"), m_stateManager() 
{
    m_initializeWindow();
    m_uiManager.loadFonts();
    memset(m_info, 0, sizeof(m_info));
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
    m_loadUserHomeInfo();

    m_isInternetConnected = m_request.isInternetConnected();

    while (!WindowShouldClose()) 
    {
        BeginDrawing();
        ClearBackground(RAYWHITE);

        m_processState();

        if (!m_errorMessage.empty()) 
            m_uiManager.drawText(m_errorMessage, 10, appSpec.WINDOW_HEIGHT - 30, 20, RED);

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
            m_stateManager.setAddressEntered(true);

        if (IsKeyPressed(KEY_ENTER) && m_stateManager.isAddressEntered() && m_errorMessage.empty()) 
        {
            std::string inputInfo(m_info);
            processData(inputInfo);
        }

        m_processSavedUserInfo();
    }
    else
    {
        m_uiManager.drawText("You are not connected to the Internet!", 430, 30, 20, RED);
        m_processSavedUserInfo();
    }
}

void Application::m_displayDataScreen() 
{
    m_stateManager.setDataProcessed(false);
    m_uiManager.drawCircles(m_request, m_uiManager.getLexendFont());

    if (m_DataListCurrentActive >= 0) 
    {
        std::string savedTime = m_allUserInfo[m_DataListCurrentActive].second;
        m_uiManager.drawText("Data saved on: " + savedTime, 10, 30, 20, BLACK);
    }

    m_processBackToInputScreen();
}

void Application::m_processBackToInputScreen() 
{
    if (m_uiManager.button({990, static_cast<float>(appSpec.WINDOW_HEIGHT - 60), 200, 30}, "Go back to data input")) 
    {
        m_stateManager.reset();
        m_errorMessage.clear();
        memset(m_info, 0, sizeof(m_info));
        m_request = ShutdownInfo();
    }
}

void Application::m_processSavedUserInfo() 
{
    if (!m_dbManager.isDatabaseEmpty() && !m_stateManager.isDataProcessed()) 
    {
        if (m_uiManager.button({10, static_cast<float>(appSpec.WINDOW_HEIGHT - 60), 200, 30}, "Use saved user info")) 
        {
            m_stateManager.setDataProcessed(true);
            m_DataListCurrentActive = -1;
            m_allUserInfo = m_dbManager.getAllUserInfo();
            m_allUserInfoStr = std::accumulate(m_allUserInfo.begin(), m_allUserInfo.end(), std::string(),
                                               [](const std::string &a, const std::pair<std::string, std::string> &b) 
                                               {
                                                   return a + (a.length() > 0 ? ";" : "") + b.first;
                                               });
        }
    }

    if (m_stateManager.isDataProcessed()) 
        m_displaySavedUserInfoList();
}

void Application::m_displaySavedUserInfoList() 
{
    Rectangle bounds = {10, 100, 200, 300};
    int scrollIndex = 0;

    std::string allUserInfoStr;
    for (const auto& userInfo : m_allUserInfo) 
        allUserInfoStr += userInfo.first + " (Saved on: " + userInfo.second + ");";

    m_uiManager.listView(bounds, m_allUserInfoStr.c_str(), &scrollIndex, &m_DataListCurrentActive);

    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && m_DataListCurrentActive >= 0) 
    {
        std::string selectedUserInfo = m_allUserInfo[m_DataListCurrentActive].first;
        processData(selectedUserInfo);
        m_stateManager.setDataProcessed(false);
    }

    m_processHomeButtons();
    m_processDeleteUserInfo();
    m_processHideSavedUserButton();
}

void Application::m_processHomeButtons() 
{
    for (int i = 0; i < m_allUserInfo.size(); i++) 
    {
        if (m_allUserInfo[i].first == m_dbManager.getHomeUserInfo()) 
        {
            if (m_uiManager.button({330, static_cast<float>(100 + i * 30), 150, 30}, "Remove as Home")) 
                m_dbManager.removeHomeUserInfo();
        }
        else
        {
            if (m_uiManager.button({330, static_cast<float>(100 + i * 30), 150, 30}, "Set as Home")) 
                m_dbManager.setHomeUserInfo(m_allUserInfo[i].first);
        }
    }
}

void Application::m_processHideSavedUserButton() 
{
    if (m_uiManager.button({240, static_cast<float>(appSpec.WINDOW_HEIGHT - 60), 200, 30}, "Hide saved user info")) 
        m_stateManager.setDataProcessed(false);
}

void Application::m_processDeleteUserInfo() 
{
    for (int i = 0; i < m_allUserInfo.size(); i++) 
    {
        if (m_uiManager.button({220, static_cast<float>(100 + i * 30), 100, 30}, "Delete")) 
        {
            m_dbManager.deleteUserInfo(m_allUserInfo[i].first);
            m_allUserInfo = m_dbManager.getAllUserInfo();
            m_allUserInfoStr = std::accumulate(m_allUserInfo.begin(), m_allUserInfo.end(), std::string(),
                                               [](const std::string &a, const std::pair<std::string, std::string> &b) 
                                               {
                                                   return a + (a.length() > 0 ? ";" : "") + b.first;
                                               });
        }
    }
}

void Application::processData(const std::string &inputInfo) 
{
   // if (m_stateManager.isDataProcessed()) {
     //   return;
   // }

    try 
    {
        std::string userChoice = std::all_of(inputInfo.begin(), inputInfo.end(), ::isdigit) ? "accountNumber" : "address";
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
    if (m_dbManager.isUserInfoExist(inputInfo)) 
        m_dbManager.deleteUserInfo(inputInfo);
    
    m_dbManager.saveUserInfo(inputInfo);

    std::string response = m_request.send();
    m_request.processRawElectricityData(response);
    m_request.formatElectricityData(response);

    auto now = std::chrono::system_clock::now();
    std::time_t now_c = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&now_c), "%F");

    for (const auto &hour : m_request.getWillBeElectricityToday()) 
        m_dbManager.saveElectricityInfo(inputInfo, ss.str(), hour.first, 1, m_request.getQueue(), m_request.getSubqueue());
    
    for (const auto &hour : m_request.getMightBeElectricityToday()) 
        m_dbManager.saveElectricityInfo(inputInfo, ss.str(), hour.first, 2, m_request.getQueue(), m_request.getSubqueue());
    
    for (const auto &hour : m_request.getWontBeElectricityToday()) 
        m_dbManager.saveElectricityInfo(inputInfo, ss.str(), hour.first, 3, m_request.getQueue(), m_request.getSubqueue());
}

