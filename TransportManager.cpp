#include "TransportManager.h"
#include <iostream>
#include <cstdlib>
#include <ctime>
using namespace std;

// Helper function to parse route string (e.g., "Stop1 > Stop2 > Stop3")
// Returns: number of stops parsed
static int parseRouteString(const string& routeStr, string* stops, int maxStops) {
    if (routeStr.empty() || stops == nullptr || maxStops <= 0) return 0;
    
    int count = 0;
    string current = "";
    
    for (size_t i = 0; i < routeStr.length() && count < maxStops; i++) {
        char c = routeStr[i];
        
        // Check for delimiter ">" (with optional spaces)
        if (c == '>') {
            // Trim whitespace from current
            int start = 0;
            int end = current.length() - 1;
            while (start < (int)current.length() && (current[start] == ' ' || current[start] == '\t')) start++;
            while (end >= 0 && (current[end] == ' ' || current[end] == '\t')) end--;
            
            if (start <= end) {
                stops[count] = current.substr(start, end - start + 1);
                count++;
            }
            current = "";
            // Skip spaces after ">"
            while (i + 1 < routeStr.length() && (routeStr[i + 1] == ' ' || routeStr[i + 1] == '\t')) {
                i++;
            }
        } else {
            current += c;
        }
    }
    
    // Add last stop
    if (!current.empty() && count < maxStops) {
        // Trim whitespace
        int start = 0;
        int end = current.length() - 1;
        while (start < (int)current.length() && (current[start] == ' ' || current[start] == '\t')) start++;
        while (end >= 0 && (current[end] == ' ' || current[end] == '\t')) end--;
        
        if (start <= end) {
            stops[count] = current.substr(start, end - start + 1);
            count++;
        }
    }
    
    return count;
}

// Helper function to parse coordinates string (e.g., "33.684, 73.025")
// Returns: true if successful, false otherwise
static bool parseCoordinates(const string& coordStr, double& lat, double& lon) {
    if (coordStr.empty()) return false;
    
    // Find comma
    size_t commaPos = coordStr.find(',');
    if (commaPos == string::npos) return false;
    
    string latStr = coordStr.substr(0, commaPos);
    string lonStr = coordStr.substr(commaPos + 1);
    
    // Trim whitespace
    int start = 0;
    int end = latStr.length() - 1;
    while (start < (int)latStr.length() && (latStr[start] == ' ' || latStr[start] == '\t')) start++;
    while (end >= 0 && (latStr[end] == ' ' || latStr[end] == '\t')) end--;
    if (start <= end) latStr = latStr.substr(start, end - start + 1);
    
    start = 0;
    end = lonStr.length() - 1;
    while (start < (int)lonStr.length() && (lonStr[start] == ' ' || lonStr[start] == '\t')) start++;
    while (end >= 0 && (lonStr[end] == ' ' || lonStr[end] == '\t')) end--;
    if (start <= end) lonStr = lonStr.substr(start, end - start + 1);
    
    // Parse latitude (manual string to double conversion)
    lat = 0.0;
    bool hasDecimal = false;
    int intPart = 0;
    int decPart = 0;
    int decDigits = 0;
    bool negative = false;
    int startIdx = 0;
    
    if (latStr.length() > 0 && latStr[0] == '-') {
        negative = true;
        startIdx = 1;
    }
    
    for (int i = startIdx; i < (int)latStr.length(); i++) {
        if (latStr[i] == '.') {
            hasDecimal = true;
            continue;
        }
        if (latStr[i] >= '0' && latStr[i] <= '9') {
            if (!hasDecimal) {
                intPart = intPart * 10 + (latStr[i] - '0');
            } else {
                decPart = decPart * 10 + (latStr[i] - '0');
                decDigits++;
            }
        }
    }
    
    double divisor = 1.0;
    for (int k = 0; k < decDigits; k++) {
        divisor *= 10.0;
    }
    lat = intPart + (decPart / divisor);
    if (negative) lat = -lat;
    
    // Parse longitude (same process)
    lon = 0.0;
    hasDecimal = false;
    intPart = 0;
    decPart = 0;
    decDigits = 0;
    negative = false;
    startIdx = 0;
    
    if (lonStr.length() > 0 && lonStr[0] == '-') {
        negative = true;
        startIdx = 1;
    }
    
    for (int i = startIdx; i < (int)lonStr.length(); i++) {
        if (lonStr[i] == '.') {
            hasDecimal = true;
            continue;
        }
        if (lonStr[i] >= '0' && lonStr[i] <= '9') {
            if (!hasDecimal) {
                intPart = intPart * 10 + (lonStr[i] - '0');
            } else {
                decPart = decPart * 10 + (lonStr[i] - '0');
                decDigits++;
            }
        }
    }
    
    divisor = 1.0;
    for (int k = 0; k < decDigits; k++) {
        divisor *= 10.0;
    }
    lon = intPart + (decPart / divisor);
    if (negative) lon = -lon;
    
    return true;
}

