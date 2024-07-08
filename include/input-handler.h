#pragma once

#include <string>
#include <array>
#include <algorithm>

#include "state-manager.h"
#include "database-manager.h"
#include "ui-manager.h"

#include <raylib.h>
#include <raygui.h>

class InputHandler {
public:
    InputHandler(StateManager& stateManager, DatabaseManager& dbManager, UIManager& uiManager);

    void processInput();
    std::string getInputInfo() const;
    void clearInput();

private:
    void processSavedUserInfo();
    void processHomeButtons();
    void processDeleteUserInfo();
    void processHideSavedUserButton();

    StateManager& m_stateManager;
    DatabaseManager& m_dbManager;
    UIManager& m_uiManager;

    std::array<char, 256> m_inputBuffer;
    std::vector<std::pair<std::string, std::string>> m_allUserInfo;
    int m_dataListCurrentActive;
};
