#include "RailwayManager.h"
#include <iostream>
#include <cmath>
using namespace std;

const double M_PI = 3.14159265358979323846;

// Helper function to parse coordinates from string "lat, lon"
static bool parseCoordinates(const string& coordStr, double& lat, double& lon) {
    if (coordStr.empty()) return false;
    
    string trimmed = coordStr;
    if (trimmed.length() >= 2 && trimmed[0] == '"' && trimmed[trimmed.length() - 1] == '"') {
        trimmed = trimmed.substr(1, trimmed.length() - 2);
    }
    
    size_t commaPos = trimmed.find(',');
    if (commaPos == string::npos) return false;
    
    string latStr = trimmed.substr(0, commaPos);
    string lonStr = trimmed.substr(commaPos + 1);
    
    // Trim whitespace
    size_t start = 0, end = latStr.length() - 1;
    while (start < latStr.length() && (latStr[start] == ' ' || latStr[start] == '\t')) start++;
    while (end > start && (latStr[end] == ' ' || latStr[end] == '\t')) end--;
    if (start <= end) latStr = latStr.substr(start, end - start + 1);
    
    start = 0; end = lonStr.length() - 1;
    while (start < lonStr.length() && (lonStr[start] == ' ' || lonStr[start] == '\t')) start++;
    while (end > start && (lonStr[end] == ' ' || lonStr[end] == '\t')) end--;
    if (start <= end) lonStr = lonStr.substr(start, end - start + 1);
    
    // Convert to double (simple conversion)
    lat = 0.0; lon = 0.0;
    bool hasDecimal = false;
    double fraction = 0.1;
    
    // Parse latitude
    bool isNegative = false;
    int i = 0;
    if (!latStr.empty() && latStr[0] == '-') {
        isNegative = true;
        i = 1;
    }
    for (; i < (int)latStr.length(); i++) {
        if (latStr[i] >= '0' && latStr[i] <= '9') {
            if (!hasDecimal) {
                lat = lat * 10.0 + (latStr[i] - '0');
            } else {
                lat += (latStr[i] - '0') * fraction;
                fraction *= 0.1;
            }
        } else if (latStr[i] == '.' && !hasDecimal) {
            hasDecimal = true;
        }
    }
    if (isNegative) lat = -lat;
    
    // Parse longitude
    hasDecimal = false;
    fraction = 0.1;
    isNegative = false;
    i = 0;
    if (!lonStr.empty() && lonStr[0] == '-') {
        isNegative = true;
        i = 1;
    }
    for (; i < (int)lonStr.length(); i++) {
        if (lonStr[i] >= '0' && lonStr[i] <= '9') {
            if (!hasDecimal) {
                lon = lon * 10.0 + (lonStr[i] - '0');
            } else {
                lon += (lonStr[i] - '0') * fraction;
                fraction *= 0.1;
            }
        } else if (lonStr[i] == '.' && !hasDecimal) {
            hasDecimal = true;
        }
    }
    if (isNegative) lon = -lon;
    
    return true;
}

// Helper function to calculate distance between two coordinates (Haversine formula)
static double calculateDistance(double lat1, double lon1, double lat2, double lon2) {
    const double R = 6371.0; // Earth radius in kilometers
    
    double dLat = (lat2 - lat1) * M_PI / 180.0;
    double dLon = (lon2 - lon1) * M_PI / 180.0;
    
    double a = sin(dLat / 2.0) * sin(dLat / 2.0) +
               cos(lat1 * M_PI / 180.0) * cos(lat2 * M_PI / 180.0) *
               sin(dLon / 2.0) * sin(dLon / 2.0);
    
    double c = 2.0 * atan2(sqrt(a), sqrt(1.0 - a));
    double distance = R * c;
    
    return distance;
}

// Constructor
RailwayManager::RailwayManager(Graph* graph)
    : cityGraph(graph), stationCount(0) {
    // Initialize hash tables with reasonable sizes (prime numbers)
    stationsByID = new HashTable(101);
    stationsByName = new HashTable(101);
    stationsByCode = new HashTable(31); // Smaller size for codes (fewer codes than stations)
    allStationsList = new SinglyLinkedList();
}

// Destructor
RailwayManager::~RailwayManager() {
    clear();
    
    // Delete hash tables and list
    if (stationsByID != nullptr) {
        delete stationsByID;
        stationsByID = nullptr;
    }
    if (stationsByName != nullptr) {
        delete stationsByName;
        stationsByName = nullptr;
    }
    if (stationsByCode != nullptr) {
        delete stationsByCode;
        stationsByCode = nullptr;
    }
    if (allStationsList != nullptr) {
        delete allStationsList;
        allStationsList = nullptr;
    }
}

