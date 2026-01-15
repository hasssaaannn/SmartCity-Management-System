#include "EmergencyVehicle.h"
#include <iostream>
using namespace std;

// Constructor
EmergencyVehicle::EmergencyVehicle(const string& vehID, const string& type,
                                   const string& busNo, const string& comp,
                                   const string& currentStop)
    : Bus(busNo, comp, currentStop), vehicleID(vehID), vehicleType(type),
      isAvailable(true), priorityLevel(1), currentEmergencyID("") {
}

// Destructor
EmergencyVehicle::~EmergencyVehicle() {
    // Bus destructor will handle route cleanup
}

// Getters
string EmergencyVehicle::getVehicleType() const {
    return vehicleType;
}

string EmergencyVehicle::getVehicleID() const {
    return vehicleID;
}

bool EmergencyVehicle::getIsAvailable() const {
    return isAvailable;
}

int EmergencyVehicle::getPriorityLevel() const {
    return priorityLevel;
}

string EmergencyVehicle::getCurrentEmergencyID() const {
    return currentEmergencyID;
}

// Setters
void EmergencyVehicle::setVehicleType(const string& type) {
    vehicleType = type;
}

void EmergencyVehicle::setVehicleID(const string& vehID) {
    vehicleID = vehID;
}

void EmergencyVehicle::setIsAvailable(bool available) {
    isAvailable = available;
}

void EmergencyVehicle::setPriorityLevel(int level) {
    if (level >= 1 && level <= 3) {
        priorityLevel = level;
    }
}

void EmergencyVehicle::setCurrentEmergencyID(const string& emergencyID) {
    currentEmergencyID = emergencyID;
}

// Dispatch vehicle to an emergency
bool EmergencyVehicle::dispatchToEmergency(const string& emergencyID) {
    if (emergencyID.empty()) return false;
    if (!isAvailable) return false; // Vehicle already on a call
    
    currentEmergencyID = emergencyID;
    isAvailable = false;
    return true;
}

// Complete emergency and make vehicle available again
bool EmergencyVehicle::completeEmergency() {
    if (currentEmergencyID.empty()) return false; // Not on any emergency
    
    currentEmergencyID = "";
    isAvailable = true;
    return true;
}

// Check if vehicle is valid
bool EmergencyVehicle::isValid() const {
    return !vehicleID.empty() && !vehicleType.empty() && Bus::isValid();
}

// Display vehicle information
void EmergencyVehicle::display() const {
    cout << "Vehicle ID: " << vehicleID << endl;
    cout << "Type: " << vehicleType << endl;
    cout << "Bus Number: " << getBusNo() << endl;
    cout << "Company: " << getCompany() << endl;
    cout << "Current Stop: " << getCurrentStopID() << endl;
    cout << "Priority Level: " << priorityLevel;
    if (priorityLevel == 1) cout << " (Critical)";
    else if (priorityLevel == 2) cout << " (Urgent)";
    else cout << " (Normal)";
    cout << endl;
    cout << "Status: " << (isAvailable ? "Available" : "On Emergency Call") << endl;
    if (!isAvailable && !currentEmergencyID.empty()) {
        cout << "Current Emergency: " << currentEmergencyID << endl;
    }
    cout << "Navigation: Uses city graph edges (automatic pathfinding)" << endl;
}

