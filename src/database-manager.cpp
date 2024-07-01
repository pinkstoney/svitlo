#include "../include/database-manager.h"

DatabaseManager::DatabaseManager(std::string dbPath)
        : m_dbPath(std::move(dbPath)), m_db(nullptr, sqlite3_close)
{
    init();
}

DatabaseManager::~DatabaseManager() noexcept = default;

void DatabaseManager::init()
{
    sqlite3* db;
    if (int rc = sqlite3_open(m_dbPath.c_str(), &db); rc != SQLITE_OK)
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
            SUBQUEUE INTEGER
        )
    )";

    m_executeSql(userInfoSql, nullptr, nullptr);
    m_executeSql(electricityInfoSql, nullptr, nullptr);
}

void DatabaseManager::m_executeSql(const std::string& sql, const std::function<int(void*, int, char**, char**)>& callback, void* userData) const
{
    char* errMsg = nullptr;
    int rc;

    auto wrapperCallback = [](void* data, int argc, char** argv, char** azColName)
    {
        auto* params = static_cast<std::pair<const std::function<int(void*, int, char**, char**)>*, void*>*>(data);
        return (*(params->first))(params->second, argc, argv, azColName);
    };

    if (callback)
    {
        auto params = std::make_pair(&callback, userData);
        rc = sqlite3_exec(m_db.get(), sql.c_str(), wrapperCallback, &params, &errMsg);
    }
    else
    {
        rc = sqlite3_exec(m_db.get(), sql.c_str(), nullptr, userData, &errMsg);
    }

    if (rc != SQLITE_OK)
    {
        std::string error = errMsg ? errMsg : "Unknown error";
        sqlite3_free(errMsg);
        throw std::runtime_error("SQL error: " + error);
    }
}

void DatabaseManager::m_prepareAndExecute(const std::string& sql, const std::vector<std::string>& params) const
{
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(m_db.get(), sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK)
        throw std::runtime_error("Failed to prepare statement: " + std::string(sqlite3_errmsg(m_db.get())));

    for (size_t i = 0; i < params.size(); ++i)
    {
        if (sqlite3_bind_text(stmt, static_cast<int>(i + 1), params[i].c_str(), -1, SQLITE_TRANSIENT) != SQLITE_OK)
        {
            sqlite3_finalize(stmt);
            throw std::runtime_error("Failed to bind parameter: " + std::string(sqlite3_errmsg(m_db.get())));
        }
    }

    if (sqlite3_step(stmt) != SQLITE_DONE)
    {
        sqlite3_finalize(stmt);
        throw std::runtime_error("Failed to execute statement: " + std::string(sqlite3_errmsg(m_db.get())));
    }

    sqlite3_finalize(stmt);
}

void DatabaseManager::setHomeUserInfo(const std::string& info) const
{
    m_prepareAndExecute("UPDATE UserInfo SET IS_HOME = 0", {});
    m_prepareAndExecute("UPDATE UserInfo SET IS_HOME = 1 WHERE INFO = ?", {info});
}

void DatabaseManager::removeHomeUserInfo() const
{
    m_prepareAndExecute("UPDATE UserInfo SET IS_HOME = 0", {});
}

std::string DatabaseManager::getHomeUserInfo() const
{
    std::string info;
    m_executeSql("SELECT INFO FROM UserInfo WHERE IS_HOME = 1",
               [](void* data, int argc, char** argv, char**)
               {
                   if (argc > 0 && argv[0])
                   {
                       *static_cast<std::string*>(data) = argv[0];
                   }
                   return 0;
               }, &info);
    return info;
}

void DatabaseManager::saveUserInfo(const std::string& info) const
{
    if (info.empty())
        throw std::invalid_argument("User info is empty.");

    auto now = std::chrono::system_clock::now();
    std::time_t now_c = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    std::tm tm = {};
    localtime_r(&now_c, &tm);
    ss << std::put_time(&tm, "%F %T");

    if (isUserInfoExist(info))
        m_prepareAndExecute("DELETE FROM UserInfo WHERE INFO = ?", {info});

    m_prepareAndExecute("INSERT INTO UserInfo (INFO, SAVED_TIME) VALUES (?, ?)", {info, ss.str()});
}

