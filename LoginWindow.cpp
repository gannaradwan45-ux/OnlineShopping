#include "LoginWindow.h"
#include "StoreWindow.h"
#include "AuthManager.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QInputDialog>
#include <QFile>
#include "FileManager.h"
#include "ProductFactory.h"

LoginWindow::LoginWindow(QWidget* parent) : QMainWindow(parent) {
    setWindowTitle("🔐 Online Shop - Login");
    setFixedSize(480, 650);
    setStyleSheet("QMainWindow { background-color: #1a1a2e; }");

    // Initialize inventory and cart
    inventory = new Inventory(10);
    inventory->addProduct(ProductFactory::createProduct("Electronics", 1, "iPhone 15", 999.99, 10));
    inventory->addProduct(ProductFactory::createProduct("Electronics", 2, "Samsung TV", 599.99, 5));
    inventory->addProduct(ProductFactory::createProduct("Electronics", 3, "Laptop Dell", 1200.00, 8));
    inventory->addProduct(ProductFactory::createProduct("Grocery", 4, "Rice 5kg", 15.00, 50));
    inventory->addProduct(ProductFactory::createProduct("Grocery", 5, "Milk 1L", 5.00, 30));
    inventory->addProduct(ProductFactory::createProduct("Grocery", 6, "Bread", 3.00, 20));
    inventory->addProduct(ProductFactory::createProduct("Books", 7, "C++ Programming", 45.00, 15));
    inventory->addProduct(ProductFactory::createProduct("Books", 8, "Data Structures", 55.00, 12));
    inventory->addProduct(ProductFactory::createProduct("Appliances", 9, "Washing Machine", 450.00, 4));
    inventory->addProduct(ProductFactory::createProduct("Appliances", 10, "Refrigerator", 700.00, 3));

    cart = new ShoppingCart(1, 0);

    QWidget* central = new QWidget(this);
    setCentralWidget(central);
    QVBoxLayout* mainLayout = new QVBoxLayout(central);
    mainLayout->setContentsMargins(30, 40, 30, 40);
    mainLayout->setSpacing(20);

    // Logo/Title
    QLabel* title = new QLabel("🛒 Online Shop");
    title->setAlignment(Qt::AlignCenter);
    title->setStyleSheet("color: #e94560; font-size: 28px; font-weight: bold;");
    mainLayout->addWidget(title);

    // Stacked widget for login/register
    stackedWidget = new QStackedWidget();
    setupLoginWidget();
    setupRegisterWidget();
    stackedWidget->addWidget(loginWidget);
    stackedWidget->addWidget(registerWidget);
    mainLayout->addWidget(stackedWidget);

    // Switch button at bottom
    QPushButton* switchBtn = new QPushButton("Create new account →");
    switchBtn->setStyleSheet(
        "QPushButton {"
        "  background-color: transparent;"
        "  color: #e94560;"
        "  font-size: 13px;"
        "  border: none;"
        "}"
        "QPushButton:hover {"
        "  color: #c73652;"
        "  text-decoration: underline;"
        "}"
        );
    connect(switchBtn, &QPushButton::clicked, this, &LoginWindow::switchToRegister);

    QHBoxLayout* switchLayout = new QHBoxLayout();
    switchLayout->addStretch();
    switchLayout->addWidget(switchBtn);
    switchLayout->addStretch();
    mainLayout->addLayout(switchLayout);
}

LoginWindow::~LoginWindow() {
    delete inventory;
    delete cart;
}