// Constructor
TransportManager::TransportManager(Graph* graph, int queueCapacity)
    : cityGraph(graph), busCount(0), companyCount(0), schoolBusCount(0) {
    // Initialize hash tables with reasonable sizes (prime numbers)
    busesByNumber = new HashTable(101);
    companiesByName = new HashTable(101);
    schoolBusesBySchool = new HashTable(101);
    schoolBusesByNumber = new HashTable(101);
    allBusesList = new SinglyLinkedList();
    allCompaniesList = new SinglyLinkedList();
    passengerQueue = new CircularQueue(queueCapacity);
    routeHistory = new Stack(100);
}

// Destructor
TransportManager::~TransportManager() {
    clear();
    
    // Delete hash tables and lists
    if (busesByNumber != nullptr) {
        delete busesByNumber;
        busesByNumber = nullptr;
    }
    if (companiesByName != nullptr) {
        delete companiesByName;
        companiesByName = nullptr;
    }
    if (schoolBusesBySchool != nullptr) {
        delete schoolBusesBySchool;
        schoolBusesBySchool = nullptr;
    }
    if (schoolBusesByNumber != nullptr) {
        delete schoolBusesByNumber;
        schoolBusesByNumber = nullptr;
    }
    if (allBusesList != nullptr) {
        delete allBusesList;
        allBusesList = nullptr;
    }
    if (allCompaniesList != nullptr) {
        delete allCompaniesList;
        allCompaniesList = nullptr;
    }
    if (passengerQueue != nullptr) {
        delete passengerQueue;
        passengerQueue = nullptr;
    }
    if (routeHistory != nullptr) {
        delete routeHistory;
        routeHistory = nullptr;
    }
}

// Set the city graph
void TransportManager::setCityGraph(Graph* graph) {
    cityGraph = graph;
}

// Get the city graph
Graph* TransportManager::getCityGraph() const {
    return cityGraph;
}

// Helper function to get all buses as array
void TransportManager::getAllBuses(Bus** buses, int& count) const {
    count = 0;
    if (buses == nullptr || busCount == 0) return;
    
    // Get all bus numbers from linked list
    int idCount = allBusesList->getSize();
    if (idCount == 0) return;
    
    string* busNumbers = new string[idCount];
    int actualCount = 0;
    allBusesList->toArray(busNumbers, actualCount);
    
    // Convert bus numbers to Bus pointers
    for (int i = 0; i < actualCount; i++) {
        Bus* bus = findBusByNumber(busNumbers[i]);
        if (bus != nullptr) {
            buses[count] = bus;
            count++;
        }
    }
    
    delete[] busNumbers;
}

// Add a bus to the manager
bool TransportManager::addBus(Bus* bus) {
    if (bus == nullptr) return false;
    if (!bus->isValid()) return false;
    
    string busNo = bus->getBusNo();
    if (busNo.empty()) return false;
    
    // Check if bus already exists
    if (findBusByNumber(busNo) != nullptr) {
        return false; // Bus already exists
    }
    
    // Automatically register the company if not already registered
    string companyName = bus->getCompany();
    if (!companyName.empty()) {
        registerCompany(companyName);
    }
    
    // Add to hash table
    busesByNumber->insert(busNo, bus);
    
    // Add to linked list for iteration
    allBusesList->insertAtTail(busNo);
    
    busCount++;
    return true;
}

// Find a bus by number
Bus* TransportManager::findBusByNumber(const string& busNo) const {
    if (busNo.empty() || busesByNumber == nullptr) return nullptr;
    
    void* result = busesByNumber->search(busNo);
    if (result == nullptr) return nullptr;
    
    return static_cast<Bus*>(result);
}

// Get number of buses
int TransportManager::getBusCount() const {
    return busCount;
}

// Get all buses (public method)
void TransportManager::getAllBusesPublic(Bus** buses, int& count) const {
    getAllBuses(buses, count);
}

