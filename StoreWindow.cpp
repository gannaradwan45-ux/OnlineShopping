#include "StoreWindow.h"
#include "CartWindow.h"
#include "AuthManager.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QScrollArea>
#include <QInputDialog>
#include <QHeaderView>
#include <QFile>
#include <QTextStream>
#include <QRegularExpression>
#include <fstream>
#include <sstream>
#include <QMenu>
#include <QMessageBox>
#include <QDebug>
#include <QDialog>

StoreWindow::StoreWindow(Inventory* inv, ShoppingCart* c, User* user, QWidget* parent)
    : QMainWindow(parent), inventory(inv), cart(c), currentUser(user) {

    setWindowTitle(QString::fromStdString(currentUser->getRole()) + " Panel - " +
                   QString::fromStdString(currentUser->getUsername()));
    setMinimumSize(1000, 700);
    setStyleSheet("QMainWindow { background-color: #1a1a2e; }");

    notifCount = 0;
    notifPanel = new NotifPanel(this);
    notifPanel->hide();

    setupUIForRole();
}

StoreWindow::~StoreWindow() {
    delete notifPanel;
}

// ---- helper: اجيب اسم الـ vendor من users_data.txt باستخدام ID ----
static QString getVendorName(int vendorID) {
    if (vendorID == 0) return "Admin";
    std::ifstream file("users_data.txt");
    std::string line;
    while (std::getline(file, line)) {
        if (line.empty()) continue;
        std::stringstream ss(line);
        std::string idStr, username, password, email, role;
        std::getline(ss, idStr, ',');
        std::getline(ss, username, ',');
        std::getline(ss, password, ',');
        std::getline(ss, email, ',');
        std::getline(ss, role, ',');
        try {
            if (std::stoi(idStr) == vendorID)
                return QString::fromStdString(username) + " (ID:" + QString::number(vendorID) + ")";
        } catch (...) {}
    }
    return QString("ID: %1").arg(vendorID);
}

