#include "EmergencyManager.h"
#include <iostream>
#include <cmath>
#include <sstream>
using namespace std;

const double M_PI = 3.14159265358979323846;

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
EmergencyManager::EmergencyManager(Graph* graph, MedicalSector* medical)
    : cityGraph(graph), medicalSector(medical), vehicleCount(0), emergencyCounter(1) {
    // Initialize hash tables with reasonable sizes (prime numbers)
    vehiclesByID = new HashTable(101);
    vehiclesByType = new HashTable(31); // Smaller size for types (fewer types than vehicles)
    activeEmergencies = new HashTable(101);
    emergencyQueue = new MinHeap(100); // Priority queue for up to 100 emergencies
    allVehiclesList = new SinglyLinkedList();
}

// Destructor
EmergencyManager::~EmergencyManager() {
    clear();
    
    // Delete hash tables and structures
    if (vehiclesByID != nullptr) {
        delete vehiclesByID;
        vehiclesByID = nullptr;
    }
    if (vehiclesByType != nullptr) {
        delete vehiclesByType;
        vehiclesByType = nullptr;
    }
    if (activeEmergencies != nullptr) {
        delete activeEmergencies;
        activeEmergencies = nullptr;
    }
    if (emergencyQueue != nullptr) {
        delete emergencyQueue;
        emergencyQueue = nullptr;
    }
    if (allVehiclesList != nullptr) {
        delete allVehiclesList;
        allVehiclesList = nullptr;
    }
}

// Set the city graph
void EmergencyManager::setCityGraph(Graph* graph) {
    cityGraph = graph;
}

// Get the city graph
Graph* EmergencyManager::getCityGraph() const {
    return cityGraph;
}

// Set the medical sector
void EmergencyManager::setMedicalSector(MedicalSector* medical) {
    medicalSector = medical;
}

// Get the medical sector
MedicalSector* EmergencyManager::getMedicalSector() const {
    return medicalSector;
}

// Helper function to get all vehicles as array
void EmergencyManager::getAllVehicles(EmergencyVehicle** vehicles, int& count) const {
    count = 0;
    if (vehicles == nullptr || vehicleCount == 0) return;
    
    // Get all vehicle IDs from linked list
    int idCount = allVehiclesList->getSize();
    if (idCount == 0) return;
    
    string* vehicleIDs = new string[idCount];
    int actualCount = 0;
    allVehiclesList->toArray(vehicleIDs, actualCount);
    
    // Convert IDs to EmergencyVehicle pointers
    for (int i = 0; i < actualCount; i++) {
        EmergencyVehicle* vehicle = findVehicleByID(vehicleIDs[i]);
        if (vehicle != nullptr) {
            vehicles[count] = vehicle;
            count++;
        }
    }
    
    delete[] vehicleIDs;
}

// Helper function to get all available vehicles of a type
void EmergencyManager::getAvailableVehicles(const string& vehicleType, EmergencyVehicle** vehicles, int& count) const {
    count = 0;
    if (vehicles == nullptr || vehicleType.empty()) return;
    
    // Get all vehicles
    EmergencyVehicle** allVehicles = new EmergencyVehicle*[vehicleCount];
    int allCount = 0;
    getAllVehicles(allVehicles, allCount);
    
    // Filter by type and availability
    for (int i = 0; i < allCount; i++) {
        if (allVehicles[i]->getVehicleType() == vehicleType && allVehicles[i]->getIsAvailable()) {
            vehicles[count] = allVehicles[i];
            count++;
        }
    }
    
    delete[] allVehicles;
}

// Helper function to calculate distance between two locations
double EmergencyManager::calculateRouteDistance(const string& fromLocation, const string& toLocation) {
    if (fromLocation.empty() || toLocation.empty() || cityGraph == nullptr) return -1.0;
    
    int pathLength = 0;
    double distance = 0.0;
    string* path = cityGraph->findShortestPath(fromLocation, toLocation, pathLength, distance);
    
    if (path != nullptr) {
        delete[] path;
        return distance;
    }
    
    return -1.0;
}

