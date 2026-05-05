#pragma once
#include <QMainWindow>
#include <QTableWidget>
#include <QLabel>
#include "ShoppingCart.h"
#include "Inventory.h"

class CartWindow : public QMainWindow {
    Q_OBJECT
public:
    CartWindow(ShoppingCart* cart, Inventory* inventory, QWidget* parent = nullptr);
private slots:
    void onCheckout();
    void onRemoveItem(int productId);
private:
    ShoppingCart* cart;
    Inventory* inventory;
    QTableWidget* cartTable;
    QLabel* totalLabel;
    void loadCart();
};
