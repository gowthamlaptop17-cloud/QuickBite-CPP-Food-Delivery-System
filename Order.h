#ifndef ORDER_H
#define ORDER_H

#include "Restaurant.h"
#include <vector>
#include <utility>
#include <iostream>
#include <iomanip>

// ─── Order Status ─────────────────────────────────────────────────────────────
enum class OrderStatus {
    PLACED,
    CONFIRMED,
    PREPARING,
    OUT_FOR_DELIVERY,
    DELIVERED,
    CANCELLED
};

std::string statusToString(OrderStatus status);

// ─── Order ────────────────────────────────────────────────────────────────────
class Order {
private:
    int orderId;
    int customerId;
    int restaurantId;
    std::string restaurantName;
    // pair<MenuItem, quantity>
    std::vector<std::pair<MenuItem, int>> items;
    double totalAmount;
    OrderStatus status;

public:
    Order(int orderId, int customerId, int restaurantId,
          const std::string& restaurantName)
        : orderId(orderId), customerId(customerId),
          restaurantId(restaurantId), restaurantName(restaurantName),
          totalAmount(0.0), status(OrderStatus::PLACED) {}

    void addItem(const MenuItem& item, int qty) {
        items.push_back({item, qty});
        totalAmount += item.getPrice() * qty;
    }

    int getOrderId() const { return orderId; }
    int getCustomerId() const { return customerId; }
    int getRestaurantId() const { return restaurantId; }
    std::string getRestaurantName() const { return restaurantName; }
    double getTotalAmount() const { return totalAmount; }
    OrderStatus getStatus() const { return status; }
    const std::vector<std::pair<MenuItem, int>>& getItems() const { return items; }

    void setStatus(OrderStatus s) { status = s; }
    void setOrderId(int id) { orderId = id; }

    bool isEmpty() const { return items.empty(); }

    void display() const {
        std::cout << "\n----- Order #" << orderId << " -----\n";
        std::cout << "Restaurant : " << restaurantName << "\n";
        std::cout << "Status     : " << statusToString(status) << "\n";
        std::cout << "Items:\n";
        for (const auto& [item, qty] : items) {
            std::cout << "  " << item.getName()
                      << " x" << qty
                      << "  =  Rs." << std::fixed << std::setprecision(2)
                      << item.getPrice() * qty << "\n";
        }
        std::cout << "  --------------------------\n";
        std::cout << "  Total  :  Rs." << std::fixed << std::setprecision(2)
                  << totalAmount << "\n";
    }
};

#endif
