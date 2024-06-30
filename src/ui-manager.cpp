#include "../include/ui-manager.h"

UIManager::~UIManager()
{
    GuiSetFont(GetFontDefault());

    UnloadFont(m_discoveryFont);
    UnloadFont(m_lexendFont);
}

void UIManager::drawCircles(const ShutdownInfo& request, const Font& font) const
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

bool UIManager::button(Rectangle bounds, const std::string& text)
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