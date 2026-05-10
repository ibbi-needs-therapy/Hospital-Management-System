// #define _CRT_SECURE_NO_WARNINGS
// #include <iostream>
// #include <fstream>
// #include <ctime>
// #include <string>
// #include "Patient.h"
// #include "Doctor.h"
// #include "Admin.h"
// #include "Appointment.h"
// #include "Bill.h"
// #include "Prescription.h"
// #include "Storage.h"
// #include "FileHandler.h"
// #include "Validator.h"
// #include "HospitalExceptions.h"

// using namespace std;

// Storage<Patient>      gPatients(200);
// Storage<Doctor>       gDoctors(100);
// Storage<Appointment>  gAppointments(500);
// Storage<Bill>         gBills(500);
// Storage<Prescription> gPrescriptions(500);
// Admin                 gAdmin;

// void clrscr() {
// #ifdef _WIN32
//     system("cls");
// #else
//     system("clear");
// #endif
// }

// void divider(char c = '=', int w = 55) { for (int i = 0; i < w; i++) cout << c; cout << "\n"; }

// void printHeader(const char* title) {
//     clrscr(); divider();
//     int w = 55, len = (int)string(title).length();
//     int pad = (w - len) / 2;
//     for (int i = 0; i < pad; i++) cout << ' ';
//     cout << title << "\n"; divider();
// }

// void pause() {
//     cout << "\nPress Enter to continue...";
//     cin.ignore(10000, '\n');
// }

// // -- Safe input readers ------------------------------------------

// // Read a line into a std::string (used for Validator calls)
// string readLineStr() {
//     string s;
//     getline(cin, s);
//     // strip trailing \r (Windows)
//     if (!s.empty() && s.back() == '\r') s.pop_back();
//     return s;
// }

// // Read into char[] (used where we still need raw buffers)
// void readLine(char* buf, int maxLen) {
//     cin.getline(buf, maxLen);
//     if (cin.fail()) { cin.clear(); cin.ignore(10000, '\n'); buf[0] = '\0'; }
//     int l = 0; while (buf[l]) l++;
//     if (l > 0 && buf[l - 1] == '\r') buf[l - 1] = '\0';
// }

// // Read a positive integer; returns -1 on bad input
// int readInt() {
//     string s = readLineStr();
//     if (s.empty()) return -1;
//     int val = 0;
//     for (char c : s) { if (c < '0' || c > '9') return -1; val = val * 10 + (c - '0'); }
//     return val;
// }

// // Read a positive double; returns -1.0 on bad input
// double readDouble() {
//     string s = readLineStr();
//     if (s.empty()) return -1.0;
//     double ip = 0, fp = 0, div = 1.0; bool frac = false, any = false;
//     for (char c : s) {
//         if (c == '.' && !frac) { frac = true; continue; }
//         if (c < '0' || c > '9') return -1.0;
//         if (frac) { fp = fp * 10 + (c - '0'); div *= 10; }
//         else { ip = ip * 10 + (c - '0'); }
//         any = true;
//     }
//     return any ? ip + fp / div : -1.0;
// }

// // ===============================================================
// //  DATE / TIME UTILITIES
// // ===============================================================
// void getTodayDate(char* buf) {   // -> "DD-MM-YYYY" in buf[11]
//     time_t t = time(nullptr); tm* n = localtime(&t);
//     strftime(buf, 11, "%d-%m-%Y", n);
// }
// int getCurrentYear() {
//     time_t t = time(nullptr); tm* n = localtime(&t); return n->tm_year + 1900;
// }
// void getTimestamp(char* buf) {   // -> "DD-MM-YYYY HH:MM:SS" in buf[20]
//     time_t t = time(nullptr); tm* n = localtime(&t);
//     strftime(buf, 20, "%d-%m-%Y %H:%M:%S", n);
// }

// // DD-MM-YYYY -> YYYYMMDD int (for sorting)
// int dateToInt(const string& d) {
//     if (d.length() < 10) return 0;
//     return (d[6] - '0') * 10000000 + (d[7] - '0') * 1000000
//         + (d[8] - '0') * 100000 + (d[9] - '0') * 10000
//         + (d[3] - '0') * 1000 + (d[4] - '0') * 100
//         + (d[0] - '0') * 10 + (d[1] - '0');
// }

// int dateDiffDays(const string& date1, const string& date2) {
//     auto parse = [](const string& d, int& dy, int& mo, int& yr) {
//         dy = (d[0] - '0') * 10 + (d[1] - '0');
//         mo = (d[3] - '0') * 10 + (d[4] - '0');
//         yr = (d[6] - '0') * 1000 + (d[7] - '0') * 100 + (d[8] - '0') * 10 + (d[9] - '0');
//         };
//     int d1, m1, y1, d2, m2, y2;
//     parse(date1, d1, m1, y1); parse(date2, d2, m2, y2);
//     tm t1{}; t1.tm_mday = d1; t1.tm_mon = m1 - 1; t1.tm_year = y1 - 1900;
//     tm t2{}; t2.tm_mday = d2; t2.tm_mon = m2 - 1; t2.tm_year = y2 - 1900;
//     return (int)(difftime(mktime(&t2), mktime(&t1)) / 86400.0);
// }

// // ===============================================================
// //  MAX-ID HELPERS
// // ===============================================================
// int maxID(Storage<Appointment>& s) { int m = 0; for (int i = 0;i < s.getSize();i++) if (s.getAll()[i].getID() > m) m = s.getAll()[i].getID(); return m; }
// int maxID(Storage<Bill>& s) { int m = 0; for (int i = 0;i < s.getSize();i++) if (s.getAll()[i].getID() > m) m = s.getAll()[i].getID(); return m; }
// int maxID(Storage<Prescription>& s) { int m = 0; for (int i = 0;i < s.getSize();i++) if (s.getAll()[i].getID() > m) m = s.getAll()[i].getID(); return m; }
// int maxID(Storage<Patient>& s) { int m = 0; for (int i = 0;i < s.getSize();i++) if (s.getAll()[i].getID() > m) m = s.getAll()[i].getID(); return m; }
// int maxID(Storage<Doctor>& s) { int m = 0; for (int i = 0;i < s.getSize();i++) if (s.getAll()[i].getID() > m) m = s.getAll()[i].getID(); return m; }

