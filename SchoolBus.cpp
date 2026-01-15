#include "SchoolBus.h"
#include <iostream>
using namespace std;

// Constructor
SchoolBus::SchoolBus(const string& busNo, const string& comp, const string& currentStop,
                     const string& school, int capacity)
    : Bus(busNo, comp, currentStop), schoolID(school), maxCapacity(capacity) {
    studentsOnBus = new SinglyLinkedList();
}

// Destructor
SchoolBus::~SchoolBus() {
    if (studentsOnBus != nullptr) {
        delete studentsOnBus;
        studentsOnBus = nullptr;
    }
}

// Getters
string SchoolBus::getSchoolID() const {
    return schoolID;
}

int SchoolBus::getMaxCapacity() const {
    return maxCapacity;
}

int SchoolBus::getCurrentStudentCount() const {
    if (studentsOnBus == nullptr) return 0;
    return studentsOnBus->getSize();
}

SinglyLinkedList* SchoolBus::getStudentsOnBus() const {
    return studentsOnBus;
}

// Setters
void SchoolBus::setSchoolID(const string& schoolID) {
    this->schoolID = schoolID;
}

void SchoolBus::setMaxCapacity(int capacity) {
    if (capacity > 0) {
        maxCapacity = capacity;
    }
}

// Add a student to the bus (boarding)
bool SchoolBus::addStudent(const string& studentID) {
    if (studentID.empty() || studentsOnBus == nullptr) return false;
    
    // Check if bus is full
    if (isFull()) {
        return false; // Bus is full
    }
    
    // Check if student is already on bus
    if (studentsOnBus->search(studentID)) {
        return false; // Student already on bus
    }
    
    // Add student
    studentsOnBus->insertAtTail(studentID);
    return true;
}

// Remove a student from the bus (alighting)
bool SchoolBus::removeStudent(const string& studentID) {
    if (studentID.empty() || studentsOnBus == nullptr) return false;
    
    return studentsOnBus->remove(studentID);
}

// Check if a student is on the bus
bool SchoolBus::isStudentOnBus(const string& studentID) const {
    if (studentID.empty() || studentsOnBus == nullptr) return false;
    
    return studentsOnBus->search(studentID);
}

// Get all students on the bus as array
void SchoolBus::getStudentsOnBus(string* students, int& count) const {
    count = 0;
    if (students == nullptr || studentsOnBus == nullptr) return;
    
    studentsOnBus->toArray(students, count);
}

// Check if bus is full
bool SchoolBus::isFull() const {
    return getCurrentStudentCount() >= maxCapacity;
}

// Display school bus information (overrides Bus::display)
void SchoolBus::display() {
    cout << "=== School Bus Information ===" << endl;
    cout << "Bus Number: " << getBusNo() << endl;
    cout << "Company: " << getCompany() << endl;
    cout << "School ID: " << schoolID << endl;
    cout << "Current Stop: " << getCurrentStopID() << endl;
    cout << "Capacity: " << getCurrentStudentCount() << " / " << maxCapacity << " students" << endl;
    cout << "Route Length: " << getRouteLength() << " stops" << endl;
    
    // Display route
    displayRoute();
    cout << endl;
    
    // Display students on bus
    if (studentsOnBus != nullptr && !studentsOnBus->isEmpty()) {
        cout << "Students on Bus (" << getCurrentStudentCount() << "): ";
        studentsOnBus->displayFormatted(", ");
        cout << endl;
    } else {
        cout << "No students currently on bus." << endl;
    }
}

// Check if school bus is valid
bool SchoolBus::isValid() const {
    return Bus::isValid() && !schoolID.empty() && maxCapacity > 0 && studentsOnBus != nullptr;
}

