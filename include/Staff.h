#pragma once
#include "Person.h"
#include <string>
using namespace std;

class Staff : public Person {
protected:
    string contact;

public:
    Staff();
    Staff(int id, const string& name, const string& pwd, const string& contact);
    virtual ~Staff() {}

    const string& getContact() const { return contact; }
};