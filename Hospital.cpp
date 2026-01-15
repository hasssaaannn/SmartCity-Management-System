#include "Hospital.h"
using namespace std;

// Default constructor
Hospital::Hospital() : hospitalID(""), name(""), sector(""), emergencyBeds(0), specialization("") {
}

// Parameterized constructor
Hospital::Hospital(const string& id, const string& n, const string& sec, 
                   int beds, const string& spec) 
    : hospitalID(id), name(n), sector(sec), emergencyBeds(beds), specialization(spec) {
}

// Destructor
Hospital::~Hospital() {
}

// Getters
string Hospital::getHospitalID() const {
    return hospitalID;
}

string Hospital::getName() const {
    return name;
}

string Hospital::getSector() const {
    return sector;
}

int Hospital::getEmergencyBeds() const {
    return emergencyBeds;
}

string Hospital::getSpecialization() const {
    return specialization;
}

// Setters
void Hospital::setHospitalID(const string& id) {
    hospitalID = id;
}

void Hospital::setName(const string& n) {
    name = n;
}

void Hospital::setSector(const string& sec) {
    sector = sec;
}

void Hospital::setEmergencyBeds(int beds) {
    emergencyBeds = beds >= 0 ? beds : 0;  // Ensure non-negative
}

void Hospital::setSpecialization(const string& spec) {
    specialization = spec;
}

// Check if hospital has available emergency beds
bool Hospital::hasAvailableBeds() const {
    return emergencyBeds > 0;
}

