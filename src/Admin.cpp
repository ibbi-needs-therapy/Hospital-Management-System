#include "Admin.h"
#include <iostream>

using namespace std;

Admin::Admin() : Person() {}
Admin::Admin(int id, const string& name, const string& pwd): Person(id, name, pwd) {}

void Admin::displayInfo() const {
    cout << "ID: " << id << "\n"
         << "Name: " << name << "\n"
         << "Role: Admin\n";
}

const char* Admin::getRole() const {
    return "Admin";
}