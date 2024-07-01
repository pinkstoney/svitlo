#include <string>
#include <iostream>
#include <memory>
#include <vector>
#include <tuple>
#include <functional>
#include <stdexcept>
#include <sstream>
#include <iomanip>

#include <sqlite3.h>

class DatabaseManager 
{
public:
    explicit DatabaseManager(std::string dbPath);
    ~DatabaseManager() noexcept;
    
    void init();

public:
    DatabaseManager(const DatabaseManager&) = delete;
    DatabaseManager& operator=(const DatabaseManager&) = delete;
    DatabaseManager(DatabaseManager&&) noexcept = default;
    DatabaseManager& operator=(DatabaseManager&&) noexcept = default;
    
public:
    void setHomeUserInfo(const std::string& info) const;
    void removeHomeUserInfo() const;
    std::string getHomeUserInfo() const;
    
public:
    void saveUserInfo(const std::string& info) const;
    std::string getUserInfo(int id) const;
    std::vector<std::pair<std::string, std::string>> getAllUserInfo() const;
    bool isUserInfoExist(const std::string& info) const;
    void deleteUserInfo(const std::string& info) const;
    
public:
    bool isDatabaseEmpty() const;
    
    void saveElectricityInfo(const std::string& info, const std::string& date, int hour, int status, int queue, int subqueue) const;
    std::vector<std::tuple<std::string, int, int, int, int>> getElectricityInfo(const std::string& info) const;

private:
    void m_executeSql(const std::string& sql, const std::function<int(void*, int, char**, char**)>& callback, void* data) const;
    void m_prepareAndExecute(const std::string& sql, const std::vector<std::string>& params) const;

private:
    std::string m_dbPath;
    std::unique_ptr<sqlite3, decltype(&sqlite3_close)> m_db;
    
};