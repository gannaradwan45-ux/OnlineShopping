#include "Inventory.h"
#include <iostream>
#include <fstream>
#include <sstream>
using namespace std;

Inventory::Inventory(int cap, string file) {
    capacity = cap;
    currentCount = 0;
    filename = file;
    products.resize(capacity, nullptr);
}

Inventory::~Inventory() {
    for (int i = 0; i < currentCount; i++) {
        delete products[i];
    }
}

// ---- حفظ المنتجات على الـ disk ----
void Inventory::saveToFile() {
    ofstream file(filename);
    for (int i = 0; i < currentCount; i++) {
        Product* p = products[i];
        file << p->getProductID() << ","
             << p->getProductName() << ","
             << p->getPrice() << ","
             << p->getQuantity() << ","
             << p->getCategory() << ","
             << p->getVendorID() << "\n";
    }
    file.close();
}

// ---- تحميل المنتجات من الـ disk ----
void Inventory::loadFromFile() {
    // امسح القديم
    for (int i = 0; i < currentCount; i++) {
        delete products[i];
        products[i] = nullptr;
    }
    currentCount = 0;

    ifstream file(filename);
    if (!file.is_open()) return;

    string line;
    while (getline(file, line) && currentCount < capacity) {
        // تجاهل السطور الفاضية
        if (line.empty() || line == "\r") continue;

        // شيل الـ \r لو موجود (Windows line endings)
        if (!line.empty() && line.back() == '\r')
            line.pop_back();

        stringstream ss(line);
        string idStr, name, priceStr, qtyStr, category, vendorStr;
        getline(ss, idStr, ',');
        getline(ss, name, ',');
        getline(ss, priceStr, ',');
        getline(ss, qtyStr, ',');
        getline(ss, category, ',');
        getline(ss, vendorStr, ',');

        // تحقق إن الحقول مش فاضية قبل stoi/stod
        if (idStr.empty() || priceStr.empty() || qtyStr.empty() || vendorStr.empty())
            continue;

        try {
            int id       = stoi(idStr);
            double price = stod(priceStr);
            int qty      = stoi(qtyStr);
            int vendorID = stoi(vendorStr);

            if (currentCount >= capacity) {
                capacity *= 2;
                products.resize(capacity, nullptr);
            }

            products[currentCount++] = new Product(id, name, price, qty, category, vendorID);
        } catch (...) {
            // سطر فيه بيانات غلط — اتجاهله وكمّل
            cout << ">> Skipping invalid product line: " << line << endl;
            continue;
        }
    }
    file.close();
}

// ---- إضافة منتج (pointer) ----
void Inventory::addProduct(Product* product) {
    if (currentCount >= capacity) {
        // وسّع المصفوفة تلقائياً
        capacity *= 2;
        products.resize(capacity, nullptr);
    }
    products[currentCount++] = product;
    saveToFile();
}

// ---- إضافة منتج (بالبيانات) ----
void Inventory::addProduct(string name, double price, string category, int quantity, int vendorID) {
    if (currentCount >= capacity) {
        capacity *= 2;
        products.resize(capacity, nullptr);
    }
    // اعمل ID جديد = أكبر ID موجود + 1
    int newID = 0;
    for (int i = 0; i < currentCount; i++) {
        if (products[i]->getProductID() > newID)
            newID = products[i]->getProductID();
    }
    newID++;

    products[currentCount++] = new Product(newID, name, price, quantity, category, vendorID);
    cout << ">> Product added: " << name << " (ID: " << newID << ")" << endl;
    saveToFile();
}

Product* Inventory::getProduct(int productID) {
    for (int i = 0; i < currentCount; i++) {
        if (products[i] && products[i]->getProductID() == productID)
            return products[i];
    }
    return nullptr;
}

vector<Product*> Inventory::getAllProducts() {
    vector<Product*> result;
    for (int i = 0; i < currentCount; i++) {
        if (products[i]) result.push_back(products[i]);
    }
    return result;
}

vector<Product*> Inventory::getProductsByCategory(string category) {
    vector<Product*> result;
    for (int i = 0; i < currentCount; i++) {
        if (products[i] && products[i]->getCategory() == category)
            result.push_back(products[i]);
    }
    return result;
}

vector<Product*> Inventory::getProductsByVendor(int vendorID) {
    vector<Product*> result;
    for (int i = 0; i < currentCount; i++) {
        if (products[i] && products[i]->getVendorID() == vendorID)
            result.push_back(products[i]);
    }
    return result;
}

bool Inventory::isProductAvailable(int productID, int quantity) {
    Product* p = getProduct(productID);
    return p && p->getQuantity() >= quantity;
}

void Inventory::updateStock(int productID, int newQuantity) {
    Product* p = getProduct(productID);
    if (p) {
        p->setQuantity(newQuantity);
        saveToFile();
    }
}

void Inventory::updateProduct(int productID, double newPrice, int newStock) {
    Product* p = getProduct(productID);
    if (p) {
        p->setPrice(newPrice);
        p->setQuantity(newStock);
        saveToFile();
        cout << ">> Product " << productID << " updated." << endl;
    }
}

bool Inventory::removeProduct(int productID) {
    for (int i = 0; i < currentCount; i++) {
        if (products[i] && products[i]->getProductID() == productID) {
            delete products[i];
            for (int j = i; j < currentCount - 1; j++)
                products[j] = products[j + 1];
            products[currentCount - 1] = nullptr;
            currentCount--;
            saveToFile();
            return true;
        }
    }
    return false;
}

bool Inventory::removeProductByVendor(int productID, int vendorID) {
    for (int i = 0; i < currentCount; i++) {
        if (products[i] &&
            products[i]->getProductID() == productID &&
            products[i]->getVendorID() == vendorID) {
            delete products[i];
            for (int j = i; j < currentCount - 1; j++)
                products[j] = products[j + 1];
            products[currentCount - 1] = nullptr;
            currentCount--;
            saveToFile();
            return true;
        }
    }
    return false;
}