std::string DatabaseManager::getUserInfo(int id) const
{
    std::string info;
    m_executeSql("SELECT INFO FROM UserInfo WHERE ID = " + std::to_string(id),
               [](void* data, int argc, char** argv, char**)
               {
                   if (argc > 0 && argv[0])
                   {
                       *static_cast<std::string*>(data) = argv[0];
                   }
                   return 0;
               }, &info);
    return info;
}


bool DatabaseManager::isUserInfoExist(const std::string& info) const
{
    int exists = 0;
    std::string sql = "SELECT EXISTS(SELECT 1 FROM UserInfo WHERE INFO='" + info + "');";
    auto callback = [](void* data, int argc, char** argv, char**)
    {
        if (argc > 0 && argv[0])
        {
            *static_cast<int*>(data) = std::stoi(argv[0]);
        }
        return 0;
    };
    m_executeSql(sql, callback, &exists);

    return exists == 1;
}

std::vector<std::pair<std::string, std::string>> DatabaseManager::getAllUserInfo() const
{
    std::vector<std::pair<std::string, std::string>> allInfo;
    std::string sql = "SELECT INFO, SAVED_TIME FROM UserInfo;";
    auto callback = [](void* data, int argc, char** argv, char**)
    {
        auto allInfoPtr = static_cast<std::vector<std::pair<std::string, std::string>>*>(data);
        if (argc > 1 && argv[0] && argv[1])
        {
            allInfoPtr->emplace_back(argv[0], argv[1]);
        }
        return 0;
    };
    m_executeSql(sql, callback, &allInfo);

    return allInfo;
}

void DatabaseManager::deleteUserInfo(const std::string& info) const
{
    m_prepareAndExecute("DELETE FROM UserInfo WHERE INFO = ?", {info});
}

bool DatabaseManager::isDatabaseEmpty() const
{
    int count = 0;
    m_executeSql("SELECT COUNT(*) FROM UserInfo",
               [](void* data, int argc, char** argv, char**)
               {
                   if (argc > 0 && argv[0])
                   {
                       *static_cast<int*>(data) = std::stoi(argv[0]);
                   }
                   return 0;
               }, &count);

    return count == 0;
}

void DatabaseManager::saveElectricityInfo(const std::string& info, const std::string& date, int hour, int status, int queue, int subqueue) const
{
    int exists = 0;
    m_executeSql("SELECT EXISTS(SELECT 1 FROM ElectricityInfo WHERE INFO = ? AND HOUR = ?)",
               [](void* data, int argc, char** argv, char**)
               {
                   if (argc > 0 && argv[0])
                   {
                       *static_cast<int*>(data) = std::stoi(argv[0]);
                   }
                   return 0;
               }, &exists);

    if (exists == 1)
    {
        m_prepareAndExecute(
                "UPDATE ElectricityInfo SET DATE = ?, STATUS = ?, QUEUE = ?, SUBQUEUE = ? WHERE INFO = ? AND HOUR = ?",
                {date, std::to_string(status), std::to_string(queue), std::to_string(subqueue), info,
                 std::to_string(hour)});
    }
    else
    {
        m_prepareAndExecute(
                "INSERT INTO ElectricityInfo (INFO, DATE, HOUR, STATUS, QUEUE, SUBQUEUE) VALUES (?, ?, ?, ?, ?, ?)",
                {info, date, std::to_string(hour), std::to_string(status), std::to_string(queue),
                 std::to_string(subqueue)});
    }
}


std::vector<std::tuple<std::string, int, int, int, int>> DatabaseManager::getElectricityInfo(const std::string& info) const
{
    std::vector<std::tuple<std::string, int, int, int, int>> electricityInfo;
    m_executeSql("SELECT DATE, HOUR, STATUS, QUEUE, SUBQUEUE FROM ElectricityInfo WHERE INFO='" + info + "';",
               [](void* data, int argc, char** argv, char**)
               {
                   if (argc == 5 && argv[0] && argv[1] && argv[2] && argv[3] && argv[4])
                   {
                       static_cast<std::vector<std::tuple<std::string, int, int, int, int>>*>(data)->emplace_back(
                               argv[0], std::stoi(argv[1]), std::stoi(argv[2]), std::stoi(argv[3]), std::stoi(argv[4]));
                   }
                   return 0;
               }, &electricityInfo);
    return electricityInfo;
}