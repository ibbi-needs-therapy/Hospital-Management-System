#pragma once
#include "raylib.h"
#include "raygui.h"
#include <string>
#include <functional>
#include <cstring>
#include <cstdio>
#include <ctime>
#include "UI.h"
#include "GuiTheme.h"
#include "GuiWidgets.h"
#include "Storage.h"
#include "Patient.h"
#include "Doctor.h"
#include "Admin.h"
#include "Appointment.h"
#include "Bill.h"
#include "Prescription.h"
#include "Validator.h"
#include "FileHandler.h"
#include "HospitalExceptions.h"
using namespace std;

inline void getTodayStr(char* buf) {
    time_t t = time(nullptr); tm* n = localtime(&t);
    strftime(buf, 11, "%d-%m-%Y", n);
}
inline int getCurrentYr() {
    time_t t = time(nullptr); tm* n = localtime(&t); return n->tm_year + 1900;
}
inline int maxIDStorage(Storage<Appointment>& s){ int m=0; for(int i=0;i<s.getSize();i++) if(s.getAll()[i].getID()>m)m=s.getAll()[i].getID(); return m;}
inline int maxIDStorage(Storage<Bill>&        s){ int m=0; for(int i=0;i<s.getSize();i++) if(s.getAll()[i].getID()>m)m=s.getAll()[i].getID(); return m;}
inline int maxIDStorage(Storage<Prescription>&s){ int m=0; for(int i=0;i<s.getSize();i++) if(s.getAll()[i].getID()>m)m=s.getAll()[i].getID(); return m;}
inline int maxIDStorage(Storage<Doctor>&      s){ int m=0; for(int i=0;i<s.getSize();i++) if(s.getAll()[i].getID()>m)m=s.getAll()[i].getID(); return m;}
inline int maxIDStorage(Storage<Patient>&     s){ int m=0; for(int i=0;i<s.getSize();i++) if(s.getAll()[i].getID()>m)m=s.getAll()[i].getID(); return m;}

inline bool startsWithCI(const string& str, const string& prefix) {
    if (prefix.empty()) return true;
    if (prefix.size() > str.size()) return false;
    for (size_t i = 0; i < prefix.size(); i++)
        if (Validator::toLower(str[i]) != Validator::toLower(prefix[i])) return false;
    return true;
}

inline void itostr(int v, char* buf, int bufSz) { snprintf(buf, bufSz, "%d", v); }
inline void dtostr(double v, char* buf, int bufSz) { snprintf(buf, bufSz, "%.2f", v); }

class Screen {
protected:
    char mNextScreen[32];
public:
    Screen() {
        memset(this->mNextScreen, 0, 32);
    }
    virtual ~Screen() = default;
    virtual void onEnter() {}
    virtual void onExit()  {}
    virtual void update()  = 0;
    virtual void draw()    = 0;
    const char* popNext() {
        static char buf[32];
        strncpy(buf, this->mNextScreen, 31);
        this->mNextScreen[0] = '\0';
        return buf;
    }
};

class LoginScreen : public Screen {
public:
    function<void(Patient*)> onPatientLogin;
    function<void(Doctor*)>  onDoctorLogin;
    function<void()>         onAdminLogin;

private:
    Storage<Patient>& mPts;
    Storage<Doctor>&  mDocs;
    Admin&            mAdmin;

    MessageBox mMsg;
    int        mRole      = 0;
    int        mFails     = 0;
    bool       mLocked    = false;
    bool       mRegMode   = false;

    Button mTabPat, mTabDoc, mTabAdm;

    TextBox mIdBox, mPwdBox;
    Button  mLoginBtn, mRegBtn;
    char    mFailTxt[128] = {};
    int     mFocusIdx = 0;
    TextBox mRName, mRAge, mRGender, mRContact, mRPwd;
    Button  mRSubmit, mRBack;
    char    mRegErr[128] = {};

    static constexpr float FX = 800.f;
    static constexpr float FW = 370.f;
    static constexpr float FY = 120.f;

    void setRole(int r) {
        mRole = r; mFails = 0; mLocked = false;
        mFailTxt[0] = '\0'; mIdBox.clear(); mPwdBox.clear(); mRegMode = false;        mTabPat.setStyle(r==0 ? Button::Style::Primary : Button::Style::Ghost);
        mTabDoc.setStyle(r==1 ? Button::Style::Primary : Button::Style::Ghost);
        mTabAdm.setStyle(r==2 ? Button::Style::Primary : Button::Style::Ghost);
        mRegBtn.setEnabled(r == 0);
    }

    void tryLogin() {
        if (mLocked) { mMsg.show("Locked","Account locked. Contact admin.",MessageBox::Type::Error); return; }
        const char* idStr = mIdBox.getValue();
        const char* pwd   = mPwdBox.getValue();
        if (!idStr[0] || !pwd[0]) { strncpy(mFailTxt,"[!] Fill in all fields.",127); return; }

        int id = 0;
        for (int i = 0; idStr[i]; i++) {
            if (idStr[i]<'0'||idStr[i]>'9') { strncpy(mFailTxt,"[!] Invalid ID.",127); return; }
            id = id*10+(idStr[i]-'0');
        }

        bool ok = false;
        if (mRole == 0) { Patient* p=mPts.findByID(id);  if (p && p->getPassword()==string(pwd)) { if(onPatientLogin) onPatientLogin(p); strncpy(mNextScreen,"patient",31); ok=true; } }
        if (mRole == 1) { Doctor*  d=mDocs.findByID(id); if (d && d->getPassword()==string(pwd)) { if(onDoctorLogin)  onDoctorLogin(d);  strncpy(mNextScreen,"doctor",31);  ok=true; } }
        if (mRole == 2) { if (mAdmin.getID()==id && mAdmin.getPassword()==string(pwd)) { if(onAdminLogin) onAdminLogin(); strncpy(mNextScreen,"admin",31); ok=true; } }

        if (!ok) {
            mFails++;
            if (mFails >= 3) { mLocked=true; mMsg.show("Locked","3 failed attempts. Contact admin.",MessageBox::Type::Error); }
            else { 
                char tmp[128]; snprintf(tmp,127,"[!] Invalid credentials. %d attempt(s) left.", 3-mFails); 
                strncpy(mFailTxt,tmp,127);
                ShowToast("Invalid Credentials", 2);
            }
        }
    }