// // ===============================================================
// //  SORTING
// // ===============================================================
// void sortApptsAsc(Appointment** a, int n) {
//     for (int i = 1;i < n;i++) {
//         Appointment* k = a[i]; int j = i - 1;
//         while (j >= 0 && dateToInt(a[j]->getDate()) > dateToInt(k->getDate())) { a[j + 1] = a[j]; j--; }
//         a[j + 1] = k;
//     }
// }
// void sortApptsDesc(Appointment** a, int n) {
//     sortApptsAsc(a, n);
//     for (int i = 0, j = n - 1;i < j;i++, j--) { Appointment* t = a[i];a[i] = a[j];a[j] = t; }
// }
// void sortApptsBySlotAsc(Appointment** a, int n) {
//     for (int i = 1;i < n;i++) {
//         Appointment* k = a[i]; int j = i - 1;
//         while (j >= 0 && a[j]->getTimeSlot() > k->getTimeSlot()) { a[j + 1] = a[j]; j--; }
//         a[j + 1] = k;
//     }
// }
// void sortPrescsDesc(Prescription** a, int n) {
//     for (int i = 1;i < n;i++) {
//         Prescription* k = a[i]; int j = i - 1;
//         while (j >= 0 && dateToInt(a[j]->getDate()) < dateToInt(k->getDate())) { a[j + 1] = a[j]; j--; }
//         a[j + 1] = k;
//     }
// }

// // ===============================================================
// //  SECURITY LOG
// // ===============================================================
// void logSecurity(const string& role, int enteredID, const string& result) {
//     char ts[20]; getTimestamp(ts);
//     string entry = string(ts) + "," + role + "," + to_string(enteredID) + "," + result;
//     try { FileHandler::appendSecurityLog(entry.c_str()); }
//     catch (...) {}
// }

// // ===============================================================
// //  STARTUP
// // ===============================================================
// void ensureFile(const char* name) { ifstream f(name); if (!f.is_open()) { ofstream c(name); } }

// void loadAllData() {
//     const char* files[] = { "patients.txt","doctors.txt","admin.txt",
//                              "appointments.txt","bills.txt","prescriptions.txt",
//                              "security_log.txt","discharged.txt" };
//     for (int i = 0; i < 8; i++) ensureFile(files[i]);

//     try { FileHandler::loadPatients(gPatients); }
//     catch (HospitalException& e) { cout << "[WARN] " << e.what() << "\n"; }
//     try { FileHandler::loadDoctors(gDoctors); }
//     catch (HospitalException& e) { cout << "[WARN] " << e.what() << "\n"; }
//     try { FileHandler::loadAdmin(gAdmin); }
//     catch (HospitalException&) {}
//     try { FileHandler::loadAppointments(gAppointments); }
//     catch (HospitalException& e) { cout << "[WARN] " << e.what() << "\n"; }
//     try { FileHandler::loadBills(gBills); }
//     catch (HospitalException& e) { cout << "[WARN] " << e.what() << "\n"; }
//     try { FileHandler::loadPrescriptions(gPrescriptions); }
//     catch (HospitalException& e) { cout << "[WARN] " << e.what() << "\n"; }
// }

// // ===============================================================
// //  SHARED DISPLAY HELPERS
// // ===============================================================
// void printAvailableSlots(int docID, const string& date) {
//     static const string SLOTS[] = { "09:00","10:00","11:00","12:00","13:00","14:00","15:00","16:00" };
//     bool taken[8] = { false };
//     Appointment* all = gAppointments.getAll();
//     for (int i = 0; i < gAppointments.getSize(); i++) {
//         Appointment& a = *(all + i);
//         if (a.getDoctorID() == docID && a.getDate() == date && a.getStatus() != "cancelled")
//             for (int s = 0; s < 8; s++)
//                 if (a.getTimeSlot() == SLOTS[s]) taken[s] = true;
//     }
//     cout << "  Available slots: ";
//     bool any = false;
//     for (int s = 0; s < 8; s++) if (!taken[s]) { cout << SLOTS[s] << "  "; any = true; }
//     if (!any) cout << "(none - fully booked)";
//     cout << "\n";
// }

// bool slotIsTaken(int docID, const string& date, const string& slot) {
//     Appointment* all = gAppointments.getAll();
//     for (int i = 0; i < gAppointments.getSize(); i++) {
//         Appointment& a = *(all + i);
//         if (a.getDoctorID() == docID && a.getDate() == date
//             && a.getTimeSlot() == slot && a.getStatus() != "cancelled")
//             return true;
//     }
//     return false;
// }

// // ===============================================================
// //  PATIENT MENU
// // ===============================================================

// void patientBookAppointment(Patient* p) {
//     printHeader("Book Appointment");

//     cout << "Enter specialization: ";
//     string spec = readLineStr();
//     if (spec.empty()) { cout << "[!] Input cannot be empty.\n"; pause(); return; }

//     // Find matching doctors (case-insensitive via Validator::EqualCI)
//     Doctor* matchBuf[100]; int mc = 0;
//     Doctor* allD = gDoctors.getAll();
//     for (int i = 0; i < gDoctors.getSize(); i++)
//         if (Validator::EqualCI(allD[i].getSpecialization(), spec))
//             matchBuf[mc++] = &allD[i];

//     if (mc == 0) { cout << "[!] No doctors available for that specialization.\n"; pause(); return; }

//     divider('-');
//     cout << "  ID  | Name                        | Fee (PKR)\n"; divider('-');
//     for (int i = 0; i < mc; i++)
//         cout << "  " << matchBuf[i]->getID() << "   | " << matchBuf[i]->getName()
//         << "   | " << matchBuf[i]->getFee() << "\n";
//     divider('-');

//     cout << "Enter Doctor ID: ";
//     int docID = readInt();
//     if (docID < 1) { cout << "[!] Invalid input.\n"; pause(); return; }

//     Doctor* doc = gDoctors.findByID(docID);
//     if (!doc || !Validator::EqualCI(doc->getSpecialization(), spec)) {
//         cout << "[!] Doctor not found.\n"; pause(); return;
//     }

//     // Balance pre-check
//     if (p->getBalance() < doc->getFee()) {
//         try { throw InsufficientFundsException("Insufficient balance. Please top up before booking."); }
//         catch (HospitalException& e) { cout << "[!] " << e.what() << "\n"; pause(); return; }
//     }

//     // Date input - up to 3 attempts
//     string date; int dateAttempts = 0; bool dateOK = false;
//     while (dateAttempts < 3) {
//         cout << "Enter date (DD-MM-YYYY): ";
//         date = readLineStr();
//         if (Validator::isValidDate(date, getCurrentYear())) { dateOK = true; break; }
//         cout << "[!] Invalid date. Use DD-MM-YYYY, current year or later. ("
//             << (2 - dateAttempts) << " attempt(s) left)\n";
//         dateAttempts++;
//     }
//     if (!dateOK) { cout << "[!] Too many invalid attempts.\n"; pause(); return; }

