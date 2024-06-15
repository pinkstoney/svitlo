#include "../include/application.h"

ApplicationSpecification appSpec;

Application::Application()
        : m_uiManager(), m_dbManager("user_info.db"), m_isAddressEntered(false), m_isAddressSent(false),
          m_isDataProcessed(false), m_isDataListDisplayed(false), m_DataListCurrentActive(-1)
{
    init();

    m_dbManager.init();
    m_uiManager.loadFonts();

    memset(m_info, 0, sizeof(m_info));
}

Application::~Application()
{
    CloseWindow();
}

void Application::init() const
{
    InitWindow(appSpec.WINDOW_WIDTH, appSpec.WINDOW_HEIGHT, "Shutdown Info");
    SetTargetFPS(60);
    GuiLoadStyle("../res/styles/candy/style_candy.rgs");

}

void Application::run()
{
    if (std::string homeUserInfo = m_dbManager.getHomeUserInfo(); !homeUserInfo.empty())
        processData(homeUserInfo);

    while (!WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(RAYWHITE);

        handleInput();

        if (m_isAddressSent && m_errorMessage.empty() && m_isDataProcessed)
        {
            m_isDataListDisplayed = false;
            m_uiManager.drawCircles(m_request, m_uiManager.getLexendFont());
        }

        handleListView();

        if (!m_errorMessage.empty())
            m_uiManager.drawText(m_errorMessage, 10, appSpec.WINDOW_HEIGHT - 30, 20, RED);

        EndDrawing();
    }
}

void Application::handleInput()
{
    if (!m_isAddressSent)
    {
        m_uiManager.drawText("Enter data:", 10, 30, 20, BLACK);
        if (m_uiManager.textBox({140, 23, 200, 30}, m_info, sizeof(m_info), true))
            m_isAddressEntered = true;

        if (IsKeyPressed(KEY_ENTER) && !m_isAddressSent && m_isAddressEntered && m_errorMessage.empty())
        {
            std::string inputInfo(m_info);
            processData(inputInfo);
        }

        handleSavedUserInfo();
    }
    else
        handleBackToDataInput();
}

void Application::handleBackToDataInput()
{
    if (m_uiManager.button({990, static_cast<float>(appSpec.WINDOW_HEIGHT - 60), 200, 30}, "Go back to data input"))
    {
        m_isAddressSent = false;
        m_isAddressEntered = false;
        m_errorMessage.clear();
        m_isDataProcessed = false;
        m_isDataListDisplayed = false;
        m_DataListCurrentActive = -1;

        memset(m_info, 0, sizeof(m_info));
        m_request = ShutdownInfo();
    }
}

void Application::handleSavedUserInfo()
{
    if (!m_dbManager.isDatabaseEmpty() && !m_isDataProcessed && !m_isDataListDisplayed)
    {
        if (m_uiManager.button({10, static_cast<float>((appSpec.WINDOW_HEIGHT - 60)), 200, 30}, "Use saved user info"))
        {
            m_isDataListDisplayed = true;
            m_DataListCurrentActive = -1;
            m_allUserInfo = m_dbManager.getAllUserInfo();
            m_allUserInfoStr.clear();
            for (const std::string &userInfo: m_allUserInfo)
                m_allUserInfoStr += userInfo + ";";
        }
    }
}

void Application::handleListView()
{
    Rectangle bounds = {10, 100, 200, 300};
    int scrollIndex = 0;

    if (m_isDataListDisplayed && !m_dbManager.isDatabaseEmpty())
    {
        m_uiManager.listView(bounds, m_allUserInfoStr.c_str(), &scrollIndex, &m_DataListCurrentActive);
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && m_DataListCurrentActive >= 0)
        {
            std::string selectedUserInfo = m_allUserInfo[m_DataListCurrentActive];
            processData(selectedUserInfo);
            m_isDataListDisplayed = false;
        }

        handleHomeButtons();
        handleDeleteUserInfo();
        handleHideSavedUser();
    }
}

void Application::handleHomeButtons()
{
    for (int i = 0; i < m_allUserInfo.size(); i++)
    {
        if (m_allUserInfo[i] == m_dbManager.getHomeUserInfo())
        {
            if (m_uiManager.button({330, static_cast<float>(100 + i * 30), 150, 30}, "Remove as Home"))
                m_dbManager.removeHomeUserInfo();
        }
        else
        {
            if (m_uiManager.button({330, static_cast<float>(100 + i * 30), 150, 30}, "Set as Home"))
                m_dbManager.setHomeUserInfo(m_allUserInfo[i]);
        }
    }
}

void Application::handleHideSavedUser()
{
    if (m_uiManager.button({240, static_cast<float>(appSpec.WINDOW_HEIGHT - 60), 200, 30}, "Hide saved user"))
        m_isDataListDisplayed = false;
}

void Application::handleDeleteUserInfo()
{
    for (int i = 0; i < m_allUserInfo.size(); i++)
    {
        if (m_uiManager.button({220, static_cast<float>(100 + i * 30), 100, 30}, "Delete"))
        {
            m_dbManager.deleteUserInfo(m_allUserInfo[i]);
            m_allUserInfo = m_dbManager.getAllUserInfo();
            m_allUserInfoStr.clear();

            for (const std::string& userInfo : m_allUserInfo)
                m_allUserInfoStr += userInfo + ";";
        }
    }
}

void Application::processData(const std::string& inputInfo)
{
    if (m_isDataProcessed)
        return;

    try
    {
        std::string userChoice = std::all_of(inputInfo.begin(), inputInfo.end(), ::isdigit) ? "accountNumber" : "address";
        m_request.setPostData(userChoice, inputInfo);
        std::string response = m_request.send();
        m_request.processRawElectricityData(response);
        m_request.formatElectricityData(response);

        m_isAddressEntered = false;
        m_isAddressSent = true;
        m_errorMessage.clear();

        if (!m_dbManager.isUserInfoExist(inputInfo))
            m_dbManager.saveUserInfo(inputInfo);

        m_isDataProcessed = true;
    }
    catch (const std::runtime_error& e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        m_errorMessage = e.what();
    }
}

