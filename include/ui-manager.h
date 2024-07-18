#pragma once

#include "shutdown-data.h"

#include <raylib.h>
#include <raygui.h>

constexpr float ANGLE_PER_HOUR = 360.0f / 24.0f;
constexpr Color CUSTOM_GREEN = {161, 221, 112, 255};
constexpr Color CUSTOM_ORANGE = {253, 208, 157, 255};
constexpr Color CUSTOM_RED = {238, 78, 78, 255};
constexpr Color DEFAULT_COLOR = LIGHTGRAY;

class UIManager
{
public:
    ~UIManager();

public:
    void drawCircles(const ShutdownData& request, const Font& font) const;

    void drawText(const std::string& text, float posX, float posY, float fontSize, Color color) const;
    bool textBox(Rectangle bounds, char* text, int textSize, bool editMode);

    bool button(Rectangle bounds, const std::string& text) const;

    void listView(Rectangle bounds, const char* items, int* scrollIndex, int* active);

public:
    void loadFonts();
    Font getFixelMediumFont() const;
    Font getFixelBoldFont() const;

public:
    void drawInternetStatus(bool isConnected) const;
    void drawDataSavedTime(const std::string& savedTime) const;
    bool drawToggleSavedUserInfoButton(int windowHeight, bool isDisplayed) const;

    bool drawHomeButton(int i, bool isHome) const;
    bool drawDeleteButton(int i) const;
    bool drawBackButton(int windowHeight) const;

    void drawInputPrompt() const;
    void drawInputHint() const;

private:
    Font m_defaultFont;
    Font m_fixelMediumFont;
    Font m_FixelBoldFont;
};
