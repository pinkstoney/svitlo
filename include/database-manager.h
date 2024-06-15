#pragma once

#include <string>
#include <vector>

#include <sqlite3.h>

class DatabaseManager
{
public:
    explicit DatabaseManager(const std::string& dbPath);
    ~DatabaseManager();

public:
    void init();
    void executeSql(const std::string& sql, int (*callback)(void*, int, char**, char**), void* data);

public:
    bool isUserInfoExist(const std::string& info);
    bool isDatabaseEmpty();

    void saveUserInfo(const std::string& info);
    void deleteUserInfo(const std::string& info);

    std::string getUserInfo(int id);
    std::vector<std::string> getAllUserInfo();
    std::string getHomeUserInfo();

    void setHomeUserInfo(const std::string &info);
    void removeHomeUserInfo();

private:
    std::string m_dbPath;
    sqlite3 *m_db;
};