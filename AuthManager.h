#pragma once
#include <vector>
#include "User.h"
using namespace std;

class AuthManager {
private:
    static vector<User*> users;
    static User* currentUser;
    static int nextID;
    static void loadUsers();
    static void saveUsers();
public:
    static bool registerUser(string username, string email, string password);
    static bool registerUser(string username, string email, string password, string role);
    static bool loginUser(string usernameOrEmail, string password);
    static User* getCurrentUser();
    static void logout();
    static bool isLoggedIn();
    static bool userExists(string usernameOrEmail);
    static void saveRememberMe(bool remember);
    static bool getRememberMe();
    static void clearUsers();
};
