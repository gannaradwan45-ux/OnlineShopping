#include "CartWindow.h"
#include "ReceiptWindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QHeaderView>
#include <QMessageBox>
#include <QInputDialog>
#include <QDateTime>
#include <QDialog>
#include <QLineEdit>
#include <QLabel>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <sstream>
#include "payment.h"
#include "OrderCommand.h"
#include "discount.h"
#include "FileManager.h"

// ---- حفظ الـ order في orders_data.txt ----
static void saveOrderToFile(int customerID, double total, QString status, QString items) {
    int lastID = 0;
    {
        std::ifstream f("orders_data.txt");
        std::string line;
        while (std::getline(f, line)) {
            if (line.empty()) continue;
            std::stringstream ss(line);
            std::string idStr;
            std::getline(ss, idStr, ',');
            try { int id = std::stoi(idStr); if (id > lastID) lastID = id; }
            catch (...) {}
        }
    }
    int newID = lastID + 1;

    std::ofstream f("orders_data.txt", std::ios::app);
    f << newID << ","
      << customerID << ","
      << QString::number(total, 'f', 2).toStdString() << ","
      << status.toStdString() << ","
      << items.toStdString() << "\n";
    f.close();
}

bool validateCreditCard(QString cardNumber, QString cvv, double amount, QWidget* parent) {
    if (cardNumber.length() != 16) {
        QMessageBox::warning(parent, "Invalid Card", "Card number must be exactly 16 digits.");
        return false;
    }
    bool isNumber;
    cardNumber.toLongLong(&isNumber);
    if (!isNumber) {
        QMessageBox::warning(parent, "Invalid Card", "Card number must contain only digits.");
        return false;
    }
    if (cvv.length() != 3) {
        QMessageBox::warning(parent, "Invalid CVV", "CVV must be exactly 3 digits.");
        return false;
    }
    if (cardNumber == "4444444444444444") {
        QMessageBox::warning(parent, "Card Blocked", "This card has been blocked.");
        return false;
    }
    if (cardNumber.startsWith("4111") && amount > 100) {
        QMessageBox::warning(parent, "Insufficient Funds",
                             QString("Insufficient balance.\nAmount due: $%1\nAvailable limit: $100.00").arg(amount));
        return false;
    }
    return true;
}

// ============================================================
//  validateWallet - يقبل WALLET_... أو رقم تليفون (10-15 رقم)
// ============================================================
bool validateWallet(QString walletId, double balance, double amount, QWidget* parent) {
    bool startsWithWallet = walletId.startsWith("WALLET_");

    bool isAllDigits = true;
    for (QChar c : walletId)
        if (!c.isDigit()) { isAllDigits = false; break; }
    bool isPhoneNumber = isAllDigits && walletId.length() >= 10 && walletId.length() <= 15;

    if (!startsWithWallet && !isPhoneNumber) {
        QMessageBox::warning(parent, "Invalid Wallet",
                             "Wallet ID must start with 'WALLET_' or be a phone number (10-15 digits).");
        return false;
    }

    if (startsWithWallet && walletId.length() < 8) {
        QMessageBox::warning(parent, "Invalid Wallet", "Wallet ID is too short.");
        return false;
    }

    if (walletId == "WALLET_BLOCKED") {
        QMessageBox::warning(parent, "Wallet Blocked", "This wallet has been suspended.");
        return false;
    }

    if (balance < amount) {
        QMessageBox::warning(parent, "Insufficient Balance",
                             QString("Insufficient wallet balance.\nAmount due: $%1\nWallet balance: $%2\nShortage: $%3")
                                 .arg(amount).arg(balance).arg(amount - balance));
        return false;
    }
    return true;
}

