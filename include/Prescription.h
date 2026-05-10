#pragma once
#include <iostream>
#include <string>
using namespace std;

class Prescription
{
private:
	int prescriptionID;
	int apptID;
	int patientID;
	int docID;
	string date;
	string medicine;
	string notes;
public:
	Prescription() : prescriptionID(-1), apptID(-1), patientID(-1),
		docID(-1), date("-"), medicine("-"), notes("-") {}

	Prescription(int prID, int aID, int paID, int docID, const string& date, const string& med, const string& notes = "-") :
	prescriptionID(prID), apptID(aID), patientID(paID), docID(docID), 
	date(date), medicine(med),notes(notes) {}

	int getID() const { return prescriptionID; }
	int getAppointmentID() const { return apptID; }
	int getPatientID() const { return patientID; }
	int getDoctorID() const { return docID; }

	const string& getDate()      const { return date; }
    const string& getMedicines() const { return medicine; }
    const string& getNotes()     const { return notes; }

	friend ostream& operator<<(ostream& os, const Prescription& rhs);
};

