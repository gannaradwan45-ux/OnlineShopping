#pragma once
#include <iostream>
#include <string>
#include <map>
#include "product.h"
#include "notification.h"
using namespace std;

class Inventory {
private:
    map<int, Product*> stock;  // productID → Product*
    int lowStockLimit;         // لو اقل من كده يبعت notification

public:
    Inventory(int lowLimit = 5);
    ~Inventory();

    // اضافة وحذف
    void addProduct(Product* product);
    void removeProduct(int productID);

    // تعديل الكمية
    void updateStock(int productID, int newQty);
    void restockProduct(int productID, int addQty);

    // فحص المخزون
    void checkLowStock() const;
    bool isProductAvailable(int productID, int requiredQty) const;

    // عرض
    void displayInventory() const;
    Product* getProduct(int productID) const;
};
