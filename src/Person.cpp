#include "Person.h"
using namespace std;

Person::Person() : id(0), name(""), password("") {}
Person::Person(int id, const string& name, const string& pwd): id(id), name(name), password(pwd) {}