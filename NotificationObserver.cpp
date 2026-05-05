#include "NotificationObserver.h"
using namespace std;

// ================================
// Customer Observer
// ================================
CustomerObserver::CustomerObserver(int id, string n) {
    userID = id;
    name = n;
}

void CustomerObserver::update(string message) {
    cout << ">> [CUSTOMER NOTIFICATION] "
        << name << ": " << message << endl;
    Notification notif(userID, message);
    notif.sendNotification();
}

string CustomerObserver::getObserverName() const {
    return name;
}

// ================================
// Vendor Observer
// ================================
VendorObserver::VendorObserver(int id, string n) {
    userID = id;
    name = n;
}

void VendorObserver::update(string message) {
    cout << ">> [VENDOR NOTIFICATION] "
        << name << ": " << message << endl;
    Notification notif(userID, message);
    notif.sendNotification();
}

string VendorObserver::getObserverName() const {
    return name;
}

// ================================
// Admin Observer
// ================================
AdminObserver::AdminObserver(int id, string n) {
    userID = id;
    name = n;
}

void AdminObserver::update(string message) {
    cout << ">> [ADMIN NOTIFICATION] "
        << name << ": " << message << endl;
    Notification notif(userID, message);
    notif.sendNotification();
}

string AdminObserver::getObserverName() const {
    return name;
}

// ================================
// Notification Manager
// ================================
void NotificationManager::addObserver(IObserver* observer) {
    observers.push_back(observer);
    cout << ">> [NotificationManager] Added: "
        << observer->getObserverName() << endl;
}

void NotificationManager::removeObserver(string name) {
    for (int i = 0; i < (int)observers.size(); i++) {
        if (observers[i]->getObserverName() == name) {
            cout << ">> [NotificationManager] Removed: "
                << name << endl;
            observers.erase(observers.begin() + i);
            return;
        }
    }
    cout << ">> Observer not found: " << name << endl;
}

void NotificationManager::notifyAll(string message) {
    cout << "\n>> [BROADCASTING] " << message << endl;
    for (auto& obs : observers) {
        obs->update(message);
    }
}

void NotificationManager::notifyOne(string name, string message) {
    for (auto& obs : observers) {
        if (obs->getObserverName() == name) {
            obs->update(message);
            return;
        }
    }
    cout << ">> Observer not found: " << name << endl;
}

void NotificationManager::displayObservers() const {
    cout << "\n===== Registered Observers =====" << endl;
    if (observers.empty()) {
        cout << "  (No observers)" << endl;
    }
    else {
        for (auto& obs : observers) {
            cout << "  - " << obs->getObserverName() << endl;
        }
    }
    cout << "================================" << endl;
}

NotificationManager::~NotificationManager() {
    observers.clear();
}
