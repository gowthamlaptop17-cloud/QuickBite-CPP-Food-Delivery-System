#ifndef PAYMENT_H
#define PAYMENT_H

#include <string>
#include <iostream>
#include <iomanip>
#include <stdexcept>

// ─── Abstract Payment ─────────────────────────────────────────────────────────
class Payment {
protected:
    double amount;
    bool paid;

public:
    explicit Payment(double amount) : amount(amount), paid(false) {
        if (amount <= 0)
            throw std::invalid_argument("Payment amount must be positive.");
    }

    virtual bool processPayment() = 0;
    virtual std::string getMethod() const = 0;

    bool isPaid() const { return paid; }
    double getAmount() const { return amount; }

    virtual void displayReceipt() const {
        std::cout << "\n===== Payment Receipt =====\n";
        std::cout << "Method : " << getMethod() << "\n";
        std::cout << "Amount : Rs." << std::fixed << std::setprecision(2) << amount << "\n";
        std::cout << "Status : " << (paid ? "SUCCESS" : "FAILED") << "\n";
        std::cout << "===========================\n";
    }

    virtual ~Payment() {}
};

// ─── UPI Payment ──────────────────────────────────────────────────────────────
class UPIPayment : public Payment {
private:
    std::string upiId;

public:
    UPIPayment(double amount, const std::string& upiId)
        : Payment(amount), upiId(upiId) {}

    bool processPayment() override {
        // In a real system: call payment gateway API here
        std::cout << "Processing UPI payment to " << upiId << "...\n";
        paid = true;
        displayReceipt();
        return true;
    }

    std::string getMethod() const override { return "UPI (" + upiId + ")"; }
};

// ─── Card Payment ─────────────────────────────────────────────────────────────
class CardPayment : public Payment {
private:
    std::string cardHolderName;
    std::string maskedCard;   // store only last 4 digits — never full number

public:
    CardPayment(double amount, const std::string& cardHolderName,
                const std::string& last4Digits)
        : Payment(amount), cardHolderName(cardHolderName),
          maskedCard("**** **** **** " + last4Digits) {}

    bool processPayment() override {
        std::cout << "Processing Card payment for " << cardHolderName << "...\n";
        paid = true;
        displayReceipt();
        return true;
    }

    std::string getMethod() const override {
        return "Card (" + maskedCard + ")";
    }
};

// ─── Cash on Delivery ─────────────────────────────────────────────────────────
class CashOnDelivery : public Payment {
public:
    explicit CashOnDelivery(double amount) : Payment(amount) {}

    bool processPayment() override {
        std::cout << "Cash on Delivery selected. Pay Rs."
                  << std::fixed << std::setprecision(2)
                  << amount << " upon delivery.\n";
        paid = true;
        displayReceipt();
        return true;
    }

    std::string getMethod() const override { return "Cash on Delivery"; }
};

#endif
