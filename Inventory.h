#pragma once
#include "product.h"
#include <vector>
#include <string>
using namespace std;

class Inventory {
private:
    vector<Product*> products;
    int capacity;
    int currentCount;

public:
    Inventory(int cap);
    ~Inventory();

    void addProduct(Product* product);
    void addProduct(string name, double price, string category, int quantity, int vendorID);
    Product* getProduct(int productID);
    vector<Product*> getAllProducts();
    vector<Product*> getProductsByCategory(string category);
    vector<Product*> getProductsByVendor(int vendorID);
    bool isProductAvailable(int productID, int quantity);
    void updateStock(int productID, int newQuantity);

    // New methods for admin
    void updateProduct(int productID, double newPrice, int newStock);  // Add this
    bool removeProduct(int productID);  // Add this

    bool removeProductByVendor(int productID, int vendorID);
};