    void tryRegister() {
        const char* name    = mRName.getValue();
        const char* ageStr  = mRAge.getValue();
        const char* gender  = mRGender.getValue();
        const char* contact = mRContact.getValue();
        const char* pwd     = mRPwd.getValue();

        if (!name[0]||!ageStr[0]||!gender[0]||!contact[0]||!pwd[0]) { strncpy(mRegErr,"[!] All fields required.",127); return; }

        int age = 0;
        for (int i=0;ageStr[i];i++) { if(ageStr[i]<'0'||ageStr[i]>'9'){strncpy(mRegErr,"[!] Invalid age.",127);return;} age=age*10+(ageStr[i]-'0'); }
        if (age<1||age>120) { strncpy(mRegErr,"[!] Age 1–120.",127); return; }

        char g = gender[0];
        if (g!='M'&&g!='m'&&g!='F'&&g!='f') { strncpy(mRegErr,"[!] Gender: M or F.",127); return; }
        if (!Validator::isValidContact(string(contact))) { strncpy(mRegErr,"[!] Contact must be 11 digits.",127); return; }
        if (!Validator::isValidPassword(string(pwd)))    { strncpy(mRegErr,"[!] Password min 6 chars.",127);   return; }

        int newID = maxIDStorage(mPts)+1;
        char gc = (g=='F'||g=='f') ? 'F' : 'M';
        Patient np(newID, name, contact, pwd, 0.0, gc, age);
        mPts.add(np);
        try { FileHandler::appendPatient(np); } catch(...) {}

        char msg[256]; snprintf(msg,255,"Your Patient ID is: %d\nSave it — you need it to log in.", newID);
        mMsg.show("Registered!", msg, MessageBox::Type::Info);
        mRegMode = false;
        mRegErr[0] = '\0';
    }

public:
    LoginScreen(Storage<Patient>& pts, Storage<Doctor>& docs, Admin& admin)
        : mPts(pts), mDocs(docs), mAdmin(admin)
    {
        mMsg.init();

        mTabPat.init("Patient", { FX,       FY,        110.f, 34.f }, Button::Style::Primary);
        mTabDoc.init("Doctor",  { FX+118.f, FY,        110.f, 34.f }, Button::Style::Ghost);
        mTabAdm.init("Admin",   { FX+236.f, FY,        110.f, 34.f }, Button::Style::Ghost);
        mTabPat.setCallback([this]{ setRole(0); });
        mTabDoc.setCallback([this]{ setRole(1); });
        mTabAdm.setCallback([this]{ setRole(2); });

        mIdBox.init("Your ID",   { FX, FY+100.f,  FW, Theme::INPUT_H });
        mPwdBox.init("Password", { FX, FY+190.f, FW, Theme::INPUT_H }, true);

        mLoginBtn.init("Login",    { FX,        FY+270.f, 174.f, Theme::BTN_H }, Button::Style::Primary);
        mRegBtn.init("Register",   { FX+184.f,  FY+270.f, 174.f, Theme::BTN_H }, Button::Style::Secondary);
        mLoginBtn.setCallback([this]{ tryLogin(); });
        mRegBtn.setCallback(  [this]{ mRegMode=true; mRegErr[0]='\0'; });

        float ry = FY + 80.f;
        mRName.init("Full Name",             { FX,          ry,      FW,       Theme::INPUT_H });
        mRAge.init("Age",                    { FX,          ry+56,   FW*0.35f, Theme::INPUT_H });
        mRGender.init("M or F",              { FX+FW*0.38f, ry+56,   FW*0.25f, Theme::INPUT_H });
        mRContact.init("Contact (11 digits)",{ FX,          ry+112,  FW,       Theme::INPUT_H });
        mRPwd.init("Password (min 6)",       { FX,          ry+168,  FW,       Theme::INPUT_H }, true);
        mRSubmit.init("Create Account",      { FX,          ry+228,  200.f,    Theme::BTN_H }, Button::Style::Primary);
        mRBack.init("Back",                  { FX+210,      ry+228,  148.f,    Theme::BTN_H }, Button::Style::Secondary);
        mRSubmit.setCallback([this]{ tryRegister(); });
        mRBack.setCallback(  [this]{ mRegMode=false; mRegErr[0]='\0'; });
    }

    void update() override {
        if (mMsg.update()) return;

        if (IsKeyPressed(KEY_TAB)) {
            mFocusIdx = (mFocusIdx + 1) % (mRegMode ? 7 : 4);
            if (!mRegMode) {
                mIdBox.setFocused(mFocusIdx == 0);
                mPwdBox.setFocused(mFocusIdx == 1);
            } else {
                mRName.setFocused(mFocusIdx == 0);
                mRAge.setFocused(mFocusIdx == 1);
                mRGender.setFocused(mFocusIdx == 2);
                mRContact.setFocused(mFocusIdx == 3);
                mRPwd.setFocused(mFocusIdx == 4);
            }
        }

        if (IsKeyPressed(KEY_ENTER)) { 
            if (!mRegMode) {
                if (mFocusIdx == 3) { mRegMode = true; mRegErr[0] = '\0'; }
                else tryLogin();
            } else {
                if (mFocusIdx == 6) { mRegMode = false; mRegErr[0] = '\0'; }
                else tryRegister();
            }
            return; 
        }

        mTabPat.update(); mTabDoc.update(); mTabAdm.update();
        if (!mRegMode) {
            mIdBox.update(); mPwdBox.update();
            if (mIdBox.isFocused()) mFocusIdx = 0;
            if (mPwdBox.isFocused()) mFocusIdx = 1;
            mLoginBtn.update(); mRegBtn.update();
        } else {
            mRName.update(); mRAge.update(); mRGender.update();
            mRContact.update(); mRPwd.update(); mRSubmit.update(); mRBack.update();
            if (mRName.isFocused()) mFocusIdx = 0;
            if (mRAge.isFocused()) mFocusIdx = 1;
            if (mRGender.isFocused()) mFocusIdx = 2;
            if (mRContact.isFocused()) mFocusIdx = 3;
            if (mRPwd.isFocused()) mFocusIdx = 4;
        }
    }

    void draw() override {
        DrawRectangle(0, 0, (int)FX - 10, Theme::WIN_H, Theme::BG_PANEL);
        DrawRectangle((int)FX - 10, 0, 3, Theme::WIN_H, Theme::ACCENT);

        DrawCircle(160, 180, 200, Theme::withAlpha(Theme::ACCENT, 14));
        DrawCircle(320, 560, 140, Theme::withAlpha(Theme::ACCENT, 9));

        DrawMedicalCross(100, 150, 60, Theme::ACCENT);

        Theme::DrawText("MediCore",                  50, 240, 48, Theme::TXT_PRI);
        Theme::DrawText("Hospital Management System",52, 300, Theme::FS_BODY, Theme::TXT_SEC);
        
        // Real-time clock in branding panel
        time_t now = time(nullptr);
        char timeBuf[64];
        strftime(timeBuf, 63, "%A, %B %d, %Y  |  %H:%M:%S", localtime(&now));
        Theme::DrawText(timeBuf, 50, 340, Theme::FS_SMALL, Theme::ACCENT);

        Theme::DrawText("v1.0  |  Spring 2026",      50, Theme::WIN_H - 38, Theme::FS_SMALL, Theme::TXT_DIM);

        mTabPat.draw(); mTabDoc.draw(); mTabAdm.draw();

        if (!mRegMode) {
            static const char* titles[] = {"Patient Login","Doctor Login","Admin Login"};
            Theme::DrawText(titles[mRole], (int)FX, (int)(FY + 44.f), Theme::FS_TITLE, Theme::TXT_PRI);

            drawLabel("ID",       FX, FY + 84.f);
            mIdBox.draw();
            drawLabel("Password", FX, FY + 174.f);
            mPwdBox.draw();
            if (mFailTxt[0]) Theme::DrawText(mFailTxt, (int)FX, (int)(FY + 238.f), Theme::FS_SMALL, Theme::DANGER);
            mLoginBtn.draw(); mRegBtn.draw();
            if (mFocusIdx == 2) { Rectangle r = mLoginBtn.getRect(); DrawRectangleLinesEx({r.x-2, r.y-2, r.width+4, r.height+4}, 1.5f, Theme::ACCENT); }
            if (mFocusIdx == 3) { Rectangle r = mRegBtn.getRect(); DrawRectangleLinesEx({r.x-2, r.y-2, r.width+4, r.height+4}, 1.5f, Theme::ACCENT); }
        } else {
            Theme::DrawText("New Patient Registration", (int)FX, (int)(FY + 44.f), Theme::FS_TITLE, Theme::TXT_PRI);
            float ry = FY + 80.f;
            drawLabel("Full Name",             FX,          ry - 14.f);
            mRName.draw();
            drawLabel("Age",                   FX,          ry + 42.f);
            mRAge.draw();
            drawLabel("Gender",                FX+FW*0.38f, ry + 42.f);
            mRGender.draw();
            drawLabel("Contact (11 digits)",   FX,          ry + 98.f);
            mRContact.draw();
            drawLabel("Password (min 6 chars)",FX,          ry + 154.f);
            mRPwd.draw();
            if (mRegErr[0]) Theme::DrawText(mRegErr, (int)FX, (int)(ry + 210.f), Theme::FS_SMALL, Theme::DANGER);
            mRSubmit.draw(); mRBack.draw();
            if (mFocusIdx == 5) { Rectangle r = mRSubmit.getRect(); DrawRectangleLinesEx({r.x-2, r.y-2, r.width+4, r.height+4}, 1.5f, Theme::ACCENT); }
            if (mFocusIdx == 6) { Rectangle r = mRBack.getRect(); DrawRectangleLinesEx({r.x-2, r.y-2, r.width+4, r.height+4}, 1.5f, Theme::ACCENT); }
        }

        mMsg.draw();
    }
};

