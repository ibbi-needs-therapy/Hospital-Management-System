#include "Appointment.h"
using namespace std;

Appointment::Appointment(): appointmentID(0), patientID(0), doctorID(0), date(""), timeSlot(""), status("pending") {}

Appointment::Appointment(int appointmentID, int patientID, int doctorID, const string& date, const string& timeSlot, const string& status): appointmentID(appointmentID), patientID(patientID), doctorID(doctorID), date(date), timeSlot(timeSlot), status(status) {
}

bool Appointment::operator==(const Appointment& rhs) const {
    if (this->status == "cancelled" || rhs.status == "cancelled")
        return false;
    return (this->doctorID == rhs.doctorID &&
        this->date == rhs.date &&
        this->timeSlot == rhs.timeSlot);
}

ostream& operator<<(ostream& os, const Appointment& a) {
    os << a.appointmentID << " | "
        << "Patient ID: " << a.patientID << " | "
        << "Doctor ID: " << a.doctorID << " | "
        << a.date << " | "
        << a.timeSlot << " | "
        << a.status;
    return os;
}