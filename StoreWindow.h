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
#include <QStackedWidget>
#include <QTableWidget>
#include <QLineEdit>
#include "Inventory.h"
#include "ShoppingCart.h"
#include "NotifPanel.h"
#include "User.h"

class StoreWindow : public QMainWindow {
    Q_OBJECT
public:
    StoreWindow(Inventory* inv, ShoppingCart* cart, User* currentUser, QWidget* parent = nullptr);
    ~StoreWindow();

private slots:
    // Customer
    void onCategoryChanged(int index);
    void onAddToCart(int productId);
    void onViewCart();
    void onLogout();
    void onRefreshProducts();

    // Vendor
    void onAddProduct();
    void onShowMyProducts();
    void onSubmitProduct();
    void onCancelAddProduct();
    void onDeleteProduct(int productId);
    void onVendorEditProduct(int productId);
    void onVendorRestock(int productId);
    void onShowSalesDashboard();

    // Admin
    void onAdminAddProduct();
    void onAdminEditUserRole(int userId);
    void onAdminDeleteUser(int userId);
    void onAdminEditProduct(int productId);
    void onAdminDeleteProduct(int productId);
    void onAdminUpdateOrderStatus(int orderId);
    void onAdminRestock(int productId);          // *** جديد - Admin Restock ***
    void onAdminShowSalesDashboard();             // *** جديد - Admin Sales Dashboard ***

private:
    Inventory* inventory;
    ShoppingCart* cart;
    User* currentUser;
    QStackedWidget* stackedWidget;

    // Customer UI
    QWidget* customerView;
    QWidget* productsWidget;
    QGridLayout* productsGrid;
    QLabel* cartCountLabel;
    NotifPanel* notifPanel;
    QPushButton* notifBtn;
    int notifCount;
    QComboBox* categoryBox;
    QPushButton* refreshBtn;

    // Vendor UI
    QWidget* vendorView;
    QWidget* addProductWidget;
    QLineEdit* productNameEdit;
    QLineEdit* productPriceEdit;
    QLineEdit* productStockEdit;
    QComboBox* productCategoryCombo;
    QTableWidget* myProductsTable;

    // Admin UI
    QWidget* adminView;
    QTableWidget* usersTable;
    QTableWidget* allProductsTable;
    QTableWidget* ordersTable;
    QWidget* adminAddProductWidget;
    QLineEdit* adminProductNameEdit;
    QLineEdit* adminProductPriceEdit;
    QLineEdit* adminProductStockEdit;
    QComboBox* adminProductCategoryCombo;
    QLineEdit* adminVendorIDEdit;

    // Helpers
    void setupUIForRole();
    QWidget* createProductCard(int id, QString name, double price,
                               int stock, QString category, int vendorID);
    void loadProductsFromInventory(QString category);
    void loadMyProducts();
    void loadAllUsersWithActions();
    void loadAllProductsAdminWithActions();
    void loadAllOrdersWithActions();
};
