#include <stdexcept>

#include <svitlo/database-manager.h>

using namespace svitlo;

class DatabaseManager::Impl
{
public:
    explicit Impl(const std::string& dbName)
        : m_db(nullptr)
    {
        if (::sqlite3_open(dbName.c_str(), &m_db) != SQLITE_OK)
            throw std::runtime_error("Can't open database: " + std::string(::sqlite3_errmsg(m_db)));
    }

    ~Impl()
    {
        if (m_db)
            ::sqlite3_close(m_db);
    }

    Impl(const Impl&) = delete;
    Impl& operator=(const Impl&) = delete;

    void createTables()
    {
        const char* sql = R"SQL(
            CREATE TABLE IF NOT EXISTS consumers (
                id TEXT PRIMARY KEY,
                queue INTEGER,
                subqueue INTEGER
            );
            CREATE TABLE IF NOT EXISTS electricity_data (
                consumer_id TEXT,
                hour INTEGER,
                status INTEGER,
                FOREIGN KEY(consumer_id) REFERENCES consumers(id)
            );
        )SQL";

        executeSql(sql);
    }

    void saveConsumer(const Consumer& consumer)
    {
        const char* sql1 = "INSERT OR REPLACE INTO consumers (id, queue, subqueue) "
                           "VALUES (?, ?, ?);";
        auto stmt1 = prepareStatement(sql1);

        bindText(stmt1.get(), 1, consumer.getId());
        bindInt(stmt1.get(), 2, consumer.getQueue());
        bindInt(stmt1.get(), 3, consumer.getSubqueue());
        executeStatement(stmt1.get());

        const char* sql2 = "INSERT OR REPLACE INTO electricity_data (consumer_id, "
                           "hour, status) VALUES (?, ?, ?);";
        auto stmt2 = prepareStatement(sql2);

        for (int hour = 0; hour < 24; ++hour)
        {
            bindText(stmt2.get(), 1, consumer.getId());
            bindInt(stmt2.get(), 2, hour);
            bindInt(stmt2.get(), 3, static_cast<int>(consumer.getElectricityStatus(hour)));

            executeStatement(stmt2.get());
            sqlite3_reset(stmt2.get());
        }
    }

    std::unique_ptr<Consumer> loadConsumer(const std::string& id)
    {
        const char* sql = "SELECT queue, subqueue FROM consumers WHERE id = ?;";
        auto stmt = prepareStatement(sql);
        bindText(stmt.get(), 1, id);

        if (::sqlite3_step(stmt.get()) != SQLITE_ROW)
            return nullptr;

        int queue = ::sqlite3_column_int(stmt.get(), 0);
        int subqueue = ::sqlite3_column_int(stmt.get(), 1);
        auto consumer = std::make_unique<Consumer>(queue, subqueue, id);

        sql = "SELECT hour, status FROM electricity_data WHERE consumer_id = ?;";
        stmt = prepareStatement(sql);
        bindText(stmt.get(), 1, id);

        while (::sqlite3_step(stmt.get()) == SQLITE_ROW)
        {
            int hour = ::sqlite3_column_int(stmt.get(), 0);
            ElectricityData::Status status = static_cast<ElectricityData::Status>(::sqlite3_column_int(stmt.get(), 1));
            consumer->setElectricityStatus(hour, status);
        }

        return consumer;
    }

private:
    ::sqlite3* m_db;

    void executeSql(const char* sql)
    {
        char* errMsg = nullptr;
        if (::sqlite3_exec(m_db, sql, nullptr, nullptr, &errMsg) != SQLITE_OK)
        {
            std::string error = "SQL error: " + std::string(errMsg);
            ::sqlite3_free(errMsg);
            throw std::runtime_error(error);
        }
    }

    std::unique_ptr<sqlite3_stmt, decltype(&sqlite3_finalize)> prepareStatement(const char* sql)
    {
        sqlite3_stmt* stmt_raw;

        if (sqlite3_prepare_v2(m_db, sql, -1, &stmt_raw, nullptr) != SQLITE_OK)
            throw std::runtime_error("Failed to prepare statement: " + std::string(sqlite3_errmsg(m_db)));

        return std::unique_ptr<sqlite3_stmt, decltype(&sqlite3_finalize)>(stmt_raw, sqlite3_finalize);
    }

    void executeStatement(sqlite3_stmt* stmt)
    {
        int rc = sqlite3_step(stmt);

        if (rc != SQLITE_DONE)
            throw std::runtime_error("Failed to execute statement: " + std::string(sqlite3_errmsg(m_db)));
    }

    void bindText(::sqlite3_stmt* stmt, int index, const std::string& value) { ::sqlite3_bind_text(stmt, index, value.c_str(), -1, SQLITE_STATIC); }

    void bindInt(::sqlite3_stmt* stmt, int index, int value) { ::sqlite3_bind_int(stmt, index, value); }
};

DatabaseManager::DatabaseManager(const std::string& dbName)
    : pImpl(std::make_unique<Impl>(dbName)) {}

DatabaseManager::~DatabaseManager() = default;
DatabaseManager::DatabaseManager(DatabaseManager&&) noexcept = default;
DatabaseManager& DatabaseManager::operator=(DatabaseManager&&) noexcept = default;

void DatabaseManager::createTables() { pImpl->createTables(); }

void DatabaseManager::saveConsumer(const Consumer& consumer) { pImpl->saveConsumer(consumer); }

std::unique_ptr<Consumer> DatabaseManager::loadConsumer(const std::string& id) { return pImpl->loadConsumer(id); }