void StoreWindow::setupUIForRole() {
    stackedWidget = new QStackedWidget(this);

    string role = currentUser->getRole();

    if (role == "Customer") {
        // ========== CUSTOMER VIEW ==========
        customerView = new QWidget();
        QVBoxLayout* mainLayout = new QVBoxLayout(customerView);
        mainLayout->setSpacing(0);
        mainLayout->setContentsMargins(0, 0, 0, 0);

        QWidget* header = new QWidget();
        header->setFixedHeight(70);
        header->setStyleSheet("background-color: #16213e;");
        QHBoxLayout* headerLayout = new QHBoxLayout(header);
        headerLayout->setContentsMargins(20, 10, 20, 10);

        QLabel* logo = new QLabel("🛒 Online Shop");
        logo->setStyleSheet("color: #e94560; font-size:22px; font-weight:bold;");
        headerLayout->addWidget(logo);
        headerLayout->addStretch();

        refreshBtn = new QPushButton("🔄 Refresh");
        refreshBtn->setStyleSheet("QPushButton { background-color: #0f3460; color:white; border-radius:8px; padding:6px 14px; } QPushButton:hover { background-color:#e94560; }");
        connect(refreshBtn, &QPushButton::clicked, this, &StoreWindow::onRefreshProducts);
        headerLayout->addWidget(refreshBtn);
        headerLayout->addSpacing(10);

        QPushButton* profileBtn = new QPushButton(("👤 " + QString::fromStdString(currentUser->getUsername())).toStdString().c_str());
        profileBtn->setStyleSheet("QPushButton { background-color:#0f3460; color:white; border-radius:8px; padding:6px 14px; } QPushButton:hover { background-color:#e94560; }");
        QMenu* profileMenu = new QMenu(this);
        QAction* userInfo = new QAction(("👤 " + QString::fromStdString(currentUser->getUsername())).toStdString().c_str(), this);
        userInfo->setEnabled(false);
        profileMenu->addAction(userInfo);
        profileMenu->addSeparator();
        QAction* logoutAction = new QAction("🚪 Logout", this);
        connect(logoutAction, &QAction::triggered, this, &StoreWindow::onLogout);
        profileMenu->addAction(logoutAction);
        profileBtn->setMenu(profileMenu);
        headerLayout->addWidget(profileBtn);
        headerLayout->addSpacing(10);

        notifBtn = new QPushButton("🔔");
        notifBtn->setFixedSize(40, 35);
        notifBtn->setStyleSheet("QPushButton { background-color:#0f3460; color:white; border-radius:8px; } QPushButton:hover { background-color:#e94560; }");
        connect(notifBtn, &QPushButton::clicked, [this]() {
            notifPanel->move(width() - 320, 70);
            notifPanel->togglePanel();
        });
        headerLayout->addWidget(notifBtn);
        headerLayout->addSpacing(10);

        QPushButton* cartBtn = new QPushButton("🛒 View Cart");
        cartBtn->setStyleSheet("QPushButton { background-color:#e94560; color:white; border-radius:8px; padding:8px 16px; } QPushButton:hover { background-color:#c73652; }");
        connect(cartBtn, &QPushButton::clicked, this, &StoreWindow::onViewCart);
        headerLayout->addWidget(cartBtn);

        cartCountLabel = new QLabel("Items: 0");
        cartCountLabel->setStyleSheet("color:#e94560; font-size:13px; font-weight:bold;");
        headerLayout->addWidget(cartCountLabel);
        mainLayout->addWidget(header);

        QWidget* catBar = new QWidget();
        catBar->setFixedHeight(55);
        catBar->setStyleSheet("background-color:#0f3460;");
        QHBoxLayout* catLayout = new QHBoxLayout(catBar);
        catLayout->setContentsMargins(20, 8, 20, 8);

        QLabel* catLabel = new QLabel("Category:");
        catLabel->setStyleSheet("color:white;");
        catLayout->addWidget(catLabel);

        categoryBox = new QComboBox();
        categoryBox->addItems({"All", "Electronics", "Grocery", "Books", "Appliances"});
        categoryBox->setStyleSheet("QComboBox { background-color:#16213e; color:white; border:2px solid #e94560; border-radius:8px; padding:5px 15px; min-width:180px; }");
        connect(categoryBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
                this, &StoreWindow::onCategoryChanged);
        catLayout->addWidget(categoryBox);
        catLayout->addStretch();
        mainLayout->addWidget(catBar);

        QScrollArea* scrollArea = new QScrollArea();
        scrollArea->setStyleSheet("QScrollArea { background-color:#1a1a2e; border:none; }");
        scrollArea->setWidgetResizable(true);

        productsWidget = new QWidget();
        productsWidget->setStyleSheet("background-color:#1a1a2e;");
        productsGrid = new QGridLayout(productsWidget);
        productsGrid->setSpacing(15);
        productsGrid->setContentsMargins(20, 20, 20, 20);

        scrollArea->setWidget(productsWidget);
        mainLayout->addWidget(scrollArea);

        stackedWidget->addWidget(customerView);
        loadProductsFromInventory("All");

    } else if (role == "Vendor") {
        // ========== VENDOR VIEW ==========
        vendorView = new QWidget();
        QVBoxLayout* mainLayout = new QVBoxLayout(vendorView);
        mainLayout->setSpacing(15);
        mainLayout->setContentsMargins(20, 20, 20, 20);

        QLabel* title = new QLabel(
            QString("🛍️ Vendor Dashboard — %1   |   🪪 Your ID: %2")
                .arg(QString::fromStdString(currentUser->getUsername()))
                .arg(currentUser->getUserID()));
        title->setStyleSheet("color:#e94560; font-size:22px; font-weight:bold;");
        mainLayout->addWidget(title);

        QHBoxLayout* btnLayout = new QHBoxLayout();

        QString btnStyle =
            "QPushButton { background-color:#e94560; color:white; border-radius:8px;"
            "  padding:10px 20px; font-size:14px; font-weight:bold; }"
            "QPushButton:hover { background-color:#c73652; }";

        QPushButton* myProductsBtn = new QPushButton("📦 My Products");
        QPushButton* addProductBtn = new QPushButton("➕ Add New Product");
        QPushButton* salesBtn      = new QPushButton("📊 Sales Dashboard");
        QPushButton* logoutBtn     = new QPushButton("🚪 Logout");

        myProductsBtn->setStyleSheet(btnStyle);
        addProductBtn->setStyleSheet(btnStyle);
        salesBtn->setStyleSheet(btnStyle);
        logoutBtn->setStyleSheet(btnStyle);

        connect(myProductsBtn, &QPushButton::clicked, this, &StoreWindow::onShowMyProducts);
        connect(addProductBtn, &QPushButton::clicked, this, &StoreWindow::onAddProduct);
        connect(salesBtn,      &QPushButton::clicked, this, &StoreWindow::onShowSalesDashboard);
        connect(logoutBtn,     &QPushButton::clicked, this, &StoreWindow::onLogout);

        btnLayout->addWidget(myProductsBtn);
        btnLayout->addWidget(addProductBtn);
        btnLayout->addWidget(salesBtn);
        btnLayout->addStretch();
        btnLayout->addWidget(logoutBtn);
        mainLayout->addLayout(btnLayout);

        myProductsTable = new QTableWidget();
        myProductsTable->setColumnCount(7);
        myProductsTable->setHorizontalHeaderLabels(
            {"ID", "Product Name", "Price", "Stock", "Category", "Status", "Actions"});
        myProductsTable->setStyleSheet(
            "QTableWidget { background-color:#16213e; color:white; border:1px solid #0f3460; border-radius:8px; }"
            "QHeaderView::section { background-color:#e94560; color:white; font-weight:bold; padding:8px; }");
        myProductsTable->horizontalHeader()->setStretchLastSection(true);
        mainLayout->addWidget(myProductsTable);

        addProductWidget = new QWidget();
        addProductWidget->hide();
        QVBoxLayout* addLayout = new QVBoxLayout(addProductWidget);
        addLayout->setSpacing(15);

        QLabel* addTitle = new QLabel("➕ Add New Product");
        addTitle->setStyleSheet("color:#e94560; font-size:18px; font-weight:bold;");
        addLayout->addWidget(addTitle);

        QFormLayout* formLayout = new QFormLayout();
        productNameEdit = new QLineEdit();
        productNameEdit->setPlaceholderText("Product Name");
        productNameEdit->setStyleSheet("QLineEdit { background-color:#0f3460; color:white; border-radius:8px; padding:10px; }");
        productPriceEdit = new QLineEdit();
        productPriceEdit->setPlaceholderText("Price");
        productPriceEdit->setStyleSheet("QLineEdit { background-color:#0f3460; color:white; border-radius:8px; padding:10px; }");
        productStockEdit = new QLineEdit();
        productStockEdit->setPlaceholderText("Stock Quantity");
        productStockEdit->setStyleSheet("QLineEdit { background-color:#0f3460; color:white; border-radius:8px; padding:10px; }");
        productCategoryCombo = new QComboBox();
        productCategoryCombo->addItems({"Electronics", "Grocery", "Books", "Appliances"});
        productCategoryCombo->setStyleSheet("QComboBox { background-color:#0f3460; color:white; border-radius:8px; padding:8px; }");

        formLayout->addRow("Product Name:", productNameEdit);
        formLayout->addRow("Price ($):",    productPriceEdit);
        formLayout->addRow("Stock:",        productStockEdit);
        formLayout->addRow("Category:",     productCategoryCombo);
        addLayout->addLayout(formLayout);

        QHBoxLayout* formBtnLayout = new QHBoxLayout();
        QPushButton* submitBtn = new QPushButton("✅ Submit");
        QPushButton* cancelBtn = new QPushButton("❌ Cancel");
        submitBtn->setStyleSheet(btnStyle);
        cancelBtn->setStyleSheet(btnStyle);
        connect(submitBtn, &QPushButton::clicked, this, &StoreWindow::onSubmitProduct);
        connect(cancelBtn, &QPushButton::clicked, this, &StoreWindow::onCancelAddProduct);
        formBtnLayout->addWidget(submitBtn);
        formBtnLayout->addWidget(cancelBtn);
        addLayout->addLayout(formBtnLayout);

        mainLayout->addWidget(addProductWidget);
        stackedWidget->addWidget(vendorView);
        loadMyProducts();

    } else if (role == "Admin") {
        // ========== ADMIN VIEW ==========
        adminView = new QWidget();
        QVBoxLayout* mainLayout = new QVBoxLayout(adminView);
        mainLayout->setSpacing(15);
        mainLayout->setContentsMargins(20, 20, 20, 20);

        QLabel* title = new QLabel("👑 Admin Dashboard - Full Control");
        title->setStyleSheet("color:#e94560; font-size:24px; font-weight:bold;");
        mainLayout->addWidget(title);

        QHBoxLayout* tabLayout = new QHBoxLayout();

        QPushButton* usersTabBtn    = new QPushButton("👥 Users Management");
        QPushButton* productsTabBtn = new QPushButton("📦 Products Management");
        QPushButton* ordersTabBtn   = new QPushButton("📋 Orders Management");
        QPushButton* addProductBtn  = new QPushButton("➕ Add New Product");
        QPushButton* salesDashBtn   = new QPushButton("📊 Sales Dashboard");   // *** جديد ***
        QPushButton* logoutBtn      = new QPushButton("🚪 Logout");

        QString tabBtnStyle =
            "QPushButton { background-color:#e94560; color:white; border-radius:8px;"
            "  padding:10px 20px; font-size:13px; font-weight:bold; }"
            "QPushButton:hover { background-color:#c73652; }";

        usersTabBtn->setStyleSheet(tabBtnStyle);
        productsTabBtn->setStyleSheet(tabBtnStyle);
        ordersTabBtn->setStyleSheet(tabBtnStyle);
        addProductBtn->setStyleSheet(tabBtnStyle);
        salesDashBtn->setStyleSheet(tabBtnStyle);
        logoutBtn->setStyleSheet(tabBtnStyle);

        usersTable = new QTableWidget();
        usersTable->setColumnCount(5);
        usersTable->setHorizontalHeaderLabels({"ID", "Username", "Email", "Role", "Actions"});
        usersTable->setStyleSheet(
            "QTableWidget { background-color:#16213e; color:white; border:1px solid #0f3460; border-radius:8px; }"
            "QHeaderView::section { background-color:#e94560; color:white; font-weight:bold; }");
        usersTable->horizontalHeader()->setStretchLastSection(true);

        // *** 8 columns: زدنا Actions لـ Restock ***
        allProductsTable = new QTableWidget();
        allProductsTable->setColumnCount(8);
        allProductsTable->setHorizontalHeaderLabels(
            {"ID", "Product Name", "Price", "Stock", "Category", "Vendor ID", "Vendor Name", "Actions"});
        allProductsTable->setStyleSheet(
            "QTableWidget { background-color:#16213e; color:white; border:1px solid #0f3460; border-radius:8px; }"
            "QHeaderView::section { background-color:#e94560; color:white; }");
        allProductsTable->horizontalHeader()->setStretchLastSection(true);

        ordersTable = new QTableWidget();
        ordersTable->setColumnCount(6);
        ordersTable->setHorizontalHeaderLabels(
            {"Order ID", "Customer ID", "Total", "Status", "Items", "Actions"});
        ordersTable->setStyleSheet(
            "QTableWidget { background-color:#16213e; color:white; border:1px solid #0f3460; border-radius:8px; }"
            "QHeaderView::section { background-color:#e94560; color:white; }");
        ordersTable->horizontalHeader()->setStretchLastSection(true);

        adminAddProductWidget = new QWidget();
        adminAddProductWidget->hide();
        QVBoxLayout* adminAddLayout = new QVBoxLayout(adminAddProductWidget);
        adminAddLayout->setSpacing(15);

        QLabel* addTitle = new QLabel("➕ Add New Product (Admin)");
        addTitle->setStyleSheet("color:#e94560; font-size:18px; font-weight:bold;");
        adminAddLayout->addWidget(addTitle);

        QFormLayout* formLayout = new QFormLayout();
        adminProductNameEdit = new QLineEdit();
        adminProductNameEdit->setPlaceholderText("Product Name");
        adminProductNameEdit->setStyleSheet("QLineEdit { background-color:#0f3460; color:white; border-radius:8px; padding:10px; }");
        adminProductPriceEdit = new QLineEdit();
        adminProductPriceEdit->setPlaceholderText("Price");
        adminProductPriceEdit->setStyleSheet("QLineEdit { background-color:#0f3460; color:white; border-radius:8px; padding:10px; }");
        adminProductStockEdit = new QLineEdit();
        adminProductStockEdit->setPlaceholderText("Stock Quantity");
        adminProductStockEdit->setStyleSheet("QLineEdit { background-color:#0f3460; color:white; border-radius:8px; padding:10px; }");
        adminProductCategoryCombo = new QComboBox();
        adminProductCategoryCombo->addItems({"Electronics", "Grocery", "Books", "Appliances"});
        adminProductCategoryCombo->setStyleSheet("QComboBox { background-color:#0f3460; color:white; border-radius:8px; padding:8px; }");
        adminVendorIDEdit = new QLineEdit();
        adminVendorIDEdit->setPlaceholderText("Vendor ID (0 for Admin)");
        adminVendorIDEdit->setStyleSheet("QLineEdit { background-color:#0f3460; color:white; border-radius:8px; padding:10px; }");

        formLayout->addRow("Product Name:", adminProductNameEdit);
        formLayout->addRow("Price ($):",    adminProductPriceEdit);
        formLayout->addRow("Stock:",        adminProductStockEdit);
        formLayout->addRow("Category:",     adminProductCategoryCombo);
        formLayout->addRow("Vendor ID:",    adminVendorIDEdit);
        adminAddLayout->addLayout(formLayout);

        QHBoxLayout* formBtnLayout = new QHBoxLayout();
        QPushButton* submitBtn    = new QPushButton("✅ Submit");
        QPushButton* cancelAddBtn = new QPushButton("❌ Cancel");
        submitBtn->setStyleSheet(tabBtnStyle);
        cancelAddBtn->setStyleSheet(tabBtnStyle);
        connect(submitBtn,    &QPushButton::clicked, this, &StoreWindow::onAdminAddProduct);
        connect(cancelAddBtn, &QPushButton::clicked, [this]() { adminAddProductWidget->hide(); });
        formBtnLayout->addWidget(submitBtn);
        formBtnLayout->addWidget(cancelAddBtn);
        adminAddLayout->addLayout(formBtnLayout);

        mainLayout->addLayout(tabLayout);
        mainLayout->addWidget(usersTable);
        mainLayout->addWidget(allProductsTable);
        mainLayout->addWidget(ordersTable);
        mainLayout->addWidget(adminAddProductWidget);

        allProductsTable->setVisible(false);
        ordersTable->setVisible(false);
        adminAddProductWidget->setVisible(false);

        connect(usersTabBtn, &QPushButton::clicked, [this]() {
            usersTable->setVisible(true);
            allProductsTable->setVisible(false);
            ordersTable->setVisible(false);
            adminAddProductWidget->setVisible(false);
            loadAllUsersWithActions();
        });
        connect(productsTabBtn, &QPushButton::clicked, [this]() {
            usersTable->setVisible(false);
            allProductsTable->setVisible(true);
            ordersTable->setVisible(false);
            adminAddProductWidget->setVisible(false);
            loadAllProductsAdminWithActions();
        });
        connect(ordersTabBtn, &QPushButton::clicked, [this]() {
            usersTable->setVisible(false);
            allProductsTable->setVisible(false);
            ordersTable->setVisible(true);
            adminAddProductWidget->setVisible(false);
            loadAllOrdersWithActions();
        });
        connect(addProductBtn, &QPushButton::clicked, [this]() {
            usersTable->setVisible(false);
            allProductsTable->setVisible(false);
            ordersTable->setVisible(false);
            adminAddProductWidget->setVisible(true);
        });
        // *** Sales Dashboard للـ Admin ***
        connect(salesDashBtn, &QPushButton::clicked, this, &StoreWindow::onAdminShowSalesDashboard);
        connect(logoutBtn, &QPushButton::clicked, this, &StoreWindow::onLogout);

        tabLayout->addWidget(usersTabBtn);
        tabLayout->addWidget(productsTabBtn);
        tabLayout->addWidget(ordersTabBtn);
        tabLayout->addWidget(addProductBtn);
        tabLayout->addWidget(salesDashBtn);   // *** جديد ***
        tabLayout->addStretch();
        tabLayout->addWidget(logoutBtn);

        stackedWidget->addWidget(adminView);
        loadAllUsersWithActions();
        usersTable->setVisible(true);
    }

    setCentralWidget(stackedWidget);
}