//     // Slot selection loop
//     string slot;
//     while (true) {
//         printAvailableSlots(docID, date);
//         cout << "Enter time slot: ";
//         slot = readLineStr();
//         if (!Validator::isValidTimeSlot(slot)) {
//             cout << "[!] Invalid slot. Choose from: 09:00 10:00 11:00 12:00 13:00 14:00 15:00 16:00\n";
//             continue;
//         }
//         if (slotIsTaken(docID, date, slot)) {
//             try { throw SlotUnavailableException(); }
//             catch (HospitalException& e) { cout << "[!] " << e.what() << "\n"; continue; }
//         }
//         break;
//     }

//     *p -= doc->getFee();

//     int newAID = maxID(gAppointments) + 1;
//     Appointment newA(newAID, p->getID(), docID, date, slot, "pending");
//     gAppointments.add(newA);
//     FileHandler::appendAppointment(newA);

//     int newBID = maxID(gBills) + 1;
//     Bill newB(newBID, p->getID(), newAID, doc->getFee(), "unpaid", date);
//     gBills.add(newB);
//     FileHandler::appendBill(newB);

//     FileHandler::rewritePatients(gPatients);

//     divider();
//     cout << "[OK] Appointment booked! ID: " << newAID
//         << "  |  Dr. " << doc->getName()
//         << "  |  " << date << "  " << slot << "\n"
//         << "    Remaining balance: PKR " << p->getBalance() << "\n";
//     divider();
//     pause();
// }

// void patientCancelAppointment(Patient* p) {
//     printHeader("Cancel Appointment");

//     Appointment* buf[200]; int cnt = 0;
//     Appointment* all = gAppointments.getAll();
//     for (int i = 0; i < gAppointments.getSize(); i++) {
//         Appointment& a = *(all + i);
//         if (a.getPatientID() == p->getID() && a.getStatus() == "pending")
//             buf[cnt++] = &a;
//     }
//     if (cnt == 0) { cout << "[i] You have no pending appointments.\n"; pause(); return; }

//     sortApptsAsc(buf, cnt);
//     divider('-');
//     cout << "  ID  | Doctor              | Date       | Slot\n"; divider('-');
//     for (int i = 0; i < cnt; i++) {
//         Doctor* d = gDoctors.findByID(buf[i]->getDoctorID());
//         cout << "  " << buf[i]->getID() << "   | Dr. "
//             << (d ? d->getName() : "?") << " | "
//             << buf[i]->getDate() << " | " << buf[i]->getTimeSlot() << "\n";
//     }
//     divider('-');

//     cout << "Enter Appointment ID to cancel (0 to go back): ";
//     int aid = readInt();
//     if (aid <= 0) return;

//     Appointment* target = nullptr;
//     for (int i = 0; i < cnt; i++) if (buf[i]->getID() == aid) { target = buf[i]; break; }
//     if (!target) { cout << "[!] Invalid appointment ID.\n"; pause(); return; }

//     Doctor* doc = gDoctors.findByID(target->getDoctorID());
//     float fee = doc ? doc->getFee() : 0.0f;

//     target->setStatus("cancelled");
//     *p += fee;

//     Bill* allB = gBills.getAll();
//     for (int i = 0; i < gBills.getSize(); i++) {
//         Bill& b = *(allB + i);
//         if (b.getAppointmentID() == aid && b.getStatus() == "unpaid") { b.setStatus("cancelled"); break; }
//     }

//     FileHandler::rewriteAppointments(gAppointments);
//     FileHandler::rewriteBills(gBills);
//     FileHandler::rewritePatients(gPatients);

//     cout << "[OK] Cancelled. PKR " << fee << " refunded. New balance: PKR " << p->getBalance() << "\n";
//     pause();
// }

// void patientViewAppointments(Patient* p) {
//     printHeader("My Appointments");

//     Appointment* buf[200]; int cnt = 0;
//     Appointment* all = gAppointments.getAll();
//     for (int i = 0; i < gAppointments.getSize(); i++)
//         if ((all + i)->getPatientID() == p->getID()) buf[cnt++] = (all + i);

//     if (cnt == 0) { cout << "[i] No appointments found.\n"; pause(); return; }
//     sortApptsAsc(buf, cnt);

//     divider('-');
//     cout << "  ID  | Doctor (Spec)                  | Date       | Slot  | Status\n"; divider('-');
//     for (int i = 0; i < cnt; i++) {
//         Doctor* d = gDoctors.findByID(buf[i]->getDoctorID());
//         cout << "  " << buf[i]->getID() << "   | Dr. "
//             << (d ? d->getName() : "?")
//             << " (" << (d ? d->getSpecialization() : "?") << ") | "
//             << buf[i]->getDate() << " | " << buf[i]->getTimeSlot()
//             << " | " << buf[i]->getStatus() << "\n";
//     }
//     divider('-');
//     pause();
// }

// void patientViewMedicalRecords(Patient* p) {
//     printHeader("My Medical Records");

//     Prescription* buf[200]; int cnt = 0;
//     Prescription* all = gPrescriptions.getAll();
//     for (int i = 0; i < gPrescriptions.getSize(); i++)
//         if ((all + i)->getPatientID() == p->getID()) buf[cnt++] = (all + i);

//     if (cnt == 0) { cout << "[i] No medical records found.\n"; pause(); return; }
//     sortPrescsDesc(buf, cnt);

//     for (int i = 0; i < cnt; i++) {
//         Doctor* d = gDoctors.findByID(buf[i]->getDoctorID());
//         divider('-');
//         cout << "  Date     : " << buf[i]->getDate() << "\n"
//             << "  Doctor   : Dr. " << (d ? d->getName() : "?") << "\n"
//             << "  Medicines: " << buf[i]->getMedicines() << "\n"
//             << "  Notes    : " << buf[i]->getNotes() << "\n";
//     }
//     divider('-');
//     pause();
// }

// void patientViewBills(Patient* p) {
//     printHeader("My Bills");

