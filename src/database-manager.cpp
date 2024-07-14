#include "../include/database-manager.h"

#include <stdexcept>
#include <sstream>
#include <iomanip>
#include <chrono>

DatabaseManager::DatabaseManager(std::string dbPath) 
    : m_dbPath(std::move(dbPath)), m_db(nullptr, sqlite3_close)
{
    initDatabase();
}

void DatabaseManager::initDatabase() 
{
    sqlite3* db;
    if (sqlite3_open(m_dbPath.c_str(), &db) != SQLITE_OK) 
        throw std::runtime_error("Failed to open database: " + std::string(sqlite3_errmsg(db)));
    m_db.reset(db);

    const char* userInfoSql = R"(
        CREATE TABLE IF NOT EXISTS UserInfo(
            ID INTEGER PRIMARY KEY AUTOINCREMENT,
            INFO TEXT NOT NULL,
            IS_HOME INTEGER DEFAULT 0,
            SAVED_TIME TEXT
        )
    )";

    const char* electricityInfoSql = R"(
        CREATE TABLE IF NOT EXISTS ElectricityInfo(
            ID INTEGER PRIMARY KEY AUTOINCREMENT,
            INFO TEXT NOT NULL,
            DATE TEXT,
            HOUR INTEGER,
            STATUS INTEGER,
            QUEUE INTEGER,
            SUBQUEUE INTEGER,
            IS_TOMORROW INTEGER DEFAULT 0
        )
    )";

    executeSql(userInfoSql);
    executeSql(electricityInfoSql);
}

std::string DatabaseManager::constructFinalSql(const std::string& sql, const std::vector<std::string>& params) const {
    std::string finalSql = sql;
    for (const auto& param : params) {
        size_t pos = finalSql.find('?');
        if (pos != std::string::npos) {
            finalSql.replace(pos, 1, "'" + param + "'");
        }
    }
    return finalSql;
}
void DatabaseManager::executeSql(const std::string& sql, 
                                 const std::vector<std::string>& params,
                                 const std::function<void(sqlite3_stmt*)>& rowCallback) const 
{
   // std::cout << constructFinalSql(sql, params) << std::endl;

    sqlite3_stmt* stmt = nullptr;
    
    if (sqlite3_prepare_v2(m_db.get(), sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK)
        throw std::runtime_error("Failed to prepare statement: " + std::string(sqlite3_errmsg(m_db.get())));

    auto guard = std::unique_ptr<sqlite3_stmt, decltype(&sqlite3_finalize)>(stmt, sqlite3_finalize);

    for (size_t i = 0; i < params.size(); ++i) 
    {
        if (sqlite3_bind_text(stmt, static_cast<int>(i + 1), params[i].c_str(), -1, SQLITE_TRANSIENT) != SQLITE_OK) 
            throw std::runtime_error("Failed to bind parameter " + std::to_string(i + 1) + ": " + 
                                     std::string(sqlite3_errmsg(m_db.get())));
    }

    int rc;
    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) 
    {
        if (rowCallback) 
            rowCallback(stmt);
    }

    if (rc != SQLITE_DONE) 
        throw std::runtime_error("Error executing statement: " + std::string(sqlite3_errmsg(m_db.get())));
}

void DatabaseManager::prepareAndExecute(const std::string& sql, const std::vector<std::string>& params) const
{
    //std::cout << constructFinalSql(sql, params) << std::endl;

    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(m_db.get(), sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK)
        throw std::runtime_error("Failed to prepare statement: " + std::string(sqlite3_errmsg(m_db.get())));

    auto guard = std::unique_ptr<sqlite3_stmt, decltype(&sqlite3_finalize)>(stmt, sqlite3_finalize);

    for (size_t i = 0; i < params.size(); ++i) 
    {
        if (sqlite3_bind_text(stmt, static_cast<int>(i + 1), params[i].c_str(), -1, SQLITE_TRANSIENT) != SQLITE_OK) 
            throw std::runtime_error("Failed to bind parameter: " + std::string(sqlite3_errmsg(m_db.get())));
    }

    if (sqlite3_step(stmt) != SQLITE_DONE) 
        throw std::runtime_error("Failed to execute statement: " + std::string(sqlite3_errmsg(m_db.get())));
}

void DatabaseManager::setHomeUserInfo(const std::string& info) 
{
    executeSql("UPDATE UserInfo SET IS_HOME = 0");
    prepareAndExecute("UPDATE UserInfo SET IS_HOME = 1 WHERE INFO = ?", {info});
}

void DatabaseManager::removeHomeUserInfo() 
{
    executeSql("UPDATE UserInfo SET IS_HOME = 0");
}

