#pragma once
#include <string>
#include <vector>
using namespace std;

// Strategy Pattern: PaymentStrategy 

class PaymentStrategy {
public:
    virtual bool pay(double amount) = 0;
    virtual string getMethodName() = 0;
    virtual ~PaymentStrategy() {}
};

// Subclasses: different payment methods 

class CreditCardPayment : public PaymentStrategy {
private:
    string cardNumber;
    string cardHolder;
public:
    CreditCardPayment(string number, string holder);
    bool pay(double amount) override;
    string getMethodName() override;
};

class CashPayment : public PaymentStrategy {
private:
    double cashGiven;
public:
    CashPayment(double cash);
    bool pay(double amount) override;
    string getMethodName() override;
    double getChange(double amount);
};

class WalletPayment : public PaymentStrategy {
private:
    string walletID;
    double balance;
public:
    WalletPayment(string id, double bal);
    bool pay(double amount) override;
    string getMethodName() override;
    double getBalance();
};

// Main Payment Class 

class Payment {
private:
    int paymentID;
    string order;
    // Full Order object
    double amount;
    string paymentMethod;
    string status;
    PaymentStrategy* strategy;

public:
    // Constructor with Strategy Pattern
    Payment(int id, string ord, double amt, PaymentStrategy* strat);

    // Overloaded Constructor (simple version)
    Payment(int id, string ord, double amt, string method);

    ~Payment();

    void processPayment();
    void displayReceipt();

    
    double getAmount() const;
    string getStatus() const;
    string getPaymentMethod() const;
    int getPaymentID() const;
};