// Helper function to calculate complete distance from coordinates to coordinates
double EmergencyManager::calculateCompleteRouteDistance(double sourceLat, double sourceLon, 
                                                          double destLat, double destLon) {
    if (cityGraph == nullptr) return -1.0;
    
    //Find nearest stop to source coordinates
    string sourceStopID = cityGraph->findNearestStop(sourceLat, sourceLon);
    if (sourceStopID.empty()) return -1.0;
    
    //Calculate distance from source coordinates to source stop (straight-line)
    double sourceStopLat, sourceStopLon;
    if (!cityGraph->getVertexCoordinates(sourceStopID, sourceStopLat, sourceStopLon)) return -1.0;
    double walkToSourceStop = calculateDistance(sourceLat, sourceLon, sourceStopLat, sourceStopLon);
    double totalDistance = walkToSourceStop;
    
    //Find nearest stop to destination coordinates
    string destStopID = cityGraph->findNearestStop(destLat, destLon);
    if (destStopID.empty()) return -1.0;
    
    //Calculate edge-based distance from source stop to destination stop (graph edges)
    int pathLength = 0;
    double edgeDistance = 0.0;
    string* path = cityGraph->findShortestPath(sourceStopID, destStopID, pathLength, edgeDistance);
    if (path == nullptr) return -1.0;
    delete[] path;
    totalDistance += edgeDistance;
    
    //Calculate distance from destination stop to destination coordinates (straight-line)
    double destStopLat, destStopLon;
    if (!cityGraph->getVertexCoordinates(destStopID, destStopLat, destStopLon)) return -1.0;
    double walkFromDestStop = calculateDistance(destStopLat, destStopLon, destLat, destLon);
    totalDistance += walkFromDestStop;
    
    return totalDistance;
}

// Helper function to calculate complete distance from coordinates to vertex ID
double EmergencyManager::calculateCompleteRouteDistanceToVertex(double sourceLat, double sourceLon, 
                                                                  const string& destVertexID) {
    if (cityGraph == nullptr || destVertexID.empty()) return -1.0;
    
    // Get destination vertex coordinates
    double destLat, destLon;
    if (!cityGraph->getVertexCoordinates(destVertexID, destLat, destLon)) return -1.0;
    
    return calculateCompleteRouteDistance(sourceLat, sourceLon, destLat, destLon);
}

// Add an emergency vehicle to the manager
bool EmergencyManager::addVehicle(EmergencyVehicle* vehicle) {
    if (vehicle == nullptr) return false;
    if (!vehicle->isValid()) return false;
    
    // Check if vehicle already exists
    if (findVehicleByID(vehicle->getVehicleID()) != nullptr) {
        return false; // Vehicle already exists
    }
    
    // Add to hash tables
    vehiclesByID->insert(vehicle->getVehicleID(), (void*)vehicle);
    vehiclesByType->insert(vehicle->getVehicleType(), (void*)vehicle);
    
    // Add vehicle ID to list for iteration
    allVehiclesList->insertAtTail(vehicle->getVehicleID());
    
    vehicleCount++;
    return true;
}

// Find a vehicle by ID
EmergencyVehicle* EmergencyManager::findVehicleByID(const string& vehicleID) const {
    if (vehicleID.empty()) return nullptr;
    
    void* data = vehiclesByID->search(vehicleID);
    if (data != nullptr) {
        return (EmergencyVehicle*)data;
    }
    return nullptr;
}

// Find nearest available vehicle of a specific type
EmergencyVehicle* EmergencyManager::findNearestAvailableVehicle(const string& vehicleType, const string& fromLocation) {
    if (vehicleType.empty() || fromLocation.empty() || cityGraph == nullptr) return nullptr;
    
    // Get all available vehicles of this type
    EmergencyVehicle** availableVehicles = new EmergencyVehicle*[vehicleCount];
    int count = 0;
    getAvailableVehicles(vehicleType, availableVehicles, count);
    
    if (count == 0) {
        delete[] availableVehicles;
        return nullptr;
    }
    
    // Find nearest vehicle
    EmergencyVehicle* nearestVehicle = nullptr;
    double minDistance = -1.0;
    
    for (int i = 0; i < count; i++) {
        string vehicleLocation = availableVehicles[i]->getCurrentStopID();
        if (vehicleLocation.empty()) continue;
        
        // Calculate distance
        double distance = calculateRouteDistance(fromLocation, vehicleLocation);
        if (distance >= 0.0) {
            if (minDistance < 0.0 || distance < minDistance) {
                minDistance = distance;
                nearestVehicle = availableVehicles[i];
            }
        }
    }
    
    delete[] availableVehicles;
    return nearestVehicle;
}

