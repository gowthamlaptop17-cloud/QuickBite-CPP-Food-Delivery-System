#ifndef FOOD_DELIVERY_APP_H
#define FOOD_DELIVERY_APP_H

#include "Customer.h"
#include "Restaurant.h"
#include "Order.h"
#include "Payment.h"
#include "Database.h"

#include <vector>
#include <memory>
#include <string>

// ─── FoodDeliveryApp (Singleton) ──────────────────────────────────────────────
// Central controller that owns all data and drives the console UI.
class FoodDeliveryApp {
private:
    // ----- data stores (in-memory; no DB dependency) -----
    std::vector<Customer>                  customers;
    std::vector<Restaurant>                restaurants;
    std::vector<Order>                     orders;

    // ----- id counters -----
    int nextCustomerId;
    int nextOrderId;

    // ----- currently logged-in customer (-1 = none) -----
    int loggedInCustomerId;

    // ----- Singleton machinery -----
    FoodDeliveryApp();
    FoodDeliveryApp(const FoodDeliveryApp&) = delete;
    FoodDeliveryApp& operator=(const FoodDeliveryApp&) = delete;

    // ----- helpers -----
    void seedData();
    void clearScreen() const;

    Customer* findCustomer(int id);
    Restaurant* findRestaurant(int id);
    Order* findOrder(int orderId, int customerId);

    // ----- UI sub-flows -----
    void registerCustomer();
    void loginCustomer();
    void browseRestaurants() const;
    void placeOrder();
    void viewMyOrders() const;
    void trackOrder();
    void cancelOrder();
    void handlePayment(Order& order);

    // input helpers
    int  getIntInput(const std::string& prompt);
    std::string getStringInput(const std::string& prompt);

public:
    static FoodDeliveryApp& getInstance();
    void run();
};

#endif
