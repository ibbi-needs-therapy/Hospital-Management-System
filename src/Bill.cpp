#include "Bill.h"
using namespace std;

Bill::Bill()
    : billID(0), patientID(0), appointmentID(0),
    amount(0.0), status("unpaid"), date("") {
}

Bill::Bill(int billID, int patientID, int appointmentID, double amount, const string& status, const string& date): billID(billID), patientID(patientID), appointmentID(appointmentID), amount(amount), status(status), date(date) {}

ostream& operator<<(ostream& os, const Bill& b) {
    os << b.billID << " | "
        << "Patient ID: " << b.patientID << " | "
        << "Appt ID: " << b.appointmentID << " | "
        << "PKR " << b.amount << " | "
        << b.status << " | "
        << b.date;
    return os;
}