// Find nearest available vehicle from coordinates
EmergencyVehicle* EmergencyManager::findNearestAvailableVehicle(const string& vehicleType, double latitude, double longitude) {
    if (cityGraph == nullptr) return nullptr;
    
    // Get all available vehicles of this type
    EmergencyVehicle** availableVehicles = new EmergencyVehicle*[vehicleCount];
    int count = 0;
    getAvailableVehicles(vehicleType, availableVehicles, count);
    
    if (count == 0) {
        delete[] availableVehicles;
        return nullptr;
    }
    
    // Find nearest vehicle using complete distance calculation
    EmergencyVehicle* nearestVehicle = nullptr;
    double minDistance = -1.0;
    
    for (int i = 0; i < count; i++) {
        string vehicleLocation = availableVehicles[i]->getCurrentStopID();
        // Vehicle must have a current stop for navigation
        if (vehicleLocation.empty()) continue;
        
        // Get vehicle location coordinates
        double vehicleLat, vehicleLon;
        if (!cityGraph->getVertexCoordinates(vehicleLocation, vehicleLat, vehicleLon)) continue;
        
        // Calculate complete distance: source coords -> source stop -> vehicle stop -> vehicle coords
        double distance = calculateCompleteRouteDistanceToVertex(latitude, longitude, vehicleLocation);
        if (distance >= 0.0) {
            if (minDistance < 0.0 || distance < minDistance) {
                minDistance = distance;
                nearestVehicle = availableVehicles[i];
            }
        }
    }
    
    delete[] availableVehicles;
    return nearestVehicle;
}

// Get number of vehicles
int EmergencyManager::getVehicleCount() const {
    return vehicleCount;
}

// Get number of available vehicles
int EmergencyManager::getAvailableVehicleCount() const {
    EmergencyVehicle** allVehicles = new EmergencyVehicle*[vehicleCount];
    int count = 0;
    getAllVehicles(allVehicles, count);
    
    int availableCount = 0;
    for (int i = 0; i < count; i++) {
        if (allVehicles[i]->getIsAvailable()) {
            availableCount++;
        }
    }
    
    delete[] allVehicles;
    return availableCount;
}

// Display all vehicles
void EmergencyManager::displayAllVehicles() const {
    if (vehicleCount == 0) {
        cout << "No emergency vehicles registered." << endl;
        return;
    }
    
    cout << "Registered Emergency Vehicles (" << vehicleCount << " vehicles):" << endl;
    
    EmergencyVehicle** vehicles = new EmergencyVehicle*[vehicleCount];
    int count = 0;
    getAllVehicles(vehicles, count);
    
    for (int i = 0; i < count; i++) {
        cout << "\n[" << (i + 1) << "] ";
        vehicles[i]->display();
        cout << endl;
    }
    
    delete[] vehicles;
}

// Display available vehicles
void EmergencyManager::displayAvailableVehicles() const {
    EmergencyVehicle** allVehicles = new EmergencyVehicle*[vehicleCount];
    int count = 0;
    getAllVehicles(allVehicles, count);
    
    int availableCount = 0;
    for (int i = 0; i < count; i++) {
        if (allVehicles[i]->getIsAvailable()) {
            availableCount++;
        }
    }
    
    if (availableCount == 0) {
        cout << "No available emergency vehicles." << endl;
        delete[] allVehicles;
        return;
    }
    
    cout << "Available Emergency Vehicles (" << availableCount << " vehicles):" << endl;
    
    int displayed = 0;
    for (int i = 0; i < count; i++) {
        if (allVehicles[i]->getIsAvailable()) {
            cout << "\n[" << (displayed + 1) << "] ";
            allVehicles[i]->display();
            cout << endl;
            displayed++;
        }
    }
    
    delete[] allVehicles;
}

// Report a new emergency
string EmergencyManager::reportEmergency(const string& location, double lat, double lon,
                                         int priority, const string& type, const string& specialization) {
    if (location.empty() || priority < 1 || priority > 3 || type.empty()) {
        return "";
    }
    
    // Generate unique emergency ID
    stringstream ss;
    ss << "EMG";
    if (emergencyCounter < 10) ss << "0";
    ss << emergencyCounter++;
    string emergencyID = ss.str();
    
    // Create emergency structure
    Emergency* emergency = new Emergency();
    emergency->emergencyID = emergencyID;
    emergency->location = location;
    emergency->latitude = lat;
    emergency->longitude = lon;
    emergency->priority = priority;
    emergency->emergencyType = type;
    emergency->requiredSpecialization = specialization;
    emergency->assignedVehicleID = "";
    emergency->assignedHospitalID = "";
    emergency->isActive = true;
    
    // Add to active emergencies
    activeEmergencies->insert(emergencyID, (void*)emergency);
    
    // Add to priority queue (lower priority number = higher priority in min-heap)
    emergencyQueue->insert(emergencyID, (double)priority, emergency);
    
    return emergencyID;
}

