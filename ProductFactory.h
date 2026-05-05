#pragma once
#include <iostream>
#include <string>
#include "product.h"
using namespace std;

// Factory Pattern
class ProductFactory {
public:
    // انواع المنتجات
    static Product* createProduct(
        string type,
        int id,
        string name,
        double price,
        int qty
    );
};
