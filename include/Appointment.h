#pragma once
#include <string>
#include <iostream>
using namespace std;

class Appointment {
private:
    int appointmentID;
    int patientID;
    int doctorID;
    string date;
    string timeSlot;
    string status;

public:
    Appointment();
    Appointment(int appointmentID, int patientID, int doctorID,
        const string& date, const string& timeSlot,
        const string& status = "pending");

    bool operator==(const Appointment& rhs) const;
    friend ostream& operator<<(ostream& os, const Appointment& a);

    int getID() const { return appointmentID; }
    int getPatientID()     const { return patientID; }
    int getDoctorID()      const { return doctorID; }
    const string& getDate()     const { return date; }
    const string& getTimeSlot() const { return timeSlot; }
    const string& getStatus()   const { return status; }

    void setStatus(const string& newStatus) { status = newStatus; }
};