// Dispatch vehicle to emergency
EmergencyRoute* EmergencyManager::dispatchToEmergency(const string& emergencyID) {
    if (emergencyID.empty()) return nullptr;
    
    // Get emergency information
    Emergency* emergency = nullptr;
    void* data = activeEmergencies->search(emergencyID);
    if (data == nullptr) return nullptr;
    emergency = (Emergency*)data;
    
    if (!emergency->isActive) return nullptr; // Emergency already completed
    
    // Find nearest available vehicle
    string vehicleType = "Ambulance"; // Default for medical emergencies
    if (emergency->emergencyType == "Fire") vehicleType = "FireTruck";
    else if (emergency->emergencyType == "Police") vehicleType = "Police";
    
    EmergencyVehicle* vehicle = nullptr;
    if (cityGraph != nullptr && !emergency->location.empty()) {
        // Try to find by location first
        vehicle = findNearestAvailableVehicle(vehicleType, emergency->location);
    }
    if (vehicle == nullptr && emergency->latitude != 0.0 && emergency->longitude != 0.0) {
        // Try by coordinates
        vehicle = findNearestAvailableVehicle(vehicleType, emergency->latitude, emergency->longitude);
    }
    
    if (vehicle == nullptr) return nullptr; // No available vehicle
    
    // Find best hospital (for medical emergencies)
    string hospitalID = "";
    if (emergency->emergencyType == "Medical" && medicalSector != nullptr) {
        Hospital* bestHospital = findBestHospital(emergency->location, emergency->requiredSpecialization);
        if (bestHospital != nullptr) {
            hospitalID = bestHospital->getHospitalID();
        }
    }
    
    // Calculate route
    EmergencyRoute* route = calculateEmergencyRoute(vehicle->getVehicleID(), emergency->location, hospitalID);
    if (route == nullptr) return nullptr;
    
    // Dispatch vehicle
    vehicle->dispatchToEmergency(emergencyID);
    emergency->assignedVehicleID = vehicle->getVehicleID();
    emergency->assignedHospitalID = hospitalID;
    
    return route;
}

// Complete an emergency
bool EmergencyManager::completeEmergency(const string& emergencyID) {
    if (emergencyID.empty()) return false;
    
    // Get emergency
    Emergency* emergency = nullptr;
    void* data = activeEmergencies->search(emergencyID);
    if (data == nullptr) return false;
    emergency = (Emergency*)data;
    
    if (!emergency->isActive) return false; // Already completed
    
    // Make vehicle available again
    if (!emergency->assignedVehicleID.empty()) {
        EmergencyVehicle* vehicle = findVehicleByID(emergency->assignedVehicleID);
        if (vehicle != nullptr) {
            vehicle->completeEmergency();
        }
    }
    
    // Mark emergency as inactive
    emergency->isActive = false;
    
    return true;
}

// Find best hospital for emergency
Hospital* EmergencyManager::findBestHospital(const string& emergencyLocation, const string& requiredSpecialization) {
    if (medicalSector == nullptr || cityGraph == nullptr) return nullptr;
    
    // Get coordinates from emergency location
    double lat = 0.0, lon = 0.0;
    bool hasCoords = cityGraph->getVertexCoordinates(emergencyLocation, lat, lon);
    
    if (!hasCoords) {
        return medicalSector->getHospitalWithMostBeds();
    }
    
    // Get hospital with most beds
    Hospital* mostBeds = medicalSector->getHospitalWithMostBeds();
    
    // Find nearest hospital
    string nearestHospitalID = medicalSector->findNearestHospital(lat, lon);
    Hospital* nearest = nullptr;
    if (!nearestHospitalID.empty()) {
        nearest = medicalSector->searchHospital(nearestHospitalID);
    }
    
    // If specialization is required, try to find hospital with that specialization
    if (!requiredSpecialization.empty()) {
        int specCount = 0;
        Hospital** specHospitals = medicalSector->findHospitalsBySpecialization(requiredSpecialization, specCount);
        if (specHospitals != nullptr && specCount > 0) {
            // Find nearest specialized hospital using complete distance calculation
            Hospital* bestSpec = nullptr;
            double minDist = -1.0;
            for (int i = 0; i < specCount; i++) {
                string hospID = specHospitals[i]->getHospitalID();
                double dist = calculateCompleteRouteDistanceToVertex(lat, lon, hospID);
                if (dist >= 0.0) {
                    if (minDist < 0.0 || dist < minDist) {
                        minDist = dist;
                        bestSpec = specHospitals[i];
                    }
                }
            }
            delete[] specHospitals;
            if (bestSpec != nullptr) {
                return bestSpec;
            }
        }
        if (specHospitals != nullptr) delete[] specHospitals;
    }
    
    if (mostBeds != nullptr) {
        return mostBeds;
    }
    
    return nearest;
}

