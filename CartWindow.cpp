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
#include "payment.h"
#include "OrderCommand.h"
#include "discount.h"
#include "FileManager.h"

// دوال التحقق من الدفع (محاكاة)
bool validateCreditCard(QString cardNumber, QString cvv, double amount) {
    // 1. التحقق من طول رقم الكارت
    if (cardNumber.length() != 16) {
        QMessageBox::warning(nullptr, "Invalid Card", "Card number must be exactly 16 digits.");
        return false;
    }

    // 2. التحقق من أن كل الأرقام (تأكد أنها أرقام وليست حروف)
    bool isNumber;
    cardNumber.toLongLong(&isNumber);
    if (!isNumber) {
        QMessageBox::warning(nullptr, "Invalid Card", "Card number must contain only digits.");
        return false;
    }

    // 3. التحقق من طول الـ CVV
    if (cvv.length() != 3) {
        QMessageBox::warning(nullptr, "Invalid CVV", "CVV must be exactly 3 digits.");
        return false;
    }

    // 4. محاكاة: كارت محظور
    if (cardNumber == "4444444444444444") {
        QMessageBox::warning(nullptr, "Card Blocked", "This card has been blocked. Please use another card.");
        return false;
    }

    // 5. محاكاة: رصيد غير كافٍ للكارت اللي بيبدأ بـ 4111
    if (cardNumber.startsWith("4111") && amount > 100) {
        QMessageBox::warning(nullptr, "Insufficient Funds",
                             QString("Insufficient balance.\n"
                                     "Amount due: $%1\n"
                                     "Available limit: $100.00").arg(amount));
        return false;
    }

    return true;
}

bool validateWallet(QString walletId, double balance, double amount) {
    // 1. التحقق من صيغة الـ Wallet ID
    if (!walletId.startsWith("WALLET_")) {
        QMessageBox::warning(nullptr, "Invalid Wallet",
                             "Wallet ID must start with 'WALLET_'");
        return false;
    }

    // 2. التحقق من أن الـ ID مش فاضي
    if (walletId.length() < 8) {
        QMessageBox::warning(nullptr, "Invalid Wallet",
                             "Wallet ID is too short");
        return false;
    }

    // 3. محاكاة: محفظة محظورة
    if (walletId == "WALLET_BLOCKED") {
        QMessageBox::warning(nullptr, "Wallet Blocked",
                             "This wallet has been suspended.");
        return false;
    }

    // 4. التحقق من الرصيد الكافي
    if (balance < amount) {
        QMessageBox::warning(nullptr, "Insufficient Balance",
                             QString("Insufficient wallet balance.\n"
                                     "Amount due: $%1\n"
                                     "Wallet balance: $%2\n"
                                     "Shortage: $%3")
                                 .arg(amount)
                                 .arg(balance)
                                 .arg(amount - balance));
        return false;
    }

    return true;
}

CartWindow::CartWindow(ShoppingCart* c, QWidget* parent)
    : QMainWindow(parent), cart(c) {

    setWindowTitle("Your Cart");
    setMinimumSize(700, 500);
    setStyleSheet("QMainWindow { background-color: #1a1a2e; }");

    QWidget* central = new QWidget(this);
    setCentralWidget(central);
    QVBoxLayout* layout = new QVBoxLayout(central);
    layout->setContentsMargins(20, 20, 20, 20);
    layout->setSpacing(15);

    QLabel* title = new QLabel("🛒 Your Shopping Cart");
    title->setStyleSheet(
        "color: #e94560; font-size: 22px; font-weight: bold;");
    layout->addWidget(title);

    // Cart Table
    cartTable = new QTableWidget();
    cartTable->setColumnCount(4);
    cartTable->setHorizontalHeaderLabels({"Product", "Price", "Qty", "Subtotal"});
    cartTable->setStyleSheet(
        "QTableWidget {"
        "  background-color: #16213e;"
        "  color: white;"
        "  border: 1px solid #0f3460;"
        "  border-radius: 8px;"
        "  gridline-color: #0f3460;"
        "}"
        "QHeaderView::section {"
        "  background-color: #e94560;"
        "  color: white;"
        "  font-weight: bold;"
        "  padding: 8px;"
        "  border: none;"
        "}"
        "QTableWidget::item { padding: 8px; }"
        "QTableWidget::item:selected { background-color: #0f3460; }"
        );
    cartTable->horizontalHeader()->setStretchLastSection(true);
    cartTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    cartTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    layout->addWidget(cartTable);

    // Total
    totalLabel = new QLabel("Total: $0.00");
    totalLabel->setStyleSheet(
        "color: #e94560; font-size: 18px; font-weight: bold;");
    totalLabel->setAlignment(Qt::AlignRight);
    layout->addWidget(totalLabel);

    // Checkout Button
    QPushButton* checkoutBtn = new QPushButton("CHECKOUT");
    checkoutBtn->setFixedHeight(50);
    checkoutBtn->setStyleSheet(
        "QPushButton {"
        "  background-color: #e94560;"
        "  color: white;"
        "  border-radius: 10px;"
        "  font-size: 16px;"
        "  font-weight: bold;"
        "}"
        "QPushButton:hover { background-color: #c73652; }"
        );
    connect(checkoutBtn, &QPushButton::clicked, this, &CartWindow::onCheckout);
    layout->addWidget(checkoutBtn);

    loadCart();
}

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
    }

    totalLabel->setText(QString("Total: $%1").arg(cart->getTotalAmount()));
}