std::string DatabaseManager::getHomeUserInfo() const 
{
    std::string info;
    executeSql("SELECT INFO FROM UserInfo WHERE IS_HOME = 1", {}, [&info](sqlite3_stmt* stmt) 
    {
        info = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
    });
    return info;
}

void DatabaseManager::saveUserInfo(const std::string& info)
{
    if (info.empty()) 
        throw std::invalid_argument("User info is empty.");
    
    auto now = std::chrono::system_clock::now();
    auto now_c = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    std::tm tm = {};
    localtime_r(&now_c, &tm);
    ss << std::put_time(&tm, "%F %T");

    if (isUserInfoExist(info)) 
        prepareAndExecute("UPDATE UserInfo SET SAVED_TIME = ? WHERE INFO = ?", {ss.str(), info});
    else
        prepareAndExecute("INSERT INTO UserInfo (INFO, SAVED_TIME) VALUES (?, ?)", {info, ss.str()});
}

std::vector<std::pair<std::string, std::string>> DatabaseManager::getAllUserInfo() const 
{
    std::vector<std::pair<std::string, std::string>> allInfo;
    executeSql("SELECT INFO, SAVED_TIME FROM UserInfo", {}, [&allInfo](sqlite3_stmt* stmt) 
    {
        allInfo.emplace_back(
            reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0)),
            reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1))
        );
    });
    return allInfo;
}

std::string DatabaseManager::getUserInfo(int id) const 
{
    std::string info;
    executeSql("SELECT INFO FROM UserInfo WHERE ID = ?", {std::to_string(id)}, [&info](sqlite3_stmt* stmt) 
    {
        info = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
    });
    return info;
}

bool DatabaseManager::isUserInfoExist(const std::string& info) const 
{
    int exists = 0;
    executeSql("SELECT EXISTS(SELECT 1 FROM UserInfo WHERE INFO = ?)", {info}, [&exists](sqlite3_stmt* stmt) 
    {
        exists = sqlite3_column_int(stmt, 0);
    });
    return exists == 1;
}

void DatabaseManager::deleteUserInfo(const std::string& info) 
{
    prepareAndExecute("DELETE FROM UserInfo WHERE INFO = ?", {info});
    prepareAndExecute("DELETE FROM ElectricityInfo WHERE INFO = ?", {info});
}

bool DatabaseManager::isDatabaseEmpty() const 
{
    int count = 0;
    executeSql("SELECT COUNT(*) FROM UserInfo", {}, [&count](sqlite3_stmt* stmt) 
    {
        count = sqlite3_column_int(stmt, 0);
    });
    return count == 0;
}

void DatabaseManager::saveElectricityInfo(const std::string& info, const std::string& date, int hour, int status, int queue, int subqueue, bool isTomorrow) 
{
    int exists = 0;
    executeSql("SELECT EXISTS(SELECT 1 FROM ElectricityInfo WHERE INFO = ? AND HOUR = ? AND IS_TOMORROW = ?)", 
               {info, std::to_string(hour), std::to_string(isTomorrow)}, 
               [&exists](sqlite3_stmt* stmt) 
    {
        exists = sqlite3_column_int(stmt, 0);
    });

    if (exists == 1) 
    {
        prepareAndExecute(
            "UPDATE ElectricityInfo SET DATE = ?, STATUS = ?, QUEUE = ?, SUBQUEUE = ? WHERE INFO = ? AND HOUR = ? AND IS_TOMORROW = ?",
            {date, std::to_string(status), std::to_string(queue), std::to_string(subqueue), info, std::to_string(hour), std::to_string(isTomorrow)}
        );
    } 
    else 
    {
        prepareAndExecute(
            "INSERT INTO ElectricityInfo (INFO, DATE, HOUR, STATUS, QUEUE, SUBQUEUE, IS_TOMORROW) VALUES (?, ?, ?, ?, ?, ?, ?)",
            {info, date, std::to_string(hour), std::to_string(status), std::to_string(queue), std::to_string(subqueue), std::to_string(isTomorrow)}
        );
    }
}

std::vector<std::tuple<std::string, int, int, int, int>> DatabaseManager::getElectricityInfo(const std::string& info, bool isTomorrow) const 
{
    std::vector<std::tuple<std::string, int, int, int, int>> electricityInfo;
    std::string sql = "SELECT DATE, HOUR, STATUS, QUEUE, SUBQUEUE FROM ElectricityInfo WHERE INFO = ? AND IS_TOMORROW = ?";
    
    executeSql(sql, {info, std::to_string(isTomorrow)}, [&electricityInfo](sqlite3_stmt* stmt) 
    {
        electricityInfo.emplace_back(
            reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0)),
            sqlite3_column_int(stmt, 1),
            sqlite3_column_int(stmt, 2),
            sqlite3_column_int(stmt, 3),
            sqlite3_column_int(stmt, 4)
        );
    });
    
    return electricityInfo;
}
