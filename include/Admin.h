#pragma once
#include "Person.h"
using namespace std;

class Admin : public Person {
public:
    Admin();
    Admin(int id, const string& name, const string& pwd);

    void displayInfo() const override;
    const char* getRole() const override;
};