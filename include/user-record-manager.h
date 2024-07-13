#pragma once

#include <string>
#include <vector>
#include <functional>

#include "database-manager.h"

class UserRecordManager
{
public:
    UserRecordManager(DatabaseManager& dbManager);

public:
    void toggleSavedUserInfo(bool show);

    void updateAllUserInfo();
    bool hasUserInfo() const;
    bool isSavedUserInfoDisplayed() const;

public:
    const std::vector<std::pair<std::string, std::string>>& getAllUserInfo() const;
    std::string getAllUserInfoStr() const;
    void handleUserInfoSelection(int selectedIndex, const std::function<void(const std::string&)>& processDataCallback);

public:
    void processHomeButton(int index);
    void processDeleteButton(int index);

private:
    DatabaseManager& m_dbManager;
    std::vector<std::pair<std::string, std::string>> m_allUserInfo;
    std::string m_allUserInfoStr;
    bool m_isSavedUserInfoDisplayed;
};
