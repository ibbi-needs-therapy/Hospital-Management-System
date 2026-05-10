#pragma once
#include <string>
#include "HospitalExceptions.h"

using namespace std;

class Validator {
public:

    static bool isValidID(int id) {
        return id > 0;
    }

    static bool isPositiveFloat(float val) {
        return val > 0.0f;
    }

    static bool isPositiveDouble(double val) {
        return val > 0.0;
    }

    static bool isValidContact(string contact) {
        if (contact.length() != 11) return false;
        for (int i = 0; i < 11; i++) {
            if (contact[i] < '0' || contact[i] > '9') {
                cout << "Contact cannot have non-numeric character!\n";
                return false;
            }
        }
        return true;
    }

    static bool isValidPassword(string pwd) {
        return pwd.length() >= 6;
    }

    static bool isValidMenuChoice(int choice, int lo, int hi) {
        return choice >= lo && choice <= hi;
    }

    static bool isValidDate(string date, int currentYear) {
        if (date.length() != 10) return false;
        if (date[2] != '-' || date[5] != '-') return false;

        int day = charToInt(date[0]) * 10 + charToInt(date[1]);
        int month = charToInt(date[3]) * 10 + charToInt(date[4]);
        int year = charToInt(date[6]) * 1000 + charToInt(date[7]) * 100
            + charToInt(date[8]) * 10 + charToInt(date[9]);

        if (day < 1 || day > 31) return false;
        if (month < 1 || month > 12) return false;
        if (year < currentYear) return false;

        int daysInMonth[] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

        if ((year % 4 == 0 && year % 100 != 0) || year % 400 == 0)
            daysInMonth[1] = 29;

        if (day > daysInMonth[month - 1]) return false;

        return true;
    }

    static bool isValidTimeSlot(string slot) {
        string valid[] = {
            "09:00", "10:00", "11:00", "12:00",
            "13:00", "14:00", "15:00", "16:00"
        };
        for (int i = 0; i < 8; i++) {
            if (slot == valid[i]) return true;
        }
        return false;
    }

    static bool EqualCI(string a, string b) {
        if (a.length() != b.length()) return false;
        for (size_t i = 0; i < a.length(); i++) {
            if (toLower(a[i]) != toLower(b[i])) return false;
        }
        return true;
    }

    static char toLower(char c) {
        if (c >= 'A' && c <= 'Z') return c + 32;
        return c;
    }

private:
    static int charToInt(char c) {
        if (c < '0' || c > '9') return -1;
        return c - '0';
    }
};