void LoginWindow::setupLoginWidget() {
    loginWidget = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(loginWidget);
    layout->setSpacing(15);

    QLabel* subtitle = new QLabel("Sign In to Your Account");
    subtitle->setAlignment(Qt::AlignCenter);
    subtitle->setStyleSheet("color: #a8a8b3; font-size: 14px; margin-bottom: 20px;");
    layout->addWidget(subtitle);

    // Username/Email
    QLabel* userLabel = new QLabel("Username or Email");
    userLabel->setStyleSheet("color: #ffffff; font-size: 13px; font-weight: bold;");
    layout->addWidget(userLabel);

    loginUsernameEdit = new QLineEdit();
    loginUsernameEdit->setPlaceholderText("Enter your username or email");
    loginUsernameEdit->setStyleSheet(
        "QLineEdit {"
        "  background-color: #16213e;"
        "  color: white;"
        "  border: 2px solid #0f3460;"
        "  border-radius: 10px;"
        "  padding: 12px;"
        "  font-size: 14px;"
        "}"
        "QLineEdit:focus {"
        "  border: 2px solid #e94560;"
        "}"
        );
    loginUsernameEdit->setFixedHeight(45);
    layout->addWidget(loginUsernameEdit);

    // Password
    QLabel* passLabel = new QLabel("Password");
    passLabel->setStyleSheet("color: #ffffff; font-size: 13px; font-weight: bold;");
    layout->addWidget(passLabel);

    QHBoxLayout* passLayout = new QHBoxLayout();
    loginPasswordEdit = new QLineEdit();
    loginPasswordEdit->setPlaceholderText("Enter your password");
    loginPasswordEdit->setEchoMode(QLineEdit::Password);
    loginPasswordEdit->setStyleSheet(
        "QLineEdit {"
        "  background-color: #16213e;"
        "  color: white;"
        "  border: 2px solid #0f3460;"
        "  border-radius: 10px;"
        "  padding: 12px;"
        "  font-size: 14px;"
        "}"
        "QLineEdit:focus {"
        "  border: 2px solid #e94560;"
        "}"
        );
    loginPasswordEdit->setFixedHeight(45);
    passLayout->addWidget(loginPasswordEdit);

    QPushButton* showPassBtn = new QPushButton("👁");
    showPassBtn->setFixedSize(45, 45);
    showPassBtn->setStyleSheet(
        "QPushButton {"
        "  background-color: #16213e;"
        "  color: #a8a8b3;"
        "  border: 2px solid #0f3460;"
        "  border-radius: 10px;"
        "  font-size: 16px;"
        "}"
        "QPushButton:hover {"
        "  background-color: #e94560;"
        "  color: white;"
        "}"
        );
    connect(showPassBtn, &QPushButton::clicked, [this]() {
        if (loginPasswordEdit->echoMode() == QLineEdit::Password) {
            loginPasswordEdit->setEchoMode(QLineEdit::Normal);
        } else {
            loginPasswordEdit->setEchoMode(QLineEdit::Password);
        }
    });
    passLayout->addWidget(showPassBtn);
    layout->addLayout(passLayout);

    // Remember Me
    rememberMeCheck = new QCheckBox("Remember Me");
    rememberMeCheck->setStyleSheet("color: #a8a8b3;");
    layout->addWidget(rememberMeCheck);

    // Forgot Password
    QPushButton* forgotBtn = new QPushButton("Forgot Password?");
    forgotBtn->setStyleSheet(
        "QPushButton {"
        "  background-color: transparent;"
        "  color: #e94560;"
        "  font-size: 12px;"
        "  border: none;"
        "  text-align: left;"
        "}"
        "QPushButton:hover {"
        "  color: #c73652;"
        "}"
        );
    connect(forgotBtn, &QPushButton::clicked, this, &LoginWindow::onForgotPassword);
    layout->addWidget(forgotBtn);

    // Error Label
    loginErrorLabel = new QLabel();
    loginErrorLabel->setStyleSheet("color: #ff6b6b; font-size: 12px;");
    loginErrorLabel->setVisible(false);
    layout->addWidget(loginErrorLabel);

    layout->addSpacing(10);

    // Login Button
    loginBtn = new QPushButton("LOGIN");
    loginBtn->setFixedHeight(50);
    loginBtn->setStyleSheet(
        "QPushButton {"
        "  background-color: #e94560;"
        "  color: white;"
        "  border-radius: 10px;"
        "  font-size: 16px;"
        "  font-weight: bold;"
        "}"
        "QPushButton:hover {"
        "  background-color: #c73652;"
        "}"
        );
    connect(loginBtn, &QPushButton::clicked, this, &LoginWindow::onLoginClicked);
    layout->addWidget(loginBtn);
}

