#include "Inventory.h"
#include <iostream>
using namespace std;

Inventory::Inventory(int cap) {
    capacity = cap;
    currentCount = 0;
    products.resize(capacity, nullptr);
}

Inventory::~Inventory() {
    for (int i = 0; i < currentCount; i++) {
        delete products[i];
    }
}

void Inventory::addProduct(Product* product) {
    if (currentCount < capacity) {
        products[currentCount++] = product;
    } else {
        cout << ">> Inventory full!" << endl;
    }
}

void Inventory::addProduct(string name, double price, string category, int quantity, int vendorID) {
    if (currentCount < capacity) {
        int newID = currentCount + 1;
        Product* p = new Product(newID, name, price, quantity, category, vendorID);
        products[currentCount++] = p;
        cout << ">> Product added: " << name << " (ID: " << newID << ")" << endl;
    } else {
        cout << ">> Inventory is full! Cannot add product." << endl;
    }
}

Product* Inventory::getProduct(int productID) {
    for (int i = 0; i < currentCount; i++) {
        if (products[i]->getProductID() == productID) {
            return products[i];
        }
    }
    return nullptr;
}

vector<Product*> Inventory::getAllProducts() {
    vector<Product*> result;
    for (int i = 0; i < currentCount; i++) {
        result.push_back(products[i]);
    }
    return result;
}

vector<Product*> Inventory::getProductsByCategory(string category) {
    vector<Product*> result;
    for (int i = 0; i < currentCount; i++) {
        if (products[i]->getCategory() == category) {
            result.push_back(products[i]);
        }
    }
    return result;
}

vector<Product*> Inventory::getProductsByVendor(int vendorID) {
    vector<Product*> result;
    for (int i = 0; i < currentCount; i++) {
        if (products[i]->getVendorID() == vendorID) {
            result.push_back(products[i]);
        }
    }
    return result;
}

bool Inventory::isProductAvailable(int productID, int quantity) {
    Product* p = getProduct(productID);
    if (p && p->getQuantity() >= quantity) {
        return true;
    }
    return false;
}

void Inventory::updateStock(int productID, int newQuantity) {
    Product* p = getProduct(productID);
    if (p) {
        p->setQuantity(newQuantity);
    }
}

void Inventory::updateProduct(int productID, double newPrice, int newStock) {
    Product* p = getProduct(productID);
    if (p) {
        p->setPrice(newPrice);
        p->setQuantity(newStock);
        cout << ">> Product " << productID << " updated: Price = $" << newPrice << ", Stock = " << newStock << endl;
    }
}

bool Inventory::removeProduct(int productID) {
    for (int i = 0; i < currentCount; i++) {
        if (products[i]->getProductID() == productID) {
            delete products[i];
            for (int j = i; j < currentCount - 1; j++) {
                products[j] = products[j + 1];
            }
            products[currentCount - 1] = nullptr;
            currentCount--;
            cout << ">> Product " << productID << " removed from inventory" << endl;
            return true;
        }
    }
    return false;
}

bool Inventory::removeProductByVendor(int productID, int vendorID) {
    for (int i = 0; i < currentCount; i++) {
        if (products[i]->getProductID() == productID && products[i]->getVendorID() == vendorID) {
            delete products[i];
            for (int j = i; j < currentCount - 1; j++) {
                products[j] = products[j + 1];
            }
            products[currentCount - 1] = nullptr;
            currentCount--;
            return true;
        }
    }
    return false;
}