// ========== CUSTOMER ==========
void StoreWindow::onCategoryChanged(int index) {
    QStringList cats = {"All", "Electronics", "Grocery", "Books", "Appliances"};
    loadProductsFromInventory(cats[index]);
}

void StoreWindow::onRefreshProducts() {
    inventory->loadFromFile();
    loadProductsFromInventory(categoryBox->currentText());
    QMessageBox::information(this, "Refreshed", "Products list refreshed!");
}

void StoreWindow::loadProductsFromInventory(QString category) {
    QLayoutItem* item;
    while ((item = productsGrid->takeAt(0)) != nullptr) {
        delete item->widget();
        delete item;
    }

    vector<Product*> allProducts = inventory->getAllProducts();
    int col = 0, row = 0;

    for (Product* p : allProducts) {
        QString prodCategory = QString::fromStdString(p->getCategory());
        if (category != "All" && prodCategory != category) continue;

        QWidget* card = createProductCard(
            p->getProductID(),
            QString::fromStdString(p->getProductName()),
            p->getPrice(),
            p->getQuantity(),
            prodCategory,
            p->getVendorID());
        productsGrid->addWidget(card, row, col);
        col++;
        if (col == 3) { col = 0; row++; }
    }
}

QWidget* StoreWindow::createProductCard(int id, QString name, double price,
                                        int stock, QString category, int vendorID) {
    Q_UNUSED(vendorID);

    QWidget* card = new QWidget();
    card->setFixedSize(250, 280);
    card->setStyleSheet("QWidget { background-color:#16213e; border-radius:12px; border:1px solid #0f3460; }");

    QVBoxLayout* layout = new QVBoxLayout(card);
    layout->setSpacing(8);
    layout->setContentsMargins(15, 15, 15, 15);

    QLabel* badge = new QLabel(category);
    badge->setAlignment(Qt::AlignCenter);
    badge->setFixedHeight(25);
    badge->setStyleSheet("background-color:#e94560; color:white; border-radius:6px; font-size:11px; font-weight:bold;");
    layout->addWidget(badge);

    QLabel* icon = new QLabel();
    icon->setAlignment(Qt::AlignCenter);
    icon->setFixedHeight(70);
    if      (category == "Electronics") icon->setText("📱");
    else if (category == "Grocery")     icon->setText("🛒");
    else if (category == "Books")       icon->setText("📚");
    else                                icon->setText("🏠");
    icon->setStyleSheet("font-size:40px; background:transparent; border:none;");
    layout->addWidget(icon);

    QLabel* nameLabel = new QLabel(name);
    nameLabel->setAlignment(Qt::AlignCenter);
    nameLabel->setWordWrap(true);
    nameLabel->setStyleSheet("color:white; font-size:14px; font-weight:bold; background:transparent;");
    layout->addWidget(nameLabel);

    QLabel* priceLabel = new QLabel(QString("$%1").arg(price));
    priceLabel->setAlignment(Qt::AlignCenter);
    priceLabel->setStyleSheet("color:#e94560; font-size:18px; font-weight:bold;");
    layout->addWidget(priceLabel);

    QLabel* stockLabel = new QLabel(QString("In Stock: %1").arg(stock));
    stockLabel->setAlignment(Qt::AlignCenter);
    stockLabel->setStyleSheet("color:#a8a8b3; font-size:11px;");
    layout->addWidget(stockLabel);

    QPushButton* addBtn = new QPushButton("Add to Cart");
    addBtn->setFixedHeight(35);
    addBtn->setStyleSheet("QPushButton { background-color:#e94560; color:white; border-radius:8px; font-weight:bold; } QPushButton:hover { background-color:#c73652; }");
    connect(addBtn, &QPushButton::clicked, [this, id]() { onAddToCart(id); });
    layout->addWidget(addBtn);

    return card;
}

