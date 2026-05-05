#pragma once
#include <QMainWindow>
#include <QWidget>
#include <QPushButton>
#include <QLabel>
#include <QGridLayout>
#include <QScrollArea>
#include <QComboBox>
#include <QMenu>
#include <QAction>
#include "Inventory.h"
#include "ShoppingCart.h"
#include "NotifPanel.h"

class StoreWindow : public QMainWindow {
    Q_OBJECT
public:
    StoreWindow(Inventory* inv, ShoppingCart* cart,
                QString username, QWidget* parent = nullptr);

private slots:
    void onCategoryChanged(int index);
    void onAddToCart(int productId);
    void onViewCart();
    void onLogout();

private:
    Inventory* inventory;
    ShoppingCart* cart;
    QString username;

    QWidget* productsWidget;
    QGridLayout* productsGrid;
    QLabel* cartCountLabel;
    NotifPanel* notifPanel;
    QPushButton* notifBtn;
    int notifCount;

    void loadProducts(QString category);
    QWidget* createProductCard(int id, QString name,
                               double price, int stock,
                               QString category);
};
