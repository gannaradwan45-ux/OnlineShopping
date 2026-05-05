#pragma once
#include <iostream>
#include <string>
using namespace std;

class Product {
private:
    int productID;
    string productName;
    double price;
    string category;
    int quantity;

public:
    Product(int id, string name, double p, string cat, int qty);

    // Getters - كلها const
    int getProductID() const;
    string getProductName() const;
    double getPrice() const;
    string getCategory() const;
    int getQuantity() const;

    // Setters
    void setQuantity(int qty);
    void setPrice(double p);

    // Methods
    double calculateFinalPrice() const;
    void updateStock(int newQty);
    void displayProduct() const;
    void displayInfo() const;
};