void StoreWindow::onAddToCart(int productId) {
    bool ok;
    int quantity = QInputDialog::getInt(this, "Add to Cart", "Enter Quantity:", 1, 1, 100, 1, &ok);
    if (!ok) return;

    Product* p = inventory->getProduct(productId);
    if (p && inventory->isProductAvailable(productId, quantity)) {
        cart->addItem(p->getProductID(), p->getProductName(), p->getPrice(), quantity);
        inventory->updateStock(productId, p->getQuantity() - quantity);

        int total = (int)cart->getItems().size();
        cartCountLabel->setText(QString("Items: %1").arg(total));

        notifCount++;
        notifBtn->setText(QString("🔔 %1").arg(notifCount));
        notifPanel->addCartNotification(
            QString::fromStdString(p->getProductName()), quantity, p->getPrice(), productId);

        QMessageBox::information(this, "Added to Cart",
                                 QString("✅ %1 x%2 added!\nTotal: $%3")
                                     .arg(QString::fromStdString(p->getProductName()))
                                     .arg(quantity)
                                     .arg(p->getPrice() * quantity));

        loadProductsFromInventory(categoryBox->currentText());
    } else {
        QMessageBox::warning(this, "Error", "Not enough stock!");
    }
}

void StoreWindow::onViewCart() {
    CartWindow* cartWin = new CartWindow(cart, inventory, this);
    cartWin->show();
}

// ========== VENDOR ==========
void StoreWindow::onAddProduct() {
    addProductWidget->show();
}

void StoreWindow::onShowMyProducts() {
    addProductWidget->hide();
    loadMyProducts();
}

void StoreWindow::loadMyProducts() {
    int vendorID = currentUser->getUserID();
    vector<Product*> vendorProducts = inventory->getProductsByVendor(vendorID);

    myProductsTable->setRowCount(vendorProducts.size());

    for (int i = 0; i < (int)vendorProducts.size(); i++) {
        Product* p = vendorProducts[i];
        myProductsTable->setItem(i, 0, new QTableWidgetItem(QString::number(p->getProductID())));
        myProductsTable->setItem(i, 1, new QTableWidgetItem(QString::fromStdString(p->getProductName())));
        myProductsTable->setItem(i, 2, new QTableWidgetItem(QString("$%1").arg(p->getPrice())));
        myProductsTable->setItem(i, 3, new QTableWidgetItem(QString::number(p->getQuantity())));
        myProductsTable->setItem(i, 4, new QTableWidgetItem(QString::fromStdString(p->getCategory())));

        QString status = p->getQuantity() == 0 ? "❌ Out of Stock"
                         : p->getQuantity() <= 5  ? "⚠️ Low Stock"
                                                 : "✅ In Stock";
        QTableWidgetItem* statusItem = new QTableWidgetItem(status);
        statusItem->setForeground(p->getQuantity() == 0 ? Qt::red
                                  : p->getQuantity() <= 5 ? QColor("#ffd93d")
                                                          : QColor("#6bcb77"));
        myProductsTable->setItem(i, 5, statusItem);

        QWidget* actionWidget = new QWidget();
        QHBoxLayout* actionLayout = new QHBoxLayout(actionWidget);
        actionLayout->setContentsMargins(5, 2, 5, 2);

        QPushButton* editBtn    = new QPushButton("✏️ Edit");
        QPushButton* restockBtn = new QPushButton("📦 Restock");
        QPushButton* deleteBtn  = new QPushButton("🗑 Delete");

        editBtn->setFixedSize(65, 30);
        restockBtn->setFixedSize(85, 30);
        deleteBtn->setFixedSize(70, 30);

        editBtn->setStyleSheet("background-color:#2196F3; color:white; border-radius:5px;");
        restockBtn->setStyleSheet("background-color:#4CAF50; color:white; border-radius:5px;");
        deleteBtn->setStyleSheet("background-color:#ff6b6b; color:white; border-radius:5px;");

        int productId = p->getProductID();
        connect(editBtn,    &QPushButton::clicked, [this, productId]() { onVendorEditProduct(productId); });
        connect(restockBtn, &QPushButton::clicked, [this, productId]() { onVendorRestock(productId); });
        connect(deleteBtn,  &QPushButton::clicked, [this, productId]() { onDeleteProduct(productId); });

        actionLayout->addWidget(editBtn);
        actionLayout->addWidget(restockBtn);
        actionLayout->addWidget(deleteBtn);
        myProductsTable->setCellWidget(i, 6, actionWidget);
    }

    myProductsTable->resizeColumnsToContents();
}

void StoreWindow::onVendorRestock(int productId) {
    Product* p = inventory->getProduct(productId);
    if (!p) return;

    bool ok;
    int addQty = QInputDialog::getInt(this, "📦 Restock Product",
                                      QString("Current Stock: %1\nAdd quantity:").arg(p->getQuantity()),
                                      1, 1, 10000, 1, &ok);
    if (ok) {
        inventory->updateStock(productId, p->getQuantity() + addQty);
        loadMyProducts();
        QMessageBox::information(this, "✅ Restocked",
                                 QString("Product '%1' restocked!\nNew Stock: %2")
                                     .arg(QString::fromStdString(p->getProductName()))
                                     .arg(p->getQuantity()));
    }
}

