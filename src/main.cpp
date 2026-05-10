#include "raylib.h"
#define RAYGUI_IMPLEMENTATION
#include "raygui.h"
#undef RAYGUI_IMPLEMENTATION
#include "GuiTheme.h"
#include "GuiWidgets.h"
#include <cstring>
#include <fstream>
#include "Storage.h"
#include "Patient.h"
#include "Doctor.h"
#include "Admin.h"
#include "Appointment.h"
#include "Bill.h"
#include "Prescription.h"
#include "FileHandler.h"
#include "HospitalExceptions.h"
#include "UI.h"
#include "GUI.h"
using namespace std;

Storage<Patient>      gPatients(200);
Storage<Doctor>       gDoctors(100);
Storage<Appointment>  gAppointments(500);
Storage<Bill>         gBills(500);
Storage<Prescription> gPrescriptions(500);
Admin                 gAdmin;

Patient* gCurrentPatient = nullptr;
Doctor*  gCurrentDoctor  = nullptr;

static void ensureFile(const char* name) {
    ifstream f(name); if (!f.is_open()) { ofstream c(name); }
}

static void loadAllData() {
    const char* files[] = {
        "data/patients.txt","data/doctors.txt","data/admin.txt",
        "data/appointments.txt","data/bills.txt","data/prescriptions.txt",
        "data/security_log.txt","data/discharged.txt"
    };
    for (int i = 0; i < 8; i++) ensureFile(files[i]);

    try { FileHandler::loadPatients(gPatients);           } catch (...) {}
    try { FileHandler::loadDoctors(gDoctors);             } catch (...) {}
    try { FileHandler::loadAdmin(gAdmin);                 } catch (...) {}
    try { FileHandler::loadAppointments(gAppointments);   } catch (...) {}
    try { FileHandler::loadBills(gBills);                 } catch (...) {}
    try { FileHandler::loadPrescriptions(gPrescriptions); } catch (...) {}
}

static Screen* makeScreen(const char* name) {
    if (strcmp(name, "login") == 0) {
        LoginScreen* ls = new LoginScreen(gPatients, gDoctors, gAdmin);
        ls->onPatientLogin = [](Patient* p) { gCurrentPatient = p; };
        ls->onDoctorLogin  = [](Doctor*  d) { gCurrentDoctor  = d; };
        ls->onAdminLogin   = []()            {};
        return ls;
    }
    if (strcmp(name, "patient") == 0 && gCurrentPatient)
        return new PatientScreen(gCurrentPatient,
            gPatients, gDoctors, gAppointments, gBills, gPrescriptions);

    if (strcmp(name, "doctor") == 0 && gCurrentDoctor)
        return new DoctorScreen(gCurrentDoctor,
            gPatients, gDoctors, gAppointments, gBills, gPrescriptions);

    if (strcmp(name, "admin") == 0)
        return new AdminScreen(
            gPatients, gDoctors, gAppointments, gBills, gPrescriptions);

    return makeScreen("login");
}

int main() {
    loadAllData();

    InitWindow(Theme::WIN_W, Theme::WIN_H, "MediCore - Hospital Management System");
    SetTargetFPS(60);
    SetExitKey(KEY_NULL); 

    Theme::ResetToDefault();

    Theme::gFont = LoadFontEx("assets/font.ttf", 72, nullptr, 0);
    if (Theme::gFont.texture.id == 0) {
        Theme::gFont = GetFontDefault();
    } else {
        SetTextureFilter(Theme::gFont.texture, TEXTURE_FILTER_BILINEAR);
    }
    GuiSetFont(Theme::gFont);

    Screen* current = makeScreen("login");
    current->onEnter();

    while (!WindowShouldClose()) {

        SetMouseCursor(MOUSE_CURSOR_DEFAULT);

        if (current) current->update();
        UpdateToast();

        if (current) {
            const char* next = current->popNext();
            if (next && next[0] != '\0') {
                current->onExit();
                delete current;
                if (strcmp(next, "login") == 0) {
                    gCurrentPatient = nullptr;
                    gCurrentDoctor  = nullptr;
                }
                current = makeScreen(next);
                current->onEnter();
            }
        }

        BeginDrawing();
            ClearBackground(Theme::BG_DARK);
            if (current) current->draw();
            DrawToast();

        EndDrawing();
    }

    delete current;
    if (Theme::gFont.texture.id > 0) UnloadFont(Theme::gFont);
    CloseWindow();
    return 0;
}