// Display all buses
void TransportManager::displayAllBuses() const {
    if (busCount == 0) {
        cout << "No buses registered." << endl;
        return;
    }
    
    cout << "=== All Buses (" << busCount << ") ===" << endl;
    
    Bus** buses = new Bus*[busCount];
    int count = 0;
    getAllBuses(buses, count);
    
    for (int i = 0; i < count; i++) {
        cout << "\n[" << (i + 1) << "] ";
        buses[i]->display();
        cout << endl;
    }
    
    delete[] buses;
}

// Display all bus routes
void TransportManager::displayAllRoutes() const {
    if (busCount == 0) {
        cout << "No buses registered." << endl;
        return;
    }
    
    cout << "=== All Bus Routes ===" << endl;
    
    Bus** buses = new Bus*[busCount];
    int count = 0;
    getAllBuses(buses, count);
    
    for (int i = 0; i < count; i++) {
        cout << "\nBus " << buses[i]->getBusNo() << " (" << buses[i]->getCompany() << "): ";
        buses[i]->displayRoute();
    }
    
    delete[] buses;
}

// Add a stop to a bus route
bool TransportManager::addStopToBusRoute(const string& busNo, const string& stopID) {
    if (busNo.empty() || stopID.empty()) return false;
    
    Bus* bus = findBusByNumber(busNo);
    if (bus == nullptr) return false;
    
    return bus->addStopToRoute(stopID);
}

// Remove a stop from a bus route
bool TransportManager::removeStopFromBusRoute(const string& busNo, const string& stopID) {
    if (busNo.empty() || stopID.empty()) return false;
    
    Bus* bus = findBusByNumber(busNo);
    if (bus == nullptr) return false;
    
    return bus->removeStopFromRoute(stopID);
}

// Get route for a specific bus
void TransportManager::getBusRoute(const string& busNo, string* stops, int& count) {
    count = 0;
    if (busNo.empty() || stops == nullptr) return;
    
    Bus* bus = findBusByNumber(busNo);
    if (bus == nullptr) return;
    
    bus->getRouteStops(stops, count);
}

// Find shortest path between two stops using Dijkstra's algorithm
bool TransportManager::findShortestPath(const string& startStopID, const string& endStopID, 
                                         string* path, int& pathLength, double& totalDistance) {
    pathLength = 0;
    totalDistance = 0.0;
    
    if (startStopID.empty() || endStopID.empty() || path == nullptr || cityGraph == nullptr) {
        return false;
    }
    
    // Use graph's built-in Dijkstra algorithm
    int tempPathLength = 0;
    double tempDistance = 0.0;
    string* graphPath = cityGraph->findShortestPath(startStopID, endStopID, tempPathLength, tempDistance);
    
    if (graphPath == nullptr || tempPathLength <= 0) {
        return false;
    }
    
    // Copy path to output array
    pathLength = tempPathLength;
    totalDistance = tempDistance;
    for (int i = 0; i < pathLength; i++) {
        path[i] = graphPath[i];
    }
    
    delete[] graphPath;
    return true;
}

// Find nearest bus to a given stop ID
Bus* TransportManager::findNearestBus(const string& fromStopID) {
    if (fromStopID.empty() || cityGraph == nullptr || busCount == 0) {
        return nullptr;
    }
    
    Bus* nearestBus = nullptr;
    double minDistance = -1.0;
    
    // Get all buses
    Bus** buses = new Bus*[busCount];
    int count = 0;
    getAllBuses(buses, count);
    
    // For each bus, find the minimum distance from fromStopID to any stop in its route
    for (int i = 0; i < count; i++) {
        // Skip school buses - they should only be found via findNearestSchoolBus
        if (getSchoolBusByNumber(buses[i]->getBusNo()) != nullptr) {
            continue;
        }
        
        // Get bus's current location or first stop in route
        string busLocation = buses[i]->getCurrentStopID();
        if (busLocation.empty()) {
            // If no current location, use first stop in route
            int routeLength = buses[i]->getRouteLength();
            if (routeLength > 0) {
                busLocation = buses[i]->getStopAtPosition(0);
            }
        }
        
        if (busLocation.empty()) continue;
        
        // Use graph to find shortest path
        int pathLength = 0;
        double distance = 0.0;
        string* path = cityGraph->findShortestPath(fromStopID, busLocation, pathLength, distance);
        
        if (path != nullptr && distance >= 0.0) {
            // Valid path found
            if (minDistance < 0.0 || distance < minDistance) {
                minDistance = distance;
                nearestBus = buses[i];
            }
            delete[] path;
        }
    }
    
    delete[] buses;
    return nearestBus;
}