void StoreWindow::onVendorEditProduct(int productId) {
    Product* p = inventory->getProduct(productId);
    if (!p) return;

    bool ok;
    double newPrice = QInputDialog::getDouble(this, "Edit Price", "New Price:",
                                              p->getPrice(), 0, 10000, 2, &ok);
    if (ok) {
        int newStock = QInputDialog::getInt(this, "Edit Stock", "New Stock:",
                                            p->getQuantity(), 0, 10000, 1, &ok);
        if (ok) {
            inventory->updateProduct(productId, newPrice, newStock);
            loadMyProducts();
            QMessageBox::information(this, "Success", "Product updated!");
        }
    }
}

void StoreWindow::onSubmitProduct() {
    QString name     = productNameEdit->text().trimmed();
    QString priceStr = productPriceEdit->text().trimmed();
    QString stockStr = productStockEdit->text().trimmed();
    QString category = productCategoryCombo->currentText();

    if (name.isEmpty() || priceStr.isEmpty() || stockStr.isEmpty()) {
        QMessageBox::warning(this, "Error", "Please fill all fields!");
        return;
    }

    bool priceOk, stockOk;
    double price = priceStr.toDouble(&priceOk);
    int stock    = stockStr.toInt(&stockOk);

    if (!priceOk || price <= 0) { QMessageBox::warning(this, "Error", "Invalid price!"); return; }
    if (!stockOk || stock < 0)  { QMessageBox::warning(this, "Error", "Invalid stock quantity!"); return; }

    int vendorID = currentUser->getUserID();
    inventory->addProduct(name.toStdString(), price, category.toStdString(), stock, vendorID);

    QMessageBox::information(this, "Success",
                             QString("✅ Product '%1' added successfully!").arg(name));
    productNameEdit->clear();
    productPriceEdit->clear();
    productStockEdit->clear();
    addProductWidget->hide();
    loadMyProducts();
}

void StoreWindow::onCancelAddProduct() {
    productNameEdit->clear();
    productPriceEdit->clear();
    productStockEdit->clear();
    addProductWidget->hide();
}

void StoreWindow::onDeleteProduct(int productId) {
    int reply = QMessageBox::question(this, "Delete Product",
                                      "Are you sure you want to delete this product?",
                                      QMessageBox::Yes | QMessageBox::No);
    if (reply == QMessageBox::Yes) {
        int vendorID = currentUser->getUserID();
        if (inventory->removeProductByVendor(productId, vendorID)) {
            QMessageBox::information(this, "Deleted", "Product deleted successfully!");
            loadMyProducts();
        } else {
            QMessageBox::warning(this, "Error", "You don't have permission to delete this product!");
        }
    }
}

// ========== VENDOR SALES DASHBOARD ==========
void StoreWindow::onShowSalesDashboard() {
    int vendorID = currentUser->getUserID();
    vector<Product*> myProducts = inventory->getProductsByVendor(vendorID);

    double totalEarnings  = 0.0;
    int    totalItemsSold = 0;
    int    totalOrders    = 0;

    QStringList myProductNames;
    for (Product* p : myProducts)
        myProductNames << QString::fromStdString(p->getProductName()).toLower();

    std::ifstream file("orders_data.txt");
    std::string line;
    while (std::getline(file, line)) {
        if (line.empty()) continue;
        if (!line.empty() && line.back() == '\r') line.pop_back();

        std::stringstream ss(line);
        std::string idStr, custStr, totalStr, status, items;
        std::getline(ss, idStr, ',');
        std::getline(ss, custStr, ',');
        std::getline(ss, totalStr, ',');
        std::getline(ss, status, ',');
        std::getline(ss, items, ',');

        if (status == "Cancelled") continue;

        QString itemsQ = QString::fromStdString(items);
        QStringList itemList = itemsQ.split("|", Qt::SkipEmptyParts);
        bool orderHasMyProduct = false;

        for (const QString& item : itemList) {
            for (const QString& pname : myProductNames) {
                if (item.toLower().contains(pname)) {
                    QRegularExpression re(R"(x(\d+))");
                    QRegularExpressionMatch match = re.match(item);
                    if (match.hasMatch())
                        totalItemsSold += match.captured(1).toInt();
                    orderHasMyProduct = true;
                    break;
                }
            }
        }
        if (orderHasMyProduct) {
            totalOrders++;
            try { totalEarnings += std::stod(totalStr); } catch (...) {}
        }
    }
    file.close();

    double stockValue = 0.0;
    int    totalStock = 0;
    for (Product* p : myProducts) {
        stockValue += p->getPrice() * p->getQuantity();
        totalStock += p->getQuantity();
    }

    QDialog* dash = new QDialog(this);
    dash->setWindowTitle("📊 Sales Dashboard");
    dash->setMinimumSize(500, 460);
    dash->setStyleSheet("QDialog { background-color:#1a1a2e; }");

    QVBoxLayout* lay = new QVBoxLayout(dash);
    lay->setSpacing(12);
    lay->setContentsMargins(25, 25, 25, 25);

    QLabel* hdr = new QLabel(
        QString("📊 Sales Dashboard\n%1   |   🪪 ID: %2")
            .arg(QString::fromStdString(currentUser->getUsername()))
            .arg(currentUser->getUserID()));
    hdr->setStyleSheet("color:#e94560; font-size:18px; font-weight:bold;");
    hdr->setAlignment(Qt::AlignCenter);
    lay->addWidget(hdr);

    auto makeCard = [&](QString icon, QString label, QString value, QString color) -> QWidget* {
        QWidget* card = new QWidget();
        card->setFixedHeight(70);
        card->setStyleSheet(
            QString("background-color:#16213e; border-radius:10px;"
                    "border-left: 4px solid %1;").arg(color));
        QHBoxLayout* cl = new QHBoxLayout(card);
        cl->setContentsMargins(15, 10, 15, 10);
        QLabel* iconLbl = new QLabel(icon);
        iconLbl->setStyleSheet("font-size:26px; background:transparent; border:none;");
        iconLbl->setFixedWidth(40);
        cl->addWidget(iconLbl);
        QVBoxLayout* textL = new QVBoxLayout();
        textL->setSpacing(2);
        QLabel* lbl = new QLabel(label);
        lbl->setStyleSheet("color:#a8a8b3; font-size:11px; background:transparent; border:none;");
        QLabel* val = new QLabel(value);
        val->setStyleSheet(QString("color:%1; font-size:20px; font-weight:bold; background:transparent; border:none;").arg(color));
        textL->addWidget(lbl);
        textL->addWidget(val);
        cl->addLayout(textL);
        cl->addStretch();
        return card;
    };

    lay->addWidget(makeCard("💰", "Total Earnings (from completed orders)",
                            QString("$%1").arg(totalEarnings, 0, 'f', 2), "#6bcb77"));
    lay->addWidget(makeCard("📋", "Total Orders containing your products",
                            QString::number(totalOrders), "#e94560"));
    lay->addWidget(makeCard("🛒", "Total Items Sold",
                            QString::number(totalItemsSold), "#ffd93d"));
    lay->addWidget(makeCard("🏪", "Your Products Listed",
                            QString::number((int)myProducts.size()), "#4fc3f7"));
    lay->addWidget(makeCard("💼", "Current Stock Value",
                            QString("$%1").arg(stockValue, 0, 'f', 2), "#ce93d8"));
    lay->addWidget(makeCard("📦", "Total Units in Stock",
                            QString::number(totalStock), "#80cbc4"));

    lay->addStretch();

    QPushButton* closeBtn = new QPushButton("✖  Close");
    closeBtn->setFixedHeight(42);
    closeBtn->setStyleSheet(
        "QPushButton { background-color:#e94560; color:white; border-radius:8px; font-size:14px; font-weight:bold; }"
        "QPushButton:hover { background-color:#c73652; }");
    connect(closeBtn, &QPushButton::clicked, dash, &QDialog::accept);
    lay->addWidget(closeBtn);

    dash->exec();
    delete dash;
}

