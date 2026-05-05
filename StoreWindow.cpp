#include "StoreWindow.h"
#include "CartWindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QScrollArea>
#include <QComboBox>
#include <QMessageBox>
#include <QInputDialog>
#include <QMenu>
#include <QAction>

StoreWindow::StoreWindow(Inventory* inv, ShoppingCart* c,
                         QString uname, QWidget* parent)
    : QMainWindow(parent), inventory(inv), cart(c), username(uname) {

    setWindowTitle("Online Shop - Store");
    setMinimumSize(900, 650);
    setStyleSheet("QMainWindow { background-color: #1a1a2e; }");

    notifCount = 0;

    QWidget* central = new QWidget(this);
    setCentralWidget(central);
    QVBoxLayout* mainLayout = new QVBoxLayout(central);
    mainLayout->setSpacing(0);
    mainLayout->setContentsMargins(0, 0, 0, 0);

    // ====== Header ======
    QWidget* header = new QWidget();
    header->setFixedHeight(70);
    header->setStyleSheet("background-color: #16213e;");
    QHBoxLayout* headerLayout = new QHBoxLayout(header);
    headerLayout->setContentsMargins(20, 10, 20, 10);

    QLabel* logo = new QLabel("🛒 Online Shop");
    logo->setStyleSheet("color: #e94560; font-size: 22px; font-weight: bold;");
    headerLayout->addWidget(logo);

    headerLayout->addStretch();

    // Profile Button مع Menu
    QPushButton* profileBtn = new QPushButton("👤 " + username);
    profileBtn->setStyleSheet(
        "QPushButton {"
        "  background-color: #0f3460;"
        "  color: white;"
        "  border-radius: 8px;"
        "  padding: 6px 14px;"
        "  font-size: 13px;"
        "  font-weight: bold;"
        "}"
        "QPushButton:hover { background-color: #e94560; }"
        "QPushButton::menu-indicator { image: none; }"
        );

    QMenu* profileMenu = new QMenu(this);
    profileMenu->setStyleSheet(
        "QMenu {"
        "  background-color: #16213e;"
        "  color: white;"
        "  border: 1px solid #e94560;"
        "  border-radius: 8px;"
        "  padding: 5px;"
        "}"
        "QMenu::item {"
        "  padding: 8px 20px;"
        "  border-radius: 5px;"
        "}"
        "QMenu::item:selected {"
        "  background-color: #e94560;"
        "}"
        );

    QAction* userInfo = new QAction("👤 " + username, this);
    userInfo->setEnabled(false);
    profileMenu->addAction(userInfo);
    profileMenu->addSeparator();

    QAction* logoutAction = new QAction("🚪 Logout", this);
    connect(logoutAction, &QAction::triggered, this, &StoreWindow::onLogout);
    profileMenu->addAction(logoutAction);

    profileBtn->setMenu(profileMenu);
    headerLayout->addWidget(profileBtn);

    headerLayout->addSpacing(10);

    // زرار الجرس
    notifBtn = new QPushButton("🔔");
    notifBtn->setFixedSize(40, 35);
    notifBtn->setStyleSheet(
        "QPushButton {"
        "  background-color: #0f3460;"
        "  color: white;"
        "  border-radius: 8px;"
        "  font-size: 16px;"
        "}"
        "QPushButton:hover { background-color: #e94560; }"
        );
    connect(notifBtn, &QPushButton::clicked, [this]() {
        notifPanel->move(width() - 320, 70);
        notifPanel->togglePanel();
    });
    headerLayout->addWidget(notifBtn);

    headerLayout->addSpacing(10);

    QPushButton* cartBtn = new QPushButton("🛒 View Cart");
    cartBtn->setStyleSheet(
        "QPushButton {"
        "  background-color: #e94560;"
        "  color: white;"
        "  border-radius: 8px;"
        "  padding: 8px 16px;"
        "  font-size: 13px;"
        "  font-weight: bold;"
        "}"
        "QPushButton:hover { background-color: #c73652; }"
        );
    connect(cartBtn, &QPushButton::clicked, this, &StoreWindow::onViewCart);
    headerLayout->addWidget(cartBtn);

    cartCountLabel = new QLabel("Items: 0");
    cartCountLabel->setStyleSheet(
        "color: #e94560; font-size: 13px; font-weight: bold;");
    headerLayout->addWidget(cartCountLabel);

    mainLayout->addWidget(header);

    // ====== Category Bar ======
    QWidget* catBar = new QWidget();
    catBar->setFixedHeight(55);
    catBar->setStyleSheet("background-color: #0f3460;");
    QHBoxLayout* catLayout = new QHBoxLayout(catBar);
    catLayout->setContentsMargins(20, 8, 20, 8);

    QLabel* catLabel = new QLabel("Category:");
    catLabel->setStyleSheet("color: white; font-size: 14px;");
    catLayout->addWidget(catLabel);

    QComboBox* categoryBox = new QComboBox();
    categoryBox->addItems({"Electronics", "Grocery", "Books", "Appliances"});
    categoryBox->setStyleSheet(
        "QComboBox {"
        "  background-color: #16213e;"
        "  color: white;"
        "  border: 2px solid #e94560;"
        "  border-radius: 8px;"
        "  padding: 5px 15px;"
        "  font-size: 13px;"
        "  min-width: 180px;"
        "}"
        "QComboBox::drop-down { border: none; }"
        "QComboBox QAbstractItemView {"
        "  background-color: #16213e;"
        "  color: white;"
        "  selection-background-color: #e94560;"
        "}"
        );
    connect(categoryBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &StoreWindow::onCategoryChanged);
    catLayout->addWidget(categoryBox);
    catLayout->addStretch();
    mainLayout->addWidget(catBar);

    // ====== Products Area ======
    QScrollArea* scrollArea = new QScrollArea();
    scrollArea->setStyleSheet(
        "QScrollArea { background-color: #1a1a2e; border: none; }");
    scrollArea->setWidgetResizable(true);

    productsWidget = new QWidget();
    productsWidget->setStyleSheet("background-color: #1a1a2e;");
    productsGrid = new QGridLayout(productsWidget);
    productsGrid->setSpacing(15);
    productsGrid->setContentsMargins(20, 20, 20, 20);

    scrollArea->setWidget(productsWidget);
    mainLayout->addWidget(scrollArea);

    // Notif Panel
    notifPanel = new NotifPanel(this);
    notifPanel->move(width() - 320, 70);

    // Connect notifications signals
    connect(notifPanel, &NotifPanel::continueShoppingClicked, [this]() {
        notifPanel->togglePanel();
    });
    connect(notifPanel, &NotifPanel::checkoutClicked, [this]() {
        onViewCart();
        notifPanel->togglePanel();
    });
    connect(notifPanel, &NotifPanel::deleteFromCartClicked, [this](int productId) {
        // Remove item from cart
        vector<CartItem> items = cart->getItems();
        for (auto& item : items) {
            if (item.getProductID() == productId) {
                cart->removeItem(productId);
                // Update stock in inventory
                Product* p = inventory->getProduct(productId);
                if (p) {
                    inventory->updateStock(productId, p->getQuantity() + item.getQuantity());
                }
                break;
            }
        }
        int total = (int)cart->getItems().size();
        cartCountLabel->setText(QString("Items: %1").arg(total));

        notifCount--;
        notifBtn->setText(QString("🔔 %1").arg(notifCount));

        QMessageBox::information(this, "Removed", "Product removed from cart!");
        notifPanel->togglePanel();
    });
    connect(notifPanel, &NotifPanel::viewDetailsClicked, [this]() {
        // Open purchase history or details
        QMessageBox::information(this, "Order Details",
                                 "📦 Your order has been confirmed!\n"
                                 "Shipping will take 3-5 business days.\n"
                                 "Tracking number will be sent to your email.");
        notifPanel->togglePanel();
    });

    loadProducts("Electronics");
}

