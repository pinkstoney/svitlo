#include "../include/ui-manager.h"

#include "../include/electricity-data.h"
#include "../include/shutdown-circle.h"

UIManager::~UIManager()
{
    GuiSetFont(GetFontDefault());

    UnloadFont(m_fixelMediumFont);
    UnloadFont(m_FixelBoldFont);
}

void UIManager::drawCircles(const ShutdownData &request, const Font &font) const
{
    DrawTextEx(m_fixelMediumFont, "Сьогодні", {100.0f, 100.0f}, 50.0f, 1.0f, BLACK);   
    ElectricityData dataToday(request.getWillBeElectricityToday(), request.getMightBeElectricityToday(), request.getWontBeElectricityToday(), request.getQueue(), request.getSubqueue());
    ShutdownCircle circleToday({300, 450}, 270, 100, dataToday, font);
    circleToday.drawHourSegments();

    DrawTextEx(m_fixelMediumFont, "Завтра", {900.0f, 100.0f}, 50.0f, 1.0f, BLACK);   
    ElectricityData dataTomorrow(request.getWillBeElectricityTomorrow(), request.getMightBeElectricityTomorrow(), request.getWontBeElectricityTomorrow(), request.getQueue(), request.getSubqueue());
    ShutdownCircle circleTomorrow({900, 450}, 270, 100, dataTomorrow, font);
    circleTomorrow.drawHourSegments();
}

void UIManager::drawText(const std::string &text, float posX, float posY, float fontSize, Color color) const 
{
    DrawTextEx(m_defaultFont, text.c_str(), { posX, posY }, fontSize, 1.0f, color ); 
}

bool UIManager::textBox(Rectangle bounds, char *text, int textSize, bool editMode) 
{
    return GuiTextBox(bounds, text, textSize, editMode);
}

bool UIManager::button(Rectangle bounds, const std::string &text) const 
{
    return GuiButton(bounds, text.c_str());
}

void UIManager::listView(Rectangle bounds, const char *items, int *scrollIndex, int *active) 
{
    GuiListView(bounds, items, scrollIndex, active);
}

void UIManager::loadFonts() 
{
    m_defaultFont = GetFontDefault();
    std::vector<int> codepoints;

    // Basic Latin (0x0000-0x007F)
    for (int i = 32; i <= 126; i++)
        codepoints.push_back(i);

    // Cyrillic (0x0400-0x04FF)
    for (int i = 0x0400; i <= 0x04FF; i++)
        codepoints.push_back(i);

    // Ukrainian-specific letters
    std::vector<int> ukrainianSpecific = {0x0404, 0x0406, 0x0407, 0x0490, 0x0454, 0x0456, 0x0457, 0x0491};

    codepoints.insert(codepoints.end(), ukrainianSpecific.begin(), ukrainianSpecific.end());

    m_fixelMediumFont = LoadFontEx("../res/fonts/FixelText-Medium.otf", 30, codepoints.data(), static_cast<int>(codepoints.size())); GuiSetFont(m_fixelMediumFont);

    m_FixelBoldFont = LoadFont("../res/fonts/FixelText-Bold.otf");
}

Font UIManager::getFixelMediumFont() const { return m_fixelMediumFont; }

Font UIManager::getFixelBoldFont() const { return m_FixelBoldFont; }

void UIManager::drawInternetStatus(bool isConnected) const 
{
    const char *statusText = isConnected ? "Online" : "Offline";
    Color statusColor = isConnected ? DARKGREEN : CUSTOM_RED;

    drawText(statusText, 575, 30, 20, statusColor);
}

void UIManager::drawDataSavedTime(const std::string &savedTime) const 
{
    DrawTextEx(m_fixelMediumFont, "Актуально на: ", {10.0f, 30.0f}, 20.0f, 1.0f, BLACK);   
    drawText(savedTime, 150, 30, 20, BLACK);
}

bool UIManager::drawToggleSavedUserInfoButton(int windowHeight, bool isDisplayed) const 
{
    const char *buttonText = isDisplayed ? "Приховати недавно вибранi" : "Показати надовно вибрані";
    return button({10, static_cast<float>(windowHeight - 60), 250, 30}, buttonText);
}

bool UIManager::drawHomeButton(int i, bool isHome) const 
{
    const char *buttonText = isHome ? "Видалити дім" : "Додати дім";
    return button({870, static_cast<float>(390 + i * 30), 150, 30}, buttonText);
}

bool UIManager::drawDeleteButton(int i) const 
{
    return button({1040, static_cast<float>(390 + i * 30), 100, 30}, "Видалити");
}

void UIManager::drawInputPrompt() const 
{
    DrawTextEx(m_fixelMediumFont, "Пошук: ", {30.0f, 170.0f}, 20.0f, 1.0f, BLACK);
}

void UIManager::drawInputHint() const 
{
    DrawTextEx(m_fixelMediumFont,
            "              Введіть o/p (наприклад 25012345) або\n"
            "адресу (наприклад Івано-Франківськ,Індустріальна,34) ",
            {400.0f, 235.0f}, 15.0f, 1.0f, GRAY);
}

bool UIManager::drawBackButton(int windowHeight) const 
{
    return button({990, static_cast<float>(windowHeight - 60), 200, 30}, "Назад до вибору");
}