// ========== ADMIN ==========
void StoreWindow::loadAllUsersWithActions() {
    usersTable->setRowCount(0);
    std::ifstream file("users_data.txt");
    if (!file.is_open()) {
        usersTable->setRowCount(1);
        usersTable->setItem(0, 0, new QTableWidgetItem("No users data"));
        return;
    }

    std::string line;
    int row = 0;
    while (std::getline(file, line)) {
        if (line.empty()) continue;
        std::stringstream ss(line);
        std::string idStr, username, password, email, role;
        std::getline(ss, idStr, ',');
        std::getline(ss, username, ',');
        std::getline(ss, password, ',');
        std::getline(ss, email, ',');
        std::getline(ss, role, ',');

        usersTable->insertRow(row);
        usersTable->setItem(row, 0, new QTableWidgetItem(QString::fromStdString(idStr)));
        usersTable->setItem(row, 1, new QTableWidgetItem(QString::fromStdString(username)));
        usersTable->setItem(row, 2, new QTableWidgetItem(QString::fromStdString(email)));
        usersTable->setItem(row, 3, new QTableWidgetItem(QString::fromStdString(role)));

        QWidget* actionWidget = new QWidget();
        QHBoxLayout* actionLayout = new QHBoxLayout(actionWidget);
        actionLayout->setContentsMargins(5, 2, 5, 2);

        QPushButton* editRoleBtn   = new QPushButton("Change Role");
        QPushButton* deleteUserBtn = new QPushButton("Delete");
        editRoleBtn->setFixedSize(100, 30);
        deleteUserBtn->setFixedSize(70, 30);
        editRoleBtn->setStyleSheet("background-color:#2196F3; color:white; border-radius:5px;");
        deleteUserBtn->setStyleSheet("background-color:#ff6b6b; color:white; border-radius:5px;");

        try {
            int userId = std::stoi(idStr);
            connect(editRoleBtn,   &QPushButton::clicked, [this, userId]() { onAdminEditUserRole(userId); });
            connect(deleteUserBtn, &QPushButton::clicked, [this, userId]() { onAdminDeleteUser(userId); });
        } catch (...) {}

        actionLayout->addWidget(editRoleBtn);
        actionLayout->addWidget(deleteUserBtn);
        usersTable->setCellWidget(row, 4, actionWidget);
        row++;
    }
    file.close();
    usersTable->resizeColumnsToContents();
}

void StoreWindow::loadAllProductsAdminWithActions() {
    vector<Product*> allProducts = inventory->getAllProducts();
    allProductsTable->setRowCount(allProducts.size());

    for (int i = 0; i < (int)allProducts.size(); i++) {
        Product* p = allProducts[i];
        allProductsTable->setItem(i, 0, new QTableWidgetItem(QString::number(p->getProductID())));
        allProductsTable->setItem(i, 1, new QTableWidgetItem(QString::fromStdString(p->getProductName())));
        allProductsTable->setItem(i, 2, new QTableWidgetItem(QString("$%1").arg(p->getPrice())));
        allProductsTable->setItem(i, 3, new QTableWidgetItem(QString::number(p->getQuantity())));
        allProductsTable->setItem(i, 4, new QTableWidgetItem(QString::fromStdString(p->getCategory())));
        allProductsTable->setItem(i, 5, new QTableWidgetItem(QString::number(p->getVendorID())));
        allProductsTable->setItem(i, 6, new QTableWidgetItem(getVendorName(p->getVendorID())));

        QWidget* actionWidget = new QWidget();
        QHBoxLayout* actionLayout = new QHBoxLayout(actionWidget);
        actionLayout->setContentsMargins(5, 2, 5, 2);

        QPushButton* editBtn    = new QPushButton("✏️ Edit");
        QPushButton* restockBtn = new QPushButton("📦 Restock");  // *** جديد ***
        QPushButton* deleteBtn  = new QPushButton("🗑 Delete");

        editBtn->setFixedSize(65, 30);
        restockBtn->setFixedSize(85, 30);
        deleteBtn->setFixedSize(70, 30);

        editBtn->setStyleSheet("background-color:#2196F3; color:white; border-radius:5px;");
        restockBtn->setStyleSheet("background-color:#4CAF50; color:white; border-radius:5px;");
        deleteBtn->setStyleSheet("background-color:#ff6b6b; color:white; border-radius:5px;");

        int productId = p->getProductID();
        connect(editBtn,    &QPushButton::clicked, [this, productId]() { onAdminEditProduct(productId); });
        connect(restockBtn, &QPushButton::clicked, [this, productId]() { onAdminRestock(productId); });  // *** جديد ***
        connect(deleteBtn,  &QPushButton::clicked, [this, productId]() { onAdminDeleteProduct(productId); });

        actionLayout->addWidget(editBtn);
        actionLayout->addWidget(restockBtn);
        actionLayout->addWidget(deleteBtn);
        allProductsTable->setCellWidget(i, 7, actionWidget);
    }
    allProductsTable->resizeColumnsToContents();
}

// *** Admin Restock ***
void StoreWindow::onAdminRestock(int productId) {
    Product* p = inventory->getProduct(productId);
    if (!p) return;

    bool ok;
    int addQty = QInputDialog::getInt(this, "📦 Admin Restock",
                                      QString("Product: %1\nCurrent Stock: %2\nAdd quantity:")
                                          .arg(QString::fromStdString(p->getProductName()))
                                          .arg(p->getQuantity()),
                                      1, 1, 10000, 1, &ok);
    if (ok) {
        inventory->updateStock(productId, p->getQuantity() + addQty);
        loadAllProductsAdminWithActions();
        QMessageBox::information(this, "✅ Restocked",
                                 QString("Product '%1' restocked!\nNew Stock: %2")
                                     .arg(QString::fromStdString(p->getProductName()))
                                     .arg(p->getQuantity()));
    }
}