//     Bill* all = gBills.getAll();
//     double totalUnpaid = 0.0; bool any = false;
//     divider('-');
//     cout << "  ID  | Appt | Amount (PKR) | Status    | Date\n"; divider('-');
//     for (int i = 0; i < gBills.getSize(); i++) {
//         Bill& b = *(all + i);
//         if (b.getPatientID() != p->getID()) continue;
//         cout << "  " << b.getID() << "   | " << b.getAppointmentID()
//             << "    | PKR " << b.getAmount() << "  | "
//             << b.getStatus() << " | " << b.getDate() << "\n";
//         if (b.getStatus() == "unpaid") totalUnpaid += b.getAmount();
//         any = true;
//     }
//     if (!any) { cout << "[i] No bills found.\n"; }
//     else { divider('-'); cout << "  Total outstanding unpaid: PKR " << totalUnpaid << "\n"; }
//     divider('-');
//     pause();
// }

// void patientPayBill(Patient* p) {
//     printHeader("Pay Bill");

//     Bill* unpaid[100]; int uc = 0;
//     Bill* all = gBills.getAll();
//     for (int i = 0; i < gBills.getSize(); i++) {
//         Bill& b = *(all + i);
//         if (b.getPatientID() == p->getID() && b.getStatus() == "unpaid")
//             unpaid[uc++] = &b;
//     }
//     if (uc == 0) { cout << "[i] No unpaid bills.\n"; pause(); return; }

//     divider('-');
//     cout << "  ID  | Appt | Amount (PKR) | Date\n"; divider('-');
//     for (int i = 0; i < uc; i++)
//         cout << "  " << unpaid[i]->getID() << "   | " << unpaid[i]->getAppointmentID()
//         << "    | PKR " << unpaid[i]->getAmount() << "  | " << unpaid[i]->getDate() << "\n";
//     divider('-');
//     cout << "Your balance: PKR " << p->getBalance() << "\n\n";

//     cout << "Enter Bill ID to pay (0 to go back): ";
//     int bid = readInt();
//     if (bid <= 0) return;

//     Bill* target = nullptr;
//     for (int i = 0; i < uc; i++) if (unpaid[i]->getID() == bid) { target = unpaid[i]; break; }
//     if (!target) { cout << "[!] Invalid bill ID.\n"; pause(); return; }

//     if (p->getBalance() < target->getAmount()) {
//         try { throw InsufficientFundsException(); }
//         catch (HospitalException& e) { cout << "[!] " << e.what() << "\n"; pause(); return; }
//     }

//     *p -= target->getAmount();
//     target->setStatus("paid");
//     FileHandler::rewriteBills(gBills);
//     FileHandler::rewritePatients(gPatients);

//     cout << "[OK] Bill paid. Remaining balance: PKR " << p->getBalance() << "\n";
//     pause();
// }

// void patientTopUp(Patient* p) {
//     printHeader("Top Up Balance");
//     cout << "Current balance: PKR " << p->getBalance() << "\n\n";

//     int attempts = 0;
//     while (attempts < 3) {
//         cout << "Enter amount to add (PKR): ";
//         double amt = readDouble();
//         if (amt > 0.0) {
//             *p += amt;
//             FileHandler::rewritePatients(gPatients);
//             cout << "[OK] Balance updated. New balance: PKR " << p->getBalance() << "\n";
//             pause(); return;
//         }
//         try { throw InvalidInputException("Amount must be a positive number greater than 0."); }
//         catch (HospitalException& e) {
//             cout << "[!] " << e.what() << " (" << (2 - attempts) << " attempt(s) left)\n";
//             attempts++;
//         }
//     }
//     cout << "[!] Too many invalid attempts.\n"; pause();
// }

// void runPatientMenu(Patient* p) {
//     int choice;
//     do {
//         printHeader("MediCore - Patient Portal");
//         cout << "  Welcome, " << p->getName() << "\n"
//             << "  Balance : PKR " << p->getBalance() << "\n\n"
//             << "  1.  Book Appointment\n"
//             << "  2.  Cancel Appointment\n"
//             << "  3.  View My Appointments\n"
//             << "  4.  View My Medical Records\n"
//             << "  5.  View My Bills\n"
//             << "  6.  Pay Bill\n"
//             << "  7.  Top Up Balance\n"
//             << "  8.  Logout\n";
//         divider();
//         cout << "  Choice: "; choice = readInt();
//         if (!Validator::isValidMenuChoice(choice, 1, 8)) { cout << "[!] Invalid choice.\n"; pause(); continue; }
//         switch (choice) {
//         case 1: patientBookAppointment(p);    break;
//         case 2: patientCancelAppointment(p);  break;
//         case 3: patientViewAppointments(p);   break;
//         case 4: patientViewMedicalRecords(p); break;
//         case 5: patientViewBills(p);           break;
//         case 6: patientPayBill(p);             break;
//         case 7: patientTopUp(p);               break;
//         case 8: cout << "[OK] Logged out.\n"; pause(); break;
//         }
//     } while (choice != 8);
// }

// // ---------------------------------------------------------------
// //  DOCTOR MENU
// // ---------------------------------------------------------------

// void doctorViewToday(Doctor* d) {
//     printHeader("Today's Appointments");
//     char today[11]; getTodayDate(today); string todayStr(today);
//     cout << "  Date: " << todayStr << "\n\n";

//     Appointment* buf[50]; int cnt = 0;
//     Appointment* all = gAppointments.getAll();
//     for (int i = 0; i < gAppointments.getSize(); i++) {
//         Appointment& a = *(all + i);
//         if (a.getDoctorID() == d->getID() && a.getDate() == todayStr)
//             buf[cnt++] = &a;
//     }
//     if (cnt == 0) { cout << "[i] No appointments scheduled for today.\n"; pause(); return; }

//     sortApptsBySlotAsc(buf, cnt);
//     divider('-');
//     cout << "  ID  | Patient             | Slot  | Status\n"; divider('-');
//     for (int i = 0; i < cnt; i++) {
//         Patient* pt = gPatients.findByID(buf[i]->getPatientID());
//         cout << "  " << buf[i]->getID() << "   | "
//             << (pt ? pt->getName() : "?") << " | "
//             << buf[i]->getTimeSlot() << " | " << buf[i]->getStatus() << "\n";
//     }
//     divider('-');
//     pause();
// }

// Appointment* pickTodayPendingAppt(Doctor* d, const string& action) {
//     char today[11]; getTodayDate(today); string todayStr(today);

//     Appointment* buf[50]; int cnt = 0;
//     Appointment* all = gAppointments.getAll();
//     for (int i = 0; i < gAppointments.getSize(); i++) {
//         Appointment& a = *(all + i);
//         if (a.getDoctorID() == d->getID() && a.getDate() == todayStr && a.getStatus() == "pending")
//             buf[cnt++] = &a;
//     }
//     if (cnt == 0) { cout << "[i] No pending appointments today.\n"; return nullptr; }