void StoreWindow::onCategoryChanged(int index) {
    QStringList cats = {"Electronics", "Grocery", "Books", "Appliances"};
    loadProducts(cats[index]);
}

void StoreWindow::loadProducts(QString category) {
    QLayoutItem* item;
    while ((item = productsGrid->takeAt(0)) != nullptr) {
        delete item->widget();
        delete item;
    }

    struct ProductData { int id; QString name; double price; int stock; };
    QList<ProductData> products;

    if (category == "Electronics") {
        products = {{1,"iPhone 15",999.99,10},
                    {2,"Samsung TV",599.99,5},
                    {3,"Laptop Dell",1200.00,8}};
    } else if (category == "Grocery") {
        products = {{4,"Rice 5kg",15.00,50},
                    {5,"Milk 1L",5.00,30},
                    {6,"Bread",3.00,20}};
    } else if (category == "Books") {
        products = {{7,"C++ Programming",45.00,15},
                    {8,"Data Structures",55.00,12}};
    } else if (category == "Appliances") {
        products = {{9,"Washing Machine",450.00,4},
                    {10,"Refrigerator",700.00,3}};
    }

    int col = 0, row = 0;
    for (auto& p : products) {
        QWidget* card = createProductCard(p.id, p.name,
                                          p.price, p.stock, category);
        productsGrid->addWidget(card, row, col);
        col++;
        if (col == 3) { col = 0; row++; }
    }
}

