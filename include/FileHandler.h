#pragma once
#include <fstream>
#include <string>
#include "Storage.h"
#include "Patient.h"
#include "Doctor.h"
#include "Admin.h"
#include "Appointment.h"
#include "Bill.h"
#include "Prescription.h"
#include "HospitalExceptions.h"

using namespace std;

class FileHandler {
public:

    static void loadAdmin(Admin& admin) {
        ifstream f("data/admin.txt");
        if (!f.is_open()) throw FileNotFoundException("data/admin.txt");
        string line;
        if (getline(f, line)) {
            if (!line.empty()) {
                int id = stoi(extractField(line));
                string name = extractField(line);
                string pwd = line;
                admin = Admin(id, name, pwd);
            }
        }
        f.close();
    }

    static void loadPatients(Storage<Patient>& store) {
        ifstream f("data/patients.txt");
        if (!f.is_open()) throw FileNotFoundException("data/patients.txt");
        string line;
        while (getline(f, line)) {
            if (line.empty()) continue;
            int id = stoi(extractField(line));
            string name = extractField(line);
            int age = stoi(extractField(line));
            string gender = extractField(line);
            string contact = extractField(line);
            string pwd = extractField(line);
            double bal = stod(line);
            store.add(Patient(id, name, contact, pwd, bal, gender[0], age));
        }
        f.close();
    }

    static void loadDoctors(Storage<Doctor>& store) {
        ifstream f("data/doctors.txt");
        if (!f.is_open()) throw FileNotFoundException("data/doctors.txt");
        string line;
        while (getline(f, line)) {
            if (line.empty()) continue;
            int id = stoi(extractField(line));
            string name = extractField(line);
            string spec = extractField(line);
            string cont = extractField(line);
            string pwd = extractField(line);
            float fee = stof(line);
            store.add(Doctor(id, name, cont, pwd, spec, fee));
        }
        f.close();
    }

    static void loadAppointments(Storage<Appointment>& store) {
        ifstream f("data/appointments.txt");
        if (!f.is_open()) throw FileNotFoundException("data/appointments.txt");
        string line;
        while (getline(f, line)) {
            if (line.empty()) continue;
            int aID = stoi(extractField(line));
            int pID = stoi(extractField(line));
            int dID = stoi(extractField(line));
            string date = extractField(line);
            string slot = extractField(line);
            string status = line;
            store.add(Appointment(aID, pID, dID, date, slot, status));
        }
        f.close();
    }

    static void loadBills(Storage<Bill>& store) {
        ifstream f("data/bills.txt");
        if (!f.is_open()) throw FileNotFoundException("data/bills.txt");
        string line;
        while (getline(f, line)) {
            if (line.empty()) continue;
            int bID = stoi(extractField(line));
            int pID = stoi(extractField(line));
            int aID = stoi(extractField(line));
            double amt = stod(extractField(line));
            string status = extractField(line);
            string date = line;
            store.add(Bill(bID, pID, aID, amt, status, date));
        }
        f.close();
    }

    static void loadPrescriptions(Storage<Prescription>& store) {
        ifstream f("data/prescriptions.txt");
        if (!f.is_open()) throw FileNotFoundException("data/prescriptions.txt");
        string line;
        while (getline(f, line)) {
            if (line.empty()) continue;
            int prID = stoi(extractField(line));
            int aID = stoi(extractField(line));
            int pID = stoi(extractField(line));
            int dID = stoi(extractField(line));
            string date = extractField(line);
            string meds = extractField(line);
            string notes = line;
            store.add(Prescription(prID, aID, pID, dID, date, meds, notes));
        }
        f.close();
    }

    static void appendBill(const Bill& b) {
        ofstream f("data/bills.txt", ios::app);
        if (!f.is_open()) throw FileNotFoundException("data/bills.txt");
        f << b.getID() << ","
            << b.getPatientID() << ","
            << b.getAppointmentID() << ","
            << b.getAmount() << ","
            << b.getStatus() << ","
            << b.getDate() << "\n";
        f.close();
    }

    static void appendAppointment(const Appointment& a) {
        ofstream f("data/appointments.txt", ios::app);
        if (!f.is_open()) throw FileNotFoundException("data/appointments.txt");
        f << a.getID() << "," << a.getPatientID() << "," << a.getDoctorID() << ","
            << a.getDate() << "," << a.getTimeSlot() << "," << a.getStatus() << "\n";
        f.close();
    }
    
    static void appendPrescription(const Prescription& pr) {
        ofstream f("data/prescriptions.txt", ios::app);
        if (!f.is_open()) throw FileNotFoundException("data/prescriptions.txt");

        f << pr.getID() << ","
            << pr.getAppointmentID() << ","
            << pr.getPatientID() << ","
            << pr.getDoctorID() << ","
            << pr.getDate() << ","
            << pr.getMedicines() << ","
            << pr.getNotes() << "\n";

        f.close();
    }

    static void appendPatient(const Patient& pt) {
        ofstream f("data/patients.txt", ios::app);
        if (!f.is_open()) throw FileNotFoundException("data/patients.txt");
        f << pt.getID() << ","
            << pt.getName() << ","
            << pt.getAge() << ","
            << pt.getGender() << ","
            << pt.getContact() << ","
            << pt.getPassword() << ","
            << pt.getBalance() << "\n";
        f.close();
    }