// Find nearest bus to given coordinates
Bus* TransportManager::findNearestBus(double latitude, double longitude) {
    if (cityGraph == nullptr || busCount == 0) {
        return nullptr;
    }
    
    // First, find nearest STOP (not any location) to the coordinates
    string nearestStopID = cityGraph->findNearestStop(latitude, longitude);
    if (nearestStopID.empty()) {
        return nullptr;
    }
    
    // Then find nearest bus from that stop
    return findNearestBus(nearestStopID);
}

// Find buses passing through a specific stop
void TransportManager::findBusesAtStop(const string& stopID, Bus** buses, int& count) {
    count = 0;
    if (stopID.empty() || buses == nullptr) return;
    
    // Get all buses
    Bus** allBuses = new Bus*[busCount];
    int allCount = 0;
    getAllBuses(allBuses, allCount);
    
    // Check each bus's route
    for (int i = 0; i < allCount; i++) {
        if (allBuses[i]->isStopInRoute(stopID)) {
            buses[count] = allBuses[i];
            count++;
        }
    }
    
    delete[] allBuses;
}

// Update bus location (move to next stop)
bool TransportManager::moveBusToNextStop(const string& busNo) {
    if (busNo.empty()) return false;
    
    Bus* bus = findBusByNumber(busNo);
    if (bus == nullptr) return false;
    
    return bus->moveToNextStop();
}

// Set bus location to a specific stop
bool TransportManager::setBusLocation(const string& busNo, const string& stopID) {
    if (busNo.empty() || stopID.empty()) return false;
    
    Bus* bus = findBusByNumber(busNo);
    if (bus == nullptr) return false;
    
    return bus->setCurrentLocation(stopID);
}

// Get current location of a bus
string TransportManager::getBusLocation(const string& busNo) const {
    if (busNo.empty()) return "";
    
    Bus* bus = findBusByNumber(busNo);
    if (bus == nullptr) return "";
    
    return bus->getCurrentStopID();
}

// Add passenger to queue
bool TransportManager::enqueuePassenger(const string& passengerID) {
    if (passengerID.empty() || passengerQueue == nullptr) return false;
    
    return passengerQueue->enqueue(passengerID);
}

// Remove passenger from queue
string TransportManager::dequeuePassenger() {
    if (passengerQueue == nullptr || passengerQueue->isEmpty()) {
        return "";
    }
    
    QueueNode node = passengerQueue->dequeue();
    return node.data;
}

// Get passenger queue size
int TransportManager::getPassengerQueueSize() const {
    if (passengerQueue == nullptr) return 0;
    return passengerQueue->getSize();
}

// Push route to history
void TransportManager::pushRouteToHistory(const string& routeInfo) {
    if (routeInfo.empty() || routeHistory == nullptr) return;
    
    routeHistory->push(routeInfo);
}

// Pop route from history
string TransportManager::popRouteFromHistory() {
    if (routeHistory == nullptr || routeHistory->isEmpty()) {
        return "";
    }
    
    StackNode node = routeHistory->pop();
    return node.data;
}

// Get route history size
int TransportManager::getRouteHistorySize() const {
    if (routeHistory == nullptr) return 0;
    return routeHistory->getSize();
}

// Load buses from CSV data
bool TransportManager::loadBusesFromCSVData(CSVRow* busData, int busCount) {
    if (busData == nullptr || busCount <= 0) return false;
    
    // Parse CSV data and create Bus objects
    // Expected format: [0]=BusNo, [1]=Company, [2]=CurrentStop, [3]=Route (e.g., "Stop1 > Stop2 > Stop3")
    
    for (int i = 0; i < busCount; i++) {
        if (busData[i].fieldCount < 4) continue; // Skip invalid rows
        
        string busNo = busData[i].fields[0];
        string company = busData[i].fields[1];
        string currentStop = busData[i].fields[2];
        string routeStr = busData[i].fields[3];
        
        // Create new bus
        Bus* bus = new Bus(busNo, company, currentStop);
        
        // Parse route string and add stops
        string* stops = new string[100]; // Max 100 stops per route
        int stopCount = parseRouteString(routeStr, stops, 100);
        
        for (int j = 0; j < stopCount; j++) {
            bus->addStopToRoute(stops[j]);
        }
        
        delete[] stops;
        
        // Add to manager
        if (!addBus(bus)) {
            // If add failed (duplicate), delete the bus to avoid memory leak
            delete bus;
        }
    }
    
    return true;
}

