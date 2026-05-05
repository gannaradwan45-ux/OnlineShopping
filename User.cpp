#include "User.h"
#include <iostream>
using namespace std;

// ================================
// User Base Class
// ================================
User::User(int id, string u, string p, string e, string r) {
    userID = id;
    username = u;
    password = p;
    email = e;
    role = r;
}

void User::login() {
    cout << ">> User [" << username << "] logged in." << endl;
}

void User::logout() {
    cout << ">> User [" << username << "] logged out." << endl;
}

void User::viewProfile() const {
    cout << "================================" << endl;
    cout << "  User Profile" << endl;
    cout << "================================" << endl;
    cout << "  ID       : " << userID << endl;
    cout << "  Username : " << username << endl;
    cout << "  Email    : " << email << endl;
    cout << "  Role     : " << role << endl;
    cout << "================================" << endl;
}

void User::updateProfile(string newUsername, string newEmail) {
    username = newUsername;
    email = newEmail;
    cout << ">> Profile updated successfully." << endl;
}

// NEW METHODS
bool User::checkPassword(string p) const {
    return password == p;
}

void User::setPassword(string p) {
    password = p;
}

string User::getPassword() const {
    return password;
}

int User::getUserID() const { return userID; }
string User::getUsername() const { return username; }
string User::getRole() const { return role; }
string User::getEmail() const { return email; }

// ================================
// Admin Class
// ================================
Admin::Admin(int id, string u, string p, string e)
    : User(id, u, p, e, "Admin") {
}

void Admin::accessPermissions() const {
    cout << ">> Admin Permissions: Full Access" << endl;
}

void Admin::addProduct() const {
    cout << ">> [Admin] Adding new product..." << endl;
}

void Admin::removeProduct() const {
    cout << ">> [Admin] Removing product..." << endl;
}

void Admin::manageUsers() const {
    cout << ">> [Admin] Managing users..." << endl;
}

void Admin::generateReports() const {
    cout << ">> [Admin] Generating reports..." << endl;
}

void Admin::managePromotions() const {
    cout << ">> [Admin] Managing promotions..." << endl;
}

// ================================
// Vendor Class
// ================================
Vendor::Vendor(int id, string u, string p, string e)
    : User(id, u, p, e, "Vendor") {
}

void Vendor::accessPermissions() const {
    cout << ">> Vendor Permissions: Inventory & Sales Access" << endl;
}

void Vendor::updateInventory() const {
    cout << ">> [Vendor] Updating inventory..." << endl;
}

void Vendor::trackSales() const {
    cout << ">> [Vendor] Tracking sales..." << endl;
}

void Vendor::receiveNotifications() const {
    cout << ">> [Vendor] Checking notifications..." << endl;
}

// ================================
// Customer Class
// ================================
Customer::Customer(int id, string u, string p, string e)
    : User(id, u, p, e, "Customer") {
}

void Customer::accessPermissions() const {
    cout << ">> Customer Permissions: Shopping Access" << endl;
}

void Customer::browseProducts() const {
    cout << ">> [Customer] Browsing products..." << endl;
}

void Customer::addToCart() const {
    cout << ">> [Customer] Adding item to cart..." << endl;
}

void Customer::checkout() const {
    cout << ">> [Customer] Checking out..." << endl;
}

void Customer::trackOrders() const {
    cout << ">> [Customer] Tracking orders..." << endl;
}

void Customer::viewHistory() const {
    cout << ">> [Customer] Viewing order history..." << endl;
}
