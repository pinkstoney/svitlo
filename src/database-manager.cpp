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

void DatabaseManager::init()
{
    if (int rc = sqlite3_open(m_dbPath.c_str(), &m_db); rc)
    {
        std::cerr << "Error: Failed to open database. " << sqlite3_errmsg(m_db) << std::endl;
        return;
    }
    else
    {
        std::string sql = "CREATE TABLE IF NOT EXISTS UserInfo("
                          "ID INTEGER PRIMARY KEY AUTOINCREMENT,"
                          "INFO TEXT NOT NULL,"
                          "IS_HOME INTEGER DEFAULT 0);";

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

void DatabaseManager::setHomeUserInfo(const std::string& info)
{
    std::string sql = "UPDATE UserInfo SET IS_HOME=0;";
    executeSql(sql, nullptr, nullptr);

    sql = "UPDATE UserInfo SET IS_HOME=1 WHERE INFO='" + info + "';";
    executeSql(sql, nullptr, nullptr);
}

void DatabaseManager::removeHomeUserInfo()
{
    std::string sql = "UPDATE UserInfo SET IS_HOME=0;";
    executeSql(sql, nullptr, nullptr);
}

std::string DatabaseManager::getHomeUserInfo()
{
    std::string sql = "SELECT INFO FROM UserInfo WHERE IS_HOME=1;";

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

void DatabaseManager::saveUserInfo(const std::string& info)
{
    if (info.empty())
    {
        std::cerr << "Error: User info is empty." << std::endl;
        return;
    }

    if (!isUserInfoExist(info))
    {
        std::string sql = "INSERT INTO UserInfo (INFO) VALUES ('" + info + "');";
        executeSql(sql, nullptr, nullptr);
    }

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

bool DatabaseManager::isUserInfoExist(const std::string& info)
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

void DatabaseManager::deleteUserInfo(const std::string& info)
{
    std::string sql = "DELETE FROM UserInfo WHERE INFO='" + info + "';";
    executeSql(sql, nullptr, nullptr);
}

bool DatabaseManager::isDatabaseEmpty()
{
    std::string sql = "SELECT COUNT(*) FROM UserInfo;";

    int count = 0;

    auto callback = [](void *data, int argc, char **argv, char ** /*azColName*/)
    {
        auto countPtr = static_cast<int*>(data);
        if (argc > 0 && argv[0])
            *countPtr = std::stoi(argv[0]);
        return 0;
    };

    executeSql(sql, callback, &count);
    return count == 0;
}