#include "Prescription.h"
using namespace std;

ostream& operator<<(ostream& os, const Prescription& rhs) {
    os << "Prescription ID: " << rhs.prescriptionID << endl
        << " Issued During Appointment ID :" << rhs.apptID << endl
        << " Issued To:" << rhs.patientID << endl 
        << " Issued By:" << rhs.docID << endl
        << " Issued on: " << rhs.date << endl 
        <<" Details:\n " << rhs.medicine << " | " << rhs.notes <<endl;
    return os;
}