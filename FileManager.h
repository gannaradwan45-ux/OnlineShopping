#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include "User.h"
#include "product.h"
#include "ShoppingCart.h"
#include "payment.h"
#include "notification.h"
using namespace std;

class FileManager {
public:
    // Users
    static void saveUser(User* user);
    static void loadUsers();

    // Products
    static void saveProduct(Product* product);
    static void loadProducts();

    // Orders
    static void saveOrder(const Order& order);
    static void loadOrders();

    // Cart
    static void saveCart(const ShoppingCart& cart);
    static void loadCart();

    // Payments
    static void savePayment(const Payment& payment);
    static void loadPayments();

    // Notifications
    static void saveNotification(int userID, const string& msg);
    static void loadNotifications();
};
