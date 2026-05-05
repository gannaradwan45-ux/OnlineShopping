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
    string filename;

public:
    Inventory(int cap, string file = "products_data.txt");
    ~Inventory();

    void addProduct(Product* product);
    void addProduct(string name, double price, string category, int quantity, int vendorID);

    void saveToFile();
    void loadFromFile();

    Product* getProduct(int productID);
    vector<Product*> getAllProducts();
    vector<Product*> getProductsByCategory(string category);
    vector<Product*> getProductsByVendor(int vendorID);
    bool isProductAvailable(int productID, int quantity);
    void updateStock(int productID, int newQuantity);
    void updateProduct(int productID, double newPrice, int newStock);
    bool removeProduct(int productID);
    bool removeProductByVendor(int productID, int vendorID);

    int getCurrentCount() const { return currentCount; }
};