class PatientScreen : public Screen {
    Patient*              mPat;
    Storage<Patient>&     mPts;
    Storage<Doctor>&      mDocs;
    Storage<Appointment>& mAppts;
    Storage<Bill>&        mBills;
    Storage<Prescription>&mPrescs;

    Sidebar    mSidebar;
    MessageBox mMsg;
    int        mPage = 0;

    char mPageTitle[64] = {};
    char mBalTxt[64]    = {};
    Table  mApptTable;
    Button mCancelBtn;

    TextBox mSpecFilter;
    Table   mDocTable;
    TextBox mDateBox, mSlotBox;
    Button  mBookBtn;
    char    mBookStatus[200] = {};
    Color   mBookStatusCol   = Theme::DANGER;
    char    mLastFilter[64] = {};
    Table mRecTable;

    Table  mBillTable;
    Button mPayBtn;
    char   mOwedTxt[64] = {};

    TextBox mTopUpBox;
    Button  mTopUpBtn;
    char    mTopUpStatus[128] = {};
    Color   mTopUpCol = Theme::SUCCESS;

    static constexpr float CX = Theme::SIDEBAR_W + Theme::PAD;
    static constexpr float CW = Theme::WIN_W - CX - Theme::PAD;

    void updateBal() {
        snprintf(this->mBalTxt, 63, "Balance: PKR %.0f", this->mPat->getBalance());
    }

    // ── Table loaders ────────────────────────────────────────
    void loadAppts() {
        this->mApptTable.clearRows();
        Appointment* all = this->mAppts.getAll();
        for (int i = 0; i < this->mAppts.getSize(); i++) {
            Appointment& a = *(all + i);
            if (a.getPatientID() != this->mPat->getID()) continue;
            Doctor* d = this->mDocs.findByID(a.getDoctorID());
            char id[12];
            itostr(a.getID(), id, 12);
            const char* cells[] = { id, d ? d->getName().c_str() : "N/A", d ? d->getSpecialization().c_str() : "N/A", a.getDate().c_str(), a.getTimeSlot().c_str(), a.getStatus().c_str() };
            Color hi = Theme::withAlpha(Theme::statusColor(a.getStatus()), 20);
            this->mApptTable.addRow(cells, 6, hi);
        }
    }

    void loadDoctorTable(const char* filter) {
        mDocTable.clearRows();
        Doctor* all = mDocs.getAll();
        for (int i = 0; i < mDocs.getSize(); i++) {
            Doctor& d = *(all+i);
            if (!startsWithCI(d.getSpecialization(), string(filter))) continue;
            char id[12], fee[20];
            itostr(d.getID(), id, 12);
            snprintf(fee, 19, "PKR %.0f", d.getFee());
            const char* cells[] = { id, d.getName().c_str(), d.getSpecialization().c_str(), fee };
            mDocTable.addRow(cells, 4);
        }
    }

    void loadBills() {
        mBillTable.clearRows();
        double total = 0.0;
        Bill* all = mBills.getAll();
        for (int i = 0; i < mBills.getSize(); i++) {
            Bill& b = *(all+i);
            if (b.getPatientID() != mPat->getID()) continue;
            if (b.getStatus() == "unpaid") total += b.getAmount();
            char bid[12], aid[12], amt[20];
            itostr(b.getID(), bid, 12); itostr(b.getAppointmentID(), aid, 12);
            snprintf(amt, 19, "PKR %.0f", b.getAmount());
            const char* cells[] = { bid, aid, amt, b.getStatus().c_str(), b.getDate().c_str() };
            Color hi = Theme::withAlpha(Theme::statusColor(b.getStatus()), 20);
            mBillTable.addRow(cells, 5, hi);
        }
        snprintf(mOwedTxt, 63, "Total outstanding: PKR %.0f", total);
    }

    void loadRecords() {
        mRecTable.clearRows();
        Prescription* all = mPrescs.getAll();
        for (int i = 0; i < mPrescs.getSize(); i++) {
            Prescription& pr = *(all+i);
            if (pr.getPatientID() != mPat->getID()) continue;
            Doctor* d = mDocs.findByID(pr.getDoctorID());
            char dname[64] = "?";
            if (d) snprintf(dname, 63, "Dr. %s", d->getName().c_str());
            const char* cells[] = { pr.getDate().c_str(), dname, pr.getMedicines().c_str(), pr.getNotes().c_str() };
            mRecTable.addRow(cells, 4);
        }
    }

    // ── Actions ──────────────────────────────────────────────
    void doCancel() {
        const TableRow* row = mApptTable.getSelectedRow();
        if (!row) { mMsg.show("No Selection","Select a pending appointment first.",MessageBox::Type::Warning); return; }
        int aid = atoi(row->cells[0]);
        Appointment* a = mAppts.findByID(aid);
        if (!a || a->getStatus() != "pending") { mMsg.show("Error","Appointment is not pending.",MessageBox::Type::Error); return; }
        
        mMsg.show("Confirm", "Cancel this appointment?", MessageBox::Type::Confirm, [this,a](bool ok){
            if (!ok) return;
            a->setStatus("cancelled");
            
            // Refund only if the bill was already paid
            Bill* allB = mBills.getAll();
            for (int i=0; i<mBills.getSize(); i++) {
                Bill& b = *(allB+i);
                if (b.getAppointmentID() == a->getID()) {
                    if (b.getStatus() == "paid") {
                        *mPat += b.getAmount();
                        mMsg.show("Refunded", "Appointment cancelled.\nFee has been refunded to your balance.", MessageBox::Type::Success);
                    } else {
                        mMsg.show("Cancelled", "Appointment cancelled.", MessageBox::Type::Success);
                    }
                    b.setStatus("cancelled");
                    break;
                }
            }
            try { FileHandler::rewriteAppointments(mAppts); FileHandler::rewriteBills(mBills); FileHandler::rewritePatients(mPts); } catch(...) {}
            updateBal(); loadAppts();
        });
    }