// Load stops from CSV data and add to graph
bool TransportManager::loadStopsFromCSVData(CSVRow* stopData, int stopCount) {
    if (stopData == nullptr || stopCount <= 0 || cityGraph == nullptr) return false;
    
    // Parse CSV data and add stops to graph
    // Expected format: [0]=StopID, [1]=Name, [2]=Coordinates (e.g., "33.684, 73.025")
    
    for (int i = 0; i < stopCount; i++) {
        if (stopData[i].fieldCount < 3) continue; // Skip invalid rows
        
        string stopID = stopData[i].fields[0];
        string name = stopData[i].fields[1];
        string coordStr = stopData[i].fields[2];
        
        // Parse coordinates
        double lat = 0.0, lon = 0.0;
        if (!parseCoordinates(coordStr, lat, lon)) {
            continue; // Skip if coordinates invalid
        }
        
        // Add vertex to graph
        cityGraph->addVertex(stopID, name, lat, lon);
    }
    
    return true;
}

// Load school buses from CSV data
bool TransportManager::loadSchoolBusesFromCSVData(CSVRow* schoolBusData, int schoolBusCount) {
    if (schoolBusData == nullptr || schoolBusCount <= 0) return false;
    
    // Parse CSV data and create SchoolBus objects
    // Expected format: [0]=BusNo, [1]=Company, [2]=CurrentStop, [3]=SchoolID, [4]=MaxCapacity, [5]=Route (e.g., "Stop1 > Stop2 > Stop3")
    
    for (int i = 0; i < schoolBusCount; i++) {
        if (schoolBusData[i].fieldCount < 6) continue; // Skip invalid rows
        
        string busNo = schoolBusData[i].fields[0];
        string company = schoolBusData[i].fields[1];
        string currentStop = schoolBusData[i].fields[2];
        string schoolID = schoolBusData[i].fields[3];
        string capacityStr = schoolBusData[i].fields[4];
        string routeStr = schoolBusData[i].fields[5];
        
        // Set max capacity to 20 (fixed)
        int maxCapacity = 20;
        
        // Create new school bus
        SchoolBus* schoolBus = new SchoolBus(busNo, company, currentStop, schoolID, maxCapacity);
        
        // Parse route string and add stops
        string* stops = new string[100]; // Max 100 stops per route
        int stopCount = parseRouteString(routeStr, stops, 100);
        
        for (int j = 0; j < stopCount; j++) {
            schoolBus->addStopToRoute(stops[j]);
        }
        
        delete[] stops;
        
        // Add random students to the bus
        srand(static_cast<unsigned int>(time(nullptr)) + busNo.length());
        int randomStudentCount = rand() % 21; // Random number 0-20
        for (int j = 1; j <= randomStudentCount; j++) {
            string studentID = schoolID + "_STU" + to_string(j);
            schoolBus->addStudent(studentID);
        }
        
        // Add to manager
        if (!addSchoolBus(schoolBus)) {
            // If add failed (duplicate), delete the bus to avoid memory leak
            delete schoolBus;
        }
    }
    
    return true;
}

// Helper function to get all companies as array
void TransportManager::getAllCompanies(string* companies, int& count) const {
    count = 0;
    if (companies == nullptr || companyCount == 0 || allCompaniesList == nullptr) return;
    
    int listSize = allCompaniesList->getSize();
    if (listSize == 0) return;
    
    string* companyNames = new string[listSize];
    int actualCount = 0;
    allCompaniesList->toArray(companyNames, actualCount);
    
    for (int i = 0; i < actualCount && count < companyCount; i++) {
        companies[count] = companyNames[i];
        count++;
    }
    
    delete[] companyNames;
}

// Register a transport company
bool TransportManager::registerCompany(const string& companyName) {
    if (companyName.empty() || companiesByName == nullptr) return false;
    
    // Check if company already exists
    if (isCompanyRegistered(companyName)) {
        return false; // Company already registered
    }
    
    // Create a new string to store in hash table
    string* companyNamePtr = new string(companyName);
    
    // Add to hash table
    companiesByName->insert(companyName, companyNamePtr);
    
    // Add to linked list for iteration
    if (allCompaniesList != nullptr) {
        allCompaniesList->insertAtTail(companyName);
    }
    
    companyCount++;
    return true;
}

// Find if a company is registered
bool TransportManager::isCompanyRegistered(const string& companyName) const {
    if (companyName.empty() || companiesByName == nullptr) return false;
    
    void* result = companiesByName->search(companyName);
    return (result != nullptr);
}

// Get number of registered companies
int TransportManager::getCompanyCount() const {
    return companyCount;
}

