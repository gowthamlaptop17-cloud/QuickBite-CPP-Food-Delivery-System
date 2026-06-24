#include "../include/Order.h"

std::string statusToString(OrderStatus status) {
    switch (status) {
        case OrderStatus::PLACED:            return "Order Placed";
        case OrderStatus::CONFIRMED:         return "Confirmed by Restaurant";
        case OrderStatus::PREPARING:         return "Being Prepared";
        case OrderStatus::OUT_FOR_DELIVERY:  return "Out for Delivery";
        case OrderStatus::DELIVERED:         return "Delivered";
        case OrderStatus::CANCELLED:         return "Cancelled";
        default:                             return "Unknown";
    }
}
