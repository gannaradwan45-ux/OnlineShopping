#pragma once
#include <QMainWindow>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QStackedWidget>
#include <QCheckBox>
#include <QComboBox>
#include "User.h"
#include "Inventory.h"
#include "ShoppingCart.h"
#include "AuthManager.h"

class StoreWindow;

class LoginWindow : public QMainWindow {
    Q_OBJECT
public:
    LoginWindow(QWidget* parent = nullptr);
    ~LoginWindow();

private slots:
    void onLoginClicked();
    void onRegisterClicked();
    void switchToRegister();
    void switchToLogin();
    void onForgotPassword();
    void checkPasswordStrength(QString password);

private:
    QWidget* loginWidget;
    QLineEdit* loginUsernameEdit;
    QLineEdit* loginPasswordEdit;
    QPushButton* loginBtn;
    QCheckBox* rememberMeCheck;
    QLabel* loginErrorLabel;

    QWidget* registerWidget;
    QLineEdit* regUsernameEdit;
    QLineEdit* regEmailEdit;
    QLineEdit* regPasswordEdit;
    QLineEdit* regConfirmEdit;
    QPushButton* registerBtn;
    QLabel* registerErrorLabel;
    QLabel* passwordStrengthLabel;
    QComboBox* roleCombo;

    QStackedWidget* stackedWidget;
    Inventory* inventory;
    ShoppingCart* cart;

    void setupLoginWidget();
    void setupRegisterWidget();
    bool validateEmail(QString email);
    QString getPasswordStrength(QString password);
};