//     sortApptsBySlotAsc(buf, cnt);
//     divider('-');
//     cout << "  ID  | Patient             | Slot\n"; divider('-');
//     for (int i = 0; i < cnt; i++) {
//         Patient* pt = gPatients.findByID(buf[i]->getPatientID());
//         cout << "  " << buf[i]->getID() << "   | "
//             << (pt ? pt->getName() : "?") << " | " << buf[i]->getTimeSlot() << "\n";
//     }
//     divider('-');
//     cout << "Enter Appointment ID to mark as " << action << " (0 to go back): ";
//     int aid = readInt();
//     if (aid <= 0) return nullptr;

//     for (int i = 0; i < cnt; i++) if (buf[i]->getID() == aid) return buf[i];
//     cout << "[!] Invalid appointment ID.\n";
//     return nullptr;
// }

// void doctorMarkComplete(Doctor* d) {
//     printHeader("Mark Appointment Complete");
//     Appointment* a = pickTodayPendingAppt(d, "completed");
//     if (!a) { pause(); return; }
//     a->setStatus("completed");
//     FileHandler::rewriteAppointments(gAppointments);
//     cout << "[OK] Appointment " << a->getID() << " marked as completed.\n";
//     pause();
// }

// void doctorMarkNoShow(Doctor* d) {
//     printHeader("Mark Appointment No-Show");
//     Appointment* a = pickTodayPendingAppt(d, "no-show");
//     if (!a) { pause(); return; }
//     a->setStatus("noshow");

//     Bill* allB = gBills.getAll();
//     for (int i = 0; i < gBills.getSize(); i++) {
//         Bill& b = *(allB + i);
//         if (b.getAppointmentID() == a->getID() && b.getStatus() == "unpaid") {
//             b.setStatus("cancelled"); break;
//         }
//     }
//     FileHandler::rewriteAppointments(gAppointments);
//     FileHandler::rewriteBills(gBills);
//     cout << "[OK] Appointment " << a->getID() << " marked as no-show. Bill cancelled. No refund.\n";
//     pause();
// }

// void doctorWritePrescription(Doctor* d) {
//     printHeader("Write Prescription");
//     cout << "Enter Appointment ID (0 to go back): ";
//     int aid = readInt();
//     if (aid <= 0) return;

//     Appointment* appt = gAppointments.findByID(aid);
//     if (!appt || appt->getDoctorID() != d->getID() || appt->getStatus() != "completed") {
//         cout << "[!] Invalid appointment ID. Must belong to you and be completed.\n"; pause(); return;
//     }

//     Prescription* allP = gPrescriptions.getAll();
//     for (int i = 0; i < gPrescriptions.getSize(); i++)
//         if ((allP + i)->getAppointmentID() == aid) {
//             cout << "[!] Prescription already written for this appointment.\n"; pause(); return;
//         }

//     Patient* pt = gPatients.findByID(appt->getPatientID());
//     cout << "  Patient: " << (pt ? pt->getName() : "?") << "\n"
//         << "  Date   : " << appt->getDate() << "\n\n";

//     cout << "Medicines (e.g. Paracetamol 500mg;Amoxicillin 250mg):\n> ";
//     string meds = readLineStr();
//     if (meds.empty()) { cout << "[!] Medicines cannot be empty.\n"; pause(); return; }

//     cout << "Notes (max 300 chars):\n> ";
//     string notes = readLineStr();

//     char today[11]; getTodayDate(today);
//     int newID = maxID(gPrescriptions) + 1;
//     Prescription pr(newID, aid, appt->getPatientID(), d->getID(), today, meds, notes);
//     gPrescriptions.add(pr);
//     FileHandler::appendPrescription(pr);

//     cout << "[OK] Prescription saved. ID: " << newID << "\n";
//     pause();
// }

// void doctorViewPatientHistory(Doctor* d) {
//     printHeader("Patient Medical History");
//     cout << "Enter Patient ID (0 to go back): ";
//     int pid = readInt();
//     if (pid <= 0) return;

//     Patient* pt = gPatients.findByID(pid);
//     if (!pt) { cout << "[!] Access denied. You can only view records of your own patients.\n"; pause(); return; }

//     bool access = false;
//     Appointment* allA = gAppointments.getAll();
//     for (int i = 0; i < gAppointments.getSize(); i++) {
//         Appointment& a = *(allA + i);
//         if (a.getPatientID() == pid && a.getDoctorID() == d->getID() && a.getStatus() == "completed")
//         {
//             access = true; break;
//         }
//     }
//     if (!access) { cout << "[!] Access denied. You can only view records of your own patients.\n"; pause(); return; }

//     Prescription* buf[200]; int cnt = 0;
//     Prescription* allP = gPrescriptions.getAll();
//     for (int i = 0; i < gPrescriptions.getSize(); i++) {
//         Prescription& pr = *(allP + i);
//         if (pr.getPatientID() == pid && pr.getDoctorID() == d->getID())
//             buf[cnt++] = &pr;
//     }
//     if (cnt == 0) { cout << "[i] No prescriptions found for this patient.\n"; pause(); return; }

//     sortPrescsDesc(buf, cnt);
//     cout << "  Patient: " << pt->getName() << "\n\n";
//     for (int i = 0; i < cnt; i++) {
//         divider('-');
//         cout << "  Date     : " << buf[i]->getDate() << "\n"
//             << "  Appt ID  : " << buf[i]->getAppointmentID() << "\n"
//             << "  Medicines: " << buf[i]->getMedicines() << "\n"
//             << "  Notes    : " << buf[i]->getNotes() << "\n";
//     }
//     divider('-');
//     pause();
// }

// void runDoctorMenu(Doctor* d) {
//     int choice;
//     do {
//         printHeader("MediCore - Doctor Portal");
//         cout << "  Welcome, Dr. " << d->getName() << "\n"
//             << "  Specialization: " << d->getSpecialization() << "\n\n"
//             << "  1.  View Today's Appointments\n"
//             << "  2.  Mark Appointment Complete\n"
//             << "  3.  Mark Appointment No-Show\n"
//             << "  4.  Write Prescription\n"
//             << "  5.  View Patient Medical History\n"
//             << "  6.  Logout\n";
//         divider();
//         cout << "  Choice: "; choice = readInt();
//         if (!Validator::isValidMenuChoice(choice, 1, 6)) { cout << "[!] Invalid choice.\n"; pause(); continue; }
//         switch (choice) {
//         case 1: doctorViewToday(d);           break;
//         case 2: doctorMarkComplete(d);         break;
//         case 3: doctorMarkNoShow(d);           break;
//         case 4: doctorWritePrescription(d);    break;
//         case 5: doctorViewPatientHistory(d);   break;
//         case 6: cout << "[OK] Logged out.\n"; pause(); break;
//         }
//     } while (choice != 6);
// }

