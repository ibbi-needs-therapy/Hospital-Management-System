#include "Patient.h"
#include <iostream>
using namespace std;

Patient::Patient()
    : Staff(0, "", "", ""), balance(0.0), gender('U'), age(0) {
}

Patient::Patient(int id, const string& name, const string& contact, const string& pwd, double balance, char gender, int age): Staff(id, name, pwd, contact), balance(balance), gender(gender), age(age) {}
void Patient::displayInfo() const {
    cout << "Patient " << id << ": " << name
        << " | Age: " << age
        << " | Gender: " << (gender == 'M' || gender == 'm' ? "Male" : "Female")
        << " | Contact: " << contact
        << " | Balance: PKR " << balance << "\n";
}

const char* Patient::getRole() const {
    return "Patient";
}

Patient& Patient::operator+=(double amount) {
    balance += amount;
    return *this;
}

Patient& Patient::operator-=(double amount) {
    balance -= amount;
    return *this;
}

bool Patient::operator==(const Patient& rhs) const {
    return this->id == rhs.id;
}

ostream& operator<<(ostream& os, const Patient& p) {
    os << p.id << " | "
        << p.name << " | "
        << p.age << " | "
        << p.gender << " | "
        << p.contact << " | PKR "
        << p.balance;
    return os;
}