#pragma once
#include "Staff.h"
#include <iostream>

using namespace std;

class Doctor : public Staff {
private:
    string specialization;
    float fee;

public:
    Doctor();
    Doctor(int id, const string& name, const string& contact, const string& pwd, const string& spec, float fee);
    ~Doctor() {}

    bool operator==(const Doctor& rhs) const;
    friend ostream& operator<<(ostream& os, const Doctor& d);

    void displayInfo() const override;
    const char* getRole() const override;

    const string& getSpecialization() const { return specialization; }
    float getFee() const { return fee; }
};