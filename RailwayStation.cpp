#include "RailwayStation.h"
#include <iostream>
using namespace std;

// Constructor
RailwayStation::RailwayStation(const string& id, const string& n, const string& c, 
                               const string& cityName, const string& vertex,
                               double lat, double lon)
    : stationID(id), name(n), code(c), city(cityName), vertexID(vertex), 
      latitude(lat), longitude(lon) {
}

// Destructor
RailwayStation::~RailwayStation() {
    // No dynamic memory, nothing to clean up
}

// Getters
string RailwayStation::getStationID() const {
    return stationID;
}

string RailwayStation::getName() const {
    return name;
}

string RailwayStation::getCode() const {
    return code;
}

string RailwayStation::getCity() const {
    return city;
}

string RailwayStation::getVertexID() const {
    return vertexID;
}

double RailwayStation::getLatitude() const {
    return latitude;
}

double RailwayStation::getLongitude() const {
    return longitude;
}

// Setters
void RailwayStation::setStationID(const string& id) {
    stationID = id;
}

void RailwayStation::setName(const string& n) {
    name = n;
}

void RailwayStation::setCode(const string& c) {
    code = c;
}

void RailwayStation::setCity(const string& cityName) {
    city = cityName;
}

void RailwayStation::setVertexID(const string& vertex) {
    vertexID = vertex;
}

void RailwayStation::setCoordinates(double lat, double lon) {
    latitude = lat;
    longitude = lon;
}

// Display station information
void RailwayStation::display() const {
    cout << "Station ID: " << stationID << endl;
    cout << "Name: " << name << endl;
    cout << "Station Code: " << code << endl;
    cout << "City: " << city << endl;
    cout << "Vertex ID: " << vertexID << endl;
    cout << "Coordinates: (" << latitude << ", " << longitude << ")" << endl;
}

// Check if station is valid
bool RailwayStation::isValid() const {
    return !stationID.empty() && !name.empty() && !code.empty() && !vertexID.empty();
}