// ============================================================
//  Constructor
// ============================================================
CartWindow::CartWindow(ShoppingCart* c, Inventory* inv, QWidget* parent)
    : QMainWindow(parent), cart(c), inventory(inv) {

    setWindowTitle("Your Cart");
    setMinimumSize(750, 500);
    setStyleSheet("QMainWindow { background-color: #1a1a2e; }");

    QWidget* central = new QWidget(this);
    setCentralWidget(central);
    QVBoxLayout* layout = new QVBoxLayout(central);
    layout->setContentsMargins(20, 20, 20, 20);
    layout->setSpacing(15);

    QLabel* title = new QLabel("🛒 Your Shopping Cart");
    title->setStyleSheet("color: #e94560; font-size: 22px; font-weight: bold;");
    layout->addWidget(title);

    cartTable = new QTableWidget();
    cartTable->setColumnCount(5);
    cartTable->setHorizontalHeaderLabels({"Product", "Price", "Qty", "Subtotal", "Action"});
    cartTable->setStyleSheet(
        "QTableWidget {"
        "  background-color: #16213e; color: white;"
        "  border: 1px solid #0f3460; border-radius: 8px;"
        "  gridline-color: #0f3460;"
        "}"
        "QHeaderView::section {"
        "  background-color: #e94560; color: white;"
        "  font-weight: bold; padding: 8px; border: none;"
        "}"
        "QTableWidget::item { padding: 8px; }"
        "QTableWidget::item:selected { background-color: #0f3460; }"
        );
    cartTable->horizontalHeader()->setStretchLastSection(true);
    cartTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    cartTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    layout->addWidget(cartTable);

    totalLabel = new QLabel("Total: $0.00");
    totalLabel->setStyleSheet("color: #e94560; font-size: 18px; font-weight: bold;");
    totalLabel->setAlignment(Qt::AlignRight);
    layout->addWidget(totalLabel);

    QPushButton* checkoutBtn = new QPushButton("💳 CHECKOUT");
    checkoutBtn->setFixedHeight(50);
    checkoutBtn->setStyleSheet(
        "QPushButton {"
        "  background-color: #e94560; color: white;"
        "  border-radius: 10px; font-size: 16px; font-weight: bold;"
        "}"
        "QPushButton:hover { background-color: #c73652; }"
        );
    connect(checkoutBtn, &QPushButton::clicked, this, &CartWindow::onCheckout);
    layout->addWidget(checkoutBtn);

    loadCart();
}

// ============================================================
//  loadCart
// ============================================================
void CartWindow::loadCart() {
    vector<CartItem> items = cart->getItems();
    cartTable->setRowCount(items.size());

    for (int i = 0; i < (int)items.size(); i++) {
        cartTable->setItem(i, 0, new QTableWidgetItem(
                                     QString::fromStdString(items[i].getProductName())));
        cartTable->setItem(i, 1, new QTableWidgetItem(
                                     QString("$%1").arg(items[i].getPrice())));
        cartTable->setItem(i, 2, new QTableWidgetItem(
                                     QString::number(items[i].getQuantity())));
        cartTable->setItem(i, 3, new QTableWidgetItem(
                                     QString("$%1").arg(items[i].getSubtotal())));

        int productId = items[i].getProductID();
        QPushButton* removeBtn = new QPushButton("🗑 Remove");
        removeBtn->setStyleSheet(
            "QPushButton { background-color:#ff6b6b; color:white;"
            "  border-radius:6px; padding:4px 10px; font-weight:bold; }"
            "QPushButton:hover { background-color:#e74c3c; }");
        connect(removeBtn, &QPushButton::clicked,
                [this, productId]() { onRemoveItem(productId); });
        cartTable->setCellWidget(i, 4, removeBtn);
    }

    totalLabel->setText(QString("Total: $%1").arg(cart->getTotalAmount()));
}

// ============================================================
//  onRemoveItem
// ============================================================
void CartWindow::onRemoveItem(int productId) {
    int reply = QMessageBox::question(this, "Remove Item",
                                      "Are you sure you want to remove this item from your cart?",
                                      QMessageBox::Yes | QMessageBox::No);
    if (reply != QMessageBox::Yes) return;

    int qty = cart->getQuantity(productId);
    cart->removeItem(productId);

    if (inventory && qty > 0) {
        Product* p = inventory->getProduct(productId);
        if (p) inventory->updateStock(productId, p->getQuantity() + qty);
    }

    loadCart();
    QMessageBox::information(this, "Item Removed", "✅ Item removed and stock has been restored.");
}

