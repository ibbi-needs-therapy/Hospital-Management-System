#include "Staff.h"
using namespace std;

Staff::Staff() : Person(), contact("") {}
Staff::Staff(int id, const string& name, const string& pwd, const string& contact): Person(id, name, pwd), contact(contact) {}