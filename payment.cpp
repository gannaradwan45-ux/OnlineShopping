#include "Payment.h"
#include <iostream>
#include <stdexcept>
#include "notification.h"
using namespace std;

CreditCardPayment::CreditCardPayment(string number, string holder) {
    cardNumber = number;
    cardHolder = holder;
}

bool CreditCardPayment::pay(double amount) {
    if (amount <= 0)
        throw invalid_argument("Amount must be greater than zero!");

    cout << "Processing Credit Card payment of $" << amount << endl;
    cout << "Card Holder: " << cardHolder << endl;
    cout << "Card Number: ****" << cardNumber.substr(cardNumber.size() - 4) << endl;
    return true;
}

string CreditCardPayment::getMethodName() {
    return "Credit Card";
}

CashPayment::CashPayment(double cash) {
    cashGiven = cash;
}

bool CashPayment::pay(double amount) {
    if (cashGiven < amount)
        throw runtime_error("Insufficient cash! Amount given: $" + to_string(cashGiven));

    cout << "Processing Cash payment of $" << amount << endl;
    cout << "Cash given: $" << cashGiven << endl;
    cout << "Change: $" << getChange(amount) << endl;
    return true;
}

string CashPayment::getMethodName() {
    return "Cash";
}

double CashPayment::getChange(double amount) {
    return cashGiven - amount;
}

WalletPayment::WalletPayment(string id, double bal) {
    walletID = id;
    balance = bal;
}

bool WalletPayment::pay(double amount) {
    if (balance < amount)
        throw runtime_error("Insufficient wallet balance!");

    balance -= amount;
    cout << "Processing Wallet payment of $" << amount << endl;
    cout << "Wallet ID: " << walletID << endl;
    cout << "Remaining balance: $" << balance << endl;
    return true;
}

string WalletPayment::getMethodName() {
    return "Digital Wallet";
}

double WalletPayment::getBalance() {
    return balance;
}

Payment::Payment(int id, string ord, double amt, PaymentStrategy* strat)
    : order(ord) {
    paymentID = id;
    amount = amt;
    strategy = strat;
    paymentMethod = strat->getMethodName();
    status = "Pending";
}

Payment::Payment(int id, string ord, double amt, string method)
    : order(ord) {
    paymentID = id;
    amount = amt;
    paymentMethod = method;
    status = "Pending";
    strategy = nullptr;
}

Payment::~Payment() {}

void Payment::processPayment() {
    try {
        if (amount <= 0)
            throw invalid_argument("Invalid payment amount!");

        if (strategy != nullptr) {
            bool success = strategy->pay(amount);
            if (success) {
                status = "Paid";
                cout << "Payment successful!" << endl;
            }
        }
        else {
            status = "Paid";
            cout << "Payment of $" << amount << " processed via " << paymentMethod << endl;
        }
    }
    catch (const invalid_argument& e) {
        status = "Failed";
        cout << "Payment Error: " << e.what() << endl;
    }
    catch (const runtime_error& e) {
        status = "Failed";
        cout << "Payment Failed: " << e.what() << endl;
    }
}

void Payment::displayReceipt() {
    
        cout << "\n========== RECEIPT ==========" << endl;
        cout << "Payment ID : " << paymentID << endl;
        cout << "Order ID   : " << order << endl;
        cout << "Total      : $" << amount << endl;
        cout << "Method     : " << paymentMethod << endl;
        cout << "Status     : " << status << endl;
        cout << "============================\n" << endl;
    
   
}
// وفي payment.cpp
double Payment::getAmount() const { return amount; }
string Payment::getStatus() const { return status; }
string Payment::getPaymentMethod() const { return paymentMethod; }
int Payment::getPaymentID() const { return paymentID; }