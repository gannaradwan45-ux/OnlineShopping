#include "product.h"
#include <iostream>
using namespace std;

Product::Product(int id, string name, double p,
    string cat, int qty) {
    productID = id;
    productName = name;
    price = p;
    category = cat;
    quantity = qty;
}

// ================================
// Getters
// ================================
int Product::getProductID() const {
    return productID;
}

string Product::getProductName() const {
    return productName;
}

double Product::getPrice() const {
    return price;
}

string Product::getCategory() const {
    return category;
}

int Product::getQuantity() const {
    return quantity;
}

// ================================
// Setters
// ================================
void Product::setQuantity(int qty) {
    if (qty >= 0)
        quantity = qty;
    else
        cout << ">> Error: Quantity cannot be negative!" << endl;
}

void Product::setPrice(double p) {
    if (p >= 0)
        price = p;
    else
        cout << ">> Error: Price cannot be negative!" << endl;
}

// ================================
// Calculate Final Price
// ================================
double Product::calculateFinalPrice() const {
    return price * quantity;
}

// ================================
// Update Stock
// ================================
void Product::updateStock(int newQty) {
    if (newQty >= 0) {
        quantity = newQty;
        cout << ">> [Product] Stock updated: "
            << productName
            << " | New Qty: " << quantity << endl;
    }
    else {
        cout << ">> Error: Invalid quantity!" << endl;
    }
}

// ================================
// Display Product
// ================================
void Product::displayProduct() const {
    cout << "-----------------------" << endl;
    cout << "ID       : " << productID << endl;
    cout << "Name     : " << productName << endl;
    cout << "Price    : $" << price << endl;
    cout << "Category : " << category << endl;
    cout << "Quantity : " << quantity << endl;
    cout << "-----------------------" << endl;
}

// ================================
// Display Info (short version)
// ================================
void Product::displayInfo() const {
    cout << "  [" << productID << "] "
        << productName
        << " | Price: $" << price
        << " | Stock: " << quantity
        << " | Category: " << category
        << endl;
}