    void doBook() {
        mBookStatusCol = Theme::DANGER;
        const TableRow* dr = mDocTable.getSelectedRow();
        if (!dr) { strncpy(mBookStatus,"[!] Select a doctor from the table.",199); return; }
        int docID = atoi(dr->cells[0]);
        Doctor* doc = mDocs.findByID(docID);
        if (!doc) { strncpy(mBookStatus,"[!] Doctor not found.",199); return; }

        string date(mDateBox.getValue()), slot(mSlotBox.getValue());
        if (!Validator::isValidDate(date, getCurrentYr())) { strncpy(mBookStatus,"[!] Invalid date. Use DD-MM-YYYY.",199); return; }
        if (!Validator::isValidTimeSlot(slot))             { strncpy(mBookStatus,"[!] Invalid slot (09:00–16:00).",199);   return; }

        int d, m, y, hh, mm;
        sscanf(date.c_str(), "%d-%d-%d", &d, &m, &y);
        sscanf(slot.c_str(), "%d:%d", &hh, &mm);
        struct tm target = {};
        target.tm_mday = d; target.tm_mon = m-1; target.tm_year = y-1900;
        target.tm_hour = hh; target.tm_min = mm; target.tm_isdst = -1;
        time_t targetT = mktime(&target);
        time_t now = time(nullptr);
        if (targetT - now < 2 * 3600) {
            strncpy(mBookStatus,"[!] Appts must be booked at least 2 hours in advance.",199); return;
        }

        Appointment* allA = mAppts.getAll();
        for (int i=0;i<mAppts.getSize();i++) {
            Appointment& a=*(allA+i);
            if (a.getDoctorID()==docID&&a.getDate()==date&&a.getTimeSlot()==slot&&a.getStatus()!="cancelled") {
                strncpy(mBookStatus,"[!] That slot is taken. Choose another.",199); return; }
        }
        char today[11]; getTodayStr(today);
        int newAID = maxIDStorage(mAppts)+1;
        Appointment na(newAID, mPat->getID(), docID, date, slot, "pending");
        mAppts.add(na);
        int newBID = maxIDStorage(mBills)+1;
        Bill nb(newBID, mPat->getID(), newAID, doc->getFee(), "unpaid", date);
        mBills.add(nb);
        try { FileHandler::appendAppointment(na); FileHandler::appendBill(nb); FileHandler::rewritePatients(mPts); } catch(...) {}

        mBookStatusCol = Theme::SUCCESS;
        snprintf(mBookStatus,199,"[✓] Booked! Appt ID: %d", newAID);
        mMsg.show("Success", "Appointment booked successfully!\nAn unpaid bill has been generated.\nPlease pay it from 'My Bills' to confirm.", MessageBox::Type::Success);
        updateBal(); mDateBox.clear(); mSlotBox.clear();
    }

    void doPay() {
        const TableRow* row = mBillTable.getSelectedRow();
        if (!row) { mMsg.show("No Selection","Select an unpaid bill to pay.",MessageBox::Type::Warning); return; }
        int bid = atoi(row->cells[0]);
        Bill* b = mBills.findByID(bid);
        if (!b||b->getStatus()!="unpaid") { mMsg.show("Error","Bill is not unpaid.",MessageBox::Type::Error); return; }
        if (mPat->getBalance()<b->getAmount()) { mMsg.show("Insufficient Funds","Top up your balance first.",MessageBox::Type::Error); return; }
        char body[200]; snprintf(body,199,"Pay PKR %.0f for Bill #%d?", b->getAmount(), bid);
        mMsg.show("Confirm Payment", body, MessageBox::Type::Confirm, [this,b](bool ok){
            if (!ok) return;
            *mPat -= b->getAmount();
            b->setStatus("paid");
            try { FileHandler::rewriteBills(mBills); FileHandler::rewritePatients(mPts); } catch(...) {}
            updateBal(); loadBills();
        });
    }

    void doTopUp() {
        const char* s = mTopUpBox.getValue();
        double amt = 0.0; bool valid = s[0]!='\0';
        for (int i=0;s[i];i++) { if(s[i]!='.'&&(s[i]<'0'||s[i]>'9')){valid=false;break;} amt=amt*10+(s[i]-'0'); }
        if (!valid || amt <= 0) { mTopUpCol=Theme::DANGER; strncpy(mTopUpStatus,"[!] Enter a positive amount.",127); return; }
        *mPat += amt;
        try { FileHandler::rewritePatients(mPts); } catch(...) {}
        updateBal(); mTopUpBox.clear();
        mTopUpCol = Theme::SUCCESS;
        snprintf(mTopUpStatus,127,"[✓] New balance: PKR %.0f", mPat->getBalance());
        ShowToast("Balance Updated Successfully", 1);
    }

    void switchPage(int p) {
        mPage = p; mSidebar.setActive(p);
        static const char* titles[] = {"My Appointments","Book Appointment","Medical Records","My Bills","Top Up Balance"};
        strncpy(mPageTitle, titles[p], 63);
        mBookStatus[0] = '\0';
        switch(p) { case 0:loadAppts();break; case 1:loadDoctorTable("");break; case 2:loadRecords();break; case 3:loadBills();break; }
    }

public:
    PatientScreen(Patient* pat,
                  Storage<Patient>& pts, Storage<Doctor>& docs,
                  Storage<Appointment>& appts, Storage<Bill>& bills,
                  Storage<Prescription>& prescs)
        : mPat(pat), mPts(pts), mDocs(docs), mAppts(appts), mBills(bills), mPrescs(prescs)
    {
        mMsg.init();
        updateBal();

        // Appointment table
        TableCol aCols[] = {{"ID",70.f},{"Doctor",220.f},{"Spec",180.f},{"Date",120.f},{"Slot",100.f},{"Status",120.f}};
        mApptTable.init({CX,78.f,CW,500.f}, aCols, 6);
        mCancelBtn.init("Cancel Selected", {CX+CW-210.f,590.f,200.f,Theme::BTN_H}, Button::Style::Danger);
        mCancelBtn.setCallback([this]{ doCancel(); });

        // Book page
        mSpecFilter.init("Type specialization to filter...", {CX,86.f,CW*0.55f,Theme::INPUT_H});
        mLastFilter[0] = '\0';
        TableCol dCols[] = {{"ID",70.f},{"Name",240.f},{"Specialization",240.f},{"Fee",120.f}};
        mDocTable.init({CX,140.f,CW,210.f}, dCols, 4);
        mDateBox.init("DD-MM-YYYY", {CX,390.f,200.f,Theme::INPUT_H},false,10);
        mSlotBox.init("e.g. 09:00", {CX+210.f,390.f,160.f,Theme::INPUT_H},false,5);
        mBookBtn.init("Book Appointment", {CX,460.f,220.f,Theme::BTN_H}, Button::Style::Success);
        mBookBtn.setCallback([this]{ doBook(); });

        // Records table
        TableCol rCols[] = {{"Date",120.f},{"Doctor",220.f},{"Medicines",350.f},{"Notes",450.f}};
        mRecTable.init({CX,78.f,CW,560.f}, rCols, 4);

        // Bills table
        TableCol pBillCols[] = {{"Bill ID",80.f},{"Appt ID",80.f},{"Amount",120.f},{"Status",110.f},{"Date",120.f}};
        mBillTable.init({CX,78.f,CW,480.f}, pBillCols, 5);
        mPayBtn.init("Pay Selected Bill",{CX+CW-220.f,574.f,210.f,Theme::BTN_H},Button::Style::Success);
        mPayBtn.setCallback([this]{ doPay(); });

        // Top Up
        mTopUpBox.init("Amount in PKR", {CX,100.f,260.f,Theme::INPUT_H});
        mTopUpBtn.init("Top Up", {CX+270.f,97.f,120.f,Theme::BTN_H}, Button::Style::Success);
        mTopUpBtn.setCallback([this]{ doTopUp(); });

        // Sidebar
        SidebarItem items[] = {
            {"My Appointments",  [this]{ switchPage(0); }},
            {"Book Appointment", [this]{ switchPage(1); }},
            {"Medical Records",  [this]{ switchPage(2); }},
            {"My Bills",         [this]{ switchPage(3); }},
            {"Top Up Balance",   [this]{ switchPage(4); }},
        };
        char uname[72]; snprintf(uname,71,"  %s", pat->getName().c_str());
        mSidebar.init("Patient Portal", uname, items, 5, [this]{ strncpy(mNextScreen,"login",31); });
        switchPage(0);
    }

