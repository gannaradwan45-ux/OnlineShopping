#pragma once
#include <string>
#include <iostream>
using namespace std;

class Product {
private:
    int productID;
    string productName;
    double price;
    int quantity;
    string category;
    int vendorID;  // 0 for admin, otherwise vendor ID

public:
    // Constructors
    Product();
    Product(int id, string name, double p, int qty, string cat, int vendor = 0);

    // Getters
    int getProductID() const;
    string getProductName() const;
    double getPrice() const;
    int getQuantity() const;
    string getCategory() const;
    int getVendorID() const;

    // Setters
    void setProductName(string name);
    void setPrice(double p);
    void setQuantity(int qty);
    void setCategory(string cat);
    void setVendorID(int vendor);

    // Methods
    void display() const;
    bool isAvailable() const;
};
