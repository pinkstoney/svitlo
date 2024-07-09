#pragma once

#include "shutdown-info.h"
#include "shutdown-circle.h"
#include "electricity-data.h"
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
    void drawCircles(const ShutdownInfo& request, const Font& font) const;
    void drawText(const std::string& text, int posX, int posY, int fontSize, Color color) const;
    bool textBox(Rectangle bounds, char* text, int textSize, bool editMode);
    bool button(Rectangle bounds, const std::string& text) const;
    void listView(Rectangle bounds, const char* items, int* scrollIndex, int* active);
    void loadFonts();
    Font getDiscoveryFont() const;
    Font getLexendFont() const;

    void drawInternetStatus(bool isConnected) const;
    void drawDataSavedTime(const std::string& savedTime) const;
    bool drawToggleSavedUserInfoButton(int windowHeight, bool isDisplayed) const;
    bool drawHomeButton(int i, bool isHome) const;
    bool drawDeleteButton(int i) const;
    void drawInputPrompt() const;
    bool drawBackButton(int windowHeight) const;

private:
    Font m_defaultFont;
    Font m_discoveryFont;
    Font m_lexendFont;
};