    void update() override {
        if (mMsg.update()) return;
        mSidebar.update();
        switch (mPage) {
            case 0: mApptTable.update(); mCancelBtn.update(); break;
            case 1: {
                mSpecFilter.update(); mDocTable.update();
                mDateBox.update(); mSlotBox.update(); mBookBtn.update();
                // Live filter — rebuild table if filter text changed
                const char* cur = mSpecFilter.getValue();
                if (strcmp(cur, mLastFilter) != 0) {
                    strncpy(mLastFilter, cur, 63);
                    loadDoctorTable(cur);
                }
                break;
            }
            case 2: mRecTable.update(); break;
            case 3: mBillTable.update(); mPayBtn.update(); break;
            case 4: mTopUpBox.update(); mTopUpBtn.update(); break;
        }
    }

    void draw() override {
        DrawRectangle((int)Theme::SIDEBAR_W, 0, (int)(Theme::WIN_W-Theme::SIDEBAR_W), Theme::WIN_H, Theme::BG_DARK);
        mSidebar.draw();

        Theme::DrawText(mPageTitle, (int)CX, 24, Theme::FS_TITLE, Theme::TXT_PRI);
        Theme::DrawText(mBalTxt, Theme::WIN_W - 220, 30, Theme::FS_BODY, Theme::ACCENT);

        // Real-time clock in header
        time_t now = time(nullptr);
        char timeBuf[32];
        strftime(timeBuf, 31, "%H:%M:%S", localtime(&now));
        Theme::DrawText(timeBuf, (int)CX + MeasureText(mPageTitle, Theme::FS_TITLE) + 20, 32, Theme::FS_SMALL, Theme::TXT_DIM);

        drawSep(68.f);

        switch (mPage) {
            case 0:
                mApptTable.draw(); mCancelBtn.draw(); break;
            case 1:
                drawLabel("Filter by specialization (live):", CX, 70.f);
                mSpecFilter.draw();
                mDocTable.draw();
                drawLabel("Date (DD-MM-YYYY):", CX,       364.f);
                drawLabel("Time Slot:",         CX+210.f, 364.f);
                mDateBox.draw(); mSlotBox.draw();
                if (mBookStatus[0]) Theme::DrawText(mBookStatus, (int)CX, 438, Theme::FS_SMALL, mBookStatusCol);
                mBookBtn.draw();
                break;
            case 2: mRecTable.draw(); break;
            case 3:
                mBillTable.draw();
                Theme::DrawText(mOwedTxt, (int)CX, 568, Theme::FS_BODY, Theme::WARNING);
                mPayBtn.draw();
                break;
            case 4:
                drawLabel("Enter amount to add:", CX, 82.f);
                mTopUpBox.draw(); mTopUpBtn.draw();
                if (mTopUpStatus[0]) Theme::DrawText(mTopUpStatus, (int)CX, 155, Theme::FS_BODY, mTopUpCol);
                break;
        }
        mMsg.draw();
    }
};

class DoctorScreen : public Screen {
    Doctor*               mDoc;
    Storage<Patient>&     mPts;
    Storage<Doctor>&      mDocs;
    Storage<Appointment>& mAppts;
    Storage<Bill>&        mBills;
    Storage<Prescription>&mPrescs;

    Sidebar    mSidebar;
    MessageBox mMsg;
    int        mPage = 0;
    char       mPageTitle[64] = {};

    static constexpr float CX = Theme::SIDEBAR_W + Theme::PAD;
    static constexpr float CW = Theme::WIN_W - CX - Theme::PAD;

    // Page 0 – Upcoming Schedule
    Table  mTodayTable;
    Button mCompleteBtn, mNoShowBtn;

    TextBox mPApptBox, mPMeds, mPNotes;
    Button  mPSaveBtn;
    char    mPStatus[200] = {};
    Color   mPStatusCol   = Theme::DANGER;

    TextBox mHPIDBox;
    Button  mHSearchBtn;
    Table   mHTable;
    char    mHStatus[200] = {};

    void loadToday() {
        mTodayTable.clearRows();
        char todayStr[11]; getTodayStr(todayStr);
        auto toInt = [](const string& d) {
            if (d.length() < 10) return 0;
            return (d[6]-'0')*10000000 + (d[7]-'0')*1000000 + (d[8]-'0')*100000 + (d[9]-'0')*10000 +
                   (d[3]-'0')*1000 + (d[4]-'0')*100 + (d[0]-'0')*10 + (d[1]-'0');
        };
        int todayVal = toInt(string(todayStr));

        Appointment* all = mAppts.getAll();
        for (int i=0;i<mAppts.getSize();i++) {
            Appointment& a=*(all+i);
            if (a.getDoctorID()!=mDoc->getID()) continue;
            if (toInt(a.getDate()) < todayVal) continue;

            Patient* p = mPts.findByID(a.getPatientID());
            char id[12]; itostr(a.getID(),id,12);
            const char* cells[] = { id, p?p->getName().c_str():"?", a.getDate().c_str(), a.getTimeSlot().c_str(), a.getStatus().c_str() };
            Color hi = Theme::withAlpha(Theme::statusColor(a.getStatus()),20);
            mTodayTable.addRow(cells,5,hi);
        }
    }

    void doMark(const string& status) {
        const TableRow* row = mTodayTable.getSelectedRow();
        if (!row) { mMsg.show("No Selection","Select an appointment.",MessageBox::Type::Warning); return; }
        int aid = atoi(row->cells[0]);
        Appointment* a = mAppts.findByID(aid);
        if (!a||a->getStatus()!="pending") { mMsg.show("Error","Must be your pending appointment.",MessageBox::Type::Error); return; }
        a->setStatus(status);
        if (status=="noshow") {
            Bill* allB=mBills.getAll();
            for(int i=0;i<mBills.getSize();i++){Bill& b=*(allB+i); if(b.getAppointmentID()==aid&&b.getStatus()=="unpaid"){b.setStatus("cancelled");break;}}
            try{FileHandler::rewriteBills(mBills);}catch(...){}
        }
        try{FileHandler::rewriteAppointments(mAppts);}catch(...){}
        loadToday();
        ShowToast("Appointment Status Updated", 1);
    }

    void doSavePrescription() {
        mPStatusCol = Theme::DANGER;
        const char* aidStr = mPApptBox.getValue();
        if (!aidStr[0]) { strncpy(mPStatus,"[!] Enter appointment ID.",199); return; }
        int aid = atoi(aidStr);
        Appointment* a = mAppts.findByID(aid);
        if (!a||a->getDoctorID()!=mDoc->getID()||a->getStatus()!="completed") { strncpy(mPStatus,"[!] Must be your completed appointment.",199); return; }
        Prescription* allP=mPrescs.getAll();
        for(int i=0;i<mPrescs.getSize();i++) if((allP+i)->getAppointmentID()==aid){strncpy(mPStatus,"[!] Prescription already exists.",199);return;}
        const char* meds=mPMeds.getValue(), *notes=mPNotes.getValue();
        if(!meds[0]){strncpy(mPStatus,"[!] Medicines required.",199);return;}
        char today[11]; getTodayStr(today);
        int newID=maxIDStorage(mPrescs)+1;
        Prescription pr(newID,aid,a->getPatientID(),mDoc->getID(),string(today),string(meds),string(notes));
        mPrescs.add(pr);
        try{FileHandler::appendPrescription(pr);}catch(...){}
        mPApptBox.clear(); mPMeds.clear(); mPNotes.clear();
        mPStatusCol=Theme::SUCCESS;
        snprintf(mPStatus,199,"[✓] Prescription saved. ID: %d",newID);
    }

