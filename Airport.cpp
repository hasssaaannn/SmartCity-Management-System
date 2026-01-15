#include "Airport.h"
#include <iostream>
using namespace std;

// Constructor
Airport::Airport(const string& id, const string& n, const string& c, 
                 const string& cityName, const string& vertex,
                 double lat, double lon)
    : airportID(id), name(n), code(c), city(cityName), vertexID(vertex), 
      latitude(lat), longitude(lon) {
}

// Destructor
Airport::~Airport() {
    // No dynamic memory, nothing to clean up
}

// Getters
string Airport::getAirportID() const {
    return airportID;
}

string Airport::getName() const {
    return name;
}

string Airport::getCode() const {
    return code;
}

string Airport::getCity() const {
    return city;
}

string Airport::getVertexID() const {
    return vertexID;
}

double Airport::getLatitude() const {
    return latitude;
}

double Airport::getLongitude() const {
    return longitude;
}

// Setters
void Airport::setAirportID(const string& id) {
    airportID = id;
}

void Airport::setName(const string& n) {
    name = n;
}

void Airport::setCode(const string& c) {
    code = c;
}

void Airport::setCity(const string& cityName) {
    city = cityName;
}

void Airport::setVertexID(const string& vertex) {
    vertexID = vertex;
}

void Airport::setCoordinates(double lat, double lon) {
    latitude = lat;
    longitude = lon;
}

// Display airport information
void Airport::display() const {
    cout << "Airport ID: " << airportID << endl;
    cout << "Name: " << name << endl;
    cout << "IATA Code: " << code << endl;
    cout << "City: " << city << endl;
    cout << "Vertex ID: " << vertexID << endl;
    cout << "Coordinates: (" << latitude << ", " << longitude << ")" << endl;
}

// Check if airport is valid
bool Airport::isValid() const {
    return !airportID.empty() && !name.empty() && !code.empty() && !vertexID.empty();
}

