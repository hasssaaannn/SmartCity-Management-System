#include "FacilityManager.h"
#include <iostream>
using namespace std;

// Helper function to get approximate coordinates for Islamabad sectors
void FacilityManager::getSectorCoordinates(const string& sector, double& latitude, double& longitude) {
    // Base coordinates for Islamabad (approximate center)
    double baseLat = 33.684;
    double baseLon = 73.047;
    
    // Default to base coordinates
    latitude = baseLat;
    longitude = baseLon;
    
    // Map common sectors to approximate coordinates (Islamabad layout)
    if (sector == "G-10" || sector == "G10") {
        latitude = 33.684; longitude = 73.025;
    } else if (sector == "F-8" || sector == "F8") {
        latitude = 33.700; longitude = 73.037;
    } else if (sector == "F-6" || sector == "F6") {
        latitude = 33.715; longitude = 73.045;
    } else if (sector == "G-9" || sector == "G9") {
        latitude = 33.690; longitude = 73.030;
    } else if (sector == "F-7" || sector == "F7") {
        latitude = 33.707; longitude = 73.040;
    } else if (sector == "G-8" || sector == "G8") {
        latitude = 33.694; longitude = 73.032;
    } else if (sector == "H-8" || sector == "H8") {
        latitude = 33.710; longitude = 73.042;
    } else if (sector == "I-8" || sector == "I8") {
        latitude = 33.720; longitude = 73.048;
    } else if (sector == "G-6" || sector == "G6") {
        latitude = 33.709; longitude = 73.044;
    } else if (sector == "F-10" || sector == "F10") {
        latitude = 33.705; longitude = 73.038;
    } else if (sector == "Blue Area") {
        latitude = 33.697; longitude = 73.039;
    } else {
        // For unknown sectors, use base coordinates
        latitude = baseLat;
        longitude = baseLon;
    }
}

// Helper function to calculate simple squared distance
static double calculateDistanceSquared(double lat1, double lon1, double lat2, double lon2) {
    double dLat = lat2 - lat1;
    double dLon = lon2 - lon1;
    return dLat * dLat + dLon * dLon;
}

// Constructor
FacilityManager::FacilityManager(Graph* graph)
    : cityGraph(graph), facilityCount(0) {
    // Initialize hash tables with reasonable sizes (prime numbers)
    facilitiesByID = new HashTable(101);
    facilitiesByName = new HashTable(101);
    facilitiesByType = new HashTable(31); // Smaller size for types (fewer types than facilities)
    allFacilitiesList = new SinglyLinkedList();
}

// Destructor
FacilityManager::~FacilityManager() {
    clear();
    
    // Delete hash tables and list
    if (facilitiesByID != nullptr) {
        delete facilitiesByID;
        facilitiesByID = nullptr;
    }
    if (facilitiesByName != nullptr) {
        delete facilitiesByName;
        facilitiesByName = nullptr;
    }
    if (facilitiesByType != nullptr) {
        delete facilitiesByType;
        facilitiesByType = nullptr;
    }
    if (allFacilitiesList != nullptr) {
        delete allFacilitiesList;
        allFacilitiesList = nullptr;
    }
}

// Set the city graph
void FacilityManager::setCityGraph(Graph* graph) {
    cityGraph = graph;
}

// Get the city graph
Graph* FacilityManager::getCityGraph() const {
    return cityGraph;
}

// Helper function to get all facilities as array
void FacilityManager::getAllFacilities(Facility** facilities, int& count) const {
    count = 0;
    if (facilities == nullptr || facilityCount == 0) return;
    
    // Get all facility IDs from linked list
    int idCount = allFacilitiesList->getSize();
    if (idCount == 0) return;
    
    string* facilityIDs = new string[idCount];
    int actualCount = 0;
    allFacilitiesList->toArray(facilityIDs, actualCount);
    
    // Convert IDs to Facility pointers
    for (int i = 0; i < actualCount; i++) {
        Facility* facility = findFacilityByID(facilityIDs[i]);
        if (facility != nullptr) {
            facilities[count] = facility;
            count++;
        }
    }
    
    delete[] facilityIDs;
}

