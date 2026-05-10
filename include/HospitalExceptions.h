#pragma once
#include <iostream>
#include <string>

using namespace std;

class HospitalException {
protected:
    string message;

public:
    HospitalException() {
        this->message = "A hospital error occurred.";
    }
    HospitalException(string msg) {
        this->message = msg;
    }

    virtual string what() const {
        return this->message;
    }

    virtual ~HospitalException() {}
};

class FileNotFoundException : public HospitalException {
public:
    FileNotFoundException(string filename) {
        this->message = "CRITICAL ERROR: Could not open " + filename + ". Ensure the file exists in the project directory.";
    }
};

class InsufficientFundsException : public HospitalException {
public:
    InsufficientFundsException() {
        this->message = "Transaction Failed: Insufficient balance. Please top up.";
    }

    InsufficientFundsException(string msg) {
        this->message = msg;
    }
};

class InvalidInputException : public HospitalException {
public:
    InvalidInputException() {
        this->message = "Validation Error: The data entered does not match the required format.";
    }

    InvalidInputException(string msg) {
        this->message = msg;
    }
};

class SlotUnavailableException : public HospitalException {
public:
    SlotUnavailableException() {
        this->message = "Booking Error: The requested time slot is already occupied by another patient.";
    }

    SlotUnavailableException(string msg) {
        this->message = msg;
    }
};