    void doHistorySearch() {
        const char* pidStr=mHPIDBox.getValue();
        if(!pidStr[0]){strncpy(mHStatus,"[!] Enter patient ID.",199);return;}
        int pid=atoi(pidStr);
        Patient* pt=mPts.findByID(pid);
        if(!pt){strncpy(mHStatus,"[!] Patient not found.",199);return;}
        bool access=false;
        Appointment* allA=mAppts.getAll();
        for(int i=0;i<mAppts.getSize();i++){Appointment& a=*(allA+i); if(a.getPatientID()==pid&&a.getDoctorID()==mDoc->getID()&&a.getStatus()=="completed"){access=true;break;}}
        if(!access){strncpy(mHStatus,"[!] No completed appointments with this patient.",199);return;}
        mHTable.clearRows(); mHStatus[0]='\0';
        Prescription* allP=mPrescs.getAll();
        for(int i=0;i<mPrescs.getSize();i++){
            Prescription& pr=*(allP+i);
            if(pr.getPatientID()!=pid||pr.getDoctorID()!=mDoc->getID()) continue;
            const char* cells[]={pr.getDate().c_str(),pt->getName().c_str(),pr.getMedicines().c_str(),pr.getNotes().c_str()};
            mHTable.addRow(cells,4);
        }
        if(mHTable.getNumRows()==0) strncpy(mHStatus,"[i] No prescriptions found.",199);
    }

    void switchPage(int p) {
        mPage=p; mSidebar.setActive(p);
        static const char* titles[]={"Upcoming Schedule","Write Prescription","Patient History"};
        strncpy(mPageTitle,titles[p],63);
        if(p==0) loadToday();
        mPStatus[0]='\0'; mHStatus[0]='\0';
    }

public:
    DoctorScreen(Doctor* doc,
                 Storage<Patient>& pts, Storage<Doctor>& docs,
                 Storage<Appointment>& appts, Storage<Bill>& bills,
                 Storage<Prescription>& prescs)
        : mDoc(doc),mPts(pts),mDocs(docs),mAppts(appts),mBills(bills),mPrescs(prescs)
    {
        mMsg.init();
        TableCol tCols[]={{"ID",70.f},{"Patient",250.f},{"Date",120.f},{"Slot",100.f},{"Status",120.f}};
        mTodayTable.init({CX,78.f,CW,460.f},tCols,5);
        mCompleteBtn.init("Mark Complete",{CX,552.f,190.f,Theme::BTN_H},Button::Style::Success);
        mNoShowBtn.init("Mark No-Show",{CX+200.f,552.f,190.f,Theme::BTN_H},Button::Style::Danger);
        mCompleteBtn.setCallback([this]{doMark("completed");});
        mNoShowBtn.setCallback(  [this]{doMark("noshow");});

        mPApptBox.init("Completed Appointment ID",{CX,100.f,320.f,Theme::INPUT_H},false,8);
        mPMeds.init("Medicines (e.g. Paracetamol 500mg;Aspirin)",{CX,188.f,CW,Theme::INPUT_H});
        mPNotes.init("Notes (optional)",{CX,274.f,CW,Theme::INPUT_H});
        mPSaveBtn.init("Save Prescription",{CX,342.f,220.f,Theme::BTN_H},Button::Style::Primary);
        mPSaveBtn.setCallback([this]{doSavePrescription();});

        TableCol hCols[]={{"Date",120.f},{"Patient",220.f},{"Medicines",350.f},{"Notes",450.f}};
        mHTable.init({CX,160.f,CW,430.f},hCols,4);
        mHPIDBox.init("Patient ID",{CX,112.f,200.f,Theme::INPUT_H},false,8);
        mHSearchBtn.init("Search",{CX+210.f,109.f,120.f,Theme::BTN_H},Button::Style::Primary);
        mHSearchBtn.setCallback([this]{doHistorySearch();});

        SidebarItem items[]={
            {"Upcoming Schedule",  [this]{switchPage(0);}},
            {"Write Prescription", [this]{switchPage(1);}},
            {"Patient History",    [this]{switchPage(2);}},
        };
        char uname[72]; snprintf(uname,71,"  Dr. %s",doc->getName().c_str());
        mSidebar.init("Doctor Portal",uname,items,3,[this]{strncpy(mNextScreen,"login",31);});
        switchPage(0);
    }

    void update() override {
        if(mMsg.update()) return;
        mSidebar.update();
        switch(mPage){
            case 0: mTodayTable.update(); mCompleteBtn.update(); mNoShowBtn.update(); break;
            case 1: mPApptBox.update(); mPMeds.update(); mPNotes.update(); mPSaveBtn.update(); break;
            case 2: mHPIDBox.update(); mHSearchBtn.update(); mHTable.update(); break;
        }
    }

    void draw() override {
        DrawRectangle((int)Theme::SIDEBAR_W,0,(int)(Theme::WIN_W-Theme::SIDEBAR_W),Theme::WIN_H,Theme::BG_DARK);
        mSidebar.draw();
        Theme::DrawText(mPageTitle,(int)CX,24,Theme::FS_TITLE,Theme::TXT_PRI);

        // Real-time clock
        time_t now = time(nullptr);
        char timeBuf[32];
        strftime(timeBuf, 31, "%H:%M:%S", localtime(&now));
        Theme::DrawText(timeBuf, (int)CX + MeasureText(mPageTitle, Theme::FS_TITLE) + 20, 32, Theme::FS_SMALL, Theme::TXT_DIM);

        drawSep(68.f);
        switch(mPage){
            case 0: mTodayTable.draw(); mCompleteBtn.draw(); mNoShowBtn.draw(); break;
            case 1:
                drawLabel("Appointment ID (completed):",CX,76.f);
                mPApptBox.draw();
                drawLabel("Medicines:",CX,160.f); mPMeds.draw();
                drawLabel("Notes:",CX,236.f); mPNotes.draw();
                if(mPStatus[0]) Theme::DrawText(mPStatus,(int)CX,296,Theme::FS_SMALL,mPStatusCol);
                mPSaveBtn.draw(); break;
            case 2:
                drawLabel("Patient ID:",CX,88.f);
                mHPIDBox.draw(); mHSearchBtn.draw();
                if(mHStatus[0]) Theme::DrawText(mHStatus,(int)CX,108,Theme::FS_SMALL,Theme::DANGER);
                mHTable.draw(); break;
        }
        mMsg.draw();
    }
};

class AdminScreen : public Screen {
    Storage<Patient>&     mPts;
    Storage<Doctor>&      mDocs;
    Storage<Appointment>& mAppts;
    Storage<Bill>&        mBills;
    Storage<Prescription>&mPrescs;

    Sidebar    mSidebar;
    MessageBox mMsg;
    int        mPage=0;
    char       mPageTitle[64]={};

    static constexpr float CX = Theme::SIDEBAR_W + Theme::PAD;
    static constexpr float CW = Theme::WIN_W - CX - Theme::PAD;
    static constexpr float FY = 90.f;
    static constexpr float ROW = 90.f;

    // 0-Patients 1-Doctors 2-Appts 3-Bills 4-AddDoctor 5-Report 6-Appearance
    Table  mPatTable,mDocTable,mApptTable,mBillTable;
    Button mDischargeBtn,mRemoveDocBtn;
    
    int mThemeChoice = 1;
    TextBox mDName,mDSpec,mDContact,mDPwd,mDFee;
    Button  mDAddBtn;
    char    mDStatus[200]={};
    Color   mDStatusCol=Theme::DANGER;

    char mReportBuf[1024]={};

