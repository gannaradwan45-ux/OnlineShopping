#pragma once
#include <string>
#include <vector>
#include <fstream>
using namespace std;

class Notification {
private:
    int userID;
    string message;
public:
    Notification(int id, const string& msg);
    void sendNotification() const;
};

class Report {
private:
    string title;
    string date;
    vector<string> entries;
public:
    Report(const string& title, const string& date);
    void addEntry(const string& entry);
    void generateSalesReport() const;
    void saveToFile(const string& filename) const;
    void loadFromFile(const string& filename);
};