    static void archivePatient(const Patient& pt,
        const Storage<Appointment>& appts,
        const Storage<Bill>& bills,
        const Storage<Prescription>& presc) {
        ofstream f("data/discharged.txt", ios::app);
        if (!f.is_open()) throw FileNotFoundException("data/discharged.txt");

        f << "--- DISCHARGED PATIENT: " << pt.getName() << " (ID: " << pt.getID() << ") ---\n";

        const Appointment* aa = appts.getAll();
        for (int i = 0; i < appts.getSize(); i++) {
            if (aa[i].getPatientID() == pt.getID()) {
                f << "[Appt] " << aa[i].getID() << "," << aa[i].getDate() << "," << aa[i].getStatus() << "\n";
            }
        }

        const Bill* bb = bills.getAll();
        for (int i = 0; i < bills.getSize(); i++) {
            if (bb[i].getPatientID() == pt.getID()) {
                f << "[Bill] " << bb[i].getID() << "," << bb[i].getAmount() << "," << bb[i].getStatus() << "\n";
            }
        }

        const Prescription* pp = presc.getAll();
        for (int i = 0; i < presc.getSize(); i++) {
            if (pp[i].getPatientID() == pt.getID()) {
                f << "[Presc] " << pp[i].getID() << "," << pp[i].getMedicines() << "\n";
            }
        }

        f << "------------------------------------------\n\n";
        f.close();
    }


    static void appendDoctor(const Doctor& d) {
        ofstream f("data/doctors.txt", ios::app);
        if (!f.is_open()) throw FileNotFoundException("data/doctors.txt");
        f << d.getID() << ","
            << d.getName() << ","
            << d.getSpecialization() << ","
            << d.getContact() << ","
            << d.getPassword() << ","
            << d.getFee() << "\n";
        f.close();
    }

    static void rewriteDoctors(const Storage<Doctor>& store) {
        ofstream f("data/doctors.txt");
        if (!f.is_open()) throw FileNotFoundException("data/doctors.txt");
        const Doctor* arr = store.getAll();
        for (int i = 0; i < store.getSize(); i++) {
            const Doctor& d = arr[i];
            f << d.getID() << ","
                << d.getName() << ","
                << d.getSpecialization() << ","
                << d.getContact() << ","
                << d.getPassword() << ","
                << d.getFee() << "\n";
        }
        f.close();
    }

    static void rewritePrescriptions(const Storage<Prescription>& store) {
        ofstream f("data/prescriptions.txt");
        if (!f.is_open()) throw FileNotFoundException("data/prescriptions.txt");

        const Prescription* arr = store.getAll();
        for (int i = 0; i < store.getSize(); i++) {
            const Prescription& pr = arr[i];

            f << pr.getID() << ","
                << pr.getAppointmentID() << ","
                << pr.getPatientID() << ","
                << pr.getDoctorID() << ","
                << pr.getDate() << ","
                << pr.getMedicines() << ","
                << pr.getNotes() << "\n";
        }

        f.close();
    }

    static void appendSecurityLog(string entry) {
        ofstream f("data/security_log.txt", ios::app);
        if (!f.is_open()) throw FileNotFoundException("data/security_log.txt");
        f << entry << "\n";
        f.close();
    }

    static void rewritePatients(const Storage<Patient>& store) {
        ofstream f("data/patients.txt");
        if (!f.is_open()) throw FileNotFoundException("data/patients.txt");
        const Patient* arr = store.getAll();
        for (int i = 0; i < store.getSize(); i++) {
            const Patient& pt = arr[i];
            f << pt.getID() << "," << pt.getName() << "," << pt.getAge() << ","
                << pt.getGender() << "," << pt.getContact() << ","
                << pt.getPassword() << "," << pt.getBalance() << "\n";
        }
        f.close();
    }

    static void rewriteBills(const Storage<Bill>& store) {
        ofstream f("data/bills.txt");
        if (!f.is_open()) throw FileNotFoundException("data/bills.txt");

        const Bill* arr = store.getAll();
        for (int i = 0; i < store.getSize(); i++) {
            const Bill& b = arr[i];
            f << b.getID() << ","
                << b.getPatientID() << ","
                << b.getAppointmentID() << ","
                << b.getAmount() << ","
                << b.getStatus() << ","
                << b.getDate() << "\n";
        }
        f.close();
    }

    static void rewriteAppointments(const Storage<Appointment>& store) {
        ofstream f("data/appointments.txt");
        if (!f.is_open()) throw FileNotFoundException("data/appointments.txt");
        const Appointment* arr = store.getAll();
        for (int i = 0; i < store.getSize(); i++) {
            const Appointment& a = arr[i];
            f << a.getID() << "," << a.getPatientID() << "," << a.getDoctorID() << ","
                << a.getDate() << "," << a.getTimeSlot() << "," << a.getStatus() << "\n";
        }
        f.close();
    }

private:
    static string extractField(string& data) {
        size_t pos = data.find(',');
        if (pos == string::npos) return data;
        string field = data.substr(0, pos);
        data.erase(0, pos + 1);
        return field;
    }
};