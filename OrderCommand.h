#pragma once
#include <iostream>
#include <string>
#include "ShoppingCart.h"
using namespace std;

// Command Pattern

// Interface
class ICommand {
public:
    virtual void execute() = 0;
    virtual void undo() = 0;
    virtual ~ICommand() {}
};

// Checkout Command
class CheckoutCommand : public ICommand {
private:
    ShoppingCart& cart;
    string status;
public:
    CheckoutCommand(ShoppingCart& c);
    void execute() override;
    void undo() override;
};

// Cancel Order Command
class CancelOrderCommand : public ICommand {
private:
    int orderID;
    string previousStatus;
    string currentStatus;
public:
    CancelOrderCommand(int id);
    void execute() override;
    void undo() override;
};