    void loadPatients() {
        mPatTable.clearRows();
        Patient* all=mPts.getAll(); Bill* allB=mBills.getAll();
        for(int i=0;i<mPts.getSize();i++){
            Patient& p=*(all+i); int ub=0;
            for(int j=0;j<mBills.getSize();j++) if((allB+j)->getPatientID()==p.getID()&&(allB+j)->getStatus()=="unpaid")ub++;
            char id[12],age[8],bal[20],ubc[8];
            itostr(p.getID(),id,12); itostr(p.getAge(),age,8);
            snprintf(bal,19,"PKR %.0f",p.getBalance()); itostr(ub,ubc,8);
            char g[2]={p.getGender(),'\0'};
            const char* cells[]={id,p.getName().c_str(),age,g,p.getContact().c_str(),bal,ubc};
            mPatTable.addRow(cells,7);
        }
    }
    void loadDoctors() {
        mDocTable.clearRows();
        Doctor* all=mDocs.getAll();
        for(int i=0;i<mDocs.getSize();i++){
            Doctor& d=*(all+i); char id[12],fee[20];
            itostr(d.getID(),id,12); snprintf(fee,19,"PKR %.0f",d.getFee());
            const char* cells[]={id,d.getName().c_str(),d.getSpecialization().c_str(),d.getContact().c_str(),fee};
            mDocTable.addRow(cells,5);
        }
    }
    void loadAppts() {
        mApptTable.clearRows();
        Appointment* all=mAppts.getAll();
        for(int i=0;i<mAppts.getSize();i++){
            Appointment& a=*(all+i);
            Patient* pt=mPts.findByID(a.getPatientID());
            Doctor*  d =mDocs.findByID(a.getDoctorID());
            char id[12]; itostr(a.getID(),id,12);
            char dname[64]="N/A"; if(d) snprintf(dname,63,"%s",d->getName().c_str());
            const char* cells[]={id,pt?pt->getName().c_str():"N/A",dname,a.getDate().c_str(),a.getTimeSlot().c_str(),a.getStatus().c_str()};
            Color hi=Theme::withAlpha(Theme::statusColor(a.getStatus()),20);
            mApptTable.addRow(cells,6,hi);
        }
    }
    void loadBills() {
        mBillTable.clearRows();
        char today[11]; getTodayStr(today);
        Bill* all=mBills.getAll();
        for(int i=0;i<mBills.getSize();i++){
            Bill& b=*(all+i); if(b.getStatus()!="unpaid") continue;
            Patient* pt=mPts.findByID(b.getPatientID());
            // date diff for overdue
            auto toInt=[](const string& d){ return (d[6]-'0')*10000000+(d[7]-'0')*1000000+(d[8]-'0')*100000+(d[9]-'0')*10000+(d[3]-'0')*1000+(d[4]-'0')*100+(d[0]-'0')*10+(d[1]-'0'); };
            bool ovd = (toInt(string(today))-toInt(b.getDate()))>7;
            char id[12],amt[20]; itostr(b.getID(),id,12); snprintf(amt,19,"PKR %.0f",b.getAmount());
            char datebuf[32]; snprintf(datebuf,31,"%s%s",b.getDate().c_str(),ovd?" [OVD]":"");
            const char* cells[]={id,pt?pt->getName().c_str():"N/A",amt,datebuf};
            Color hi=ovd?Theme::withAlpha(Theme::DANGER,20):Color{0,0,0,0};
            mBillTable.addRow(cells,4,hi);
        }
    }
    void buildReport() {
        char today[11]; getTodayStr(today);
        int tot=0,pend=0,comp=0,nosh=0,canc=0; double rev=0.0;
        Appointment* allA=mAppts.getAll();
        for(int i=0;i<mAppts.getSize();i++){Appointment& a=*(allA+i); if(a.getDate()!=string(today))continue; tot++; string st=a.getStatus(); if(st=="pending")pend++; else if(st=="completed")comp++; else if(st=="noshow")nosh++; else if(st=="cancelled")canc++;}
        Bill* allB=mBills.getAll();
        for(int i=0;i<mBills.getSize();i++){Bill& b=*(allB+i); if(b.getDate()==string(today)&&b.getStatus()=="paid")rev+=b.getAmount();}
        char part[512]={};
        snprintf(mReportBuf,1023,"Report Date: %s\n\nAppointments today: %d\n  Pending: %d  |  Completed: %d  |  No-show: %d  |  Cancelled: %d\n\nRevenue today (paid bills): PKR %.0f\n\nPatients with unpaid bills:\n",today,tot,pend,comp,nosh,canc,rev);
        Patient* allP=mPts.getAll();
        for(int i=0;i<mPts.getSize();i++){Patient& p=*(allP+i); double owed=0.0; for(int j=0;j<mBills.getSize();j++){Bill& b=*(allB+j); if(b.getPatientID()==p.getID()&&b.getStatus()=="unpaid")owed+=b.getAmount();} if(owed>0){snprintf(part,511,"  %s - PKR %.0f\n",p.getName().c_str(),owed); strncat(mReportBuf,part,1023-strlen(mReportBuf));}}
    }

    void doDischarge() {
        const TableRow* row=mPatTable.getSelectedRow();
        if(!row){mMsg.show("No Selection","Select a patient.",MessageBox::Type::Warning);return;}
        int pid=atoi(row->cells[0]);
        Patient* pt=mPts.findByID(pid); if(!pt)return;
        Bill* allB=mBills.getAll();
        for(int i=0;i<mBills.getSize();i++) if((allB+i)->getPatientID()==pid&&(allB+i)->getStatus()=="unpaid"){mMsg.show("Cannot Discharge","Patient has unpaid bills.",MessageBox::Type::Error);return;}
        Appointment* allA=mAppts.getAll();
        for(int i=0;i<mAppts.getSize();i++) if((allA+i)->getPatientID()==pid&&(allA+i)->getStatus()=="pending"){mMsg.show("Cannot Discharge","Patient has pending appointments.",MessageBox::Type::Error);return;}
        char body[200]; snprintf(body,199,"Discharge and archive %s?",pt->getName().c_str());
        mMsg.show("Confirm Discharge",body,MessageBox::Type::Confirm,[this,pid](bool ok){
            if(!ok)return;
            Patient* p=mPts.findByID(pid);
            try{FileHandler::archivePatient(*p,mAppts,mBills,mPrescs);}catch(...){}
            auto purge=[&](auto& store,auto rw){auto* all=store.getAll();int sz=store.getSize();int* ids=new int[sz];int cnt=0;for(int i=0;i<sz;i++)if((all+i)->getPatientID()==pid)ids[cnt++]=(all+i)->getID();for(int i=0;i<cnt;i++)store.removeByID(ids[i]);rw(store);delete[]ids;};
            mPts.removeByID(pid);
            try{FileHandler::rewritePatients(mPts);}catch(...){}
            purge(mAppts,   [](Storage<Appointment>&  s){try{FileHandler::rewriteAppointments(s);}catch(...){}});
            purge(mBills,   [](Storage<Bill>&          s){try{FileHandler::rewriteBills(s);}catch(...){}});
            purge(mPrescs,  [](Storage<Prescription>&  s){try{FileHandler::rewritePrescriptions(s);}catch(...){}});
            loadPatients();
        });
    }

    void doRemoveDoc() {
        const TableRow* row=mDocTable.getSelectedRow();
        if(!row){mMsg.show("No Selection","Select a doctor.",MessageBox::Type::Warning);return;}
        int did=atoi(row->cells[0]);
        Appointment* allA=mAppts.getAll();
        for(int i=0;i<mAppts.getSize();i++){Appointment& a=*(allA+i); if(a.getDoctorID()==did&&a.getStatus()=="pending"){mMsg.show("Cannot Remove","Doctor has pending appointments.",MessageBox::Type::Error);return;}}
        mMsg.show("Confirm","Remove this doctor?",MessageBox::Type::Confirm,[this,did](bool ok){
            if(!ok)return;
            mDocs.removeByID(did);
            try{FileHandler::rewriteDoctors(mDocs);}catch(...){}
            loadDoctors();
        });
    }