// // ---------------------------------------------------------------
// //  ADMIN MENU
// // ---------------------------------------------------------------

// void adminAddDoctor() {
//     printHeader("Add Doctor");
//     string name, spec, contactStr, pwd; float fee;

//     cout << "Name (max 50 chars): "; name = readLineStr();
//     if (name.empty()) { cout << "[!] Name cannot be empty.\n"; pause(); return; }

//     cout << "Specialization: "; spec = readLineStr();
//     if (spec.empty()) { cout << "[!] Specialization cannot be empty.\n"; pause(); return; }

//     while (true) {
//         cout << "Contact (11 digits): "; contactStr = readLineStr();
//         if (Validator::isValidContact(contactStr)) break;
//         cout << "[!] Must be exactly 11 digits.\n";
//     }
//     while (true) {
//         cout << "Password (min 6 chars): "; pwd = readLineStr();
//         if (Validator::isValidPassword(pwd)) break;
//         cout << "[!] Password must be at least 6 characters.\n";
//     }
//     while (true) {
//         cout << "Consultation fee (PKR): ";
//         double tmp = readDouble();
//         if (tmp > 0.0) { fee = (float)tmp; break; }
//         cout << "[!] Fee must be a positive number.\n";
//     }

//     int newID = maxID(gDoctors) + 1;
//     Doctor nd(newID, name, contactStr, pwd, spec, fee);
//     gDoctors.add(nd);
//     FileHandler::appendDoctor(nd);
//     cout << "[OK] Doctor added. ID: " << newID << "\n";
//     pause();
// }

// void adminRemoveDoctor() {
//     printHeader("Remove Doctor");
//     if (gDoctors.getSize() == 0) { cout << "[i] No doctors on record.\n"; pause(); return; }

//     Doctor* all = gDoctors.getAll();
//     divider('-');
//     cout << "  ID  | Name                  | Specialization   | Fee\n"; divider('-');
//     for (int i = 0; i < gDoctors.getSize(); i++)
//         cout << "  " << all[i].getID() << "   | " << all[i].getName()
//         << " | " << all[i].getSpecialization() << " | PKR " << all[i].getFee() << "\n";
//     divider('-');

//     cout << "Enter Doctor ID to remove (0 to go back): ";
//     int did = readInt();
//     if (did <= 0) return;

//     Doctor* doc = gDoctors.findByID(did);
//     if (!doc) { cout << "[!] Doctor not found.\n"; pause(); return; }

//     Appointment* allA = gAppointments.getAll();
//     for (int i = 0; i < gAppointments.getSize(); i++) {
//         Appointment& a = *(allA + i);
//         if (a.getDoctorID() == did && a.getStatus() == "pending") {
//             cout << "[!] Cannot remove doctor with pending appointments.\n"
//                 << "    Cancel or reassign them first.\n";
//             pause(); return;
//         }
//     }

//     gDoctors.removeByID(did);
//     FileHandler::rewriteDoctors(gDoctors);
//     cout << "[OK] Doctor removed.\n";
//     pause();
// }

// void adminViewAllPatients() {
//     printHeader("All Patients");
//     if (gPatients.getSize() == 0) { cout << "[i] No patients on record.\n"; pause(); return; }

//     divider('-');
//     cout << "  ID  | Name              | Age | G | Contact     | Balance     | Unpaid\n"; divider('-');
//     Patient* all = gPatients.getAll();
//     Bill* allB = gBills.getAll();
//     for (int i = 0; i < gPatients.getSize(); i++) {
//         Patient& pt = *(all + i);
//         int ub = 0;
//         for (int j = 0; j < gBills.getSize(); j++)
//             if ((allB + j)->getPatientID() == pt.getID() && (allB + j)->getStatus() == "unpaid") ub++;
//         cout << "  " << pt.getID() << "   | " << pt.getName()
//             << " | " << pt.getAge() << " | " << pt.getGender()
//             << " | " << pt.getContact() << " | PKR " << pt.getBalance()
//             << " | " << ub << "\n";
//     }
//     divider('-');
//     pause();
// }

// void adminViewAllDoctors() {
//     printHeader("All Doctors");
//     if (gDoctors.getSize() == 0) { cout << "[i] No doctors on record.\n"; pause(); return; }

//     divider('-');
//     cout << "  ID  | Name                  | Specialization   | Contact     | Fee\n"; divider('-');
//     Doctor* all = gDoctors.getAll();
//     for (int i = 0; i < gDoctors.getSize(); i++)
//         cout << "  " << all[i].getID() << "   | " << all[i].getName()
//         << " | " << all[i].getSpecialization()
//         << " | " << all[i].getContact()
//         << " | PKR " << all[i].getFee() << "\n";
//     divider('-');
//     pause();
// }

// void adminViewAllAppointments() {
//     printHeader("All Appointments");
//     int n = gAppointments.getSize();
//     if (n == 0) { cout << "[i] No appointments on record.\n"; pause(); return; }

//     Appointment** buf = new Appointment * [n];
//     Appointment* all = gAppointments.getAll();
//     for (int i = 0; i < n; i++) buf[i] = (all + i);
//     sortApptsDesc(buf, n);

//     divider('-');
//     cout << "  ID  | Patient           | Doctor            | Date       | Slot  | Status\n"; divider('-');
//     for (int i = 0; i < n; i++) {
//         Patient* pt = gPatients.findByID(buf[i]->getPatientID());
//         Doctor* d = gDoctors.findByID(buf[i]->getDoctorID());
//         cout << "  " << buf[i]->getID() << "   | "
//             << (pt ? pt->getName() : "?") << " | Dr. "
//             << (d ? d->getName() : "?") << " | "
//             << buf[i]->getDate() << " | " << buf[i]->getTimeSlot()
//             << " | " << buf[i]->getStatus() << "\n";
//     }
//     divider('-');
//     delete[] buf;
//     pause();
// }

// void adminViewUnpaidBills() {
//     printHeader("Unpaid Bills");
//     char today[11]; getTodayDate(today); string todayStr(today);
//     Bill* all = gBills.getAll(); bool any = false;

