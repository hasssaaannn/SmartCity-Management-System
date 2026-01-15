#include "Facility.h"
#include <iostream>
using namespace std;

// Constructor
Facility::Facility(const string& id, const string& n, const string& t, 
                  const string& sec, const string& vertex)
    : facilityID(id), name(n), type(t), sector(sec), vertexID(vertex) {
}

// Destructor
Facility::~Facility() {
}

// Getters
string Facility::getFacilityID() const {
    return facilityID;
}

string Facility::getName() const {
    return name;
}

string Facility::getType() const {
    return type;
}

string Facility::getSector() const {
    return sector;
}

string Facility::getVertexID() const {
    return vertexID;
}

// Setters
void Facility::setFacilityID(const string& id) {
    facilityID = id;
}

void Facility::setName(const string& n) {
    name = n;
}

void Facility::setType(const string& t) {
    type = t;
}

void Facility::setSector(const string& sec) {
    sector = sec;
}

void Facility::setVertexID(const string& vertex) {
    vertexID = vertex;
}

// Display facility information
void Facility::display() const {
    cout << "Facility ID: " << facilityID << endl;
    cout << "Name: " << name << endl;
    cout << "Type: " << type << endl;
    cout << "Sector: " << sector << endl;
    cout << "Vertex ID: " << vertexID << endl;
}

// Check if facility is valid
bool Facility::isValid() const {
    return !facilityID.empty() && !name.empty() && !type.empty() && !vertexID.empty();
}

