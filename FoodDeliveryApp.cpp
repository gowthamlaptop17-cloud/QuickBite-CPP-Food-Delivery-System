#include "../include/FoodDeliveryApp.h"

#include <iostream>
#include <limits>
#include <memory>
#include <algorithm>
#include <iomanip>

// ─── Singleton ────────────────────────────────────────────────────────────────

FoodDeliveryApp::FoodDeliveryApp()
    : nextCustomerId(1), nextOrderId(1001), loggedInCustomerId(-1) {
    seedData();
}

FoodDeliveryApp& FoodDeliveryApp::getInstance() {
    static FoodDeliveryApp instance;   // created once, destroyed at program exit
    return instance;
}

// ─── Seed demo restaurants & menus ───────────────────────────────────────────

void FoodDeliveryApp::seedData() {
    // Restaurant 1
    Restaurant r1(1, "Spice Garden", "North Indian", "Koramangala", 4.5);
    r1.addMenuItem(MenuItem(101, "Paneer Butter Masala", 280.0, "Main Course"));
    r1.addMenuItem(MenuItem(102, "Dal Makhani",          220.0, "Main Course"));
    r1.addMenuItem(MenuItem(103, "Garlic Naan",           50.0, "Breads"));
    r1.addMenuItem(MenuItem(104, "Gulab Jamun",           80.0, "Desserts"));
    r1.addMenuItem(MenuItem(105, "Mango Lassi",           90.0, "Drinks"));
    restaurants.push_back(r1);

    // Restaurant 2
    Restaurant r2(2, "Wok N Roll", "Chinese", "Indiranagar", 4.2);
    r2.addMenuItem(MenuItem(201, "Veg Fried Rice",       180.0, "Rice"));
    r2.addMenuItem(MenuItem(202, "Chicken Manchurian",   250.0, "Starters"));
    r2.addMenuItem(MenuItem(203, "Hakka Noodles",        190.0, "Noodles"));
    r2.addMenuItem(MenuItem(204, "Spring Rolls",         140.0, "Starters"));
    r2.addMenuItem(MenuItem(205, "Hot & Sour Soup",      120.0, "Soups"));
    restaurants.push_back(r2);

    // Restaurant 3
    Restaurant r3(3, "The Burger Lab", "Fast Food", "HSR Layout", 4.0);
    r3.addMenuItem(MenuItem(301, "Classic Veg Burger",   150.0, "Burgers"));
    r3.addMenuItem(MenuItem(302, "Chicken Zinger",       200.0, "Burgers"));
    r3.addMenuItem(MenuItem(303, "Loaded Fries",         120.0, "Sides"));
    r3.addMenuItem(MenuItem(304, "Chocolate Shake",      130.0, "Shakes"));
    r3.addMenuItem(MenuItem(305, "Onion Rings",           90.0, "Sides"));
    restaurants.push_back(r3);
}

// ─── Lookup helpers ───────────────────────────────────────────────────────────

Customer* FoodDeliveryApp::findCustomer(int id) {
    for (auto& c : customers)
        if (c.getId() == id) return &c;
    return nullptr;
}

Restaurant* FoodDeliveryApp::findRestaurant(int id) {
    for (auto& r : restaurants)
        if (r.getId() == id) return &r;
    return nullptr;
}

Order* FoodDeliveryApp::findOrder(int orderId, int customerId) {
    for (auto& o : orders)
        if (o.getOrderId() == orderId && o.getCustomerId() == customerId)
            return &o;
    return nullptr;
}

// ─── Input helpers ────────────────────────────────────────────────────────────

int FoodDeliveryApp::getIntInput(const std::string& prompt) {
    int val;
    while (true) {
        std::cout << prompt;
        if (std::cin >> val) {
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            return val;
        }
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "  Invalid input. Please enter a number.\n";
    }
}

std::string FoodDeliveryApp::getStringInput(const std::string& prompt) {
    std::string s;
    std::cout << prompt;
    std::getline(std::cin, s);
    return s;
}