// *** Admin Sales Dashboard - إجمالي كل المبيعات ***
void StoreWindow::onAdminShowSalesDashboard() {
    double totalRevenue   = 0.0;
    int    totalOrders    = 0;
    int    cancelledOrders = 0;
    int    totalItemsSold = 0;

    std::ifstream file("orders_data.txt");
    std::string line;
    while (std::getline(file, line)) {
        if (line.empty()) continue;
        if (!line.empty() && line.back() == '\r') line.pop_back();

        std::stringstream ss(line);
        std::string idStr, custStr, totalStr, status, items;
        std::getline(ss, idStr, ',');
        std::getline(ss, custStr, ',');
        std::getline(ss, totalStr, ',');
        std::getline(ss, status, ',');
        std::getline(ss, items, ',');

        if (idStr.empty()) continue;
        totalOrders++;

        if (status == "Cancelled") {
            cancelledOrders++;
            continue;
        }

        try { totalRevenue += std::stod(totalStr); } catch (...) {}

        // احسب عدد الـ items المباعة
        QString itemsQ = QString::fromStdString(items);
        QStringList itemList = itemsQ.split("|", Qt::SkipEmptyParts);
        for (const QString& item : itemList) {
            QRegularExpression re(R"(x(\d+))");
            QRegularExpressionMatch match = re.match(item);
            if (match.hasMatch())
                totalItemsSold += match.captured(1).toInt();
        }
    }
    file.close();

    // احسب إجمالي المنتجات والـ stock
    vector<Product*> allProducts = inventory->getAllProducts();
    int    totalProducts  = (int)allProducts.size();
    double totalStockValue = 0.0;
    int    totalStock      = 0;
    int    lowStockCount   = 0;
    int    outOfStockCount = 0;

    for (Product* p : allProducts) {
        totalStockValue += p->getPrice() * p->getQuantity();
        totalStock      += p->getQuantity();
        if (p->getQuantity() == 0)      outOfStockCount++;
        else if (p->getQuantity() <= 5) lowStockCount++;
    }

    // عدد المستخدمين
    int totalUsers = 0;
    std::ifstream usersFile("users_data.txt");
    std::string uLine;
    while (std::getline(usersFile, uLine))
        if (!uLine.empty()) totalUsers++;
    usersFile.close();

    // ابني الـ Dialog
    QDialog* dash = new QDialog(this);
    dash->setWindowTitle("📊 Admin Sales Dashboard");
    dash->setMinimumSize(550, 580);
    dash->setStyleSheet("QDialog { background-color:#1a1a2e; }");

    QVBoxLayout* lay = new QVBoxLayout(dash);
    lay->setSpacing(10);
    lay->setContentsMargins(25, 25, 25, 25);

    QLabel* hdr = new QLabel("📊 Store-Wide Sales Dashboard\n👑 Admin Overview");
    hdr->setStyleSheet("color:#e94560; font-size:18px; font-weight:bold;");
    hdr->setAlignment(Qt::AlignCenter);
    lay->addWidget(hdr);

    auto makeCard = [&](QString icon, QString label, QString value, QString color) -> QWidget* {
        QWidget* card = new QWidget();
        card->setFixedHeight(65);
        card->setStyleSheet(
            QString("background-color:#16213e; border-radius:10px;"
                    "border-left: 4px solid %1;").arg(color));
        QHBoxLayout* cl = new QHBoxLayout(card);
        cl->setContentsMargins(15, 8, 15, 8);
        QLabel* iconLbl = new QLabel(icon);
        iconLbl->setStyleSheet("font-size:24px; background:transparent; border:none;");
        iconLbl->setFixedWidth(38);
        cl->addWidget(iconLbl);
        QVBoxLayout* textL = new QVBoxLayout();
        textL->setSpacing(1);
        QLabel* lbl = new QLabel(label);
        lbl->setStyleSheet("color:#a8a8b3; font-size:11px; background:transparent; border:none;");
        QLabel* val = new QLabel(value);
        val->setStyleSheet(QString("color:%1; font-size:19px; font-weight:bold; background:transparent; border:none;").arg(color));
        textL->addWidget(lbl);
        textL->addWidget(val);
        cl->addLayout(textL);
        cl->addStretch();
        return card;
    };

    // --- Revenue & Orders ---
    QLabel* sec1 = new QLabel("💹 Revenue & Orders");
    sec1->setStyleSheet("color:#a8a8b3; font-size:12px; font-weight:bold; margin-top:5px;");
    lay->addWidget(sec1);

    lay->addWidget(makeCard("💰", "Total Revenue (non-cancelled orders)",
                            QString("$%1").arg(totalRevenue, 0, 'f', 2), "#6bcb77"));
    lay->addWidget(makeCard("📋", "Total Orders",
                            QString::number(totalOrders), "#e94560"));
    lay->addWidget(makeCard("✅", "Completed Orders",
                            QString::number(totalOrders - cancelledOrders), "#4fc3f7"));
    lay->addWidget(makeCard("❌", "Cancelled Orders",
                            QString::number(cancelledOrders), "#ff6b6b"));
    lay->addWidget(makeCard("🛒", "Total Items Sold",
                            QString::number(totalItemsSold), "#ffd93d"));

    // --- Inventory ---
    QLabel* sec2 = new QLabel("📦 Inventory Overview");
    sec2->setStyleSheet("color:#a8a8b3; font-size:12px; font-weight:bold; margin-top:8px;");
    lay->addWidget(sec2);

    lay->addWidget(makeCard("🏪", "Total Products Listed",
                            QString::number(totalProducts), "#ce93d8"));
    lay->addWidget(makeCard("💼", "Total Stock Value",
                            QString("$%1").arg(totalStockValue, 0, 'f', 2), "#80cbc4"));
    lay->addWidget(makeCard("📦", "Total Units in Stock",
                            QString::number(totalStock), "#80cbc4"));
    lay->addWidget(makeCard("⚠️", "Low Stock Products (≤5)",
                            QString::number(lowStockCount), "#ffd93d"));
    lay->addWidget(makeCard("🚫", "Out of Stock Products",
                            QString::number(outOfStockCount), "#ff6b6b"));

    // --- Users ---
    QLabel* sec3 = new QLabel("👥 Users");
    sec3->setStyleSheet("color:#a8a8b3; font-size:12px; font-weight:bold; margin-top:8px;");
    lay->addWidget(sec3);

    lay->addWidget(makeCard("👤", "Total Registered Users",
                            QString::number(totalUsers), "#4fc3f7"));

    lay->addStretch();

    QPushButton* closeBtn = new QPushButton("✖  Close");
    closeBtn->setFixedHeight(42);
    closeBtn->setStyleSheet(
        "QPushButton { background-color:#e94560; color:white; border-radius:8px; font-size:14px; font-weight:bold; }"
        "QPushButton:hover { background-color:#c73652; }");
    connect(closeBtn, &QPushButton::clicked, dash, &QDialog::accept);
    lay->addWidget(closeBtn);

    dash->exec();
    delete dash;
}

void StoreWindow::loadAllOrdersWithActions() {
    ordersTable->setRowCount(0);
    std::ifstream file("orders_data.txt");
    if (!file.is_open()) {
        ordersTable->setRowCount(1);
        ordersTable->setItem(0, 0, new QTableWidgetItem("No orders yet"));
        return;
    }

    std::string line;
    int row = 0;
    while (std::getline(file, line)) {
        if (line.empty()) continue;
        if (!line.empty() && line.back() == '\r') line.pop_back();

        std::stringstream ss(line);
        std::string idStr, custStr, totalStr, status, items;
        std::getline(ss, idStr, ',');
        std::getline(ss, custStr, ',');
        std::getline(ss, totalStr, ',');
        std::getline(ss, status, ',');
        std::getline(ss, items, ',');

        if (idStr.empty()) continue;

        ordersTable->insertRow(row);
        ordersTable->setItem(row, 0, new QTableWidgetItem(QString::fromStdString(idStr)));
        ordersTable->setItem(row, 1, new QTableWidgetItem(QString::fromStdString(custStr)));
        ordersTable->setItem(row, 2, new QTableWidgetItem(
                                         QString("$%1").arg(QString::fromStdString(totalStr))));
        ordersTable->setItem(row, 3, new QTableWidgetItem(QString::fromStdString(status)));
        QString itemsDisplay = QString::fromStdString(items).replace("|", "\n");
        ordersTable->setItem(row, 4, new QTableWidgetItem(itemsDisplay));

        QWidget* actionWidget = new QWidget();
        QHBoxLayout* actionLayout = new QHBoxLayout(actionWidget);
        actionLayout->setContentsMargins(5, 2, 5, 2);

        QPushButton* updateBtn = new QPushButton("Update Status");
        updateBtn->setFixedSize(120, 30);
        updateBtn->setStyleSheet("background-color:#2196F3; color:white; border-radius:5px;");

        try {
            int orderId = std::stoi(idStr);
            connect(updateBtn, &QPushButton::clicked,
                    [this, orderId]() { onAdminUpdateOrderStatus(orderId); });
        } catch (...) {}

        actionLayout->addWidget(updateBtn);
        ordersTable->setCellWidget(row, 5, actionWidget);
        row++;
    }
    file.close();
    ordersTable->resizeColumnsToContents();
}