// ============================================================
//  onCheckout
// ============================================================
void CartWindow::onCheckout() {
    if (cart->isEmpty()) {
        QMessageBox::warning(this, "Empty Cart", "Your cart is empty!");
        return;
    }

    QStringList methods = {"Credit Card", "Cash", "Digital Wallet"};
    bool ok;
    QString method = QInputDialog::getItem(this, "Payment",
                                           "Choose Payment Method:", methods, 0, false, &ok);
    if (!ok) return;

    PaymentStrategy* strategy = nullptr;
    QString paymentDetails;

    if (method == "Credit Card") {
        QDialog cardDialog(this);
        cardDialog.setWindowTitle("💳 Credit Card Payment");
        cardDialog.setStyleSheet("QDialog { background-color: #16213e; }");
        cardDialog.setFixedSize(400, 350);

        QVBoxLayout* dialogLayout = new QVBoxLayout(&cardDialog);
        dialogLayout->setSpacing(15);
        dialogLayout->setContentsMargins(20, 20, 20, 20);

        QLabel* titleLabel = new QLabel("💳 Enter Card Details");
        titleLabel->setStyleSheet("color: #e94560; font-size: 18px; font-weight: bold;");
        titleLabel->setAlignment(Qt::AlignCenter);
        dialogLayout->addWidget(titleLabel);

        QString fieldStyle =
            "QLineEdit {"
            "  background-color: #0f3460; color: white;"
            "  border: 2px solid #e94560; border-radius: 8px;"
            "  padding: 10px; font-size: 14px;"
            "}";

        auto addField = [&](QString labelText, int maxLen,
                            QLineEdit::EchoMode echo = QLineEdit::Normal) -> QLineEdit* {
            QLabel* lbl = new QLabel(labelText);
            lbl->setStyleSheet("color: white; font-size: 13px;");
            dialogLayout->addWidget(lbl);
            QLineEdit* edit = new QLineEdit();
            edit->setMaxLength(maxLen);
            edit->setEchoMode(echo);
            edit->setStyleSheet(fieldStyle);
            dialogLayout->addWidget(edit);
            return edit;
        };

        QLineEdit* cardEdit   = addField("Card Number (16 digits):", 16);
        cardEdit->setPlaceholderText("1234567890123456");
        QLineEdit* cvvEdit    = addField("CVV (3 digits):", 3, QLineEdit::Password);
        cvvEdit->setPlaceholderText("123");
        QLineEdit* holderEdit = addField("Card Holder Name:", 50);
        holderEdit->setPlaceholderText("John Smith");

        dialogLayout->addStretch();

        QHBoxLayout* btnLayout = new QHBoxLayout();
        QPushButton* cancelBtn = new QPushButton("Cancel");
        cancelBtn->setStyleSheet(
            "QPushButton { background-color: #0f3460; color: white;"
            "  border-radius: 8px; padding: 8px 20px; }"
            "QPushButton:hover { background-color: #e94560; }");
        QPushButton* payBtn = new QPushButton("Pay Now");
        payBtn->setStyleSheet(
            "QPushButton { background-color: #e94560; color: white;"
            "  border-radius: 8px; padding: 8px 20px; font-weight: bold; }"
            "QPushButton:hover { background-color: #c73652; }");
        btnLayout->addWidget(cancelBtn);
        btnLayout->addWidget(payBtn);
        dialogLayout->addLayout(btnLayout);

        connect(cancelBtn, &QPushButton::clicked, &cardDialog, &QDialog::reject);
        connect(payBtn,    &QPushButton::clicked, &cardDialog, &QDialog::accept);

        if (cardDialog.exec() != QDialog::Accepted) return;
        if (!validateCreditCard(cardEdit->text(), cvvEdit->text(), cart->getTotalAmount(), this)) return;

        strategy = new CreditCardPayment(cardEdit->text().toStdString(),
                                         holderEdit->text().toStdString());
        paymentDetails = QString("Card: ****%1").arg(cardEdit->text().right(4));

    } else if (method == "Cash") {
        double cash = QInputDialog::getDouble(this, "Cash",
                                              "Cash Amount: $", 0, 0, 99999, 2, &ok);
        if (!ok) return;

        if (cash < cart->getTotalAmount()) {
            QMessageBox::critical(this, "Payment Declined",
                                  QString("❌ Insufficient Cash!\nAmount due: $%1\nCash given: $%2\nShortage: $%3")
                                      .arg(cart->getTotalAmount()).arg(cash)
                                      .arg(cart->getTotalAmount() - cash));
            return;
        }
        strategy = new CashPayment(cash);
        // *** Change بدل New Balance ***
        paymentDetails = QString("Cash: $%1\nChange: $%2")
                             .arg(cash).arg(cash - cart->getTotalAmount());

    } else if (method == "Digital Wallet") {
        QString walletId = QInputDialog::getText(this, "Digital Wallet",
                                                 "Wallet ID (WALLET_... or phone number 10-15 digits):",
                                                 QLineEdit::Normal, "", &ok);
        if (!ok || walletId.trimmed().isEmpty()) return;
        walletId = walletId.trimmed();

        double balance = QInputDialog::getDouble(this, "Digital Wallet",
                                                 "Current Balance ($):",
                                                 0, 0, 999999, 2, &ok);
        if (!ok) return;

        if (!validateWallet(walletId, balance, cart->getTotalAmount(), this)) return;

        strategy = new WalletPayment(walletId.toStdString(), balance);
        double newBalance = balance - cart->getTotalAmount();
        paymentDetails = QString("Wallet: %1\nChange: $%2")
                             .arg(walletId).arg(newBalance, 0, 'f', 2);
    }

    Payment payment(1, "Order#1", cart->getTotalAmount(), strategy);
    payment.processPayment();

    // ====== Coupon - مع Do you want to continue? ======
    double finalPrice    = cart->getTotalAmount();
    double discountValue = 0;

    int hasCoupon = QMessageBox::question(this, "🎟 Discount Coupon",
                                          "Do you have a coupon code?",
                                          QMessageBox::Yes | QMessageBox::No);

    if (hasCoupon == QMessageBox::Yes) {
        // Loop عشان نعمل Go Back لو الكود غلط
        while (true) {
            int code = QInputDialog::getInt(this, "Coupon",
                                            "Enter Coupon Code:", 0, 0, 99999, 1, &ok);
            if (!ok) break; // Cancel = خروج

            QList<int> validCoupons = {1234, 5678, 9999, 1111, 4242};

            if (validCoupons.contains(code)) {
                // كوبون صح - اسأل Do you want to continue?
                srand(time(nullptr));
                int percent = (rand() % 46) + 5;
                Discount discount(code, percent);
                double oldTotal = finalPrice;

                QMessageBox confirmBox(this);
                confirmBox.setWindowTitle("🎉 Valid Coupon!");
                confirmBox.setText(
                    QString("✅ Coupon is VALID!\n\nDiscount: %1%\nYou will save: $%2\nNew Total: $%3\n\nDo you want to continue?")
                        .arg(percent)
                        .arg(oldTotal - discount.applyDiscount(oldTotal), 0, 'f', 2)
                        .arg(discount.applyDiscount(oldTotal), 0, 'f', 2));
                confirmBox.setStyleSheet("QMessageBox { background-color:#16213e; color:white; }");

                QPushButton* yesBtn  = confirmBox.addButton("✅ Yes", QMessageBox::AcceptRole);
                QPushButton* backBtn = confirmBox.addButton("🔙 Go Back", QMessageBox::ResetRole);

                confirmBox.exec();

                if (confirmBox.clickedButton() == yesBtn) {
                    finalPrice    = discount.applyDiscount(oldTotal);
                    discountValue = oldTotal - finalPrice;
                    QMessageBox::information(this, "✅ Applied!",
                                             QString("Discount applied!\nYou saved: $%1")
                                                 .arg(discountValue, 0, 'f', 2));
                    break;
                } else {
                    // Go Back - يرجع يدخل كود تاني
                    continue;
                }

            } else {
                // كوبون غلط - اسأل Do you want to continue?
                QMessageBox invalidBox(this);
                invalidBox.setWindowTitle("❌ Invalid Coupon");
                invalidBox.setText("This coupon code is not valid!\n\nDo you want to continue?");
                invalidBox.setStyleSheet("QMessageBox { background-color:#16213e; color:white; }");

                QPushButton* yesBtn  = invalidBox.addButton("✅ Yes - Continue without discount", QMessageBox::AcceptRole);
                QPushButton* backBtn = invalidBox.addButton("🔙 Go Back - Try another code",      QMessageBox::ResetRole);

                invalidBox.exec();

                if (invalidBox.clickedButton() == yesBtn) {
                    // يكمل بدون خصم
                    break;
                } else {
                    // Go Back - يرجع يدخل كود تاني
                    continue;
                }
            }
        }
    }

    FileManager::savePayment(payment);

    // ====== حفظ الـ Order ======
    QString itemsSummary;
    vector<CartItem> items = cart->getItems();
    for (auto& item : items) {
        itemsSummary += QString::fromStdString(item.getProductName())
        + " x" + QString::number(item.getQuantity()) + "|";
    }
    saveOrderToFile(cart->getCustomerID(), finalPrice, "Pending", itemsSummary);

    // ====== Receipt ======
    QString receiptText = "🧾 PURCHASE RECEIPT 🧾\n\n";
    receiptText += "Store: Online Shop\n";
    receiptText += "Date: " + QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss") + "\n";
    receiptText += "Payment Method: " + method + "\n";
    receiptText += "Payment Details:\n" + paymentDetails + "\n";
    receiptText += "----------------------------------------\n\n";

    for (auto& item : items) {
        receiptText += QString("%1 x%2 ............ $%3\n")
        .arg(QString::fromStdString(item.getProductName()))
            .arg(item.getQuantity())
            .arg(item.getSubtotal(), 0, 'f', 2);
    }

    receiptText += "\n----------------------------------------\n";
    receiptText += QString("Subtotal:    $%1\n").arg(cart->getTotalAmount(), 0, 'f', 2);
    if (discountValue > 0)
        receiptText += QString("Discount:   -$%1\n").arg(discountValue, 0, 'f', 2);
    receiptText += QString("Final Total: $%1\n").arg(finalPrice, 0, 'f', 2);
    receiptText += QString("Status: %1\n").arg(QString::fromStdString(payment.getStatus()));
    receiptText += "----------------------------------------\n";
    receiptText += "✅ Thank you for shopping with us! 🛒\n";

    ReceiptWindow* receiptWin = new ReceiptWindow(receiptText, this);
    receiptWin->show();

    if (strategy) delete strategy;
    cart->clearCart();
    close();
}