void LoginWindow::setupRegisterWidget() {
    registerWidget = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(registerWidget);
    layout->setSpacing(15);

    QLabel* subtitle = new QLabel("Create New Account");
    subtitle->setAlignment(Qt::AlignCenter);
    subtitle->setStyleSheet("color: #a8a8b3; font-size: 14px; margin-bottom: 20px;");
    layout->addWidget(subtitle);

    // Username
    QLabel* userLabel = new QLabel("Username");
    userLabel->setStyleSheet("color: #ffffff; font-size: 13px; font-weight: bold;");
    layout->addWidget(userLabel);

    regUsernameEdit = new QLineEdit();
    regUsernameEdit->setPlaceholderText("Choose a username");
    regUsernameEdit->setStyleSheet(
        "QLineEdit {"
        "  background-color: #16213e;"
        "  color: white;"
        "  border: 2px solid #0f3460;"
        "  border-radius: 10px;"
        "  padding: 12px;"
        "  font-size: 14px;"
        "}"
        "QLineEdit:focus {"
        "  border: 2px solid #e94560;"
        "}"
        );
    regUsernameEdit->setFixedHeight(45);
    layout->addWidget(regUsernameEdit);

    // Email
    QLabel* emailLabel = new QLabel("Email");
    emailLabel->setStyleSheet("color: #ffffff; font-size: 13px; font-weight: bold;");
    layout->addWidget(emailLabel);

    regEmailEdit = new QLineEdit();
    regEmailEdit->setPlaceholderText("your@email.com");
    regEmailEdit->setStyleSheet(
        "QLineEdit {"
        "  background-color: #16213e;"
        "  color: white;"
        "  border: 2px solid #0f3460;"
        "  border-radius: 10px;"
        "  padding: 12px;"
        "  font-size: 14px;"
        "}"
        "QLineEdit:focus {"
        "  border: 2px solid #e94560;"
        "}"
        );
    regEmailEdit->setFixedHeight(45);
    layout->addWidget(regEmailEdit);

    // Password
    QLabel* passLabel = new QLabel("Password");
    passLabel->setStyleSheet("color: #ffffff; font-size: 13px; font-weight: bold;");
    layout->addWidget(passLabel);

    regPasswordEdit = new QLineEdit();
    regPasswordEdit->setPlaceholderText("Create a strong password (min 6 chars)");
    regPasswordEdit->setEchoMode(QLineEdit::Password);
    regPasswordEdit->setStyleSheet(
        "QLineEdit {"
        "  background-color: #16213e;"
        "  color: white;"
        "  border: 2px solid #0f3460;"
        "  border-radius: 10px;"
        "  padding: 12px;"
        "  font-size: 14px;"
        "}"
        "QLineEdit:focus {"
        "  border: 2px solid #e94560;"
        "}"
        );
    regPasswordEdit->setFixedHeight(45);
    connect(regPasswordEdit, &QLineEdit::textChanged, this, &LoginWindow::checkPasswordStrength);
    layout->addWidget(regPasswordEdit);

    // Password strength
    passwordStrengthLabel = new QLabel();
    passwordStrengthLabel->setStyleSheet("font-size: 11px; margin-top: -5px;");
    layout->addWidget(passwordStrengthLabel);

    // Confirm Password
    QLabel* confirmLabel = new QLabel("Confirm Password");
    confirmLabel->setStyleSheet("color: #ffffff; font-size: 13px; font-weight: bold;");
    layout->addWidget(confirmLabel);

    regConfirmEdit = new QLineEdit();
    regConfirmEdit->setPlaceholderText("Confirm your password");
    regConfirmEdit->setEchoMode(QLineEdit::Password);
    regConfirmEdit->setStyleSheet(
        "QLineEdit {"
        "  background-color: #16213e;"
        "  color: white;"
        "  border: 2px solid #0f3460;"
        "  border-radius: 10px;"
        "  padding: 12px;"
        "  font-size: 14px;"
        "}"
        "QLineEdit:focus {"
        "  border: 2px solid #e94560;"
        "}"
        );
    regConfirmEdit->setFixedHeight(45);
    layout->addWidget(regConfirmEdit);

    // Error Label
    registerErrorLabel = new QLabel();
    registerErrorLabel->setStyleSheet("color: #ff6b6b; font-size: 12px;");
    registerErrorLabel->setVisible(false);
    layout->addWidget(registerErrorLabel);

    layout->addSpacing(10);

    // Register Button
    registerBtn = new QPushButton("SIGN UP");
    registerBtn->setFixedHeight(50);
    registerBtn->setStyleSheet(
        "QPushButton {"
        "  background-color: #e94560;"
        "  color: white;"
        "  border-radius: 10px;"
        "  font-size: 16px;"
        "  font-weight: bold;"
        "}"
        "QPushButton:hover {"
        "  background-color: #c73652;"
        "}"
        );
    connect(registerBtn, &QPushButton::clicked, this, &LoginWindow::onRegisterClicked);
    layout->addWidget(registerBtn);

    // Back to login
    QPushButton* backBtn = new QPushButton("← Back to Sign In");
    backBtn->setStyleSheet(
        "QPushButton {"
        "  background-color: transparent;"
        "  color: #e94560;"
        "  font-size: 13px;"
        "  border: none;"
        "  margin-top: 10px;"
        "}"
        "QPushButton:hover {"
        "  color: #c73652;"
        "}"
        );
    connect(backBtn, &QPushButton::clicked, this, &LoginWindow::switchToLogin);
    layout->addWidget(backBtn);
}

