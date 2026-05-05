#pragma once
#include <QMainWindow>
#include <QTableWidget>
#include <QLabel>
#include "ShoppingCart.h"

class CartWindow : public QMainWindow {
    Q_OBJECT

public:
    CartWindow(ShoppingCart* cart, QWidget* parent = nullptr);

signals:
    void purchaseCompleted(QString productName, int quantity, double price);

private slots:
    void onCheckout();

private:
    ShoppingCart* cart;
    QTableWidget* cartTable;
    QLabel* totalLabel;
    void loadCart();
};