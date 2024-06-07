#include "../include/application.h"


Application::Application()
    : m_addressEntered(false), m_addressSent(false), m_dbManager("user_info.db")
{
    m_dbManager.initialize();
    m_initializeWindow();
    std::tie(m_defaultFont, m_discoveryFont, m_lexendFont) = m_loadFonts();
}

Application::~Application()
{
    GuiSetFont(m_defaultFont);
    UnloadFont(m_discoveryFont);
    CloseWindow();
}

void Application::m_initializeWindow() const
{
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Electricity Information");
    GuiLoadStyle("../res/styles/candy/style_candy.rgs");
    SetTargetFPS(TARGET_FPS);
}

std::tuple<Font, Font, Font> Application::m_loadFonts() const
{
    Font localDefaultFont = GetFontDefault();

    std::vector<int> codepoints(512, 0);
    for (int i = 0; i < 95; i++) codepoints[i] = 32 + i;
    for (int i = 0; i < 255; i++) codepoints[96 + i] = 0x400 + i;
    Font localDiscovery = LoadFontEx("../res/fonts/discovery.otf", 30, codepoints.data(), static_cast<int>(codepoints.size()));

    GuiSetFont(localDiscovery);

    Font localLexend = LoadFont("../res/fonts/Lexend/static/Lexend-Bold.ttf");

    return std::make_tuple(localDefaultFont, localDiscovery, localLexend);
}

bool Application::m_handleUserInput(char* m_info)
{
    if (m_dataProcessed)
        return true;

    if (!m_addressSent)
    {
        DrawText("Enter data:", 10, 30, 20, BLACK);
        m_addressEntered = GuiTextBox((Rectangle){ 140, 23, 200, 30 }, m_info, 256, true);
    }

    if (m_addressEntered && strlen(m_info) == 0)
    {
        m_addressEntered = true;
        return false;
    }

    // Only process the user input when the Enter key is pressed
    if (IsKeyPressed(KEY_ENTER))
    {
        std::string info(m_info);
        if (!info.empty())
            m_processData(m_request, info);
        else
            std::cerr << "Error: User info is empty." << std::endl;

    }
    return true;
}

void Application::m_processData(ShutdownInfo& request, const std::string& inputInfo)
{
    if (m_dataProcessed)
        return;

    try
    {
        request.setPostData(inputInfo);
        std::string response = request.send();

        request.processRawElectricityData(response);
        request.formatElectricityData(response);

        m_addressEntered = false;
        m_addressSent = true;
        m_errorMessage.clear();

        // Save user input to the database only when no exceptions are thrown and the user info doesn't already exist
        if (!m_dbManager.userInfoExist(inputInfo)) {
            m_dbManager.saveUserInfo(inputInfo);
        }

        m_dataProcessed = true;
    }
    catch (const std::runtime_error& e)
    {
        std::cerr << "Error: " << e.what() << std::endl;

        if (std::string(e.what()).find("Received empty data") != std::string::npos)
            m_errorMessage = "Received empty data. Please check the correctness of your input or try again later.";
        else
            m_errorMessage = "An error occurred. Please try again later.";

        m_addressEntered = true;
    }
}

void Application::m_drawCircles(const ShutdownInfo& request, const Font& font) const
{
    DrawText("Today", 100, 100, 50, BLACK);
    ShutdownCircle circleToday({300, 450}, 270, 100, request.getWillBeElectricityToday(), request.getMightBeElectricityToday(), request.getWontBeElectricityToday(), font, request.getQueue(), request.getSubqueue());
    circleToday.drawHourSegments();

    DrawText("Tomorrow", 850, 100, 50, BLACK);
    ShutdownCircle circleTomorrow({900, 450}, 270, 100, request.getWillBeElectricityTomorrow(), request.getMightBeElectricityTomorrow(), request.getWontBeElectricityTomorrow(), font, request.getQueue(), request.getSubqueue());
    circleTomorrow.drawHourSegments();
}

void Application::run()
{
    std::vector<std::string> allUserInfo;
    std::string allUserInfoStr;
    Rectangle bounds = { 10, 100, 200, 300 };
    int scrollIndex = 0;

    while (!WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(RAYWHITE);

        handleInput();
        drawUI();
        handleListView();

        if (!m_errorMessage.empty())
            DrawText(m_errorMessage.c_str(), 10, WINDOW_HEIGHT - 30, 20, RED);

        EndDrawing();
    }
}

void Application::handleInput()
{
    if (m_addressSent)
    {
        if (GuiButton((Rectangle){ 10, WINDOW_HEIGHT - 60, 200, 30 }, "Go back to data input"))
        {
            m_addressSent = false;
            m_addressEntered = false;
            m_errorMessage.clear();
            m_dataProcessed = false;
            m_displayListView = false;
            m_listViewActive = -1;

            memset(m_info, 0, sizeof(m_info));
            m_request = ShutdownInfo();
        }
    }
    else
    {
        bool inputHandled = m_handleUserInput(m_info);

        if (inputHandled && IsKeyPressed(KEY_ENTER))
        {
            // Only process user input and retrieve user input from the database when the user input is valid
            if (m_errorMessage.empty())
            {
                std::string inputInfo = m_dbManager.getUserInfo(1);  // Assuming '1' as the user ID
                m_processData(m_request, inputInfo);
            }
        }

        if (GuiButton((Rectangle){ 10, WINDOW_HEIGHT - 60, 200, 30 }, "Use saved user info"))
        {
            m_displayListView = true;
            m_listViewActive = -1;
            m_allUserInfo = m_dbManager.getAllUserInfo();
            m_allUserInfoStr.clear();
            for (const std::string& userInfo : m_allUserInfo) {
                m_allUserInfoStr += userInfo + ";";
            }
        }
    }
}

void Application::drawUI()
{
    if (m_addressSent)
    {
        if (m_errorMessage.empty() && m_dataProcessed)
            m_drawCircles(m_request, m_lexendFont);
    }
}

void Application::handleListView()
{
    Rectangle bounds = { 10, 100, 200, 300 };
    int scrollIndex = 0;

    if (m_displayListView)
    {
        GuiListView(bounds, m_allUserInfoStr.c_str(), &scrollIndex, &m_listViewActive);
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && m_listViewActive >= 0) {  // Only process the data when the left mouse button is pressed and an item is selected
            std::string selectedUserInfo = m_allUserInfo[m_listViewActive];
            m_processData(m_request, selectedUserInfo);
            m_displayListView = false;
        }

        if (GuiButton((Rectangle){ 240, WINDOW_HEIGHT - 60, 200, 30 }, "Hide saved user"))
            m_displayListView = false;
    }
}