// Helper function to get all buses by company (internal const version)
void TransportManager::getBusesByCompanyInternal(const string& companyName, Bus** buses, int& count) const {
    count = 0;
    if (companyName.empty() || buses == nullptr) return;
    
    // Get all buses
    Bus** allBuses = new Bus*[busCount];
    int allCount = 0;
    getAllBuses(allBuses, allCount);
    
    // Filter buses by company
    for (int i = 0; i < allCount; i++) {
        if (allBuses[i] != nullptr && allBuses[i]->getCompany() == companyName) {
            buses[count] = allBuses[i];
            count++;
        }
    }
    
    delete[] allBuses;
}

// Get all buses belonging to a specific company
void TransportManager::getBusesByCompany(const string& companyName, Bus** buses, int& count) const {
    getBusesByCompanyInternal(companyName, buses, count);
}

// Display all registered companies
void TransportManager::displayAllCompanies() const {
    if (companyCount == 0) {
        cout << "No transport companies registered." << endl;
        return;
    }
    
    cout << "=== Registered Transport Companies (" << companyCount << ") ===" << endl;
    
    string* companies = new string[companyCount];
    int count = 0;
    getAllCompanies(companies, count);
    
    for (int i = 0; i < count; i++) {
        // Count buses for this company
        Bus** companyBuses = new Bus*[busCount];
        int busCountForCompany = 0;
        getBusesByCompanyInternal(companies[i], companyBuses, busCountForCompany);
        
        cout << "  [" << (i + 1) << "] " << companies[i] 
             << " (" << busCountForCompany << " bus(es))" << endl;
        
        delete[] companyBuses;
    }
    
    delete[] companies;
}

// Clear all buses and reset queues
void TransportManager::clear() {
    // Get all buses and delete them
    Bus** buses = new Bus*[busCount];
    int count = 0;
    getAllBuses(buses, count);
    
    for (int i = 0; i < count; i++) {
        if (buses[i] != nullptr) {
            delete buses[i];
        }
    }
    
    delete[] buses;
    
    // Clear hash tables and lists
    if (busesByNumber != nullptr) {
        busesByNumber->clear();
    }
    if (allBusesList != nullptr) {
        allBusesList->clear();
    }
    
    // Clear companies (delete string pointers from hash table)
    if (companiesByName != nullptr && allCompaniesList != nullptr) {
        int companyListSize = allCompaniesList->getSize();
        if (companyListSize > 0) {
            string* companyNames = new string[companyListSize];
            int actualCount = 0;
            allCompaniesList->toArray(companyNames, actualCount);
            for (int i = 0; i < actualCount; i++) {
                void* companyPtr = companiesByName->search(companyNames[i]);
                if (companyPtr != nullptr) {
                    delete static_cast<string*>(companyPtr);
                }
            }
            delete[] companyNames;
        }
        companiesByName->clear();
    }
    if (allCompaniesList != nullptr) {
        allCompaniesList->clear();
    }
    
    // Clear school buses hash tables
    if (schoolBusesBySchool != nullptr) {
        schoolBusesBySchool->clear();
    }
    if (schoolBusesByNumber != nullptr) {
        schoolBusesByNumber->clear();
    }
    
    if (passengerQueue != nullptr) {
        while (!passengerQueue->isEmpty()) {
            passengerQueue->dequeue();
        }
    }
    if (routeHistory != nullptr) {
        while (!routeHistory->isEmpty()) {
            routeHistory->pop();
        }
    }
    
    busCount = 0;
    companyCount = 0;
    schoolBusCount = 0;
}

// Helper function to get SchoolBus pointer from bus number
SchoolBus* TransportManager::getSchoolBusByNumber(const string& busNo) const {
    if (busNo.empty() || schoolBusesByNumber == nullptr) return nullptr;
    
    void* result = schoolBusesByNumber->search(busNo);
    if (result == nullptr) return nullptr;
    
    return static_cast<SchoolBus*>(result);
}

// Add a school bus to the manager
bool TransportManager::addSchoolBus(SchoolBus* bus) {
    if (bus == nullptr) return false;
    if (!bus->isValid()) return false;
    
    string busNo = bus->getBusNo();
    if (busNo.empty()) return false;
    
    string schoolID = bus->getSchoolID();
    if (schoolID.empty()) return false;
    
    // Check if bus already exists
    if (findBusByNumber(busNo) != nullptr) {
        return false; // Bus already exists
    }
    
    // Add to main bus hash table (polymorphism - SchoolBus* → Bus*)
    if (!addBus(bus)) {
        return false; // Failed to add to main bus system
    }
    
    // Add to school buses hash tables
    // Store in schoolBusesBySchool for lookup by school
    schoolBusesBySchool->insert(schoolID, bus);
    // Store in schoolBusesByNumber for fast SchoolBus lookup
    schoolBusesByNumber->insert(busNo, bus);
    schoolBusCount++;
    
    return true;
}

