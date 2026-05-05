#include "product.h"

// Default constructor
Product::Product() {
    productID = 0;
    productName = "";
    price = 0.0;
    quantity = 0;
    category = "";
    vendorID = 0;
}

// Parameterized constructor
Product::Product(int id, string name, double p, int qty, string cat, int vendor) {
    productID = id;
    productName = name;
    price = p;
    quantity = qty;
    category = cat;
    vendorID = vendor;
}

// Getters
int Product::getProductID() const {
    return productID;
}

string Product::getProductName() const {
    return productName;
}

double Product::getPrice() const {
    return price;
}

int Product::getQuantity() const {
    return quantity;
}

string Product::getCategory() const {
    return category;
}

int Product::getVendorID() const {
    return vendorID;
}

// Setters
void Product::setProductName(string name) {
    productName = name;
}

void Product::setPrice(double p) {
    price = p;
}

void Product::setQuantity(int qty) {
    quantity = qty;
}

void Product::setCategory(string cat) {
    category = cat;
}

void Product::setVendorID(int vendor) {
    vendorID = vendor;
}

// Methods
void Product::display() const {
    cout << "Product ID: " << productID << endl;
    cout << "Name: " << productName << endl;
    cout << "Price: $" << price << endl;
    cout << "Stock: " << quantity << endl;
    cout << "Category: " << category << endl;
    cout << "Vendor ID: " << vendorID << endl;
}

bool Product::isAvailable() const {
    return quantity > 0;
}
