#include "AuthManager.h"
#include <fstream>
#include <sstream>
#include <iostream>
using namespace std;

vector<User*> AuthManager::users;
User* AuthManager::currentUser = nullptr;
int AuthManager::nextID = 1;
static string filename = "users_data.txt";

void AuthManager::loadUsers() {
    for (User* u : users) {
        delete u;
    }
    users.clear();

    ifstream file(filename);
    if (!file.is_open()) return;

    string line;
    while (getline(file, line)) {
        stringstream ss(line);
        string idStr, username, password, email, role;

        getline(ss, idStr, ',');
        getline(ss, username, ',');
        getline(ss, password, ',');
        getline(ss, email, ',');
        getline(ss, role, ',');

        int id = stoi(idStr);

        if (role == "Admin") {
            users.push_back(new Admin(id, username, password, email));
        } else if (role == "Vendor") {
            users.push_back(new Vendor(id, username, password, email));
        } else {
            users.push_back(new Customer(id, username, password, email));
        }

        if (id >= nextID) nextID = id + 1;
    }
    file.close();
}

void AuthManager::saveUsers() {
    ofstream file(filename);
    for (User* user : users) {
        file << user->getUserID() << ","
             << user->getUsername() << ","
             << user->getPassword() << ","
             << user->getEmail() << ","
             << user->getRole() << "\n";
    }
    file.close();
}

bool AuthManager::registerUser(string username, string email, string password) {
    loadUsers();

    for (User* user : users) {
        if (user->getUsername() == username || user->getEmail() == email) {
            return false;
        }
    }

    Customer* newUser = new Customer(nextID++, username, password, email);
    users.push_back(newUser);
    saveUsers();
    return true;
}

bool AuthManager::loginUser(string usernameOrEmail, string password) {
    loadUsers();

    for (User* user : users) {
        if ((user->getUsername() == usernameOrEmail || user->getEmail() == usernameOrEmail)
            && user->checkPassword(password)) {
            currentUser = user;
            currentUser->login();
            return true;
        }
    }
    return false;
}

User* AuthManager::getCurrentUser() {
    return currentUser;
}

void AuthManager::logout() {
    if (currentUser) {
        currentUser->logout();
        currentUser = nullptr;
    }
}

bool AuthManager::isLoggedIn() {
    return currentUser != nullptr;
}

bool AuthManager::userExists(string usernameOrEmail) {
    loadUsers();
    for (User* user : users) {
        if (user->getUsername() == usernameOrEmail || user->getEmail() == usernameOrEmail) {
            return true;
        }
    }
    return false;
}

void AuthManager::saveRememberMe(bool remember) {
    ofstream file("remember.txt");
    if (remember && currentUser != nullptr) {
        file << currentUser->getUsername() << "\n";
    } else {
        file << "false\n";
    }
    file.close();
}

bool AuthManager::getRememberMe() {
    ifstream file("remember.txt");
    string line;
    if (getline(file, line) && line != "false" && !line.empty()) {
        return true;
    }
    return false;
}

void AuthManager::clearUsers() {
    for (User* u : users) {
        delete u;
    }
    users.clear();
    currentUser = nullptr;
    nextID = 1;
}