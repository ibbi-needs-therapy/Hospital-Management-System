#pragma once
#include <string>
#include <iostream>
using namespace std;

class Bill {
private:
    int billID;
    int patientID;
    int appointmentID;
    double amount;
    string status;
    string date;

public:
    Bill();
    Bill(int billID, int patientID, int appointmentID, double amount, const string& status, const string& date);
    friend ostream& operator<<(ostream& os, const Bill& b);

    int getID()         const { return billID; }
    int getPatientID()      const { return patientID; }
    int getAppointmentID()  const { return appointmentID; }
    double getAmount()      const { return amount; }
    const string& getStatus() const { return status; }
    const string& getDate()   const { return date; }

    void setStatus(const string& newStatus) { status = newStatus; }
};