// Set the city graph
void RailwayManager::setCityGraph(Graph* graph) {
    cityGraph = graph;
}

// Get the city graph
Graph* RailwayManager::getCityGraph() const {
    return cityGraph;
}

// Helper function to get all stations as array
void RailwayManager::getAllStations(RailwayStation** stations, int& count) const {
    count = 0;
    if (stations == nullptr || stationCount == 0) return;
    
    // Get all station IDs from linked list
    int idCount = allStationsList->getSize();
    if (idCount == 0) return;
    
    string* stationIDs = new string[idCount];
    int actualCount = 0;
    allStationsList->toArray(stationIDs, actualCount);
    
    // Convert IDs to RailwayStation pointers
    for (int i = 0; i < actualCount; i++) {
        RailwayStation* station = findStationByID(stationIDs[i]);
        if (station != nullptr) {
            stations[count] = station;
            count++;
        }
    }
    
    delete[] stationIDs;
}

// Add a station to the manager
bool RailwayManager::addStation(RailwayStation* station) {
    if (station == nullptr) return false;
    if (!station->isValid()) return false;
    
    // Check if station already exists
    if (findStationByID(station->getStationID()) != nullptr) {
        return false; // Station already exists
    }
    
    // Add to all hash tables
    stationsByID->insert(station->getStationID(), (void*)station);
    stationsByName->insert(station->getName(), (void*)station);
    stationsByCode->insert(station->getCode(), (void*)station);
    
    // Add station ID to list for iteration
    allStationsList->insertAtTail(station->getStationID());
    
    // Add station to graph and connect to nearest bus stop
    if (cityGraph != nullptr) {
        cityGraph->addVertex(station->getStationID(), station->getName(), 
                            station->getLatitude(), station->getLongitude(), station);
        cityGraph->connectToNearestStop(station->getStationID());
        
        int listSize = allStationsList->getSize();
        if (listSize > 1) { // Only if there are other stations 
            RailwayStation** allStations = new RailwayStation*[listSize];
            int allCount = 0;
            getAllStations(allStations, allCount);
            
            for (int i = 0; i < allCount; i++) {
                if (allStations[i]->getStationID() != station->getStationID()) {
                    // Calculate rail distance (straight line)
                    double railDist = calculateDistance(station->getLatitude(), station->getLongitude(),
                                                       allStations[i]->getLatitude(),
                                                       allStations[i]->getLongitude());
                    // Add edge with rail distance (bidirectional)
                    cityGraph->addEdge(station->getStationID(), allStations[i]->getStationID(), railDist);
                }
            }
            
            delete[] allStations;
        }
    }
    
    stationCount++;
    return true;
}

// Find a station by ID
RailwayStation* RailwayManager::findStationByID(const string& stationID) const {
    if (stationID.empty()) return nullptr;
    
    void* data = stationsByID->search(stationID);
    if (data != nullptr) {
        return (RailwayStation*)data;
    }
    return nullptr;
}

// Find a station by name
RailwayStation* RailwayManager::findStationByName(const string& name) const {
    if (name.empty()) return nullptr;
    
    void* data = stationsByName->search(name);
    if (data != nullptr) {
        return (RailwayStation*)data;
    }
    return nullptr;
}

// Find a station by code
RailwayStation* RailwayManager::findStationByCode(const string& code) const {
    if (code.empty()) return nullptr;
    
    void* data = stationsByCode->search(code);
    if (data != nullptr) {
        return (RailwayStation*)data;
    }
    return nullptr;
}

// Get number of stations
int RailwayManager::getStationCount() const {
    return stationCount;
}

// Display all stations
void RailwayManager::displayAllStations() const {
    if (stationCount == 0) {
        cout << "No railway stations registered." << endl;
        return;
    }
    
    cout << "Registered Railway Stations (" << stationCount << " stations):" << endl;
    
    // Get all stations
    RailwayStation** stations = new RailwayStation*[stationCount];
    int count = 0;
    getAllStations(stations, count);
    
    // Display each station
    for (int i = 0; i < count; i++) {
        cout << "  [" << (i + 1) << "] ";
        stations[i]->display();
        cout << endl;
    }
    
    delete[] stations;
}

