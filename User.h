#ifndef USER_H
#define USER_H

#include <string>
#include <iostream>

class User {
protected:
    int id;
    std::string name;
    std::string email;

public:
    User(int id, const std::string& name, const std::string& email)
        : id(id), name(name), email(email) {}

    int getId() const { return id; }
    std::string getName() const { return name; }
    std::string getEmail() const { return email; }

    virtual void display() const = 0;
    virtual std::string getRole() const = 0;
    virtual ~User() {}
};

#endif