void LoginWindow::switchToRegister() {
    stackedWidget->setCurrentIndex(1);
    registerErrorLabel->setVisible(false);
    regUsernameEdit->clear();
    regEmailEdit->clear();
    regPasswordEdit->clear();
    regConfirmEdit->clear();
    passwordStrengthLabel->setText("");
}

void LoginWindow::switchToLogin() {
    stackedWidget->setCurrentIndex(0);
    loginErrorLabel->setVisible(false);
    loginUsernameEdit->clear();
    loginPasswordEdit->clear();
}

void LoginWindow::onLoginClicked() {
    QString usernameOrEmail = loginUsernameEdit->text().trimmed();
    QString password = loginPasswordEdit->text();

    if (usernameOrEmail.isEmpty() || password.isEmpty()) {
        loginErrorLabel->setText("❌ Please fill in all fields");
        loginErrorLabel->setVisible(true);
        return;
    }

    if (AuthManager::loginUser(usernameOrEmail.toStdString(), password.toStdString())) {
        AuthManager::saveRememberMe(rememberMeCheck->isChecked());
        string username = AuthManager::getCurrentUser()->getUsername();

        StoreWindow* store = new StoreWindow(inventory, cart, QString::fromStdString(username), this);
        store->show();
        this->hide();
    } else {
        loginErrorLabel->setText("❌ Invalid username/email or password");
        loginErrorLabel->setVisible(true);
    }
}

void LoginWindow::onRegisterClicked() {
    QString username = regUsernameEdit->text().trimmed();
    QString email = regEmailEdit->text().trimmed();
    QString password = regPasswordEdit->text();
    QString confirm = regConfirmEdit->text();

    if (username.isEmpty() || email.isEmpty() || password.isEmpty() || confirm.isEmpty()) {
        registerErrorLabel->setText("❌ Please fill in all fields");
        registerErrorLabel->setVisible(true);
        return;
    }

    if (password != confirm) {
        registerErrorLabel->setText("❌ Passwords do not match");
        registerErrorLabel->setVisible(true);
        return;
    }

    if (!validateEmail(email)) {
        registerErrorLabel->setText("❌ Invalid email format (must contain @ and .)");
        registerErrorLabel->setVisible(true);
        return;
    }

    if (password.length() < 6) {
        registerErrorLabel->setText("❌ Password must be at least 6 characters");
        registerErrorLabel->setVisible(true);
        return;
    }

    if (AuthManager::registerUser(username.toStdString(), email.toStdString(), password.toStdString())) {
        QMessageBox::information(this, "Success", "✅ Account created successfully!\n\nPlease sign in.");
        switchToLogin();
        loginUsernameEdit->setText(email);
    } else {
        registerErrorLabel->setText("❌ Username or email already exists");
        registerErrorLabel->setVisible(true);
    }
}

void LoginWindow::onForgotPassword() {
    QString email = QInputDialog::getText(this, "Reset Password",
                                          "Enter your email address:", QLineEdit::Normal, "", nullptr);

    if (!email.isEmpty()) {
        if (AuthManager::userExists(email.toStdString())) {
            QMessageBox::information(this, "Reset Password",
                                     "📧 A password reset link has been sent to:\n" + email +
                                         "\n\n(Simulated - Check your email in a real app)");
        } else {
            QMessageBox::warning(this, "Error", "No account found with this email!");
        }
    }
}

bool LoginWindow::validateEmail(QString email) {
    return email.contains('@') && email.contains('.') && email.length() > 5;
}

void LoginWindow::checkPasswordStrength(QString password) {
    QString strength = getPasswordStrength(password);
    QString color;

    if (strength == "Weak") color = "#ff6b6b";
    else if (strength == "Medium") color = "#ffd93d";
    else if (strength == "Strong") color = "#6bcb77";
    else color = "#a8a8b3";

    passwordStrengthLabel->setText("🔐 " + strength);
    passwordStrengthLabel->setStyleSheet(QString("color: %1; font-size: 11px;").arg(color));
}

QString LoginWindow::getPasswordStrength(QString password) {
    int strength = 0;

    if (password.length() >= 6) strength++;
    if (password.length() >= 10) strength++;

    bool hasUpper = false, hasLower = false, hasDigit = false, hasSpecial = false;
    for (QChar c : password) {
        if (c.isUpper()) hasUpper = true;
        else if (c.isLower()) hasLower = true;
        else if (c.isDigit()) hasDigit = true;
        else if (!c.isLetterOrNumber()) hasSpecial = true;
    }

    if (hasUpper && hasLower) strength++;
    if (hasDigit) strength++;
    if (hasSpecial) strength++;

    if (strength >= 4) return "Strong";
    if (strength >= 2) return "Medium";
    return "Weak";
}