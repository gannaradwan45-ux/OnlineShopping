#include "Inventory.h"
using namespace std;

Inventory::Inventory(int lowLimit) {
    lowStockLimit = lowLimit;
}

Inventory::~Inventory() {
    stock.clear();
}

// ================================
// اضافة منتج
// ================================
void Inventory::addProduct(Product* product) {
    if (product == nullptr) return;

    int id = product->getProductID();

    if (stock.find(id) != stock.end()) return;

    stock[id] = product;
}

// ================================
// حذف منتج
// ================================
void Inventory::removeProduct(int productID) {
    if (stock.find(productID) == stock.end()) {
        cout << ">> Product ID " << productID
            << " not found!" << endl;
        return;
    }
    stock.erase(productID);
    cout << ">> [Inventory] Product removed." << endl;
}

// ================================
// تعديل الكمية
// ================================
void Inventory::updateStock(int productID, int newQty) {
    if (stock.find(productID) == stock.end()) {
        cout << ">> Product ID " << productID
            << " not found!" << endl;
        return;
    }

    if (newQty < 0) {
        cout << ">> Error: Quantity cannot be negative!" << endl;
        return;
    }

    stock[productID]->setQuantity(newQty);

    if (newQty < lowStockLimit) {
        Notification notif(0,
            "LOW STOCK WARNING: " +
            stock[productID]->getProductName() +
            " has only " + to_string(newQty) + " units left!");
        notif.sendNotification();
    }
}

// ================================
// اعادة تخزين
// ================================
void Inventory::restockProduct(int productID, int addQty) {
    if (stock.find(productID) == stock.end()) {
        cout << ">> Product ID " << productID
            << " not found!" << endl;
        return;
    }

    if (addQty <= 0) {
        cout << ">> Error: Add quantity must be positive!" << endl;
        return;
    }

    int currentQty = stock[productID]->getQuantity();
    int newQty = currentQty + addQty;
    stock[productID]->setQuantity(newQty);

    cout << ">> [Inventory] Restocked: "
        << stock[productID]->getProductName()
        << " | New Qty: " << newQty << endl;
}

// ================================
// فحص المخزون القليل
// ================================
void Inventory::checkLowStock() const {
    for (auto& pair : stock) {
        if (pair.second->getQuantity() < lowStockLimit) {
            Notification notif(0,
                "LOW STOCK: " +
                pair.second->getProductName() +
                " | Qty: " +
                to_string(pair.second->getQuantity()));
            notif.sendNotification();
        }
    }
}

// ================================
// فحص توافر منتج
// ================================
bool Inventory::isProductAvailable(int productID,
    int requiredQty) const {
    if (stock.find(productID) == stock.end()) {
        return false;
    }
    return stock.at(productID)->getQuantity() >= requiredQty;
}

// ================================
// عرض المخزون كامل
// ================================
void Inventory::displayInventory() const {
    cout << "\n===== Inventory =====" << endl;

    if (stock.empty()) {
        cout << "  (No products in inventory)" << endl;
    }
    else {
        for (auto& pair : stock) {
            Product* p = pair.second;
            cout << "  [" << p->getProductID() << "] "
                << p->getProductName()
                << " | Price: $" << p->getPrice()
                << " | Qty: " << p->getQuantity();

            if (p->getQuantity() < lowStockLimit)
                cout << "  !! LOW STOCK";

            cout << endl;
        }
    }
    cout << "=====================" << endl;
}

// ================================
// جيب منتج بالـ ID
// ================================
Product* Inventory::getProduct(int productID) const {
    if (stock.find(productID) == stock.end()) {
        return nullptr;
    }
    return stock.at(productID);
}