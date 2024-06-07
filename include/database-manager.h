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
    void initialize();
    void executeSql(const std::string& sql, int (*callback)(void*, int, char**, char**), void* data);

public:
    bool userInfoExist(const std::string& info);
    void saveUserInfo(const std::string& info);
    std::string getUserInfo(int id);
    std::vector<std::string> getAllUserInfo();

private:
    std::string m_dbPath;
    sqlite3 *m_db;
};