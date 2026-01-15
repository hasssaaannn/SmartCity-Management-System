#include "Bus.h"
#include <iostream>
using namespace std;

// Constructor
Bus::Bus(const string& busNo, const string& comp, const string& currentStop)
    : busNo(busNo), company(comp), currentStopID(currentStop) {
    route = new SinglyLinkedList();
}

// Destructor
Bus::~Bus() {
    if (route != nullptr) {
        delete route;
        route = nullptr;
    }
}

// Getters
string Bus::getBusNo() const {
    return busNo;
}

string Bus::getCompany() const {
    return company;
}

string Bus::getCurrentStopID() const {
    return currentStopID;
}

SinglyLinkedList* Bus::getRoute() const {
    return route;
}

int Bus::getRouteLength() const {
    if (route == nullptr) return 0;
    return route->getSize();
}

// Setters
void Bus::setBusNo(const string& busNo) {
    this->busNo = busNo;
}

void Bus::setCompany(const string& comp) {
    company = comp;
}

void Bus::setCurrentStopID(const string& stopID) {
    currentStopID = stopID;
}

// Add a stop to the route (at the end)
bool Bus::addStopToRoute(const string& stopID) {
    if (stopID.empty() || route == nullptr) return false;
    
    // Check if stop already exists in route
    if (route->search(stopID)) {
        return false; // Stop already in route
    }
    
    route->insertAtTail(stopID);
    return true;
}

// Add a stop at a specific position in the route
bool Bus::addStopAtPosition(int position, const string& stopID) {
    if (stopID.empty() || route == nullptr) return false;
    
    // Check if stop already exists
    if (route->search(stopID)) {
        return false; // Stop already in route
    }
    
    return route->insertAtPosition(position, stopID);
}

// Remove a stop from the route
bool Bus::removeStopFromRoute(const string& stopID) {
    if (stopID.empty() || route == nullptr) return false;
    
    return route->remove(stopID);
}

// Remove stop at a specific position
bool Bus::removeStopAtPosition(int position) {
    if (route == nullptr) return false;
    
    return route->removeAtPosition(position);
}

// Get stop at a specific position in the route
string Bus::getStopAtPosition(int position) {
    if (route == nullptr) return "";
    
    return route->getAtPosition(position);
}

// Get all stops in route as array
void Bus::getRouteStops(string* stops, int& count) {
    count = 0;
    if (stops == nullptr || route == nullptr) return;
    
    route->toArray(stops, count);
}

// Check if a stop is in the route
bool Bus::isStopInRoute(const string& stopID) {
    if (stopID.empty() || route == nullptr) return false;
    
    return route->search(stopID);
}

// Update current location (move bus to next stop)
bool Bus::moveToNextStop() {
    if (route == nullptr || route->isEmpty()) return false;
    if (currentStopID.empty()) {
        // If no current location, set to first stop
        currentStopID = route->getHead();
        return true;
    }
    
    // Find current stop position in route
    int routeLength = route->getSize();
    int currentPosition = -1;
    
    for (int i = 0; i < routeLength; i++) {
        if (route->getAtPosition(i) == currentStopID) {
            currentPosition = i;
            break;
        }
    }
    
    if (currentPosition == -1) return false; // Current stop not in route
    
    // Move to next stop (circular: if at end, go to first)
    int nextPosition = (currentPosition + 1) % routeLength;
    currentStopID = route->getAtPosition(nextPosition);
    
    return true;
}

// Set current location to a specific stop (if stop is in route)
bool Bus::setCurrentLocation(const string& stopID) {
    if (stopID.empty() || route == nullptr) return false;
    
    if (route->search(stopID)) {
        currentStopID = stopID;
        return true;
    }
    
    return false; // Stop not in route
}

// Display the bus route
void Bus::displayRoute() {
    if (route == nullptr || route->isEmpty()) {
        cout << "   No route defined." << endl;
        return;
    }
    
    cout << "   Route: ";
    route->displayFormatted(" -> ");
}

// Display bus information
void Bus::display() {
    cout << "Bus Number: " << busNo << endl;
    cout << "Company: " << company << endl;
    cout << "Current Stop: " << currentStopID << endl;
    cout << "Route Length: " << getRouteLength() << " stops" << endl;
    displayRoute();
}

// Check if bus is valid
bool Bus::isValid() const {
    return !busNo.empty() && !company.empty() && route != nullptr;
}