// Add a facility to the manager
bool FacilityManager::addFacility(Facility* facility) {
    if (facility == nullptr) return false;
    if (!facility->isValid()) return false;
    
    // Check if facility already exists
    if (findFacilityByID(facility->getFacilityID()) != nullptr) {
        return false; // Facility already exists
    }
    
    // Add to all hash tables
    facilitiesByID->insert(facility->getFacilityID(), (void*)facility);
    facilitiesByName->insert(facility->getName(), (void*)facility);
    facilitiesByType->insert(facility->getType(), (void*)facility);
    
    // Add facility ID to list for iteration
    allFacilitiesList->insertAtTail(facility->getFacilityID());
    
    facilityCount++;
    return true;
}

// Find a facility by ID
Facility* FacilityManager::findFacilityByID(const string& facilityID) const {
    if (facilityID.empty()) return nullptr;
    
    void* data = facilitiesByID->search(facilityID);
    if (data != nullptr) {
        return (Facility*)data;
    }
    return nullptr;
}

// Find a facility by name
Facility* FacilityManager::findFacilityByName(const string& name) const {
    if (name.empty()) return nullptr;
    
    void* data = facilitiesByName->search(name);
    if (data != nullptr) {
        return (Facility*)data;
    }
    return nullptr;
}

// Find all facilities of a specific type
void FacilityManager::findFacilitiesByType(const string& type, Facility** results, int& count) {
    count = 0;
    if (results == nullptr || type.empty()) return;
    
    // Get all facilities
    Facility** facilities = new Facility*[facilityCount];
    int totalCount = 0;
    getAllFacilities(facilities, totalCount);
    
    // Filter by type
    for (int i = 0; i < totalCount; i++) {
        if (facilities[i]->getType() == type) {
            results[count] = facilities[i];
            count++;
        }
    }
    
    delete[] facilities;
}

// Get number of facilities
int FacilityManager::getFacilityCount() const {
    return facilityCount;
}

// Display all facilities
void FacilityManager::displayAllFacilities() const {
    if (facilityCount == 0) {
        cout << "No facilities registered." << endl;
        return;
    }
    
    cout << "Registered Facilities (" << facilityCount << " facilities):" << endl;
    
    // Get all facilities
    Facility** facilities = new Facility*[facilityCount];
    int count = 0;
    getAllFacilities(facilities, count);
    
    // Display each facility
    for (int i = 0; i < count; i++) {
        cout << "  [" << (i + 1) << "] ";
        facilities[i]->display();
        cout << endl;
    }
    
    delete[] facilities;
}

// Find nearest facility to a given stop ID
Facility* FacilityManager::findNearestFacility(const string& fromStopID) {
    if (fromStopID.empty() || cityGraph == nullptr || facilityCount == 0) {
        return nullptr;
    }
    
    Facility* nearestFacility = nullptr;
    double minDistance = -1.0;
    
    // Get all facilities
    Facility** facilities = new Facility*[facilityCount];
    int count = 0;
    getAllFacilities(facilities, count);
    
    // For each facility, calculate distance from fromStopID to facility's vertexID
    for (int i = 0; i < count; i++) {
        string facilityVertexID = facilities[i]->getVertexID();
        if (facilityVertexID.empty()) continue;
        
        // Use graph to find shortest path
        int pathLength = 0;
        double distance = 0.0;
        string* path = cityGraph->findShortestPath(fromStopID, facilityVertexID, pathLength, distance);
        
        if (path != nullptr && distance >= 0.0) {
            // Valid path found
            if (minDistance < 0.0 || distance < minDistance) {
                minDistance = distance;
                nearestFacility = facilities[i];
            }
            delete[] path;
        }
    }
    
    delete[] facilities;
    return nearestFacility;
}

// Find nearest facility to given coordinates
Facility* FacilityManager::findNearestFacility(double latitude, double longitude) {
    if (facilityCount == 0) {
        return nullptr;
    }
    
    // Iterate through all registered facilities and find the nearest one
    double minDistance = 1000000.0; // Large initial distance
    Facility* nearestFacility = nullptr;
    
    // Get all facilities
    Facility** facilities = new Facility*[facilityCount];
    int count = 0;
    getAllFacilities(facilities, count);
    
    // For each facility, get coordinates from graph vertex (CSV coordinates) and calculate distance
    for (int i = 0; i < count; i++) {
        Facility* facility = facilities[i];
        if (facility == nullptr) continue;
        
        // Get facility coordinates from graph vertex (using facilityID as vertexID)
        string facilityVertexID = facility->getVertexID();
        if (facilityVertexID.empty()) continue;
        
        double facilityLat, facilityLon;
        if (cityGraph != nullptr && cityGraph->getVertexCoordinates(facilityVertexID, facilityLat, facilityLon)) {
            // Use CSV coordinates from graph
            double distSquared = calculateDistanceSquared(latitude, longitude, facilityLat, facilityLon);
            
            if (distSquared < minDistance) {
                minDistance = distSquared;
                nearestFacility = facility;
            }
        } else {
            // Fallback: use sector coordinates if graph vertex not found
            getSectorCoordinates(facility->getSector(), facilityLat, facilityLon);
            double distSquared = calculateDistanceSquared(latitude, longitude, facilityLat, facilityLon);
            
            if (distSquared < minDistance) {
                minDistance = distSquared;
                nearestFacility = facility;
            }
        }
    }
    
    delete[] facilities;
    return nearestFacility;
}

