#pragma once

#include <memory>
#include <string>

#include <sqlite3.h>

#include "consumer.h"

namespace svitlo
{
struct sqlite3;

class DatabaseManager
{
public:
    DatabaseManager(const std::string& dbName);
    ~DatabaseManager();

public:
    void createTables();
    void saveConsumer(const Consumer& consumer);
    std::unique_ptr<Consumer> loadConsumer(const std::string& id);

private:
    ::sqlite3* m_db;
};
} // namespace svitlo
