#include "AirportManager.h"
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
AirportManager::AirportManager(Graph* graph)
    : cityGraph(graph), airportCount(0) {
    // Initialize hash tables with reasonable sizes (prime numbers)
    airportsByID = new HashTable(101);
    airportsByName = new HashTable(101);
    airportsByCode = new HashTable(31); // Smaller size for codes (fewer codes than airports)
    allAirportsList = new SinglyLinkedList();
}

// Destructor
AirportManager::~AirportManager() {
    clear();
    
    // Delete hash tables and list
    if (airportsByID != nullptr) {
        delete airportsByID;
        airportsByID = nullptr;
    }
    if (airportsByName != nullptr) {
        delete airportsByName;
        airportsByName = nullptr;
    }
    if (airportsByCode != nullptr) {
        delete airportsByCode;
        airportsByCode = nullptr;
    }
    if (allAirportsList != nullptr) {
        delete allAirportsList;
        allAirportsList = nullptr;
    }
}

// Set the city graph
void AirportManager::setCityGraph(Graph* graph) {
    cityGraph = graph;
}

// Get the city graph
Graph* AirportManager::getCityGraph() const {
    return cityGraph;
}

// Helper function to get all airports as array
void AirportManager::getAllAirports(Airport** airports, int& count) const {
    count = 0;
    if (airports == nullptr || airportCount == 0) return;
    
    // Get all airport IDs from linked list
    int idCount = allAirportsList->getSize();
    if (idCount == 0) return;
    
    string* airportIDs = new string[idCount];
    int actualCount = 0;
    allAirportsList->toArray(airportIDs, actualCount);
    
    // Convert IDs to Airport pointers
    for (int i = 0; i < actualCount; i++) {
        Airport* airport = findAirportByID(airportIDs[i]);
        if (airport != nullptr) {
            airports[count] = airport;
            count++;
        }
    }
    
    delete[] airportIDs;
}

// Add an airport to the manager
bool AirportManager::addAirport(Airport* airport) {
    if (airport == nullptr) return false;
    if (!airport->isValid()) return false;
    
    // Check if airport already exists
    if (findAirportByID(airport->getAirportID()) != nullptr) {
        return false; // Airport already exists
    }
    
    // Add to all hash tables
    airportsByID->insert(airport->getAirportID(), (void*)airport);
    airportsByName->insert(airport->getName(), (void*)airport);
    airportsByCode->insert(airport->getCode(), (void*)airport);
    
    // Add airport ID to list for iteration
    allAirportsList->insertAtTail(airport->getAirportID());
    
    // Add airport to graph and connect to nearest bus stop
    if (cityGraph != nullptr) {
        cityGraph->addVertex(airport->getAirportID(), airport->getName(), 
                            airport->getLatitude(), airport->getLongitude(), airport);
        cityGraph->connectToNearestStop(airport->getAirportID());
        
        // Connect this airport to all other existing airports for direct flights
        // Use linked list size to determine array size (includes the airport we just added)
        int listSize = allAirportsList->getSize();
        if (listSize > 1) { // Only if there are other airports (listSize includes the one we just added)
            Airport** allAirports = new Airport*[listSize];
            int allCount = 0;
            getAllAirports(allAirports, allCount);
            
            for (int i = 0; i < allCount; i++) {
                if (allAirports[i]->getAirportID() != airport->getAirportID()) {
                    // Calculate air distance
                    double airDist = calculateDistance(airport->getLatitude(), airport->getLongitude(),
                                                      allAirports[i]->getLatitude(),
                                                      allAirports[i]->getLongitude());
                    // Add edge with air distance (bidirectional)
                    cityGraph->addEdge(airport->getAirportID(), allAirports[i]->getAirportID(), airDist);
                }
            }
            
            delete[] allAirports;
        }
    }
    
    airportCount++;
    return true;
}

// Find an airport by ID
Airport* AirportManager::findAirportByID(const string& airportID) const {
    if (airportID.empty()) return nullptr;
    
    void* data = airportsByID->search(airportID);
    if (data != nullptr) {
        return (Airport*)data;
    }
    return nullptr;
}

// Find an airport by name
Airport* AirportManager::findAirportByName(const string& name) const {
    if (name.empty()) return nullptr;
    
    void* data = airportsByName->search(name);
    if (data != nullptr) {
        return (Airport*)data;
    }
    return nullptr;
}

// Find an airport by IATA code
Airport* AirportManager::findAirportByCode(const string& code) const {
    if (code.empty()) return nullptr;
    
    void* data = airportsByCode->search(code);
    if (data != nullptr) {
        return (Airport*)data;
    }
    return nullptr;
}

// Get number of airports
int AirportManager::getAirportCount() const {
    return airportCount;
}

// Display all airports
void AirportManager::displayAllAirports() const {
    if (airportCount == 0) {
        cout << "No airports registered." << endl;
        return;
    }
    
    cout << "Registered Airports (" << airportCount << " airports):" << endl;
    
    // Get all airports
    Airport** airports = new Airport*[airportCount];
    int count = 0;
    getAllAirports(airports, count);
    
    // Display each airport
    for (int i = 0; i < count; i++) {
        cout << "  [" << (i + 1) << "] ";
        airports[i]->display();
        cout << endl;
    }
    
    delete[] airports;
}