    void doAddDoc() {
        mDStatusCol=Theme::DANGER;
        const char* name=mDName.getValue(),*spec=mDSpec.getValue(),*contact=mDContact.getValue(),*pwd=mDPwd.getValue(),*feeStr=mDFee.getValue();
        if(!name[0]||!spec[0]||!contact[0]||!pwd[0]||!feeStr[0]){strncpy(mDStatus,"[!] All fields required.",199);return;}
        if(!Validator::isValidContact(string(contact))){strncpy(mDStatus,"[!] Contact must be 11 digits.",199);return;}
        if(!Validator::isValidPassword(string(pwd)))   {strncpy(mDStatus,"[!] Password min 6 chars.",199);return;}
        float fee=0.f;
        for(int i=0;feeStr[i];i++){if(feeStr[i]<'0'||feeStr[i]>'9'){strncpy(mDStatus,"[!] Invalid fee.",199);return;}fee=fee*10+(feeStr[i]-'0');}
        if(fee<=0){strncpy(mDStatus,"[!] Fee must be > 0.",199);return;}
        int newID=maxIDStorage(mDocs)+1;
        Doctor nd(newID,string(name),string(contact),string(pwd),string(spec),fee);
        mDocs.add(nd);
        try{FileHandler::appendDoctor(nd);}catch(...){}
        mDName.clear();mDSpec.clear();mDContact.clear();mDPwd.clear();mDFee.clear();
        mDStatusCol=Theme::SUCCESS;
        snprintf(this->mDStatus,199,"[OK] Doctor added. ID: %d",newID);
    }

    void switchPage(int p) {
        this->mPage = p;
        this->mSidebar.setActive(p);
        static const char* titles[] = { "All Patients", "All Doctors", "All Appointments", "Unpaid Bills", "Add Doctor", "Daily Report" };
        strncpy(this->mPageTitle, titles[p], 63);
        this->mDStatus[0] = '\0';
        switch (p) {
            case 0: this->loadPatients(); break;
            case 1: this->loadDoctors(); break;
            case 2: this->loadAppts(); break;
            case 3: this->loadBills(); break;
            case 5: this->buildReport(); break;
        }
    }

public:
    AdminScreen(Storage<Patient>& pts,Storage<Doctor>& docs,
                Storage<Appointment>& appts,Storage<Bill>& bills,
                Storage<Prescription>& prescs)
        : mPts(pts),mDocs(docs),mAppts(appts),mBills(bills),mPrescs(prescs)
    {
        mMsg.init();
        TableCol pCols[]={{"ID",70.f},{"Name",240.f},{"Age",60.f},{"G",50.f},{"Contact",160.f},{"Balance",120.f},{"Unpaid",100.f}};
        mPatTable.init({CX,78.f,CW,490.f},pCols,7);
        TableCol dCols[]={{"ID",70.f},{"Name",240.f},{"Spec",200.f},{"Contact",160.f},{"Fee",110.f}};
        mDocTable.init({CX,78.f,CW,490.f},dCols,5);
        TableCol aCols[]={{"ID",70.f},{"Patient",250.f},{"Doctor",220.f},{"Date",120.f},{"Slot",100.f},{"Status",120.f}};
        mApptTable.init({CX,78.f,CW,490.f},aCols,6);
        TableCol bCols[]={{"ID",70.f},{"Patient",240.f},{"Amount",120.f},{"Status",110.f}};
        mBillTable.init({CX,78.f,CW,490.f},bCols,4);

        mDischargeBtn.init("Discharge Selected",{CX+CW-230.f,582.f,220.f,Theme::BTN_H},Button::Style::Danger);
        mRemoveDocBtn.init("Remove Selected",    {CX+CW-210.f,582.f,200.f,Theme::BTN_H},Button::Style::Danger);
        mDischargeBtn.setCallback([this]{doDischarge();});
        mRemoveDocBtn.setCallback([this]{doRemoveDoc();});

        mDName.init("Doctor name",          {CX,        FY,      280.f,Theme::INPUT_H});
        mDSpec.init("Specialization",       {CX+300.f,  FY,      280.f,Theme::INPUT_H});
        mDContact.init("Contact (11 digits)",{CX,       FY+ROW,  280.f,Theme::INPUT_H}, false, 11);
        mDPwd.init("Password (min 6)",      {CX+300.f,  FY+ROW,  280.f,Theme::INPUT_H},true);
        mDFee.init("Fee in PKR",            {CX,        FY+ROW*2,200.f,Theme::INPUT_H},false,8);
        mDAddBtn.init("Add Doctor",         {CX,        FY+ROW*3,200.f,Theme::BTN_H},Button::Style::Primary);
        mDAddBtn.setCallback([this]{doAddDoc();});

        SidebarItem items[]={
            {"All Patients",     [this]{switchPage(0);}},
            {"All Doctors",      [this]{switchPage(1);}},
            {"All Appointments", [this]{switchPage(2);}},
            {"Unpaid Bills",     [this]{switchPage(3);}},
            {"Add Doctor",       [this]{switchPage(4);}},
            {"Daily Report",     [this]{switchPage(5);}},
        };
        mSidebar.init("Admin Panel","  MediCore Admin",items,6,[this]{strncpy(mNextScreen,"login",31);});
        switchPage(0);
    }

    void update() override {
        if(mMsg.update())return;
        mSidebar.update();
        switch(mPage){
            case 0: mPatTable.update();  mDischargeBtn.update(); break;
            case 1: mDocTable.update();  mRemoveDocBtn.update(); break;
            case 2: mApptTable.update(); break;
            case 3: mBillTable.update(); break;
            case 4: mDName.update();mDSpec.update();mDContact.update();mDPwd.update();mDFee.update();mDAddBtn.update(); break;
        }
    }

    void draw() override {
        DrawRectangle((int)Theme::SIDEBAR_W,0,(int)(Theme::WIN_W-Theme::SIDEBAR_W),Theme::WIN_H,Theme::BG_DARK);
        mSidebar.draw();
        Theme::DrawText(mPageTitle,(int)CX,24,Theme::FS_TITLE,Theme::TXT_PRI);

        // Real-time clock
        time_t now = time(nullptr);
        char timeBuf[32];
        strftime(timeBuf, 31, "%H:%M:%S", localtime(&now));
        Theme::DrawText(timeBuf, (int)CX + MeasureText(mPageTitle, Theme::FS_TITLE) + 20, 32, Theme::FS_SMALL, Theme::TXT_DIM);

        drawSep(68.f);
        switch(mPage){
            case 0: mPatTable.draw();  mDischargeBtn.draw(); break;
            case 1: mDocTable.draw();  mRemoveDocBtn.draw(); break;
            case 2: mApptTable.draw(); break;
            case 3: mBillTable.draw(); break;
            case 4: {
                float ly = FY - 22.f;
                drawLabel("Name",                   CX,         ly);
                drawLabel("Specialization",          CX+300.f,   ly);
                mDName.draw(); mDSpec.draw();
                drawLabel("Contact",                 CX,         ly+ROW);
                drawLabel("Password",                CX+300.f,   ly+ROW);
                mDContact.draw(); mDPwd.draw();
                drawLabel("Consultation Fee (PKR)",  CX,         ly+ROW*2);
                mDFee.draw();
                if(mDStatus[0]) Theme::DrawText(mDStatus,(int)CX,(int)(ly+ROW*2.85f),Theme::FS_SMALL,mDStatusCol);
                mDAddBtn.draw(); break;
            }
            case 5: {
                const char* p=mReportBuf; float ty=82.f; char line[256]{}; int li=0;
                while(*p){ if(*p=='\n'||li>=254){line[li]='\0';Theme::DrawText(line,(int)CX,(int)ty,Theme::FS_BODY,Theme::TXT_PRI);ty+=Theme::FS_BODY+5;li=0;}else{line[li++]=*p;}p++;}
                line[li]='\0'; if(li>0)Theme::DrawText(line,(int)CX,(int)ty,Theme::FS_BODY,Theme::TXT_PRI);
                break;
            }
        }
        mMsg.draw();
    }
};