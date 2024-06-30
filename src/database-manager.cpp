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
                          "IS_HOME INTEGER DEFAULT 0,"
                          "SAVED_TIME TEXT);";

        executeSql(sql, nullptr, nullptr);

        sql = "CREATE TABLE IF NOT EXISTS ElectricityInfo("
              "ID INTEGER PRIMARY KEY AUTOINCREMENT,"
              "INFO TEXT NOT NULL,"
              "DATE TEXT,"
              "HOUR INTEGER,"
              "STATUS INTEGER,"
              "QUEUE INTEGER,"
              "SUBQUEUE INTEGER);";

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

    auto now = std::chrono::system_clock::now();
    std::time_t now_c = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&now_c), "%F %T");

    if (isUserInfoExist(info))
    {
        std::string sql = "DELETE FROM UserInfo WHERE INFO='" + info + "';";
        executeSql(sql, nullptr, nullptr);
    }

    std::string sql = "INSERT INTO UserInfo (INFO, SAVED_TIME) VALUES ('" + info + "', '" + ss.str() + "');";
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

std::vector<std::pair<std::string, std::string>> DatabaseManager::getAllUserInfo()
{
    std::string sql = "SELECT INFO, SAVED_TIME FROM UserInfo;";

    std::vector<std::pair<std::string, std::string>> allInfo;

    auto callback = [](void *data, int argc, char **argv, char ** /*azColName*/)
    {
        auto allInfoPtr = static_cast<std::vector<std::pair<std::string, std::string>>*>(data);
        if (argc > 1 && argv[0] && argv[1])
            allInfoPtr->emplace_back(std::make_pair(argv[0], argv[1]));
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
void DatabaseManager::saveElectricityInfo(const std::string& info, const std::string& date, int hour, int status, int queue, int subqueue)
{
    std::string sql = "SELECT EXISTS(SELECT 1 FROM ElectricityInfo WHERE INFO='" + info + "' AND HOUR=" + std::to_string(hour) + ");";

    int exists = 0;

    auto callback = [](void *data, int argc, char **argv, char ** /*azColName*/)
    {
        auto existsPtr = static_cast<int*>(data);
        if (argc > 0 && argv[0])
            *existsPtr = std::stoi(argv[0]);
        return 0;
    };

    executeSql(sql, callback, &exists);

    if (exists == 1)
    {
        sql = "UPDATE ElectricityInfo SET DATE='" + date + "', STATUS=" + std::to_string(status) + ", QUEUE=" + std::to_string(queue) + ", SUBQUEUE=" + std::to_string(subqueue) + " WHERE INFO='" + info + "' AND HOUR=" + std::to_string(hour) + ";";
    }
    else
    {
        sql = "INSERT INTO ElectricityInfo (INFO, DATE, HOUR, STATUS, QUEUE, SUBQUEUE) VALUES ('" + info + "', '" + date + "', " + std::to_string(hour) + ", " + std::to_string(status) + ", " + std::to_string(queue) + ", " + std::to_string(subqueue) + ");";
    }

    executeSql(sql, nullptr, nullptr);
}

std::vector<std::tuple<std::string, int, int, int, int>> DatabaseManager::getElectricityInfo(const std::string& info, const std::string& date)
{
    std::cout << "INFO: " << info << std::endl;
    std::cout << "DATE: " << date << std::endl;

    std::string sql = "SELECT INFO, HOUR, STATUS, QUEUE, SUBQUEUE FROM ElectricityInfo WHERE INFO='" + info + "';";
    std::cout << "Executing SQL: " << sql << std::endl;

    std::vector<std::tuple<std::string, int, int, int, int>> electricityInfo;

    auto callback = [](void *data, int argc, char **argv, char ** /*azColName*/)
    {
        auto electricityInfoPtr = static_cast<std::vector<std::tuple<std::string, int, int, int, int>>*>(data);
        if (argc == 5 && argv[0] && argv[1] && argv[2] && argv[3] && argv[4])
        {
            std::string info = argv[0];
            int hour = std::stoi(argv[1]);
            int status = std::stoi(argv[2]);
            int queue = std::stoi(argv[3]);
            int subqueue = std::stoi(argv[4]);
            electricityInfoPtr->emplace_back(info, hour, status, queue, subqueue);
        }
        return 0;
    };

    executeSql(sql, callback, &electricityInfo);

    std::cout << "Query Results: " << std::endl;
    for (const auto& entry : electricityInfo)
    {
        std::cout << "Info: " << std::get<0>(entry) << ", Hour: " << std::get<1>(entry) << ", Status: " << std::get<2>(entry) << ", Queue: " << std::get<3>(entry) << ", Subqueue: " << std::get<4>(entry) << std::endl;
    }

    return electricityInfo;
}