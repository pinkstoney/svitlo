#include <svitlo/database-manager.h>

#include <stdexcept>

using namespace svitlo;

DatabaseManager::DatabaseManager(const std::string& dbName)
{
    if (sqlite3_open(dbName.c_str(), &m_db) != SQLITE_OK)
        throw std::runtime_error("Can't open database: " + std::string(sqlite3_errmsg(m_db)));
}

DatabaseManager::~DatabaseManager()
{
    sqlite3_close(m_db);
}

void DatabaseManager::createTables()
{
    const char* sql = "CREATE TABLE IF NOT EXISTS consumers ("
                      "id TEXT PRIMARY KEY,"
                      "queue INTEGER,"
                      "subqueue INTEGER"
                      ");"
                      "CREATE TABLE IF NOT EXISTS electricity_data ("
                      "consumer_id TEXT,"
                      "hour INTEGER,"
                      "status INTEGER,"
                      "FOREIGN KEY(consumer_id) REFERENCES consumers(id)"
                      ");";

    char* errMsg = nullptr;
    if (sqlite3_exec(m_db, sql, nullptr, nullptr, &errMsg) != SQLITE_OK)
    {
        std::string error = "SQL error: " + std::string(errMsg);
        sqlite3_free(errMsg);
        throw std::runtime_error(error);
    }
}

void DatabaseManager::saveConsumer(const Consumer& consumer)
{
    const char* sql = "INSERT OR REPLACE INTO consumers (id, queue, subqueue) VALUES (?, ?, ?);";

    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(m_db, sql, -1, &stmt, nullptr) != SQLITE_OK)
    {
        throw std::runtime_error("Failed to prepare statement");
    }

    sqlite3_bind_text(stmt, 1, consumer.getId().c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 2, consumer.getQueue());
    sqlite3_bind_int(stmt, 3, consumer.getSubqueue());

    if (sqlite3_step(stmt) != SQLITE_DONE)
    {
        sqlite3_finalize(stmt);
        throw std::runtime_error("Failed to insert consumer");
    }

    sqlite3_finalize(stmt);

    sql = "INSERT OR REPLACE INTO electricity_data (consumer_id, hour, status) VALUES (?, ?, ?);";
    if (sqlite3_prepare_v2(m_db, sql, -1, &stmt, nullptr) != SQLITE_OK)
        throw std::runtime_error("Failed to prepare statement");

    for (int hour = 0; hour < 24; ++hour)
    {
        sqlite3_bind_text(stmt, 1, consumer.getId().c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_int(stmt, 2, hour);
        sqlite3_bind_int(stmt, 3, static_cast<int>(consumer.getElectricityStatus(hour)));

        if (sqlite3_step(stmt) != SQLITE_DONE)
        {
            sqlite3_finalize(stmt);
            throw std::runtime_error("Failed to insert electricity data");
        }
        sqlite3_reset(stmt);
    }

    sqlite3_finalize(stmt);
}

std::unique_ptr<Consumer> DatabaseManager::loadConsumer(const std::string& id)
{
    const char* sql = "SELECT queue, subqueue FROM consumers WHERE id = ?;";

    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(m_db, sql, -1, &stmt, nullptr) != SQLITE_OK)
    {
        throw std::runtime_error("Failed to prepare statement");
    }

    sqlite3_bind_text(stmt, 1, id.c_str(), -1, SQLITE_STATIC);

    if (sqlite3_step(stmt) != SQLITE_ROW)
    {
        sqlite3_finalize(stmt);
        return nullptr;
    }

    int queue = sqlite3_column_int(stmt, 0);
    int subqueue = sqlite3_column_int(stmt, 1);

    sqlite3_finalize(stmt);

    auto consumer = std::make_unique<Consumer>(queue, subqueue, id);

    sql = "SELECT hour, status FROM electricity_data WHERE consumer_id = ?;";
    if (sqlite3_prepare_v2(m_db, sql, -1, &stmt, nullptr) != SQLITE_OK)
    {
        throw std::runtime_error("Failed to prepare statement");
    }

    sqlite3_bind_text(stmt, 1, id.c_str(), -1, SQLITE_STATIC);

    while (sqlite3_step(stmt) == SQLITE_ROW)
    {
        int hour = sqlite3_column_int(stmt, 0);
        ElectricityData::Status status = static_cast<ElectricityData::Status>(sqlite3_column_int(stmt, 1));
        consumer->setElectricityStatus(hour, status);
    }

    sqlite3_finalize(stmt);

    return consumer;
}
