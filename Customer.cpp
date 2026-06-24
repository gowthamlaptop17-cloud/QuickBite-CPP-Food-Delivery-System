#include "../include/Customer.h"
#include <iostream>

Customer::Customer(int id, const std::string& name, const std::string& email,
                   const std::string& phone, const std::string& address)
    : User(id, name, email), phone(phone), address(address) {}

void Customer::display() const {
    std::cout << "Customer #" << id << "\n"
              << "  Name    : " << name    << "\n"
              << "  Email   : " << email   << "\n"
              << "  Phone   : " << phone   << "\n"
              << "  Address : " << address << "\n";
}
