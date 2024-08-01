#pragma once

#include <memory>
#include <string>

#include <sqlite3.h>

#include "consumer.h"

namespace svitlo
{

class DatabaseManager
{
public:
    explicit DatabaseManager(const std::string& dbName);
    ~DatabaseManager();
    DatabaseManager(const DatabaseManager&) = delete;
    DatabaseManager& operator=(const DatabaseManager&) = delete;
    DatabaseManager(DatabaseManager&&) noexcept;
    DatabaseManager& operator=(DatabaseManager&&) noexcept;

public:
    void createTables();
    void saveConsumer(const Consumer& consumer);
    std::unique_ptr<Consumer> loadConsumer(const std::string& id);

private:
    class Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace svitlo
