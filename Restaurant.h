#ifndef RESTAURANT_H
#define RESTAURANT_H

#include <string>
#include <vector>
#include <iostream>

// ─── MenuItem ────────────────────────────────────────────────────────────────
class MenuItem {
private:
    int id;
    std::string name;
    double price;
    std::string category;   // e.g. "Starters", "Main Course", "Drinks"

public:
    MenuItem(int id, const std::string& name, double price, const std::string& category)
        : id(id), name(name), price(price), category(category) {}

    int getId() const { return id; }
    std::string getName() const { return name; }
    double getPrice() const { return price; }
    std::string getCategory() const { return category; }

    void display() const {
        std::cout << "  [" << id << "] " << name
                  << "  |  Rs." << price
                  << "  |  " << category << "\n";
    }
};

// ─── Restaurant ──────────────────────────────────────────────────────────────
class Restaurant {
private:
    int id;
    std::string name;
    std::string cuisine;
    std::string location;
    double rating;
    std::vector<MenuItem> menu;

public:
    Restaurant(int id, const std::string& name, const std::string& cuisine,
               const std::string& location, double rating)
        : id(id), name(name), cuisine(cuisine),
          location(location), rating(rating) {}

    int getId() const { return id; }
    std::string getName() const { return name; }
    std::string getCuisine() const { return cuisine; }
    double getRating() const { return rating; }

    void addMenuItem(const MenuItem& item) { menu.push_back(item); }

    const std::vector<MenuItem>& getMenu() const { return menu; }

    // Returns nullptr if not found
    const MenuItem* findMenuItem(int itemId) const {
        for (const auto& item : menu) {
            if (item.getId() == itemId) return &item;
        }
        return nullptr;
    }

    void displayMenu() const {
        std::cout << "\n============================\n";
        std::cout << " " << name << " Menu\n";
        std::cout << " Cuisine: " << cuisine << " | Rating: " << rating << "/5\n";
        std::cout << "============================\n";
        for (const auto& item : menu) item.display();
    }

    void display() const {
        std::cout << "[" << id << "] " << name
                  << "  |  " << cuisine
                  << "  |  " << location
                  << "  |  Rating: " << rating << "/5\n";
    }
};

#endif
