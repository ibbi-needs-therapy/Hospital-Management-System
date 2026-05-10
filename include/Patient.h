#pragma once
#include "Staff.h"
#include <iostream>
using namespace std;

class Patient : public Staff {
private:
    double balance;
    char gender;
    int age;

public:
    Patient();
    Patient(int id, const string& name, const string& contact, const string& pwd, double balance, char gender, int age);
    ~Patient() {};

    void displayInfo() const override;
    const char* getRole() const override;

    int getAge() const { return age; }
    double getBalance() const { return balance; }
    char getGender() const { return gender; }

    Patient& operator+=(double amount);
    Patient& operator-=(double amount);
    bool operator==(const Patient& rhs) const;
    friend ostream& operator<<(ostream& os, const Patient& p);
};