void FoodDeliveryApp::clearScreen() const {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

// ─── Register Customer ────────────────────────────────────────────────────────

void FoodDeliveryApp::registerCustomer() {
    std::cout << "\n--- Register ---\n";
    std::string name    = getStringInput("Full Name    : ");
    std::string email   = getStringInput("Email        : ");
    std::string phone   = getStringInput("Phone        : ");
    std::string address = getStringInput("Address      : ");

    Database& dbRef = Database::getInstance();

    if (dbRef.emailExists(email)) {
        std::cout << "An account with this email already exists.\n";
        return;
    }

    int newId = dbRef.insertCustomer(name, email, phone, address);
    customers.emplace_back(newId, name, email, phone, address);
    std::cout << "Registration successful! Your Customer ID is " << newId << ".\n";
}

// ─── Login ────────────────────────────────────────────────────────────────────

void FoodDeliveryApp::loginCustomer() {
    std::cout << "\n--- Login ---\n";
    std::string email = getStringInput("Email : ");

    // Check in-memory first (current session registrations)
    for (const auto& c : customers) {
        if (c.getEmail() == email) {
            loggedInCustomerId = c.getId();
            std::cout << "Welcome back, " << c.getName() << "!\n";
            return;
        }
    }
    // Fall back to DB (users from previous sessions)
    Database& dbRef = Database::getInstance();
    int dbId = dbRef.getCustomerIdByEmail(email);
    if (dbId != -1) {
        // Load customer from DB using a raw query
        const char* sql = "SELECT id,name,email,phone,address FROM customers WHERE id=?;";
        sqlite3_stmt* stmt;
        sqlite3_prepare_v2(dbRef.get(), sql, -1, &stmt, nullptr);
        sqlite3_bind_int(stmt, 1, dbId);
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            int    id   = sqlite3_column_int (stmt, 0);
            std::string nm  = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
            std::string em  = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
            std::string ph  = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
            std::string addr= reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4));
            sqlite3_finalize(stmt);
            customers.emplace_back(id, nm, em, ph, addr);
            loggedInCustomerId = id;
            std::cout << "Welcome back, " << nm << "!\n";
            return;
        }
        sqlite3_finalize(stmt);
    }
    std::cout << "No account found with that email. Please register first.\n";
}

// ─── Browse Restaurants ───────────────────────────────────────────────────────

void FoodDeliveryApp::browseRestaurants() const {
    std::cout << "\n=== Available Restaurants ===\n";
    for (const auto& r : restaurants) r.display();
}

// ─── Place Order ──────────────────────────────────────────────────────────────

void FoodDeliveryApp::placeOrder() {
    browseRestaurants();

    int rid = getIntInput("\nEnter Restaurant ID : ");
    Restaurant* restaurant = findRestaurant(rid);
    if (!restaurant) {
        std::cout << "Restaurant not found.\n";
        return;
    }

    restaurant->displayMenu();

    Order order(nextOrderId++, loggedInCustomerId,
                restaurant->getId(), restaurant->getName());

    while (true) {
        int itemId = getIntInput("\nEnter Item ID to add (0 to finish) : ");
        if (itemId == 0) break;

        const MenuItem* item = restaurant->findMenuItem(itemId);
        if (!item) {
            std::cout << "Item not found in this restaurant's menu.\n";
            continue;
        }

        int qty = getIntInput("Quantity : ");
        if (qty <= 0) { std::cout << "Quantity must be at least 1.\n"; continue; }

        order.addItem(*item, qty);
        std::cout << item->getName() << " x" << qty << " added.\n";
    }

    if (order.isEmpty()) {
        std::cout << "No items selected. Order cancelled.\n";
        nextOrderId--;   // reclaim the id
        return;
    }

    order.display();
    order.setStatus(OrderStatus::CONFIRMED);
    handlePayment(order);
    order.setStatus(OrderStatus::PREPARING);

    // Persist to DB — get real DB-assigned id back
    Database& dbRef = Database::getInstance();
    int dbOrderId = dbRef.insertOrder(
        order.getCustomerId(),
        order.getRestaurantId(),
        order.getRestaurantName(),
        order.getTotalAmount(),
        statusToString(order.getStatus())
    );
    // Persist each item
    for (const auto& [item, qty] : order.getItems()) {
        dbRef.insertOrderItem(dbOrderId, item.getId(), item.getName(), item.getPrice(), qty);
    }
    // Sync in-memory order id to DB-assigned id
    order.setOrderId(dbOrderId);

    orders.push_back(order);
    std::cout << "\nOrder #" << dbOrderId
              << " placed successfully! Estimated delivery: 30-45 mins.\n";
}

// ─── Payment ─────────────────────────────────────────────────────────────────

