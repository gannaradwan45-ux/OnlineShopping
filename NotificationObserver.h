#pragma once
#include <iostream>
#include <string>
#include <vector>
#include "notification.h"
using namespace std;

// Observer Pattern

// Interface
class IObserver {
public:
    virtual void update(string message) = 0;
    virtual string getObserverName() const = 0;
    virtual ~IObserver() {}
};

// Customer Observer
class CustomerObserver : public IObserver {
private:
    int userID;
    string name;
public:
    CustomerObserver(int id, string n);
    void update(string message) override;
    string getObserverName() const override;
};

// Vendor Observer
class VendorObserver : public IObserver {
private:
    int userID;
    string name;
public:
    VendorObserver(int id, string n);
    void update(string message) override;
    string getObserverName() const override;
};

// Admin Observer
class AdminObserver : public IObserver {
private:
    int userID;
    string name;
public:
    AdminObserver(int id, string n);
    void update(string message) override;
    string getObserverName() const override;
};

// Notification Manager
class NotificationManager {
private:
    vector<IObserver*> observers;

public:
    void addObserver(IObserver* observer);
    void removeObserver(string name);
    void notifyAll(string message);
    void notifyOne(string name, string message);
    void displayObservers() const;
    ~NotificationManager();
};