// Find nearest station to a given stop ID
RailwayStation* RailwayManager::findNearestStation(const string& fromStopID) {
    if (fromStopID.empty() || cityGraph == nullptr || stationCount == 0) {
        return nullptr;
    }
    
    // Get stop coordinates
    double stopLat, stopLon;
    if (!cityGraph->getVertexCoordinates(fromStopID, stopLat, stopLon)) {
        return nullptr;
    }
    
    return findNearestStation(stopLat, stopLon);
}

// Find nearest station to given coordinates
RailwayStation* RailwayManager::findNearestStation(double latitude, double longitude) {
    if (stationCount == 0) return nullptr;
    
    RailwayStation* nearestStation = nullptr;
    double minDistance = -1.0;
    
    // Get all stations
    RailwayStation** stations = new RailwayStation*[stationCount];
    int count = 0;
    getAllStations(stations, count);
    
    // Find nearest station
    for (int i = 0; i < count; i++) {
        double dist = calculateDistance(latitude, longitude,
                                       stations[i]->getLatitude(), stations[i]->getLongitude());
        if (minDistance < 0.0 || dist < minDistance) {
            minDistance = dist;
            nearestStation = stations[i];
        }
    }
    
    delete[] stations;
    return nearestStation;
}

// Calculate rail distance between two stations (straight line distance)
bool RailwayManager::calculateRailDistance(const string& fromStationID, const string& toStationID, double& distance) {
    distance = 0.0;
    if (fromStationID.empty() || toStationID.empty()) return false;
    
    RailwayStation* fromStation = findStationByID(fromStationID);
    RailwayStation* toStation = findStationByID(toStationID);
    
    if (fromStation == nullptr || toStation == nullptr) return false;
    
    // Calculate rail distance (straight line using Haversine formula)
    distance = calculateDistance(fromStation->getLatitude(), fromStation->getLongitude(),
                                 toStation->getLatitude(), toStation->getLongitude());
    
    return true;
}

// Find path from one station to another (direct train route)
bool RailwayManager::findStationPath(const string& fromStationID, const string& toStationID, 
                                     string* path, int& pathLength, double& distance) {
    pathLength = 0;
    distance = 0.0;
    if (fromStationID.empty() || toStationID.empty() || path == nullptr) return false;
    
    RailwayStation* fromStation = findStationByID(fromStationID);
    RailwayStation* toStation = findStationByID(toStationID);
    
    if (fromStation == nullptr || toStation == nullptr) return false;
    
    // Direct train route path: fromStation -> toStation
    path[0] = fromStationID;
    path[1] = toStationID;
    pathLength = 2;
    
    // Calculate rail distance
    distance = calculateDistance(fromStation->getLatitude(), fromStation->getLongitude(),
                                 toStation->getLatitude(), toStation->getLongitude());
    
    return true;
}

// Load stations from CSV data
bool RailwayManager::loadStationsFromCSVData(CSVRow* stationData, int stationCount) {
    if (stationData == nullptr || stationCount <= 0) return false;
    
    // fields[0]=StationID, [1]=Name, [2]=Code, [3]=City, [4]=Coordinates
    
    for (int i = 0; i < stationCount; i++) {
        if (stationData[i].fieldCount < 5) continue; // Skip invalid rows
        
        string stationID = stationData[i].fields[0];
        string name = stationData[i].fields[1];
        string code = stationData[i].fields[2];
        string city = stationData[i].fields[3];
        string coordStr = stationData[i].fields[4];
        
        // Parse coordinates from CSV
        double lat = 0.0, lon = 0.0;
        
        if (!parseCoordinates(coordStr, lat, lon)) {
            // Invalid coordinates, skip this station
            continue;
        }
        
        // Create new station (use stationID as vertexID)
        RailwayStation* station = new RailwayStation(stationID, name, code, city, stationID, lat, lon);
        
        if (!addStation(station)) {
            // If add failed (duplicate), delete the station to avoid memory leak
            delete station;
            continue;
        }
    }
    
    return true;
}

// Clear all stations
void RailwayManager::clear() {
    // Get all stations and delete them
    RailwayStation** stations = new RailwayStation*[stationCount];
    int count = 0;
    getAllStations(stations, count);
    
    // Delete each station
    for (int i = 0; i < count; i++) {
        delete stations[i];
    }
    
    delete[] stations;
    
    // Clear hash tables and list
    if (stationsByID != nullptr) stationsByID->clear();
    if (stationsByName != nullptr) stationsByName->clear();
    if (stationsByCode != nullptr) stationsByCode->clear();
    if (allStationsList != nullptr) allStationsList->clear();
    
    stationCount = 0;
}