void CartWindow::onCheckout() {
    if (cart->isEmpty()) {
        QMessageBox::warning(this, "Empty Cart", "Your cart is empty!");
        return;
    }

    // Payment Method
    QStringList methods = {"Credit Card", "Cash", "Digital Wallet"};
    bool ok;
    QString method = QInputDialog::getItem(this, "Payment",
                                           "Choose Payment Method:", methods, 0, false, &ok);
    if (!ok) return;

    PaymentStrategy* strategy = nullptr;
    QString paymentDetails;

    if (method == "Credit Card") {
        // نافذة مخصصة لإدخال الكارت
        QDialog cardDialog(this);
        cardDialog.setWindowTitle("💳 Credit Card Payment");
        cardDialog.setStyleSheet("QDialog { background-color: #16213e; }");
        cardDialog.setFixedSize(400, 350);

        QVBoxLayout* dialogLayout = new QVBoxLayout(&cardDialog);
        dialogLayout->setSpacing(15);
        dialogLayout->setContentsMargins(20, 20, 20, 20);

        // عنوان
        QLabel* titleLabel = new QLabel("💳 Enter Card Details");
        titleLabel->setStyleSheet("color: #e94560; font-size: 18px; font-weight: bold;");
        titleLabel->setAlignment(Qt::AlignCenter);
        dialogLayout->addWidget(titleLabel);

        // Card Number
        QLabel* cardLabel = new QLabel("Card Number (16 digits):");
        cardLabel->setStyleSheet("color: white; font-size: 13px;");
        dialogLayout->addWidget(cardLabel);

        QLineEdit* cardEdit = new QLineEdit();
        cardEdit->setPlaceholderText("1234567890123456");
        cardEdit->setMaxLength(16);
        cardEdit->setStyleSheet(
            "QLineEdit {"
            "  background-color: #0f3460;"
            "  color: white;"
            "  border: 2px solid #e94560;"
            "  border-radius: 8px;"
            "  padding: 10px;"
            "  font-size: 14px;"
            "}"
            );
        dialogLayout->addWidget(cardEdit);

        // CVV
        QLabel* cvvLabel = new QLabel("CVV (3 digits):");
        cvvLabel->setStyleSheet("color: white; font-size: 13px;");
        dialogLayout->addWidget(cvvLabel);

        QLineEdit* cvvEdit = new QLineEdit();
        cvvEdit->setPlaceholderText("123");
        cvvEdit->setMaxLength(3);
        cvvEdit->setEchoMode(QLineEdit::Password);
        cvvEdit->setStyleSheet(
            "QLineEdit {"
            "  background-color: #0f3460;"
            "  color: white;"
            "  border: 2px solid #e94560;"
            "  border-radius: 8px;"
            "  padding: 10px;"
            "  font-size: 14px;"
            "}"
            );
        dialogLayout->addWidget(cvvEdit);

        // Card Holder
        QLabel* holderLabel = new QLabel("Card Holder Name:");
        holderLabel->setStyleSheet("color: white; font-size: 13px;");
        dialogLayout->addWidget(holderLabel);

        QLineEdit* holderEdit = new QLineEdit();
        holderEdit->setPlaceholderText("John Smith");
        holderEdit->setStyleSheet(
            "QLineEdit {"
            "  background-color: #0f3460;"
            "  color: white;"
            "  border: 2px solid #e94560;"
            "  border-radius: 8px;"
            "  padding: 10px;"
            "  font-size: 14px;"
            "}"
            );
        dialogLayout->addWidget(holderEdit);

        dialogLayout->addStretch();

        // Buttons
        QHBoxLayout* btnLayout = new QHBoxLayout();

        QPushButton* cancelBtn = new QPushButton("Cancel");
        cancelBtn->setStyleSheet(
            "QPushButton {"
            "  background-color: #0f3460;"
            "  color: white;"
            "  border-radius: 8px;"
            "  padding: 8px 20px;"
            "}"
            "QPushButton:hover { background-color: #e94560; }"
            );

        QPushButton* payBtn = new QPushButton("Pay Now");
        payBtn->setStyleSheet(
            "QPushButton {"
            "  background-color: #e94560;"
            "  color: white;"
            "  border-radius: 8px;"
            "  padding: 8px 20px;"
            "  font-weight: bold;"
            "}"
            "QPushButton:hover { background-color: #c73652; }"
            );

        btnLayout->addWidget(cancelBtn);
        btnLayout->addWidget(payBtn);
        dialogLayout->addLayout(btnLayout);

        connect(cancelBtn, &QPushButton::clicked, &cardDialog, &QDialog::reject);
        connect(payBtn, &QPushButton::clicked, &cardDialog, &QDialog::accept);

        if (cardDialog.exec() != QDialog::Accepted) return;

        QString card = cardEdit->text();
        QString cvv = cvvEdit->text();
        QString holder = holderEdit->text();

        if (!validateCreditCard(card, cvv, cart->getTotalAmount())) {
            return;
        }

        strategy = new CreditCardPayment(card.toStdString(), holder.toStdString());
        paymentDetails = QString("Card: ****%1").arg(card.right(4));

    } else if (method == "Cash") {
        double cash = QInputDialog::getDouble(this, "Cash",
                                              "Cash Amount: $", 0, 0, 99999, 2, &ok);
        if (!ok) return;

        if (cash < cart->getTotalAmount()) {
            QMessageBox::critical(this, "Payment Declined",
                                  QString("❌ Insufficient Cash!\n"
                                          "Amount due: $%1\n"
                                          "Cash given: $%2\n"
                                          "Shortage: $%3")
                                      .arg(cart->getTotalAmount())
                                      .arg(cash)
                                      .arg(cart->getTotalAmount() - cash));
            return;
        }

        strategy = new CashPayment(cash);
        paymentDetails = QString("Cash: $%1\nChange: $%2")
                             .arg(cash)
                             .arg(cash - cart->getTotalAmount());

    } else if (method == "Digital Wallet") {
        QString walletId = QInputDialog::getText(this, "Wallet",
                                                 "Wallet ID (must start with WALLET_):",
                                                 QLineEdit::Normal, "", &ok);
        if (!ok) return;

        double balance = QInputDialog::getDouble(this, "Wallet",
                                                 "Current Balance: $", 0, 0, 99999, 2, &ok);
        if (!ok) return;

        // التحقق من صحة المحفظة
        if (!validateWallet(walletId, balance, cart->getTotalAmount())) {
            return;
        }

        strategy = new WalletPayment(walletId.toStdString(), balance);
        paymentDetails = QString("Wallet ID: %1\nBalance: $%2\nNew Balance: $%3")
                             .arg(walletId)
                             .arg(balance)
                             .arg(balance - cart->getTotalAmount());
    }

    // Process Payment
    Payment payment(1, "Order#1", cart->getTotalAmount(), strategy);
    payment.processPayment();

    // Discount
    int hasCoupon = QMessageBox::question(this, "Discount",
                                          "Do you have a coupon?",
                                          QMessageBox::Yes | QMessageBox::No);

    double finalPrice = cart->getTotalAmount();
    double discountValue = 0;

    if (hasCoupon == QMessageBox::Yes) {
        int code = QInputDialog::getInt(this, "Coupon",
                                        "Coupon Code:", 0, 0, 99999, 1, &ok);
        if (ok) {
            // التحقق من صحة الكوبون
            if (code == 12345) { // كوبون صحيح
                double percent = QInputDialog::getDouble(this, "Coupon",
                                                         "Discount %:", 0, 0, 100, 1, &ok);
                if (ok) {
                    Discount discount(code, percent);
                    double oldTotal = finalPrice;
                    finalPrice = discount.applyDiscount(cart->getTotalAmount());
                    discountValue = oldTotal - finalPrice;
                    QMessageBox::information(this, "Coupon Applied",
                                             QString("✅ Coupon applied!\n"
                                                     "You saved: $%1").arg(discountValue));
                }
            } else {
                QMessageBox::warning(this, "Invalid Coupon",
                                     "⚠️ Coupon code not recognized!\n"
                                     "No discount applied.");
            }
        }
    }

    // Save
    FileManager::savePayment(payment);

    // بناء الـ Receipt
    QString receiptText = "🧾 PURCHASE RECEIPT 🧾\n\n";
    receiptText += "Store: Online Shop\n";
    receiptText += "Date: " + QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss") + "\n";
    receiptText += "Payment Method: " + method + "\n";
    receiptText += "Payment Details:\n" + paymentDetails + "\n";
    receiptText += "----------------------------------------\n\n";

    vector<CartItem> items = cart->getItems();
    for (auto& item : items) {
        receiptText += QString("%1 x%2")
        .arg(QString::fromStdString(item.getProductName()))
            .arg(item.getQuantity());
        receiptText += QString(" ................ $%1\n").arg(item.getSubtotal(), 0, 'f', 2);
    }

    receiptText += "\n----------------------------------------\n";
    receiptText += QString("Subtotal: $%1\n").arg(cart->getTotalAmount(), 0, 'f', 2);

    if (discountValue > 0) {
        receiptText += QString("Discount: -$%1\n").arg(discountValue, 0, 'f', 2);
    }

    receiptText += QString("Final Total: $%1\n").arg(finalPrice, 0, 'f', 2);
    receiptText += QString("Status: %1\n").arg(QString::fromStdString(payment.getStatus()));
    receiptText += "----------------------------------------\n";
    receiptText += "✅ Payment Successful!\n";
    receiptText += "Thank you for shopping with us! 🛒\n";

    // فتح نافذة الـ Receipt
    ReceiptWindow* receiptWin = new ReceiptWindow(receiptText, this);
    receiptWin->show();

    if (strategy) delete strategy;
    cart->clearCart();
    close();
}