#include "../include/input-handler.h"


InputHandler::InputHandler(StateManager& stateManager, DatabaseManager& dbManager, UIManager& uiManager)
    : m_stateManager(stateManager)
    , m_dbManager(dbManager)
    , m_uiManager(uiManager)
    , m_dataListCurrentActive(-1)
{
    m_inputBuffer.fill(0);
}

void InputHandler::processInput()
{
    std::cout << "before appstate::input" << "\n ";
    if (m_stateManager.getCurrentState() == AppState::INPUT)
    {
    std::cout << "inside if\n";
        Rectangle inputBox = { 140, 23, 200, 30 };
        if (m_uiManager.isTextBoxActive(inputBox, m_inputBuffer.data(), m_inputBuffer.size()))
        {
            m_stateManager.setAddressEntered(true);
        }

        if (IsKeyPressed(KEY_ENTER) && m_stateManager.isAddressEntered())
        {
            m_stateManager.setCurrentState(AppState::DISPLAYING_RESULTS);
        }

        processSavedUserInfo();
    }
    else if (m_stateManager.getCurrentState() == AppState::DISPLAYING_RESULTS)
    {
        std::cout << "inside elseif displaying results\n";
        Rectangle backButton = { 990, static_cast<float>(UIManager::WINDOW_HEIGHT - 60), 200, 30 };
        if (m_uiManager.isButtonPressed(backButton, "Go back to data input"))
        {
            m_stateManager.reset();
        }
    }
}

std::string InputHandler::getInputInfo() const
{
    return std::string(m_inputBuffer.data());
}

void InputHandler::clearInput()
{
    m_inputBuffer.fill(0);
}

void InputHandler::processSavedUserInfo()
{
    if (!m_dbManager.isDatabaseEmpty() && !m_stateManager.isDataProcessed())
    {
        Rectangle savedInfoButton = { 10, static_cast<float>(UIManager::WINDOW_HEIGHT - 60), 200, 30 };
        if (m_uiManager.isButtonPressed(savedInfoButton, "Use saved user info"))
        {
            m_stateManager.setDataProcessed(true);
            m_dataListCurrentActive = -1;
            m_allUserInfo = m_dbManager.getAllUserInfo();
        }
    }

    if (m_stateManager.isDataProcessed())
    {
        processHomeButtons();
        processDeleteUserInfo();
        processHideSavedUserButton();
    }
}

void InputHandler::processHomeButtons()
{
    std::string homeUserInfo = m_dbManager.getHomeUserInfo();
    for (size_t i = 0; i < m_allUserInfo.size(); ++i)
    {
        Rectangle button = { 330, static_cast<float>(100 + i * 30), 150, 30 };
        if (m_allUserInfo[i].first == homeUserInfo)
        {
            if (m_uiManager.isButtonPressed(button, "Remove as Home"))
            {
                m_dbManager.removeHomeUserInfo();
            }
        }
        else
        {
            if (m_uiManager.isButtonPressed(button, "Set as Home"))
            {
                m_dbManager.setHomeUserInfo(m_allUserInfo[i].first);
            }
        }
    }
}

void InputHandler::processDeleteUserInfo()
{
    for (size_t i = 0; i < m_allUserInfo.size(); ++i)
    {
        Rectangle button = { 220, static_cast<float>(100 + i * 30), 100, 30 };
        if (m_uiManager.isButtonPressed(button, "Delete"))
        {
            m_dbManager.deleteUserInfo(m_allUserInfo[i].first);
            m_allUserInfo = m_dbManager.getAllUserInfo();
        }
    }
}

void InputHandler::processHideSavedUserButton()
{
    Rectangle hideButton = { 240, static_cast<float>(UIManager::WINDOW_HEIGHT - 60), 200, 30 };
    if (m_uiManager.isButtonPressed(hideButton, "Hide saved user info"))
    {
        m_stateManager.setDataProcessed(false);
    }
}
