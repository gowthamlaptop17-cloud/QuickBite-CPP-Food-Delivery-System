#include "../include/Database.h"
#include <iostream>
#include <stdexcept>

// ─── Constructor / Destructor ─────────────────────────────────────────────────

Database::Database() : db(nullptr) {
    int rc = sqlite3_open("quickbite.db", &db);
    if (rc != SQLITE_OK) {
        std::string err = sqlite3_errmsg(db);
        sqlite3_close(db);
        throw std::runtime_error("Cannot open database: " + err);
    }
    // WAL mode: better concurrent read performance
    execute("PRAGMA journal_mode=WAL;");
    // Enforce foreign keys
    execute("PRAGMA foreign_keys=ON;");
    createTables();
}

Database::~Database() {
    if (db) sqlite3_close(db);
}

// ─── Singleton ────────────────────────────────────────────────────────────────

Database& Database::getInstance() {
    static Database instance;   // created once, destroyed at program exit — no leak
    return instance;
}

// ─── Internal helper ──────────────────────────────────────────────────────────

void Database::execute(const std::string& sql) {
    char* errMsg = nullptr;
    int rc = sqlite3_exec(db, sql.c_str(), nullptr, nullptr, &errMsg);
    if (rc != SQLITE_OK) {
        std::string err = errMsg ? errMsg : "Unknown error";
        sqlite3_free(errMsg);
        throw std::runtime_error("SQL error: " + err);
    }
}

// ─── Schema ───────────────────────────────────────────────────────────────────

void Database::createTables() {
    execute(R"(
        CREATE TABLE IF NOT EXISTS customers (
            id      INTEGER PRIMARY KEY AUTOINCREMENT,
            name    TEXT    NOT NULL,
            email   TEXT    NOT NULL UNIQUE,
            phone   TEXT    NOT NULL,
            address TEXT    NOT NULL
        );
    )");

    execute(R"(
        CREATE TABLE IF NOT EXISTS orders (
            id              INTEGER PRIMARY KEY AUTOINCREMENT,
            customer_id     INTEGER NOT NULL,
            restaurant_id   INTEGER NOT NULL,
            restaurant_name TEXT    NOT NULL,
            total           REAL    NOT NULL,
            status          TEXT    NOT NULL,
            created_at      TEXT    DEFAULT (datetime('now','localtime')),
            FOREIGN KEY (customer_id) REFERENCES customers(id)
        );
    )");

    execute(R"(
        CREATE TABLE IF NOT EXISTS order_items (
            id        INTEGER PRIMARY KEY AUTOINCREMENT,
            order_id  INTEGER NOT NULL,
            item_id   INTEGER NOT NULL,
            item_name TEXT    NOT NULL,
            price     REAL    NOT NULL,
            quantity  INTEGER NOT NULL,
            FOREIGN KEY (order_id) REFERENCES orders(id)
        );
    )");
}

// ─── Customer operations ──────────────────────────────────────────────────────

bool Database::emailExists(const std::string& email) {
    const char* sql = "SELECT COUNT(*) FROM customers WHERE email = ?;";
    sqlite3_stmt* stmt;
    sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
    sqlite3_bind_text(stmt, 1, email.c_str(), -1, SQLITE_STATIC);
    sqlite3_step(stmt);
    int count = sqlite3_column_int(stmt, 0);
    sqlite3_finalize(stmt);
    return count > 0;
}

int Database::insertCustomer(const std::string& name, const std::string& email,
                              const std::string& phone, const std::string& address) {
    const char* sql =
        "INSERT INTO customers (name, email, phone, address) VALUES (?, ?, ?, ?);";
    sqlite3_stmt* stmt;
    sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
    sqlite3_bind_text(stmt, 1, name.c_str(),    -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, email.c_str(),   -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, phone.c_str(),   -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 4, address.c_str(), -1, SQLITE_STATIC);

    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    if (rc != SQLITE_DONE)
        throw std::runtime_error("Failed to insert customer.");

    return static_cast<int>(sqlite3_last_insert_rowid(db));
}

int Database::getCustomerIdByEmail(const std::string& email) {
    const char* sql = "SELECT id FROM customers WHERE email = ?;";
    sqlite3_stmt* stmt;
    sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
    sqlite3_bind_text(stmt, 1, email.c_str(), -1, SQLITE_STATIC);

    int id = -1;
    if (sqlite3_step(stmt) == SQLITE_ROW)
        id = sqlite3_column_int(stmt, 0);

    sqlite3_finalize(stmt);
    return id;
}

// ─── Order operations ─────────────────────────────────────────────────────────

int Database::insertOrder(int customerId, int restaurantId,
                           const std::string& restaurantName,
                           double total, const std::string& status) {
    const char* sql =
        "INSERT INTO orders (customer_id, restaurant_id, restaurant_name, total, status)"
        " VALUES (?, ?, ?, ?, ?);";
    sqlite3_stmt* stmt;
    sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
    sqlite3_bind_int (stmt, 1, customerId);
    sqlite3_bind_int (stmt, 2, restaurantId);
    sqlite3_bind_text(stmt, 3, restaurantName.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_double(stmt, 4, total);
    sqlite3_bind_text(stmt, 5, status.c_str(), -1, SQLITE_STATIC);

    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    if (rc != SQLITE_DONE)
        throw std::runtime_error("Failed to insert order.");

    return static_cast<int>(sqlite3_last_insert_rowid(db));
}

void Database::insertOrderItem(int orderId, int itemId,
                                const std::string& itemName,
                                double price, int qty) {
    const char* sql =
        "INSERT INTO order_items (order_id, item_id, item_name, price, quantity)"
        " VALUES (?, ?, ?, ?, ?);";
    sqlite3_stmt* stmt;
    sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
    sqlite3_bind_int   (stmt, 1, orderId);
    sqlite3_bind_int   (stmt, 2, itemId);
    sqlite3_bind_text  (stmt, 3, itemName.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_double(stmt, 4, price);
    sqlite3_bind_int   (stmt, 5, qty);

    sqlite3_step(stmt);
    sqlite3_finalize(stmt);
}

void Database::updateOrderStatus(int orderId, const std::string& status) {
    const char* sql = "UPDATE orders SET status = ? WHERE id = ?;";
    sqlite3_stmt* stmt;
    sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
    sqlite3_bind_text(stmt, 1, status.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_int (stmt, 2, orderId);

    sqlite3_step(stmt);
    sqlite3_finalize(stmt);
}
