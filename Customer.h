#ifndef CUSTOMER_H
#define CUSTOMER_H

#include "User.h"
#include <string>

class Customer : public User {
private:
    std::string phone;
    std::string address;

public:
    Customer(int id, const std::string& name, const std::string& email,
             const std::string& phone, const std::string& address);

    std::string getPhone() const { return phone; }
    std::string getAddress() const { return address; }

    void display() const override;
    std::string getRole() const override { return "Customer"; }
};

#endif