// Find nearest facility of a specific type
Facility* FacilityManager::findNearestFacilityByType(const string& type, const string& fromStopID) {
    if (type.empty() || fromStopID.empty() || cityGraph == nullptr || facilityCount == 0) {
        return nullptr;
    }
    
    // Get all facilities of the specified type
    Facility** typeFacilities = new Facility*[facilityCount];
    int typeCount = 0;
    findFacilitiesByType(type, typeFacilities, typeCount);
    
    if (typeCount == 0) {
        delete[] typeFacilities;
        return nullptr;
    }
    
    Facility* nearestFacility = nullptr;
    double minDistance = -1.0;
    
    // For each facility of the type, calculate distance
    for (int i = 0; i < typeCount; i++) {
        string facilityVertexID = typeFacilities[i]->getVertexID();
        if (facilityVertexID.empty()) continue;
        
        // Use graph to find shortest path
        int pathLength = 0;
        double distance = 0.0;
        string* path = cityGraph->findShortestPath(fromStopID, facilityVertexID, pathLength, distance);
        
        if (path != nullptr && distance >= 0.0) {
            // Valid path found
            if (minDistance < 0.0 || distance < minDistance) {
                minDistance = distance;
                nearestFacility = typeFacilities[i];
            }
            delete[] path;
        }
    }
    
    delete[] typeFacilities;
    return nearestFacility;
}

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
    
    // Convert to double
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

// Load facilities from CSV data
bool FacilityManager::loadFacilitiesFromCSVData(CSVRow* facilityData, int facilityCount) {
    if (facilityData == nullptr || facilityCount <= 0 || cityGraph == nullptr) return false;
    
    // CSV Format: FacilityID,Name,Type,Sector,Coordinates
    // fields[0]=FacilityID, [1]=Name, [2]=Type, [3]=Sector, [4]=Coordinates
    
    for (int i = 0; i < facilityCount; i++) {
        if (facilityData[i].fieldCount < 5) continue; // Skip invalid rows
        
        string facilityID = facilityData[i].fields[0];
        string name = facilityData[i].fields[1];
        string type = facilityData[i].fields[2];
        string sector = facilityData[i].fields[3];
        string coordStr = facilityData[i].fields[4];
        
        // Parse coordinates from CSV
        double lat = 0.0, lon = 0.0;
        
        if (!parseCoordinates(coordStr, lat, lon)) {
            // Invalid coordinates, skip this facility
            continue;
        }
        
        // Create new facility (use facilityID as vertexID)
        Facility* facility = new Facility(facilityID, name, type, sector, facilityID);
        
        // Add to manager first
        if (!addFacility(facility)) {
            // If add failed (duplicate), delete the facility to avoid memory leak
            delete facility;
            continue;
        }
        
        // Add facility as a vertex to the graph with CSV coordinates
        cityGraph->addVertex(facilityID, name, lat, lon, facility);
        
        // Connect facility to the nearest bus stop for pathfinding
        cityGraph->connectToNearestStop(facilityID);
    }
    
    return true;
}

// Clear all facilities
void FacilityManager::clear() {
    // Get all facilities and delete them
    Facility** facilities = new Facility*[facilityCount];
    int count = 0;
    getAllFacilities(facilities, count);
    
    // Delete each facility
    for (int i = 0; i < count; i++) {
        delete facilities[i];
    }
    
    delete[] facilities;
    
    // Clear hash tables and list
    if (facilitiesByID != nullptr) {
        facilitiesByID->clear();
    }
    if (facilitiesByName != nullptr) {
        facilitiesByName->clear();
    }
    if (facilitiesByType != nullptr) {
        facilitiesByType->clear();
    }
    if (allFacilitiesList != nullptr) {
        allFacilitiesList->clear();
    }
    
    facilityCount = 0;
}

