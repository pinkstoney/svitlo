#pragma once

#include "raylib.h"
#include <vector>
#include <tuple>
#include "shutdown-info.h"
#include "shutdown-circle.h"
#include "raygui.h"

const int WINDOW_WIDTH = 1200;
const int WINDOW_HEIGHT = 800;
const int TARGET_FPS = 60;

class Application
{
public:
    Application();
    ~Application();

public:
    void run();

private:
    void m_initializeWindow() const;
    std::tuple<Font, Font, Font> m_loadFonts() const;
    bool m_handleUserInput(char* info);
    void m_processData(ShutdownInfo& request, const std::string& inputInfo);
    void m_drawCircles(const ShutdownInfo& request, const Font& font) const;

private:
    ShutdownInfo m_request;
    char m_info[256];

private:
    bool m_addressEntered;
    bool m_addressSent;

    std::string m_errorMessage;

private:
    Font m_defaultFont;
    Font m_discoveryFont;
    Font m_lexendFont;
};