// Get all school buses for a specific school
void TransportManager::getSchoolBuses(const string& schoolID, Bus** buses, int& count) const {
    count = 0;
    if (schoolID.empty() || buses == nullptr || schoolBusesBySchool == nullptr) return;
    
    // Search all buses and filter by school
    // Since schoolBusesBySchool can only store one bus per school key,
    // we need to search all school buses and check their school ID
    Bus** allBuses = new Bus*[busCount];
    int allCount = 0;
    getAllBuses(allBuses, allCount);
    
    for (int i = 0; i < allCount; i++) {
        string busNo = allBuses[i]->getBusNo();
        SchoolBus* schoolBus = getSchoolBusByNumber(busNo);
        if (schoolBus != nullptr && schoolBus->getSchoolID() == schoolID) {
            buses[count] = allBuses[i];
            count++;
        }
    }
    
    delete[] allBuses;
}

// Get number of school buses
int TransportManager::getSchoolBusCount() const {
    return schoolBusCount;
}

// Get number of school buses for a specific school
int TransportManager::getSchoolBusCountForSchool(const string& schoolID) const {
    if (schoolID.empty() || schoolBusesBySchool == nullptr) return 0;
    
    Bus** buses = new Bus*[busCount];
    int count = 0;
    getSchoolBuses(schoolID, buses, count);
    delete[] buses;
    
    return count;
}

// Track a student boarding a school bus
bool TransportManager::trackStudentOnBus(const string& busNo, const string& studentID) {
    if (busNo.empty() || studentID.empty()) return false;
    
    SchoolBus* schoolBus = getSchoolBusByNumber(busNo);
    if (schoolBus == nullptr) return false; // Not a school bus
    
    // Check if student is already on bus
    if (schoolBus->isStudentOnBus(studentID)) {
        return false; // Student already on bus
    }
    
    // Check if bus is full
    if (schoolBus->isFull()) {
        return false; // Bus is full
    }
    
    // Add student
    bool added = schoolBus->addStudent(studentID);
    
    // Verify student was actually added
    if (added && !schoolBus->isStudentOnBus(studentID)) {
        return false; // Failed to add student
    }
    
    return added;
}

// Remove a student from a school bus
bool TransportManager::removeStudentFromBus(const string& busNo, const string& studentID) {
    if (busNo.empty() || studentID.empty()) return false;
    
    SchoolBus* schoolBus = getSchoolBusByNumber(busNo);
    if (schoolBus == nullptr) return false; // Not a school bus
    
    return schoolBus->removeStudent(studentID);
}

// Check if a student is on any bus
bool TransportManager::isStudentOnAnyBus(const string& studentID, string& busNo) const {
    if (studentID.empty() || schoolBusesByNumber == nullptr) return false;
    
    busNo = "";
    
    // Search all school buses by iterating through all buses
    // and checking if they're school buses
    Bus** allBuses = new Bus*[busCount];
    int allCount = 0;
    getAllBuses(allBuses, allCount);
    
    for (int i = 0; i < allCount; i++) {
        string currentBusNo = allBuses[i]->getBusNo();
        SchoolBus* schoolBus = getSchoolBusByNumber(currentBusNo);
        if (schoolBus != nullptr && schoolBus->isStudentOnBus(studentID)) {
            busNo = currentBusNo;
            delete[] allBuses;
            return true;
        }
    }
    
    delete[] allBuses;
    return false;
}

// Get all students on a specific school bus
void TransportManager::getStudentsOnBus(const string& busNo, string* students, int& count) const {
    count = 0;
    if (busNo.empty() || students == nullptr) return;
    
    SchoolBus* schoolBus = getSchoolBusByNumber(busNo);
    if (schoolBus == nullptr) return;
    
    schoolBus->getStudentsOnBus(students, count);
}

