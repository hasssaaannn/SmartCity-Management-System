#ifndef EMERGENCYMANAGER_H
#define EMERGENCYMANAGER_H

#include <string>
#include "core_classes/Graph.h"
#include "core_classes/HashTable.h"
#include "core_classes/SinglyLinkedList.h"
#include "core_classes/MinHeap.h"
#include "EmergencyVehicle.h"
#include "MedicalSector.h"
#include "Hospital.h"
using namespace std;

// Structure to store emergency information
struct Emergency {
    string emergencyID;           // Unique emergency ID (e.g., "EMG001")
    string location;              // Emergency location (stop ID or coordinates)
    double latitude;              // Emergency latitude (if coordinates provided)
    double longitude;             // Emergency longitude (if coordinates provided)
    int priority;                 // 1=Critical, 2=Urgent, 3=Normal
    string emergencyType;         // "Medical", "Fire", "Police"
    string requiredSpecialization; // Hospital specialization needed (for medical emergencies)
    string assignedVehicleID;     // Vehicle assigned to this emergency
    string assignedHospitalID;    // Hospital assigned for this emergency
    bool isActive;                // Is emergency still active
};

// Structure to store emergency route information
struct EmergencyRoute {
    string vehicleID;             // Vehicle handling the emergency
    string emergencyID;           // Emergency ID
    string* route;                // Array of stop IDs: [vehicleLocation → emergency → hospital]
    int routeLength;              // Number of stops in route
    double totalDistance;         // Total distance in km
    string emergencyLocation;     // Emergency location
    string hospitalID;            // Destination hospital
};

// EmergencyManager class - Central manager for emergency transport routing
// Used for: Emergency transport routing (bonus module)
class EmergencyManager {
private:
    HashTable* vehiclesByID;          // Hash table for vehicle lookup by ID (key = vehicleID, value = EmergencyVehicle*)
    HashTable* vehiclesByType;        // Hash table for vehicle lookup by type (key = type, value = EmergencyVehicle*)
    HashTable* activeEmergencies;     // Hash table for active emergencies (key = emergencyID, value = Emergency*)
    MinHeap* emergencyQueue;          // Priority queue for emergencies (priority 1=highest, 3=lowest)
    SinglyLinkedList* allVehiclesList; // Linked list to track all vehicles (stores vehicleID)
    Graph* cityGraph;                 // Pointer to shared city graph (for pathfinding)
    MedicalSector* medicalSector;     // Pointer to medical sector (for hospital selection)
    int vehicleCount;                  // Current number of emergency vehicles
    int emergencyCounter;              // Counter for generating unique emergency IDs
    
    // Helper function to get all vehicles as array
    void getAllVehicles(EmergencyVehicle** vehicles, int& count) const;
    
    // Helper function to get all available vehicles of a type
    void getAvailableVehicles(const string& vehicleType, EmergencyVehicle** vehicles, int& count) const;
    
    // Helper function to calculate distance between two locations (graph edges only)
    double calculateRouteDistance(const string& fromLocation, const string& toLocation);
    
    // Helper function to calculate complete distance from coordinates to coordinates
    // Returns: total distance (source coords -> source stop -> dest stop -> dest coords)
    double calculateCompleteRouteDistance(double sourceLat, double sourceLon, 
                                          double destLat, double destLon);
    
    // Helper function to calculate complete distance from coordinates to vertex ID
    // Returns: total distance (source coords -> source stop -> dest stop -> dest vertex coords)
    double calculateCompleteRouteDistanceToVertex(double sourceLat, double sourceLon, 
                                                   const string& destVertexID);
    
public:
    // Constructor
    // Parameters: graph - pointer to shared city graph, medical - pointer to medical sector
    EmergencyManager(Graph* graph = nullptr, MedicalSector* medical = nullptr);
    
    // Destructor
    ~EmergencyManager();
    
    // Set the city graph
    void setCityGraph(Graph* graph);
    
    // Get the city graph
    Graph* getCityGraph() const;
    
    // Set the medical sector
    void setMedicalSector(MedicalSector* medical);
    
    // Get the medical sector
    MedicalSector* getMedicalSector() const;
    
    // Vehicle management
    // Add an emergency vehicle to the manager
    // Returns: true if successful, false if vehicle already exists
    bool addVehicle(EmergencyVehicle* vehicle);
    
    // Find a vehicle by ID
    // Returns: Pointer to EmergencyVehicle if found, nullptr otherwise
    EmergencyVehicle* findVehicleByID(const string& vehicleID) const;
    
    // Find nearest available vehicle of a specific type
    // Parameters: vehicleType, fromLocation (stop ID)
    // Returns: Pointer to nearest available EmergencyVehicle, nullptr if none available
    EmergencyVehicle* findNearestAvailableVehicle(const string& vehicleType, const string& fromLocation);
    
    // Find nearest available vehicle from coordinates
    // Parameters: vehicleType, latitude, longitude
    // Returns: Pointer to nearest available EmergencyVehicle, nullptr if none available
    EmergencyVehicle* findNearestAvailableVehicle(const string& vehicleType, double latitude, double longitude);
    
    // Get number of vehicles
    int getVehicleCount() const;
    
    // Get number of available vehicles
    int getAvailableVehicleCount() const;
    
    // Display all vehicles
    void displayAllVehicles() const;
    
    // Display available vehicles
    void displayAvailableVehicles() const;
    
    // Emergency management
    // Report a new emergency
    // Parameters: location (stop ID or "coordinates"), lat, lon, priority, type, specialization
    // Returns: Emergency ID if successful, empty string otherwise
    string reportEmergency(const string& location, double lat, double lon, 
                          int priority, const string& type, const string& specialization = "");
    
    // Dispatch vehicle to emergency (finds best vehicle and hospital, calculates route)
    // Parameters: emergencyID
    // Returns: EmergencyRoute pointer if successful, nullptr otherwise
    // Note: Caller must delete the EmergencyRoute and route array
    EmergencyRoute* dispatchToEmergency(const string& emergencyID);
    
    // Complete an emergency (make vehicle available again)
    // Parameters: emergencyID
    // Returns: true if successful, false otherwise
    bool completeEmergency(const string& emergencyID);
    
    // Find best hospital for emergency (combines beds, distance, specialization)
    // Parameters: emergencyLocation, requiredSpecialization
    // Returns: Hospital pointer if found, nullptr otherwise
    Hospital* findBestHospital(const string& emergencyLocation, const string& requiredSpecialization = "");
    
    // Calculate emergency route (vehicle → emergency → hospital)
    // Parameters: vehicleID, emergencyLocation, hospitalID
    // Returns: EmergencyRoute pointer if successful, nullptr otherwise
    // Note: Caller must delete the EmergencyRoute and route array
    EmergencyRoute* calculateEmergencyRoute(const string& vehicleID, const string& emergencyLocation, const string& hospitalID);
    
    // Get emergency information
    // Parameters: emergencyID, emergency (output)
    // Returns: true if emergency found, false otherwise
    bool getEmergencyInfo(const string& emergencyID, Emergency& emergency) const;
    
    // Display all active emergencies
    void displayActiveEmergencies() const;
    
    // Clear all emergencies and make all vehicles available
    void clearAllEmergencies();
    
    // Clear all vehicles
    void clear();
};

#endif // EMERGENCYMANAGER_H