//     divider('-');
//     cout << "  ID  | Patient           | Amount    | Date\n"; divider('-');
//     for (int i = 0; i < gBills.getSize(); i++) {
//         Bill& b = *(all + i);
//         if (b.getStatus() != "unpaid") continue;
//         Patient* pt = gPatients.findByID(b.getPatientID());
//         bool overdue = dateDiffDays(b.getDate(), todayStr) > 7;
//         cout << "  " << b.getID() << "   | "
//             << (pt ? pt->getName() : "?") << " | PKR "
//             << b.getAmount() << " | " << b.getDate()
//             << (overdue ? " [OVERDUE]" : "") << "\n";
//         any = true;
//     }
//     if (!any) cout << "[i] No unpaid bills.\n";
//     divider('-');
//     pause();
// }

// void adminDischargePatient() {
//     printHeader("Discharge Patient");
//     cout << "Enter Patient ID (0 to go back): ";
//     int pid = readInt();
//     if (pid <= 0) return;

//     Patient* pt = gPatients.findByID(pid);
//     if (!pt) { cout << "[!] Patient not found.\n"; pause(); return; }

//     Bill* allB = gBills.getAll();
//     for (int i = 0; i < gBills.getSize(); i++)
//         if ((allB + i)->getPatientID() == pid && (allB + i)->getStatus() == "unpaid") {
//             cout << "[!] Cannot discharge patient with unpaid bills.\n"; pause(); return;
//         }

//     Appointment* allA = gAppointments.getAll();
//     for (int i = 0; i < gAppointments.getSize(); i++)
//         if ((allA + i)->getPatientID() == pid && (allA + i)->getStatus() == "pending") {
//             cout << "[!] Cannot discharge patient with pending appointments.\n"; pause(); return;
//         }

//     FileHandler::archivePatient(*pt, gAppointments, gBills, gPrescriptions);

//     // Collect IDs then remove (avoids mutating while iterating)
//     auto purge = [&](auto& store, auto rewriteFn) {
//         auto* all = store.getAll();
//         int sz = store.getSize();
//         int* ids = new int[sz]; int cnt = 0;
//         for (int i = 0; i < sz; i++)
//             if ((all + i)->getPatientID() == pid) ids[cnt++] = (all + i)->getID();
//         for (int i = 0; i < cnt; i++) store.removeByID(ids[i]);
//         rewriteFn(store);
//         delete[] ids;
//         };

//     gPatients.removeByID(pid);
//     FileHandler::rewritePatients(gPatients);
//     purge(gAppointments, [](Storage<Appointment>& s) { FileHandler::rewriteAppointments(s);  });
//     purge(gBills, [](Storage<Bill>& s) { FileHandler::rewriteBills(s);         });
//     purge(gPrescriptions, [](Storage<Prescription>& s) { FileHandler::rewritePrescriptions(s); });

//     cout << "[OK] Patient discharged and archived successfully.\n";
//     pause();
// }

// void adminViewSecurityLog() {
//     printHeader("Security Log");
//     ifstream f("security_log.txt");
//     if (!f.is_open()) { cout << "[i] No security events logged.\n"; pause(); return; }
//     char line[256]; bool any = false;
//     while (f.getline(line, 256)) { if (line[0] == '\0') continue; cout << "  " << line << "\n"; any = true; }
//     if (!any) cout << "[i] No security events logged.\n";
//     f.close();
//     pause();
// }

// void adminDailyReport() {
//     printHeader("Daily Report");
//     char today[11]; getTodayDate(today); string todayStr(today);
//     cout << "  Report Date: " << todayStr << "\n\n";

//     int total = 0, pend = 0, comp = 0, nosh = 0, canc = 0;
//     Appointment* allA = gAppointments.getAll();
//     for (int i = 0; i < gAppointments.getSize(); i++) {
//         Appointment& a = *(allA + i);
//         if (a.getDate() != todayStr) continue;
//         total++;
//         string st = a.getStatus();
//         if (st == "pending")   pend++;
//         else if (st == "completed") comp++;
//         else if (st == "noshow")    nosh++;
//         else if (st == "cancelled") canc++;
//     }
//     divider('-');
//     cout << "  Appointments today: " << total
//         << "  (Pending:" << pend << "  Completed:" << comp
//         << "  No-show:" << nosh << "  Cancelled:" << canc << ")\n";

//     double rev = 0.0;
//     Bill* allB = gBills.getAll();
//     for (int i = 0; i < gBills.getSize(); i++) {
//         Bill& b = *(allB + i);
//         if (b.getDate() == todayStr && b.getStatus() == "paid") rev += b.getAmount();
//     }
//     cout << "  Revenue collected today: PKR " << rev << "\n";

//     divider('-');
//     cout << "  Patients with outstanding unpaid bills:\n";
//     Patient* allP = gPatients.getAll(); bool anyU = false;
//     for (int i = 0; i < gPatients.getSize(); i++) {
//         Patient& pt = *(allP + i); double owed = 0.0;
//         for (int j = 0; j < gBills.getSize(); j++) {
//             Bill& b = *(allB + j);
//             if (b.getPatientID() == pt.getID() && b.getStatus() == "unpaid") owed += b.getAmount();
//         }
//         if (owed > 0.0) { cout << "    " << pt.getName() << " | Total owed: PKR " << owed << "\n"; anyU = true; }
//     }
//     if (!anyU) cout << "    None\n";

//     divider('-');
//     cout << "  Doctor-wise summary (today):\n";
//     Doctor* allD = gDoctors.getAll();
//     for (int i = 0; i < gDoctors.getSize(); i++) {
//         Doctor& d = *(allD + i); int dc = 0, dp = 0, dn = 0;
//         for (int j = 0; j < gAppointments.getSize(); j++) {
//             Appointment& a = *(allA + j);
//             if (a.getDoctorID() != d.getID() || a.getDate() != todayStr) continue;
//             string st = a.getStatus();
//             if (st == "completed") dc++;
//             else if (st == "pending")   dp++;
//             else if (st == "noshow")    dn++;
//         }
//         if (dc + dp + dn > 0)
//             cout << "    Dr. " << d.getName()
//             << " | Completed:" << dc << "  Pending:" << dp << "  No-show:" << dn << "\n";
//     }
//     divider('-');
//     pause();
// }