// Find nearest airport to a given stop ID
Airport* AirportManager::findNearestAirport(const string& fromStopID) {
    if (fromStopID.empty() || cityGraph == nullptr || airportCount == 0) {
        return nullptr;
    }
    
    // Get stop coordinates
    double stopLat, stopLon;
    if (!cityGraph->getVertexCoordinates(fromStopID, stopLat, stopLon)) {
        return nullptr;
    }
    
    return findNearestAirport(stopLat, stopLon);
}

// Find nearest airport to given coordinates
Airport* AirportManager::findNearestAirport(double latitude, double longitude) {
    if (airportCount == 0) return nullptr;
    
    Airport* nearestAirport = nullptr;
    double minDistance = -1.0;
    
    // Get all airports
    Airport** airports = new Airport*[airportCount];
    int count = 0;
    getAllAirports(airports, count);
    
    // Find nearest airport
    for (int i = 0; i < count; i++) {
        double dist = calculateDistance(latitude, longitude,
                                       airports[i]->getLatitude(), airports[i]->getLongitude());
        if (minDistance < 0.0 || dist < minDistance) {
            minDistance = dist;
            nearestAirport = airports[i];
        }
    }
    
    delete[] airports;
    return nearestAirport;
}

// Calculate air distance between two airports (straight line distance)
bool AirportManager::calculateAirDistance(const string& fromAirportID, const string& toAirportID, double& distance) {
    distance = 0.0;
    if (fromAirportID.empty() || toAirportID.empty()) return false;
    
    Airport* fromAirport = findAirportByID(fromAirportID);
    Airport* toAirport = findAirportByID(toAirportID);
    
    if (fromAirport == nullptr || toAirport == nullptr) return false;
    
    // Calculate air distance (straight line using Haversine formula)
    distance = calculateDistance(fromAirport->getLatitude(), fromAirport->getLongitude(),
                                 toAirport->getLatitude(), toAirport->getLongitude());
    
    return true;
}

// Find path from one airport to another (direct flight)
bool AirportManager::findAirportPath(const string& fromAirportID, const string& toAirportID, 
                                     string* path, int& pathLength, double& distance) {
    pathLength = 0;
    distance = 0.0;
    if (fromAirportID.empty() || toAirportID.empty() || path == nullptr) return false;
    
    Airport* fromAirport = findAirportByID(fromAirportID);
    Airport* toAirport = findAirportByID(toAirportID);
    
    if (fromAirport == nullptr || toAirport == nullptr) return false;
    
    // Direct flight path: fromAirport -> toAirport
    path[0] = fromAirportID;
    path[1] = toAirportID;
    pathLength = 2;
    
    // Calculate air distance
    distance = calculateDistance(fromAirport->getLatitude(), fromAirport->getLongitude(),
                                 toAirport->getLatitude(), toAirport->getLongitude());
    
    return true;
}

// Load airports from CSV data
bool AirportManager::loadAirportsFromCSVData(CSVRow* airportData, int airportCount) {
    if (airportData == nullptr || airportCount <= 0) return false;
    
    // fields[0]=AirportID, [1]=Name, [2]=Code, [3]=City, [4]=Coordinates
    
    for (int i = 0; i < airportCount; i++) {
        if (airportData[i].fieldCount < 5) continue; // Skip invalid rows
        
        string airportID = airportData[i].fields[0];
        string name = airportData[i].fields[1];
        string code = airportData[i].fields[2];
        string city = airportData[i].fields[3];
        string coordStr = airportData[i].fields[4];
        
        // Parse coordinates from CSV
        double lat = 0.0, lon = 0.0;
        
        if (!parseCoordinates(coordStr, lat, lon)) {
            // Invalid coordinates, skip this airport
            continue;
        }
        
        // Create new airport (use airportID as vertexID)
        Airport* airport = new Airport(airportID, name, code, city, airportID, lat, lon);
        
        // Add to manager (this will automatically connect to nearest bus stop)
        if (!addAirport(airport)) {
            // If add failed (duplicate), delete the airport to avoid memory leak
            delete airport;
            continue;
        }
        
        // Connect this airport to all other airports for direct air travel
        if (cityGraph != nullptr) {
            Airport** allAirports = new Airport*[this->airportCount];
            int allCount = 0;
            getAllAirports(allAirports, allCount);
            
            for (int j = 0; j < allCount; j++) {
                if (allAirports[j]->getAirportID() != airportID) {
                    // Calculate air distance
                    double airDist = calculateDistance(lat, lon,
                                                      allAirports[j]->getLatitude(),
                                                      allAirports[j]->getLongitude());
                    // Add edge with air distance (bidirectional)
                    cityGraph->addEdge(airportID, allAirports[j]->getAirportID(), airDist);
                }
            }
            
            delete[] allAirports;
        }
    }
    
    return true;
}

// Clear all airports
void AirportManager::clear() {
    // Get all airports and delete them
    Airport** airports = new Airport*[airportCount];
    int count = 0;
    getAllAirports(airports, count);
    
    // Delete each airport
    for (int i = 0; i < count; i++) {
        delete airports[i];
    }
    
    delete[] airports;
    
    // Clear hash tables and list
    if (airportsByID != nullptr) airportsByID->clear();
    if (airportsByName != nullptr) airportsByName->clear();
    if (airportsByCode != nullptr) airportsByCode->clear();
    if (allAirportsList != nullptr) allAirportsList->clear();
    
    airportCount = 0;
}