// Calculate emergency route
EmergencyRoute* EmergencyManager::calculateEmergencyRoute(const string& vehicleID, const string& emergencyLocation, const string& hospitalID) {
    if (vehicleID.empty() || emergencyLocation.empty() || cityGraph == nullptr) return nullptr;
    
    EmergencyVehicle* vehicle = findVehicleByID(vehicleID);
    if (vehicle == nullptr) return nullptr;
    
    string vehicleLocation = vehicle->getCurrentStopID();
    if (vehicleLocation.empty()) {
        return nullptr;
    }
    
    // Calculate route: vehicle → emergency → hospital
    EmergencyRoute* route = new EmergencyRoute();
    route->vehicleID = vehicleID;
    route->emergencyLocation = emergencyLocation;
    route->hospitalID = hospitalID;
    route->totalDistance = 0.0;
    
    // Part 1: Vehicle → Emergency
    int pathLength1 = 0;
    double distance1 = 0.0;
    string* path1 = cityGraph->findShortestPath(vehicleLocation, emergencyLocation, pathLength1, distance1);
    
    if (path1 == nullptr) {
        delete route;
        return nullptr;
    }
    
    // Part 2: Emergency → Hospital (if hospital specified)
    int pathLength2 = 0;
    double distance2 = 0.0;
    string* path2 = nullptr;
    
    if (!hospitalID.empty()) {
        path2 = cityGraph->findShortestPath(emergencyLocation, hospitalID, pathLength2, distance2);
        if (path2 == nullptr) {
            delete[] path1;
            delete route;
            return nullptr;
        }
    }
    
    // Combine paths (remove duplicate emergency location)
    int totalLength = pathLength1;
    if (path2 != nullptr) {
        totalLength += pathLength2 - 1; // Subtract 1 to avoid duplicate emergency location
    }
    
    route->route = new string[totalLength];
    route->routeLength = totalLength;
    
    // Copy path1
    for (int i = 0; i < pathLength1; i++) {
        route->route[i] = path1[i];
    }
    
    // Copy path2 (skip first element as it's the emergency location)
    if (path2 != nullptr) {
        for (int i = 1; i < pathLength2; i++) {
            route->route[pathLength1 + i - 1] = path2[i];
        }
    }
    
    route->totalDistance = distance1;
    if (path2 != nullptr) {
        route->totalDistance += distance2;
    }
    
    delete[] path1;
    if (path2 != nullptr) delete[] path2;
    
    return route;
}

// Get emergency information
bool EmergencyManager::getEmergencyInfo(const string& emergencyID, Emergency& emergency) const {
    if (emergencyID.empty()) return false;
    
    void* data = activeEmergencies->search(emergencyID);
    if (data == nullptr) return false;
    
    Emergency* emg = (Emergency*)data;
    emergency = *emg;
    return true;
}

// Display all active emergencies
void EmergencyManager::displayActiveEmergencies() const {
    // Get all emergencies from hash table
    
    cout << "Active Emergencies:" << endl;
    cout << "(Note: Full emergency list requires additional tracking structure)" << endl;
    // In a complete implementation, we'd iterate through all emergencies
}

// Clear all emergencies and make all vehicles available
void EmergencyManager::clearAllEmergencies() {
    EmergencyVehicle** vehicles = new EmergencyVehicle*[vehicleCount];
    int count = 0;
    getAllVehicles(vehicles, count);
    
    for (int i = 0; i < count; i++) {
        vehicles[i]->completeEmergency();
    }
    
    delete[] vehicles;
    
    // Clear priority queue and hash table
    if (emergencyQueue != nullptr) {
        delete emergencyQueue;
        emergencyQueue = new MinHeap(100);
    }
    
    if (activeEmergencies != nullptr) {
        activeEmergencies->clear();
    }
}

// Clear all vehicles
void EmergencyManager::clear() {
    clearAllEmergencies();
    
    // Get all vehicles and delete them
    EmergencyVehicle** vehicles = new EmergencyVehicle*[vehicleCount];
    int count = 0;
    getAllVehicles(vehicles, count);
    
    for (int i = 0; i < count; i++) {
        delete vehicles[i];
    }
    
    delete[] vehicles;
    
    // Clear hash tables and list
    if (vehiclesByID != nullptr) vehiclesByID->clear();
    if (vehiclesByType != nullptr) vehiclesByType->clear();
    if (allVehiclesList != nullptr) allVehiclesList->clear();
    
    vehicleCount = 0;
    emergencyCounter = 1;
}