// void runAdminMenu() {
//     int choice;
//     do {
//         printHeader("MediCore - Admin Panel");
//         cout << "  1.  Add Doctor\n"
//             << "  2.  Remove Doctor\n"
//             << "  3.  View All Patients\n"
//             << "  4.  View All Doctors\n"
//             << "  5.  View All Appointments\n"
//             << "  6.  View Unpaid Bills\n"
//             << "  7.  Discharge Patient\n"
//             << "  8.  View Security Log\n"
//             << "  9.  Generate Daily Report\n"
//             << "  10. Logout\n";
//         divider();
//         cout << "  Choice: "; choice = readInt();
//         if (!Validator::isValidMenuChoice(choice, 1, 10)) { cout << "[!] Invalid choice.\n"; pause(); continue; }
//         switch (choice) {
//         case 1:  adminAddDoctor();            break;
//         case 2:  adminRemoveDoctor();          break;
//         case 3:  adminViewAllPatients();       break;
//         case 4:  adminViewAllDoctors();        break;
//         case 5:  adminViewAllAppointments();   break;
//         case 6:  adminViewUnpaidBills();       break;
//         case 7:  adminDischargePatient();      break;
//         case 8:  adminViewSecurityLog();       break;
//         case 9:  adminDailyReport();           break;
//         case 10: cout << "[OK] Logged out.\n"; pause(); break;
//         }
//     } while (choice != 10);
// }

// // ---------------------------------------------------------------
// //  LOGIN
// // ---------------------------------------------------------------

// void loginPatient() {
//     printHeader("Patient Login / Register");
//     cout << "  1. Login\n  2. Register as new patient\n  0. Back\n";
//     divider();
//     cout << "  Choice: ";
//     int opt = readInt();
//     if (opt == 0) return;

//     if (opt == 2) {
//         printHeader("Patient Registration");
//         string name, contactStr, pwd, gStr; int age;

//         cout << "Full name: "; name = readLineStr();
//         if (name.empty()) { cout << "[!] Name cannot be empty.\n"; pause(); return; }

//         cout << "Age: "; age = readInt();
//         if (age <= 0 || age > 120) { cout << "[!] Invalid age.\n"; pause(); return; }

//         cout << "Gender (M/F): "; gStr = readLineStr();
//         char g = gStr.empty() ? ' ' : gStr[0];
//         if (g != 'M' && g != 'm' && g != 'F' && g != 'f') { cout << "[!] Gender must be M or F.\n"; pause(); return; }

//         while (true) {
//             cout << "Contact (11 digits): "; contactStr = readLineStr();
//             if (Validator::isValidContact(contactStr)) break;
//             cout << "[!] Must be exactly 11 digits.\n";
//         }
//         while (true) {
//             cout << "Password (min 6 chars): "; pwd = readLineStr();
//             if (Validator::isValidPassword(pwd)) break;
//             cout << "[!] At least 6 characters required.\n";
//         }

//         int newID = maxID(gPatients) + 1;
//         char gc = (g == 'f' || g == 'F') ? 'F' : 'M';
//         Patient np(newID, name, contactStr, pwd, 0.0, gc, age);
//         gPatients.add(np);
//         FileHandler::appendPatient(np);

//         cout << "[OK] Registered! Your Patient ID: " << newID << "\n"
//             << "    Keep it safe - you need it to log in.\n";
//         pause();
//         Patient* p = gPatients.findByID(newID);
//         if (p) runPatientMenu(p);
//         return;
//     }

//     if (opt != 1) { cout << "[!] Invalid choice.\n"; pause(); return; }

//     int fails = 0;
//     while (fails < 3) {
//         printHeader("Patient Login");
//         cout << "  Attempt " << (fails + 1) << " of 3\n\n";
//         cout << "  Patient ID : "; int id = readInt();
//         cout << "  Password   : "; string pwd = readLineStr();

//         Patient* p = gPatients.findByID(id);
//         if (p && p->getPassword() == pwd) { runPatientMenu(p); return; }

//         fails++;
//         if (id > 0) logSecurity("Patient", id, "FAILED");
//         if (fails < 3) cout << "[!] Invalid credentials. " << (3 - fails) << " attempt(s) remaining.\n";
//         pause();
//     }
//     cout << "[!] Account locked. Contact admin.\n";
//     logSecurity("Patient", -1, "LOCKED");
//     pause();
// }

// void loginDoctor() {
//     int fails = 0;
//     while (fails < 3) {
//         printHeader("Doctor Login");
//         cout << "  Attempt " << (fails + 1) << " of 3\n\n";
//         cout << "  Doctor ID  : "; int id = readInt();
//         cout << "  Password   : "; string pwd = readLineStr();

//         Doctor* d = gDoctors.findByID(id);
//         if (d && d->getPassword() == pwd) { runDoctorMenu(d); return; }

//         fails++;
//         if (id > 0) logSecurity("Doctor", id, "FAILED");
//         if (fails < 3) cout << "[!] Invalid credentials. " << (3 - fails) << " attempt(s) remaining.\n";
//         pause();
//     }
//     cout << "[!] Account locked. Contact admin.\n";
//     logSecurity("Doctor", -1, "LOCKED");
//     pause();
// }

// void loginAdmin() {
//     int fails = 0;
//     while (fails < 3) {
//         printHeader("Admin Login");
//         cout << "  Attempt " << (fails + 1) << " of 3\n\n";
//         cout << "  Admin ID   : "; int id = readInt();
//         cout << "  Password   : "; string pwd = readLineStr();

//         if (gAdmin.getID() == id && gAdmin.getPassword() == pwd) { runAdminMenu(); return; }

//         fails++;
//         if (id > 0) logSecurity("Admin", id, "FAILED");
//         if (fails < 3) cout << "[!] Invalid credentials. " << (3 - fails) << " attempt(s) remaining.\n";
//         pause();
//     }
//     cout << "[!] Account locked. Contact admin.\n";
//     logSecurity("Admin", -1, "LOCKED");
//     pause();
// }

// // ===============================================================
// //  MAIN
// // ===============================================================
// int main() {
//     loadAllData();

//     int choice;
//     do {
//         printHeader("MediCore Hospital Management System");
//         cout << "  Login as:\n\n"
//             << "  1.  Patient\n"
//             << "  2.  Doctor\n"
//             << "  3.  Admin\n"
//             << "  4.  Exit\n";
//         divider();
//         cout << "  Choice: "; choice = readInt();
//         if (!Validator::isValidMenuChoice(choice, 1, 4)) { cout << "[!] Invalid choice.\n"; pause(); continue; }
//         switch (choice) {
//         case 1: loginPatient(); break;
//         case 2: loginDoctor();  break;
//         case 3: loginAdmin();   break;
//         case 4: printHeader("Goodbye!"); cout << "  Thank you for using MediCore.\n\n"; break;
//         }
//     } while (choice != 4);

//     return 0;
// }