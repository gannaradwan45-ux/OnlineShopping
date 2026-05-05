#pragma once
#include <string>
#include <iostream>
using namespace std;

class User {
protected:
    int userID;
    string username;
    string password;
    string email;
    string role;
public:
    User(int id, string u, string p, string e, string r);
    virtual ~User() {}
    void login();
    void logout();
    void viewProfile() const;
    void updateProfile(string newUsername, string newEmail);
    bool checkPassword(string p) const;
    void setPassword(string p);
    string getPassword() const;
    virtual void accessPermissions() const = 0;
    int getUserID() const;
    string getUsername() const;
    string getRole() const;
    string getEmail() const;
};

class Admin : public User {
public:
    Admin(int id, string u, string p, string e);
    void addProduct() const;
    void removeProduct() const;
    void manageUsers() const;
    void generateReports() const;
    void managePromotions() const;
    void accessPermissions() const override;
};

class Vendor : public User {
public:
    Vendor(int id, string u, string p, string e);
    void updateInventory() const;
    void trackSales() const;
    void receiveNotifications() const;
    void accessPermissions() const override;
};

class Customer : public User {
public:
    Customer(int id, string u, string p, string e);
    void browseProducts() const;
    void addToCart() const;
    void checkout() const;
    void trackOrders() const;
    void viewHistory() const;
    void accessPermissions() const override;
};