QWidget* StoreWindow::createProductCard(int id, QString name,
                                        double price, int stock,
                                        QString category) {
    QWidget* card = new QWidget();
    card->setFixedSize(250, 280);
    card->setStyleSheet(
        "QWidget {"
        "  background-color: #16213e;"
        "  border-radius: 12px;"
        "  border: 1px solid #0f3460;"
        "}"
        );

    QVBoxLayout* layout = new QVBoxLayout(card);
    layout->setSpacing(8);
    layout->setContentsMargins(15, 15, 15, 15);

    QLabel* badge = new QLabel(category);
    badge->setAlignment(Qt::AlignCenter);
    badge->setFixedHeight(25);
    badge->setStyleSheet(
        "background-color: #e94560; color: white;"
        "border-radius: 6px; font-size: 11px; font-weight: bold;");
    layout->addWidget(badge);

    QLabel* icon = new QLabel();
    icon->setAlignment(Qt::AlignCenter);
    icon->setFixedHeight(70);
    if (category == "Electronics") icon->setText("📱");
    else if (category == "Grocery")  icon->setText("🛒");
    else if (category == "Books")    icon->setText("📚");
    else                             icon->setText("🏠");
    icon->setStyleSheet(
        "font-size: 40px; background: transparent; border: none;");
    layout->addWidget(icon);

    QLabel* nameLabel = new QLabel(name);
    nameLabel->setAlignment(Qt::AlignCenter);
    nameLabel->setWordWrap(true);
    nameLabel->setStyleSheet(
        "color: white; font-size: 14px; font-weight: bold;"
        "background: transparent; border: none;");
    layout->addWidget(nameLabel);

    QLabel* priceLabel = new QLabel(QString("$%1").arg(price));
    priceLabel->setAlignment(Qt::AlignCenter);
    priceLabel->setStyleSheet(
        "color: #e94560; font-size: 18px; font-weight: bold;"
        "background: transparent; border: none;");
    layout->addWidget(priceLabel);

    QLabel* stockLabel = new QLabel(QString("In Stock: %1").arg(stock));
    stockLabel->setAlignment(Qt::AlignCenter);
    stockLabel->setStyleSheet(
        "color: #a8a8b3; font-size: 11px;"
        "background: transparent; border: none;");
    layout->addWidget(stockLabel);

    QPushButton* addBtn = new QPushButton("Add to Cart");
    addBtn->setFixedHeight(35);
    addBtn->setStyleSheet(
        "QPushButton {"
        "  background-color: #e94560; color: white;"
        "  border-radius: 8px; font-size: 13px;"
        "  font-weight: bold; border: none;"
        "}"
        "QPushButton:hover { background-color: #c73652; }"
        "QPushButton:pressed { background-color: #a02840; }"
        );
    int productId = id;
    connect(addBtn, &QPushButton::clicked, [this, productId]() {
        onAddToCart(productId);
    });
    layout->addWidget(addBtn);

    return card;
}

void StoreWindow::onAddToCart(int productId) {
    bool ok;
    int quantity = QInputDialog::getInt(this, "Add to Cart",
                                        "Enter Quantity:", 1, 1, 100, 1, &ok);
    if (!ok) return;

    Product* p = inventory->getProduct(productId);
    if (p && inventory->isProductAvailable(productId, quantity)) {
        cart->addItem(p->getProductID(), p->getProductName(),
                      p->getPrice(), quantity);
        inventory->updateStock(productId, p->getQuantity() - quantity);

        int total = (int)cart->getItems().size();
        cartCountLabel->setText(QString("Items: %1").arg(total));

        notifCount++;
        notifBtn->setText(QString("🔔 %1").arg(notifCount));

        // Add to notifications panel
        notifPanel->addCartNotification(
            QString::fromStdString(p->getProductName()),
            quantity,
            p->getPrice(),
            productId
            );

        // Show confirmation message
        QMessageBox::information(this, "Added to Cart",
                                 QString("✅ %1 x%2 added to cart!\nTotal: $%3")
                                     .arg(QString::fromStdString(p->getProductName()))
                                     .arg(quantity)
                                     .arg(p->getPrice() * quantity));

    } else {
        QMessageBox::warning(this, "Error", "Not enough stock!");
    }
}

void StoreWindow::onViewCart() {
    CartWindow* cartWin = new CartWindow(cart, this);
    cartWin->show();
}

void StoreWindow::onLogout() {
    int reply = QMessageBox::question(this, "Logout",
                                      "Are you sure you want to logout?",
                                      QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::Yes) {
        // Return to login window
        QWidget* parent = parentWidget();
        while (parent && !parent->isWindow()) {
            parent = parent->parentWidget();
        }
        if (parent) {
            parent->show();
        }
        this->close();
    }
}