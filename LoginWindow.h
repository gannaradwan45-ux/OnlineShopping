#ifndef LOGINWINDOW_H
#define LOGINWINDOW_H

#pragma once
#include <QMainWindow>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QStackedWidget>
#include <QCheckBox>
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

private:
    // Login widgets
    QWidget* loginWidget;
    QLineEdit* loginUsernameEdit;
    QLineEdit* loginPasswordEdit;
    QPushButton* loginBtn;
    QCheckBox* rememberMeCheck;
    QLabel* loginErrorLabel;

    // Register widgets
    QWidget* registerWidget;
    QLineEdit* regUsernameEdit;
    QLineEdit* regEmailEdit;
    QLineEdit* regPasswordEdit;
    QLineEdit* regConfirmEdit;
    QPushButton* registerBtn;
    QLabel* registerErrorLabel;
    QLabel* passwordStrengthLabel;

    QStackedWidget* stackedWidget;

    Inventory* inventory;
    ShoppingCart* cart;

    void setupLoginWidget();
    void setupRegisterWidget();
    bool validateEmail(QString email);
    QString getPasswordStrength(QString password);
    void checkPasswordStrength(QString password);
};

#endif