#include "notification.h"
#include <fstream>
#include <iostream>
using namespace std;

// Notification
Notification::Notification(int id, const string& msg)
    : userID(id), message(msg) {
}

void Notification::sendNotification() const {
    cout << "[SYSTEM ALERT] User " << userID << ": " << message << endl;
}

// Report
Report::Report(const string& title, const string& date)
    : title(title), date(date) {
}

void Report::addEntry(const string& entry) {
    entries.push_back(entry);
}

void Report::generateSalesReport() const {
    cout << "--- " << title << " ---" << endl;
    cout << "Date: " << date << endl;
    if (entries.empty()) {
        cout << "(No entries found)" << endl;
    }
    else {
        for (const auto& entry : entries)
            cout << "  - " << entry << endl;
    }
}

void Report::saveToFile(const string& filename) const {
    ofstream file(filename);
    if (file.is_open()) {
        file << "--- " << title << " ---" << endl;
        file << "Date: " << date << endl;
        for (const auto& entry : entries)
            file << "  - " << entry << endl;
        file.close();
        cout << "Report saved to " << filename << endl;
    }
    else {
        cerr << "Error: Could not open file" << endl;
    }
}

void Report::loadFromFile(const string& filename) {
    ifstream file(filename);
    if (file.is_open()) {
        entries.clear();
        string line;
        while (getline(file, line)) {
            cout << line << endl;
            if (line.rfind("  - ", 0) == 0)
                entries.push_back(line.substr(4));
        }
        file.close();
    }
    else {
        cerr << "Error: Could not open file" << endl;
    }
}