#include <memory>
#include <iostream>
#include <vector>
#include <tuple>
#include <functional>
#include <stdexcept>
#include <sstream>
#include <iomanip>
#include <chrono>
#include <sqlite3.h>

class DatabaseManager {
public:
    explicit DatabaseManager(std::string dbPath);
    ~DatabaseManager() = default;

    DatabaseManager(const DatabaseManager&) = delete;
    DatabaseManager& operator=(const DatabaseManager&) = delete;
    DatabaseManager(DatabaseManager&&) noexcept = default;
    DatabaseManager& operator=(DatabaseManager&&) noexcept = default;

    void setHomeUserInfo(const std::string& info);
    void removeHomeUserInfo();
    [[nodiscard]] std::string getHomeUserInfo() const;

    void saveUserInfo(const std::string& info);
    [[nodiscard]] std::string getUserInfo(int id) const;
    [[nodiscard]] std::vector<std::pair<std::string, std::string>> getAllUserInfo() const;
    [[nodiscard]] bool isUserInfoExist(const std::string& info) const;
    void deleteUserInfo(const std::string& info);

    [[nodiscard]] bool isDatabaseEmpty() const;

    void saveElectricityInfo(const std::string& info, const std::string& date, int hour, int status, int queue, int subqueue);
    [[nodiscard]] std::vector<std::tuple<std::string, int, int, int, int>> getElectricityInfo(const std::string& info) const;

private:
    void initDatabase();
    void executeSql(const std::string& sql, const std::vector<std::string>& params = {}, const std::function<void(sqlite3_stmt*)>& rowCallback = nullptr) const; 
    std::string constructFinalSql(const std::string& sql, const std::vector<std::string>& params) const; 
    void prepareAndExecute(const std::string& sql, const std::vector<std::string>& params) const;

    std::string m_dbPath;
    std::unique_ptr<sqlite3, decltype(&sqlite3_close)> m_db;
};
