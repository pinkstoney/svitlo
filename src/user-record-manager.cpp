#include "../include/user-record-manager.h"

UserRecordManager::UserRecordManager(DatabaseManager& dbManager)
    : m_dbManager(dbManager), m_isSavedUserInfoDisplayed(false) {}

void UserRecordManager::toggleSavedUserInfo(bool show) 
{
    m_isSavedUserInfoDisplayed = show;
    if (show) 
        updateAllUserInfo();
}

void UserRecordManager::updateAllUserInfo() 
{
    m_allUserInfo = m_dbManager.getAllUserInfo();
    m_allUserInfoStr.clear();
    for (const auto& info : m_allUserInfo) 
        m_allUserInfoStr += info.first + "\n";
}

bool UserRecordManager::hasUserInfo() const 
{
    return !m_allUserInfo.empty();
}

bool UserRecordManager::isSavedUserInfoDisplayed() const 
{
    return m_isSavedUserInfoDisplayed;
}

const std::vector<std::pair<std::string, std::string>>& UserRecordManager::getAllUserInfo() const 
{
    return m_allUserInfo;
}

std::string UserRecordManager::getAllUserInfoStr() const 
{
    return m_allUserInfoStr;
}

void UserRecordManager::handleUserInfoSelection(int selectedIndex, const std::function<void(const std::string&)>& processDataCallback) 
{
    if (selectedIndex >= 0 && selectedIndex < m_allUserInfo.size()) 
    {
        std::string selectedUserInfo = m_allUserInfo[selectedIndex].first;
        processDataCallback(selectedUserInfo);
        m_isSavedUserInfoDisplayed = false;
    }
}

void UserRecordManager::processHomeButton(int index) 
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

void UserRecordManager::processDeleteButton(int index) 
{
    if (index >= 0 && index < m_allUserInfo.size())
    {
        m_dbManager.deleteUserInfo(m_allUserInfo[index].first);
        updateAllUserInfo();
    }
}