void StoreWindow::onAdminAddProduct() {
    QString name        = adminProductNameEdit->text().trimmed();
    QString priceStr    = adminProductPriceEdit->text().trimmed();
    QString stockStr    = adminProductStockEdit->text().trimmed();
    QString category    = adminProductCategoryCombo->currentText();
    QString vendorIdStr = adminVendorIDEdit->text().trimmed();

    if (name.isEmpty() || priceStr.isEmpty() || stockStr.isEmpty()) {
        QMessageBox::warning(this, "Error", "Please fill all fields!"); return;
    }

    bool priceOk, stockOk;
    double price = priceStr.toDouble(&priceOk);
    int stock    = stockStr.toInt(&stockOk);
    int vendorID = vendorIdStr.isEmpty() ? 0 : vendorIdStr.toInt();

    if (!priceOk || price <= 0) { QMessageBox::warning(this, "Error", "Invalid price!"); return; }
    if (!stockOk || stock < 0)  { QMessageBox::warning(this, "Error", "Invalid stock!"); return; }

    inventory->addProduct(name.toStdString(), price, category.toStdString(), stock, vendorID);
    QMessageBox::information(this, "Success",
                             QString("Product '%1' added successfully!").arg(name));

    adminProductNameEdit->clear();
    adminProductPriceEdit->clear();
    adminProductStockEdit->clear();
    adminVendorIDEdit->clear();
    adminAddProductWidget->setVisible(false);
    loadAllProductsAdminWithActions();
    allProductsTable->setVisible(true);
}

void StoreWindow::onAdminEditUserRole(int userId) {
    QStringList roles = {"Customer", "Vendor", "Admin"};
    bool ok;
    QString newRole = QInputDialog::getItem(this, "Change Role",
                                            "Select new role:", roles, 0, false, &ok);
    if (!ok || newRole.isEmpty()) return;

    std::vector<std::string> lines;
    std::ifstream file("users_data.txt");
    std::string line;
    while (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string idStr, username, password, email, role;
        std::getline(ss, idStr, ',');
        std::getline(ss, username, ',');
        std::getline(ss, password, ',');
        std::getline(ss, email, ',');
        std::getline(ss, role, ',');
        try {
            if (std::stoi(idStr) == userId)
                line = idStr + "," + username + "," + password + "," + email + ","
                       + newRole.toStdString();
        } catch (...) {}
        lines.push_back(line);
    }
    file.close();

    std::ofstream outFile("users_data.txt");
    for (const std::string& l : lines) outFile << l << "\n";
    outFile.close();

    QMessageBox::information(this, "Success", "User role updated!");
    loadAllUsersWithActions();
}

void StoreWindow::onAdminDeleteUser(int userId) {
    int reply = QMessageBox::question(this, "Delete User",
                                      "Are you sure you want to delete this user?",
                                      QMessageBox::Yes | QMessageBox::No);
    if (reply != QMessageBox::Yes) return;

    std::vector<std::string> lines;
    std::ifstream file("users_data.txt");
    std::string line;
    while (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string idStr;
        std::getline(ss, idStr, ',');
        try { if (std::stoi(idStr) != userId) lines.push_back(line); }
        catch (...) { lines.push_back(line); }
    }
    file.close();

    std::ofstream outFile("users_data.txt");
    for (const std::string& l : lines) outFile << l << "\n";
    outFile.close();

    QMessageBox::information(this, "Success", "User deleted!");
    loadAllUsersWithActions();
}

void StoreWindow::onAdminEditProduct(int productId) {
    Product* p = inventory->getProduct(productId);
    if (!p) return;

    bool ok;
    double newPrice = QInputDialog::getDouble(this, "Edit Product", "New Price:",
                                              p->getPrice(), 0, 10000, 2, &ok);
    if (ok) {
        int newStock = QInputDialog::getInt(this, "Edit Product", "New Stock:",
                                            p->getQuantity(), 0, 10000, 1, &ok);
        if (ok) {
            inventory->updateProduct(productId, newPrice, newStock);
            loadAllProductsAdminWithActions();
            QMessageBox::information(this, "Success", "Product updated!");
        }
    }
}

void StoreWindow::onAdminDeleteProduct(int productId) {
    int reply = QMessageBox::question(this, "Delete Product",
                                      "Are you sure you want to delete this product?",
                                      QMessageBox::Yes | QMessageBox::No);
    if (reply == QMessageBox::Yes) {
        if (inventory->removeProduct(productId)) {
            loadAllProductsAdminWithActions();
            QMessageBox::information(this, "Success", "Product deleted!");
        } else {
            QMessageBox::warning(this, "Error", "Could not delete product!");
        }
    }
}

void StoreWindow::onAdminUpdateOrderStatus(int orderId) {
    QStringList statuses = {"Pending", "Processing", "Shipped", "Delivered", "Cancelled"};
    bool ok;
    QString newStatus = QInputDialog::getItem(this, "Update Order Status",
                                              "Select new status:", statuses, 0, false, &ok);
    if (!ok || newStatus.isEmpty()) return;

    std::vector<std::string> lines;
    std::ifstream file("orders_data.txt");
    std::string line;
    while (std::getline(file, line)) {
        if (line.empty()) continue;
        std::stringstream ss(line);
        std::string idStr, custStr, totalStr, status, items;
        std::getline(ss, idStr, ',');
        std::getline(ss, custStr, ',');
        std::getline(ss, totalStr, ',');
        std::getline(ss, status, ',');
        std::getline(ss, items, ',');
        try {
            if (std::stoi(idStr) == orderId)
                line = idStr + "," + custStr + "," + totalStr + ","
                       + newStatus.toStdString() + "," + items;
        } catch (...) {}
        lines.push_back(line);
    }
    file.close();

    std::ofstream outFile("orders_data.txt");
    for (const std::string& l : lines) outFile << l << "\n";
    outFile.close();

    QMessageBox::information(this, "Success", "Order status updated!");
    loadAllOrdersWithActions();
}

// ========== COMMON ==========
void StoreWindow::onLogout() {
    int reply = QMessageBox::question(this, "Logout", "Are you sure you want to logout?",
                                      QMessageBox::Yes | QMessageBox::No);
    if (reply == QMessageBox::Yes) {
        AuthManager::logout();
        QWidget* parent = parentWidget();
        while (parent && !parent->isWindow()) parent = parent->parentWidget();
        if (parent) parent->show();
        this->close();
    }
}
