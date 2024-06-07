#include "../include/application.h"


Application::Application()
    : m_addressEntered(false), m_addressSent(false)
{
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
    if (!m_addressSent)
    {
        DrawText("Enter data:", 10, 30, 20, BLACK);
        m_addressEntered = GuiTextBox((Rectangle){ 140, 23, 200, 30 }, m_info, 256, true);
    }

    if (m_addressEntered && strlen(m_info) == 0)
    {
        m_errorMessage = "Please enter some data before sending.";
        m_addressEntered = true;
        return false;
    }

    return true;
}
void Application::m_processData(ShutdownInfo& request, const std::string& inputInfo)
{
    try
    {
        request.setPostData(inputInfo);
        std::string response = request.send();

        request.processRawElectricityData(response);
        request.formatElectricityData(response);

        m_addressEntered = false;
        m_addressSent = true;
        m_errorMessage.clear();
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
    while (!WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(RAYWHITE);

        if (m_addressSent)
        {
            m_drawCircles(m_request, m_lexendFont);

            if (GuiButton((Rectangle){ 10, WINDOW_HEIGHT - 60, 200, 30 }, "Go back to data input"))
            {
                m_addressSent = false;
                m_addressEntered = false;
                m_errorMessage.clear();

                memset(m_info, 0, sizeof(m_info));
                m_request = ShutdownInfo();
            }
        }
        else
        {
            if (m_handleUserInput(m_info) && IsKeyPressed(KEY_ENTER))
                m_processData(m_request, m_info);
        }

        if (!m_errorMessage.empty())
            DrawText(m_errorMessage.c_str(), 10, WINDOW_HEIGHT - 30, 20, RED);

        EndDrawing();
    }
}