// Find nearest school bus for a specific school
Bus* TransportManager::findNearestSchoolBus(const string& schoolID, const string& fromStopID) {
    if (schoolID.empty() || fromStopID.empty() || cityGraph == nullptr) return nullptr;
    
    // Get all school buses for this school
    Bus** schoolBuses = new Bus*[busCount];
    int count = 0;
    getSchoolBuses(schoolID, schoolBuses, count);
    
    if (count == 0) {
        delete[] schoolBuses;
        return nullptr;
    }
    
    // Find nearest bus using existing logic
    Bus* nearestBus = nullptr;
    double minDistance = -1.0;
    
    for (int i = 0; i < count; i++) {
        string busLocation = schoolBuses[i]->getCurrentStopID();
        if (busLocation.empty()) {
            int routeLength = schoolBuses[i]->getRouteLength();
            if (routeLength > 0) {
                busLocation = schoolBuses[i]->getStopAtPosition(0);
            }
        }
        
        if (busLocation.empty()) continue;
        
        int pathLength = 0;
        double distance = 0.0;
        string* path = cityGraph->findShortestPath(fromStopID, busLocation, pathLength, distance);
        
        if (path != nullptr && distance >= 0.0) {
            if (minDistance < 0.0 || distance < minDistance) {
                minDistance = distance;
                nearestBus = schoolBuses[i];
            }
            delete[] path;
        }
    }
    
    delete[] schoolBuses;
    return nearestBus;
}

// Find nearest school bus for a specific school from coordinates
Bus* TransportManager::findNearestSchoolBus(const string& schoolID, double latitude, double longitude) {
    if (schoolID.empty() || cityGraph == nullptr) return nullptr;
    
    string nearestStopID = cityGraph->findNearestStop(latitude, longitude);
    if (nearestStopID.empty()) return nullptr;
    
    return findNearestSchoolBus(schoolID, nearestStopID);
}

// Display all school buses (grouped by school)
void TransportManager::displayAllSchoolBuses() const {
    if (schoolBusCount == 0) {
        cout << "No school buses registered." << endl;
        return;
    }
    
    cout << "=== All School Buses (" << schoolBusCount << ") ===" << endl;
    
    // Get all school buses
    Bus** allBuses = new Bus*[busCount];
    int allCount = 0;
    getAllBuses(allBuses, allCount);
    
    // Collect all school buses and their school IDs
    SchoolBus** schoolBuses = new SchoolBus*[schoolBusCount];
    string* schoolIDs = new string[schoolBusCount];
    int schoolBusIndex = 0;
    
    for (int i = 0; i < allCount && schoolBusIndex < schoolBusCount; i++) {
        string busNo = allBuses[i]->getBusNo();
        SchoolBus* schoolBus = getSchoolBusByNumber(busNo);
        if (schoolBus != nullptr) {
            schoolBuses[schoolBusIndex] = schoolBus;
            schoolIDs[schoolBusIndex] = schoolBus->getSchoolID();
            schoolBusIndex++;
        }
    }
    
    // Group by school ID
    string* uniqueSchools = new string[schoolBusCount];
    int uniqueSchoolCount = 0;
    
    // Find unique school IDs
    for (int i = 0; i < schoolBusIndex; i++) {
        bool found = false;
        for (int j = 0; j < uniqueSchoolCount; j++) {
            if (schoolIDs[i] == uniqueSchools[j]) {
                found = true;
                break;
            }
        }
        if (!found) {
            uniqueSchools[uniqueSchoolCount] = schoolIDs[i];
            uniqueSchoolCount++;
        }
    }
    
    // Display buses grouped by school
    for (int s = 0; s < uniqueSchoolCount; s++) {
        cout << "\n========================================\n";
        cout << "SCHOOL: " << uniqueSchools[s] << "\n";
        cout << "========================================\n";
        
        int busNum = 1;
        for (int i = 0; i < schoolBusIndex; i++) {
            if (schoolIDs[i] == uniqueSchools[s]) {
                cout << "\n[" << busNum << "] ";
                schoolBuses[i]->display();
                cout << endl;
                busNum++;
            }
        }
    }
    
    delete[] allBuses;
    delete[] schoolBuses;
    delete[] schoolIDs;
    delete[] uniqueSchools;
}

// Display school buses for a specific school
void TransportManager::displaySchoolBusesForSchool(const string& schoolID) const {
    if (schoolID.empty()) {
        cout << "Invalid school ID." << endl;
        return;
    }
    
    Bus** buses = new Bus*[busCount];
    int count = 0;
    getSchoolBuses(schoolID, buses, count);
    
    if (count == 0) {
        cout << "No school buses found for school: " << schoolID << endl;
        delete[] buses;
        return;
    }
    
    cout << "=== School Buses for " << schoolID << " (" << count << ") ===" << endl;
    for (int i = 0; i < count; i++) {
        string busNo = buses[i]->getBusNo();
        SchoolBus* schoolBus = getSchoolBusByNumber(busNo);
        if (schoolBus != nullptr) {
            cout << "\n[" << (i + 1) << "] ";
            schoolBus->display();
            cout << endl;
        }
    }
    
    delete[] buses;
}

