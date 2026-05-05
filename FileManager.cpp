#include "FileManager.h"
using namespace std;

// ================================
// USERS
// ================================
void FileManager::saveUser(User* user) {
    ofstream file("users.txt", ios::app);
    if (!file.is_open()) {
        cout << ">> Error: Cannot open users.txt" << endl;
        return;
    }
    file << user->getUserID() << ","
        << user->getUsername() << ","
        << user->getRole() << ","
        << user->getEmail() << endl;
    file.close();
    cout << ">> [FileManager] User saved: "
        << user->getUsername() << endl;
}

void FileManager::loadUsers() {
    ifstream file("users.txt");
    if (!file.is_open()) {
        cout << ">> Error: Cannot open users.txt" << endl;
        return;
    }
    string line;
    cout << "\n===== Users =====" << endl;
    while (getline(file, line)) {
        cout << "  " << line << endl;
    }
    cout << "=================" << endl;
    file.close();
}

// ================================
// PRODUCTS
// ================================
void FileManager::saveProduct(Product* product) {
    ofstream file("products.txt", ios::app);
    if (!file.is_open()) {
        cout << ">> Error: Cannot open products.txt" << endl;
        return;
    }
    file << product->getProductID() << ","
        << product->getProductName() << ","
        << product->getPrice() << ","
        << product->getQuantity() << endl;
    file.close();
    cout << ">> [FileManager] Product saved: "
        << product->getProductName() << endl;
}

void FileManager::loadProducts() {
    ifstream file("products.txt");
    if (!file.is_open()) {
        cout << ">> Error: Cannot open products.txt" << endl;
        return;
    }
    string line;
    cout << "\n===== Products =====" << endl;
    while (getline(file, line)) {
        cout << "  " << line << endl;
    }
    cout << "===================" << endl;
    file.close();
}

// ================================
// ORDERS
// ================================
void FileManager::saveOrder(const Order& order) {
    ofstream file("orders.txt", ios::app);
    if (!file.is_open()) {
        cout << ">> Error: Cannot open orders.txt" << endl;
        return;
    }
    file << order.getOrderID() << ","
        << order.getStatus() << ","
        << order.getTotalPrice() << endl;
    file.close();
    cout << ">> [FileManager] Order saved: #"
        << order.getOrderID() << endl;
}

void FileManager::loadOrders() {
    ifstream file("orders.txt");
    if (!file.is_open()) {
        cout << ">> Error: Cannot open orders.txt" << endl;
        return;
    }
    string line;
    cout << "\n===== Orders =====" << endl;
    while (getline(file, line)) {
        cout << "  " << line << endl;
    }
    cout << "=================" << endl;
    file.close();
}

// ================================
// CART
// ================================
void FileManager::saveCart(const ShoppingCart& cart) {
    ofstream file("cart.txt", ios::app);
    if (!file.is_open()) {
        cout << ">> Error: Cannot open cart.txt" << endl;
        return;
    }
    file << "CartID: " << cart.getCartID() << endl;
    file << "CustomerID: " << cart.getCustomerID() << endl;

    vector<CartItem> items = cart.getItems();
    for (int i = 0; i < (int)items.size(); i++) {
        file << items[i].getProductID() << ","
            << items[i].getProductName() << ","
            << items[i].getPrice() << ","
            << items[i].getQuantity() << ","
            << items[i].getSubtotal() << endl;
    }
    file << "Total: " << cart.getTotalAmount() << endl;
    file << "---" << endl;
    file.close();
    cout << ">> [FileManager] Cart saved." << endl;
}

void FileManager::loadCart() {
    ifstream file("cart.txt");
    if (!file.is_open()) {
        cout << ">> Error: Cannot open cart.txt" << endl;
        return;
    }
    string line;
    cout << "\n===== Cart =====" << endl;
    while (getline(file, line)) {
        cout << "  " << line << endl;
    }
    cout << "================" << endl;
    file.close();
}

// ================================
// PAYMENTS
// ================================
void FileManager::savePayment(const Payment& payment) {
    ofstream file("payments.txt", ios::app);
    if (!file.is_open()) {
        cout << ">> Error: Cannot open payments.txt" << endl;
        return;
    }
    file << payment.getPaymentID() << ","
        << payment.getPaymentMethod() << ","
        << payment.getAmount() << ","
        << payment.getStatus() << endl;
    file.close();
    cout << ">> [FileManager] Payment saved: #"
        << payment.getPaymentID() << endl;
}

void FileManager::loadPayments() {
    ifstream file("payments.txt");
    if (!file.is_open()) {
        cout << ">> Error: Cannot open payments.txt" << endl;
        return;
    }
    string line;
    cout << "\n===== Payments =====" << endl;
    while (getline(file, line)) {
        cout << "  " << line << endl;
    }
    cout << "===================" << endl;
    file.close();
}

// ================================
// NOTIFICATIONS
// ================================
void FileManager::saveNotification(int userID, const string& msg) {
    ofstream file("notifications.txt", ios::app);
    if (!file.is_open()) {
        cout << ">> Error: Cannot open notifications.txt" << endl;
        return;
    }
    file << "UserID: " << userID << " | " << msg << endl;
    file.close();
    cout << ">> [FileManager] Notification saved." << endl;
}

void FileManager::loadNotifications() {
    ifstream file("notifications.txt");
    if (!file.is_open()) {
        cout << ">> Error: Cannot open notifications.txt" << endl;
        return;
    }
    string line;
    cout << "\n===== Notifications =====" << endl;
    while (getline(file, line)) {
        cout << "  " << line << endl;
    }
    cout << "========================" << endl;
    file.close();
}