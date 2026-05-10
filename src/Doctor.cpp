#include "Doctor.h"
#include <iostream>
using namespace std;

Doctor::Doctor() : Staff(), specialization(""), fee(0.0f) {}
Doctor::Doctor(int id, const string& name, const string& contact, const string& pwd, const string& spec, float fee) : Staff(id, name, pwd, contact), specialization(spec), fee(fee) {}

void Doctor::displayInfo() const {
    cout << "Dr. " << name
        << " | Specialization: " << specialization
        << " | Fee: PKR " << fee
        << " | Contact: " << contact << "\n";
}

const char* Doctor::getRole() const {
    return "Doctor";
}

bool Doctor::operator==(const Doctor& rhs) const {
    return this->id == rhs.id;
}

ostream& operator<<(ostream& os, const Doctor& d) {
    os << d.id << " | "
        << d.name << " | "
        << d.specialization << " | "
        << d.contact << " | PKR "
        << d.fee;
    return os;
}