void FoodDeliveryApp::handlePayment(Order& order) {
    std::cout << "\n--- Choose Payment Method ---\n";
    std::cout << "1. UPI\n2. Credit/Debit Card\n3. Cash on Delivery\n";
    int choice = getIntInput("Choice : ");

    std::unique_ptr<Payment> payment;

    if (choice == 1) {
        std::string upiId = getStringInput("Enter UPI ID : ");
        payment = std::make_unique<UPIPayment>(order.getTotalAmount(), upiId);
    } else if (choice == 2) {
        std::string cardHolder = getStringInput("Card Holder Name : ");
        std::string last4      = getStringInput("Last 4 digits of card : ");
        payment = std::make_unique<CardPayment>(order.getTotalAmount(), cardHolder, last4);
    } else {
        payment = std::make_unique<CashOnDelivery>(order.getTotalAmount());
    }

    payment->processPayment();
}

// ─── View My Orders ───────────────────────────────────────────────────────────

void FoodDeliveryApp::viewMyOrders() const {
    bool found = false;
    for (const auto& o : orders) {
        if (o.getCustomerId() == loggedInCustomerId) {
            o.display();
            found = true;
        }
    }
    if (!found) std::cout << "You have no orders yet.\n";
}

// ─── Track Order ─────────────────────────────────────────────────────────────

void FoodDeliveryApp::trackOrder() {
    int orderId = getIntInput("Enter Order ID to track : ");
    Order* order = findOrder(orderId, loggedInCustomerId);
    if (!order) {
        std::cout << "Order not found.\n";
        return;
    }

    // Simulate status progression for demo purposes
    std::cout << "\nOrder #" << orderId
              << " current status: " << statusToString(order->getStatus()) << "\n";

    if (order->getStatus() == OrderStatus::PREPARING) {
        order->setStatus(OrderStatus::OUT_FOR_DELIVERY);
        Database::getInstance().updateOrderStatus(orderId, statusToString(OrderStatus::OUT_FOR_DELIVERY));
        std::cout << "Update: Your order is now OUT FOR DELIVERY!\n";
    } else if (order->getStatus() == OrderStatus::OUT_FOR_DELIVERY) {
        order->setStatus(OrderStatus::DELIVERED);
        Database::getInstance().updateOrderStatus(orderId, statusToString(OrderStatus::DELIVERED));
        std::cout << "Update: Your order has been DELIVERED! Enjoy your meal.\n";
    }
}

// ─── Cancel Order ─────────────────────────────────────────────────────────────

void FoodDeliveryApp::cancelOrder() {
    int orderId = getIntInput("Enter Order ID to cancel : ");
    Order* order = findOrder(orderId, loggedInCustomerId);
    if (!order) {
        std::cout << "Order not found.\n";
        return;
    }
    if (order->getStatus() == OrderStatus::DELIVERED) {
        std::cout << "Cannot cancel an already delivered order.\n";
        return;
    }
    if (order->getStatus() == OrderStatus::CANCELLED) {
        std::cout << "Order is already cancelled.\n";
        return;
    }
    order->setStatus(OrderStatus::CANCELLED);
    Database::getInstance().updateOrderStatus(orderId, statusToString(OrderStatus::CANCELLED));
    std::cout << "Order #" << orderId << " has been cancelled.\n";
}

// ─── Main Run Loop ────────────────────────────────────────────────────────────

void FoodDeliveryApp::run() {
    std::cout << "========================================\n";
    std::cout << "   Welcome to QuickBite Food Delivery   \n";
    std::cout << "========================================\n";

    while (true) {
        if (loggedInCustomerId == -1) {
            // ── Guest menu ──
            std::cout << "\n1. Register\n2. Login\n3. Exit\n";
            int choice = getIntInput("Choice : ");
            if (choice == 1)      registerCustomer();
            else if (choice == 2) loginCustomer();
            else { std::cout << "Goodbye!\n"; break; }
        } else {
            // ── Logged-in menu ──
            Customer* c = findCustomer(loggedInCustomerId);
            std::cout << "\n--- Hello, " << c->getName() << " ---\n";
            std::cout << "1. Browse Restaurants & Place Order\n";
            std::cout << "2. My Orders\n";
            std::cout << "3. Track Order\n";
            std::cout << "4. Cancel Order\n";
            std::cout << "5. My Profile\n";
            std::cout << "6. Logout\n";
            std::cout << "7. Exit\n";

            int choice = getIntInput("Choice : ");
            switch (choice) {
                case 1: placeOrder();         break;
                case 2: viewMyOrders();       break;
                case 3: trackOrder();         break;
                case 4: cancelOrder();        break;
                case 5: c->display();         break;
                case 6:
                    std::cout << "Logged out.\n";
                    loggedInCustomerId = -1;
                    break;
                case 7: std::cout << "Goodbye!\n"; return;
                default: std::cout << "Invalid choice.\n";
            }
        }
    }
}
