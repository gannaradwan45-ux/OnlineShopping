#include "ShoppingCart.h"
#include <iostream>
using namespace std;

CartItem::CartItem(int pid, string pname, double pprice, int qty) {
    productID = pid;
    productName = pname;
    price = pprice;
    quantity = qty;
    subtotal = price * quantity;
}
CartItem::~CartItem() {}

int CartItem::getProductID() const { return productID; }
string CartItem::getProductName() const { return productName; }
double CartItem::getPrice() const { return price; }
int CartItem::getQuantity() const { return quantity; }
double CartItem::getSubtotal() const { return subtotal; }

void CartItem::setQuantity(int qty) {
    quantity = qty;
    subtotal = price * quantity;
}

void CartItem::display() const {
    cout << "  [" << productID << "] " << productName
         << " | Price: $" << price << " | Qty: " << quantity << endl;
}
void CartItem::display(bool showSubtotal) const {
    cout << "  [" << productID << "] " << productName
         << " | Price: $" << price << " | Qty: " << quantity;
    if (showSubtotal) cout << " | Subtotal: $" << subtotal;
    cout << endl;
}

ShoppingCart::ShoppingCart(int cid, int custID) {
    cartID = cid;
    customerID = custID;
    totalAmount = 0.0;
}
ShoppingCart::~ShoppingCart() { items.clear(); }

void ShoppingCart::recalculateTotal() {
    totalAmount = 0.0;
    for (int i = 0; i < (int)items.size(); i++)
        totalAmount += items[i].getSubtotal();
}

void ShoppingCart::addItem(int productID, string productName, double price, int quantity) {
    for (int i = 0; i < (int)items.size(); i++) {
        if (items[i].getProductID() == productID) {
            items[i].setQuantity(items[i].getQuantity() + quantity);
            recalculateTotal();
            cout << "  >> Quantity updated for: " << productName << endl;
            return;
        }
    }
    CartItem newItem(productID, productName, price, quantity);
    items.push_back(newItem);
    recalculateTotal();
    cout << "  >> Added to cart: " << productName << endl;
}

void ShoppingCart::removeItem(int productID) {
    for (int i = 0; i < (int)items.size(); i++) {
        if (items[i].getProductID() == productID) {
            cout << "  >> Removed: " << items[i].getProductName() << endl;
            items.erase(items.begin() + i);
            recalculateTotal();
            return;
        }
    }
    cout << "  >> Product not found." << endl;
}

void ShoppingCart::updateQuantity(int productID, int newQty) {
    if (newQty <= 0) { removeItem(productID); return; }
    for (int i = 0; i < (int)items.size(); i++) {
        if (items[i].getProductID() == productID) {
            items[i].setQuantity(newQty);
            recalculateTotal();
            cout << "  >> Quantity updated." << endl;
            return;
        }
    }
}

int ShoppingCart::getQuantity(int productID) const {
    for (int i = 0; i < (int)items.size(); i++) {
        if (items[i].getProductID() == productID) {
            return items[i].getQuantity();
        }
    }
    return 0;
}

void ShoppingCart::clearCart() { items.clear(); totalAmount = 0.0; }

int ShoppingCart::getCartID() const { return cartID; }
int ShoppingCart::getCustomerID() const { return customerID; }
double ShoppingCart::getTotalAmount() const { return totalAmount; }
vector<CartItem> ShoppingCart::getItems() const { return items; }
bool ShoppingCart::isEmpty() const { return items.empty(); }

void ShoppingCart::display() const {
    cout << "\n===== Shopping Cart (ID: " << cartID << ") =====" << endl;
    if (items.empty()) cout << "  Cart is empty." << endl;
    else {
        for (int i = 0; i < (int)items.size(); i++) items[i].display(true);
        cout << "  --------------------------" << endl;
        cout << "  TOTAL: $" << totalAmount << endl;
    }
    cout << "========================================" << endl;
}
void ShoppingCart::display(string header) const {
    cout << "\n===== " << header << " =====" << endl;
    if (items.empty()) cout << "  Cart is empty." << endl;
    else {
        for (int i = 0; i < (int)items.size(); i++) items[i].display(true);
        cout << "  --------------------------" << endl;
        cout << "  TOTAL: $" << totalAmount << endl;
    }
    cout << "========================================" << endl;
}

Order::Order(int id, int custID, ShoppingCart& cart, string date) {
    orderID = id;
    customerID = custID;
    totalPrice = cart.getTotalAmount();
    status = "Pending";
    orderDate = date;
}
Order::Order(int id, int custID, double total, string date) {
    orderID = id;
    customerID = custID;
    totalPrice = total;
    status = "Pending";
    orderDate = date;
}
Order::~Order() {}

int Order::getOrderID() const { return orderID; }
string Order::getStatus() const { return status; }
double Order::getTotalPrice() const { return totalPrice; }

void Order::setStatus(string s) { status = s; }

void Order::cancelOrder() {
    if (status == "Delivered")
        cout << "  >> Cannot cancel a delivered order." << endl;
    else {
        status = "Cancelled";
        cout << "  >> Order #" << orderID << " cancelled." << endl;
    }
}

void Order::trackOrder() const {
    cout << "\n--- Tracking Order #" << orderID << " ---" << endl;
    cout << "  Date   : " << orderDate << endl;
    cout << "  Status : " << status << endl;
    cout << "  Total  : $" << totalPrice << endl;
}

void Order::display() const {
    cout << "  Order #" << orderID
         << " | Customer: " << customerID
         << " | Date: " << orderDate
         << " | Status: " << status
         << " | Total: $" << totalPrice << endl;
}
void Order::display(bool detailed) const {
    cout << "\n========== Order #" << orderID << " ==========" << endl;
    cout << "  Customer : " << customerID << endl;
    cout << "  Date     : " << orderDate << endl;
    cout << "  Status   : " << status << endl;
    cout << "  Total    : $" << totalPrice << endl;
    cout << "==========================================" << endl;
}

RecordKeeper::RecordKeeper(int id) { ownerID = id; }
RecordKeeper::~RecordKeeper() {}

OrderHistory::OrderHistory(int customerID) : RecordKeeper(customerID) {}
OrderHistory::~OrderHistory() { orders.clear(); }

void OrderHistory::addOrder(Order& order) { orders.push_back(order); }

void OrderHistory::displayAll() const {
    cout << "\n===== Order History (Customer: " << ownerID << ") =====" << endl;
    if (orders.empty()) cout << "  No orders found." << endl;
    else for (int i = 0; i < (int)orders.size(); i++) orders[i].display();
    cout << "=====================================================" << endl;
}
void OrderHistory::displayAll(string filterStatus) const {
    cout << "\n===== Orders [" << filterStatus << "] =====" << endl;
    bool found = false;
    for (int i = 0; i < (int)orders.size(); i++) {
        if (orders[i].getStatus() == filterStatus) {
            orders[i].display();
            found = true;
        }
    }
    if (!found) cout << "  No orders with status: " << filterStatus << endl;
    cout << "=================================" << endl;
}