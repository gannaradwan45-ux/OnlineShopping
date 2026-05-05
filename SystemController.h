#pragma once
#include <iostream>
#include <string>
#include "User.h"
#include "ShoppingCart.h"
#include "notification.h"
using namespace std;

// Singleton Pattern
class SystemController {
private:
    static SystemController* instance;
    bool isRunning;

    // private constructor
    SystemController();

public:
    // منع النسخ
    SystemController(const SystemController&) = delete;
    SystemController& operator=(const SystemController&) = delete;

    static SystemController* getInstance();

    void startSystem();
    void shutdownSystem();
    bool getIsRunning() const;

    ~SystemController();
};
