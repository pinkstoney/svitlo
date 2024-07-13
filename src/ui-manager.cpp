#include "../include/ui-manager.h"

#include "../include/shutdown-circle.h"
#include "../include/electricity-data.h"

UIManager::~UIManager()
{
    GuiSetFont(GetFontDefault());

    UnloadFont(m_discoveryFont);
    UnloadFont(m_lexendFont);
}

void UIManager::drawCircles(const ShutdownData& request, const Font& font) const
{
    DrawText("Today", 100, 100, 50, BLACK);
    ElectricityData dataToday(request.getWillBeElectricityToday(), request.getMightBeElectricityToday(), request.getWontBeElectricityToday(), request.getQueue(), request.getSubqueue());
    ShutdownCircle circleToday({300, 450}, 270, 100, dataToday, font);
    circleToday.drawHourSegments();

    DrawText("Tomorrow", 850, 100, 50, BLACK);
    ElectricityData dataTomorrow(request.getWillBeElectricityTomorrow(), request.getMightBeElectricityTomorrow(), request.getWontBeElectricityTomorrow(), request.getQueue(), request.getSubqueue());
    ShutdownCircle circleTomorrow({900, 450}, 270, 100, dataTomorrow, font);
    circleTomorrow.drawHourSegments();
}

void UIManager::drawText(const std::string& text, int posX, int posY, int fontSize, Color color) const
{
    DrawTextEx(m_defaultFont, text.c_str(), { static_cast<float>(posX), static_cast<float>(posY) }, static_cast<float>(fontSize), 1.0f, color);
}

bool UIManager::textBox(Rectangle bounds, char* text, int textSize, bool editMode)
{
    return GuiTextBox(bounds, text, textSize, editMode);
}

bool UIManager::button(Rectangle bounds, const std::string& text) const
{
    return GuiButton(bounds, text.c_str());
}

void UIManager::listView(Rectangle bounds, const char* items, int* scrollIndex, int* active)
{
    GuiListView(bounds, items, scrollIndex, active);
}

void UIManager::loadFonts()
{
    m_defaultFont = GetFontDefault();

    std::vector<int> codepoints(512, 0);
    for (int i = 0; i < 95; i++) codepoints[i] = 32 + i;
    for (int i = 0; i < 255; i++) codepoints[96 + i] = 0x400 + i;
    m_discoveryFont = LoadFontEx("../res/fonts/discovery.otf", 30, codepoints.data(), static_cast<int>(codepoints.size()));

    GuiSetFont(m_discoveryFont);

    m_lexendFont = LoadFont("../res/fonts/Lexend/static/Lexend-Bold.ttf");
}

Font UIManager::getDiscoveryFont() const
{
    return m_discoveryFont;
}

Font UIManager::getLexendFont() const
{
    return m_lexendFont;
}

void UIManager::drawInternetStatus(bool isConnected) const
{
    const char* statusText = isConnected ? "Online" : "Offline";
    Color statusColor = isConnected ? DARKGREEN : CUSTOM_RED;
    drawText(statusText, 575, 30, 20, statusColor);
}

void UIManager::drawDataSavedTime(const std::string& savedTime) const 
{
    drawText("Data saved on: " + savedTime, 10, 30, 20, BLACK);
}

bool UIManager::drawToggleSavedUserInfoButton(int windowHeight, bool isDisplayed) const
{
    const char* buttonText = isDisplayed ? "Hide saved user info" : "Use saved user info";
    return button({10, static_cast<float>(windowHeight - 60), 200, 30}, buttonText);
}

bool UIManager::drawHomeButton(int i, bool isHome) const
{
    const char* buttonText = isHome ? "Remove as Home" : "Set as Home";
    return button({330, static_cast<float>(100 + i * 30), 150, 30}, buttonText);
}

bool UIManager::drawDeleteButton(int i) const
{
    return button({220, static_cast<float>(100 + i * 30), 100, 30}, "Delete");
}

void UIManager::drawInputPrompt() const 
{
    drawText("Enter data:", 10, 30, 20, BLACK);
}

bool UIManager::drawBackButton(int windowHeight) const 
{
    return button({990, static_cast<float>(windowHeight - 60), 200, 30}, "Go back to data input");
}
