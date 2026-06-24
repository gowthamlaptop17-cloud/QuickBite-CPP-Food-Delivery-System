#ifndef DATABASE_H
#define DATABASE_H

#include <sqlite3.h>
#include <string>
#include <stdexcept>

// ─── Database (Singleton) ─────────────────────────────────────────────────────
// Owns the SQLite connection and provides helpers used by FoodDeliveryApp.
// All queries use prepared statements — no string concatenation, no SQL injection.
class Database {
private:
    sqlite3* db;

    Database();
    Database(const Database&) = delete;
    Database& operator=(const Database&) = delete;
    ~Database();

    void execute(const std::string& sql);   // run a no-result SQL statement

public:
    static Database& getInstance();

    sqlite3* get() { return db; }           // raw handle for prepared stmts

    // Schema creation
    void createTables();

    // Customer helpers
    int  insertCustomer(const std::string& name, const std::string& email,
                        const std::string& phone, const std::string& address);
    bool emailExists(const std::string& email);
    int  getCustomerIdByEmail(const std::string& email);   // -1 if not found

    // Order helpers
    int  insertOrder(int customerId, int restaurantId,
                     const std::string& restaurantName, double total,
                     const std::string& status);
    void insertOrderItem(int orderId, int itemId,
                         const std::string& itemName, double price, int qty);
    void updateOrderStatus(int orderId, const std::string& status);
};

#endif
