#pragma once

#include "raygui.h"

#include "../include/shutdown-circle.h"
#include "../include/shutdown-info.h"

class ShutdownInfo;

class UIManager
        {
public:
    UIManager() = default;
    ~UIManager();

public:
    void drawCircles(const ShutdownInfo &request, const Font &font) const;
    void drawText(const std::string &text, int posX, int posY, int fontSize, Color color) const;
    bool textBox(Rectangle bounds, char *text, int textSize, bool editMode);
    bool button(Rectangle bounds, const std::string &text);
    void listView(Rectangle bounds, const char *items, int *scrollIndex, int *active);

public:
    void loadFonts();
    Font getDefaultFont() const;
    Font getDiscoveryFont() const;
    Font getLexendFont() const;

private:
    Font m_defaultFont;
    Font m_discoveryFont;
    Font m_lexendFont;
};

