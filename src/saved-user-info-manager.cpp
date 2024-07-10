#include "../include/saved-user-info-manager.h"

SavedUserInfoManager::SavedUserInfoManager(DatabaseManager& dbManager)
    : m_dbManager(dbManager), m_isSavedUserInfoDisplayed(false) {}

void SavedUserInfoManager::toggleSavedUserInfo(bool show) 
{
    m_isSavedUserInfoDisplayed = show;
    if (show) 
        updateAllUserInfo();
}

void SavedUserInfoManager::updateAllUserInfo() 
{
    m_allUserInfo = m_dbManager.getAllUserInfo();
    m_allUserInfoStr.clear();
    for (const auto& info : m_allUserInfo) 
        m_allUserInfoStr += info.first + "\n";
}

bool SavedUserInfoManager::hasUserInfo() const 
{
    return !m_allUserInfo.empty();
}

bool SavedUserInfoManager::isSavedUserInfoDisplayed() const 
{
    return m_isSavedUserInfoDisplayed;
}

const std::vector<std::pair<std::string, std::string>>& SavedUserInfoManager::getAllUserInfo() const 
{
    return m_allUserInfo;
}

std::string SavedUserInfoManager::getAllUserInfoStr() const 
{
    return m_allUserInfoStr;
}

void SavedUserInfoManager::handleUserInfoSelection(int selectedIndex, const std::function<void(const std::string&)>& processDataCallback) 
{
    if (selectedIndex >= 0 && selectedIndex < m_allUserInfo.size()) 
    {
        std::string selectedUserInfo = m_allUserInfo[selectedIndex].first;
        processDataCallback(selectedUserInfo);
        m_isSavedUserInfoDisplayed = false;
    }
}

void SavedUserInfoManager::processHomeButton(int index) 
{
    if (index >= 0 && index < m_allUserInfo.size())
    {
        std::string homeUserInfo = m_dbManager.getHomeUserInfo();
        bool isHome = (m_allUserInfo[index].first == homeUserInfo);
        if (isHome) 
            m_dbManager.removeHomeUserInfo();
        else 
            m_dbManager.setHomeUserInfo(m_allUserInfo[index].first);
    }
}

void SavedUserInfoManager::processDeleteButton(int index) 
{
    if (index >= 0 && index < m_allUserInfo.size())
    {
        m_dbManager.deleteUserInfo(m_allUserInfo[index].first);
        updateAllUserInfo();
    }
}
