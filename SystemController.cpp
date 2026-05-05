#include "SystemController.h"
using namespace std;

// تهيئة الـ instance بـ nullptr
SystemController* SystemController::instance = nullptr;

SystemController::SystemController() {
    isRunning = false;
}

SystemController* SystemController::getInstance() {
    if (instance == nullptr) {
        instance = new SystemController();
    }
    return instance;
}

void SystemController::startSystem() {
    isRunning = true;
    cout << "================================" << endl;
    cout << "  Online Shopping System Started" << endl;
    cout << "================================" << endl;
}

void SystemController::shutdownSystem() {
    isRunning = false;
    cout << "================================" << endl;
    cout << "  System Shutdown Successfully  " << endl;
    cout << "================================" << endl;

    // حذف الـ instance
    delete instance;
    instance = nullptr;
}

bool SystemController::getIsRunning() const {
    return isRunning;
}

SystemController::~SystemController() {
    isRunning = false;
}