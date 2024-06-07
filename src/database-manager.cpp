#include <string>
#include <iostream>

#include <sqlite3.h>

#include "../include/database-manager.h"

DatabaseManager::DatabaseManager(const std::string& dbPath)
        : m_dbPath(dbPath), m_db(nullptr) {}

DatabaseManager::~DatabaseManager()
{
    if (m_db)
        sqlite3_close(m_db);
}

void DatabaseManager::initialize()
{
    if (int rc = sqlite3_open(m_dbPath.c_str(), &m_db); rc)
    {
        std::cerr << "Error: Failed to open database. " << sqlite3_errmsg(m_db) << std::endl;
        return;
    }
    else
    {
        char *errMsg = nullptr;

        std::string sql = "CREATE TABLE IF NOT EXISTS UserInfo("
                          "ID INTEGER PRIMARY KEY AUTOINCREMENT,"
                          "INFO TEXT NOT NULL);";

        executeSql(sql, nullptr, nullptr);
    }
}

void DatabaseManager::executeSql(const std::string& sql, int (*callback)(void*, int, char**, char**), void* data)
{
    char *errMsg = nullptr;

    if (int rc = sqlite3_exec(m_db, sql.c_str(), callback, data, &errMsg); rc != SQLITE_OK)
    {
        std::cerr << "Error: Failed to execute SQL statement. " << errMsg << std::endl;
        sqlite3_free(errMsg);
    }
}

void DatabaseManager::saveUserInfo(const std::string& info)
{
    if (info.empty())
    {
        std::cerr << "Error: User info is empty." << std::endl;
        return;
    }

    std::string sql = "INSERT INTO UserInfo (INFO) VALUES ('" + info + "');";
    executeSql(sql, nullptr, nullptr);
}

std::string DatabaseManager::getUserInfo(int id)
{
    std::string sql = "SELECT INFO FROM UserInfo WHERE ID = " + std::to_string(id) + ";";

    std::string info;
    auto callback = [](void *data, int argc, char **argv, char ** /*azColName*/)
    {
        auto infoPtr = static_cast<std::string *>(data);
        if (argc > 0 && argv[0])
            *infoPtr = argv[0];

        return 0;
    };

    executeSql(sql, callback, &info);
    return info;
}

bool DatabaseManager::userInfoExist(const std::string& info)
{
    std::string sql = "SELECT EXISTS(SELECT 1 FROM UserInfo WHERE INFO='" + info + "');";

    int exists = 0;

    auto callback = [](void *data, int argc, char **argv, char ** /*azColName*/)
    {
        auto existsPtr = static_cast<int*>(data);
        if (argc > 0 && argv[0])
            *existsPtr = std::stoi(argv[0]);
        return 0;
    };

    executeSql(sql, callback, &exists);
    return exists == 1;
}

std::vector<std::string> DatabaseManager::getAllUserInfo()
{
    std::string sql = "SELECT INFO FROM UserInfo;";

    std::vector<std::string> allInfo;

    auto callback = [](void *data, int argc, char **argv, char ** /*azColName*/)
    {
        auto allInfoPtr = static_cast<std::vector<std::string>*>(data);
        if (argc > 0 && argv[0])
            allInfoPtr->emplace_back(argv[0]);
        return 0;
    };

    executeSql(sql, callback, &allInfo);
    return allInfo;
}