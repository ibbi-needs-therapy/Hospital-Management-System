#pragma once
#include <string>
using namespace std;

class Person {
protected:
    int id;
    string name;
    string password;

public:
    Person();
    Person(int id, const string& name, const string& pwd);
    virtual ~Person() {}

    virtual void displayInfo() const = 0;
    virtual const char* getRole() const = 0;

    int getID() const { return id; }
    const string& getName() const { return name; }
    const string& getPassword() const { return password; }
};