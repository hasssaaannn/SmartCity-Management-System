#ifndef TRANSPORTMANAGER_H
#define TRANSPORTMANAGER_H

#include <string>
#include "core_classes/Graph.h"
#include "core_classes/HashTable.h"
#include "core_classes/SinglyLinkedList.h"
#include "core_classes/CircularQueue.h"
#include "core_classes/Stack.h"
#include "core_classes/DataLoader.h"
#include "Bus.h"
#include "SchoolBus.h"
using namespace std;

// TransportManager class - Central manager for all buses and transport operations
// Used for: Transport sector management, bus operations, route management, pathfinding, nearest bus queries
class TransportManager {
private:
    HashTable* busesByNumber;         // Hash table for bus lookup by number (key = busNo, value = Bus*)
    HashTable* companiesByName;       // Hash table for transport company lookup by name (key = company name, value = company name string*)
    HashTable* schoolBusesBySchool;   // Hash table for school bus lookup by school ID (key = schoolID, value = Bus*)
    HashTable* schoolBusesByNumber;    // Hash table for fast SchoolBus lookup by bus number (key = busNo, value = SchoolBus*)
    SinglyLinkedList* allBusesList;   // Linked list to track all buses for iteration (stores busNo)
    SinglyLinkedList* allCompaniesList; // Linked list to track all companies for iteration (stores company name)
    Graph* cityGraph;                 // Pointer to shared city graph (for pathfinding and nearest queries)
    CircularQueue* passengerQueue;   // Circular queue for passenger queue simulation (optional)
    Stack* routeHistory;              // Stack for route travel history (optional)
    int busCount;                     // Current number of buses
    int companyCount;                 // Current number of registered companies
    int schoolBusCount;               // Current number of school buses
    
    // Helper function to get all buses as array
    void getAllBuses(Bus** buses, int& count) const;
    
    // Helper function to get all companies as array
    void getAllCompanies(string* companies, int& count) const;
    
    // Helper function to get all buses by company (non-const version for internal use)
    void getBusesByCompanyInternal(const string& companyName, Bus** buses, int& count) const;
    
public:
    // Helper function to get SchoolBus pointer from bus number (public for menu access)
    SchoolBus* getSchoolBusByNumber(const string& busNo) const;
    // Constructor
    // Parameters: graph - pointer to shared city graph, queueCapacity - capacity for passenger queue
    TransportManager(Graph* graph = nullptr, int queueCapacity = 100);
    
    // Destructor
    ~TransportManager();
    
    // Set the city graph (if not provided in constructor)
    void setCityGraph(Graph* graph);
    
    // Get the city graph
    Graph* getCityGraph() const;
    
    // Bus management
    // Add a bus to the manager
    // Returns: true if successful, false if bus already exists
    bool addBus(Bus* bus);
    
    // Find a bus by number
    // Returns: Pointer to Bus if found, nullptr otherwise
    Bus* findBusByNumber(const string& busNo) const;
    
    // Get number of buses
    int getBusCount() const;
    
    // Get all buses (for iteration)
    // Parameters: buses array (output), count (output)
    // Note: Caller must allocate buses array with sufficient size
    void getAllBusesPublic(Bus** buses, int& count) const;
    
    // Display all buses
    void displayAllBuses() const;
    
    // Display all bus routes
    void displayAllRoutes() const;
    
    // Transport company management
    // Register a transport company
    // Returns: true if successful, false if company already exists
    bool registerCompany(const string& companyName);
    
    // Find if a company is registered
    // Returns: true if company is registered, false otherwise
    bool isCompanyRegistered(const string& companyName) const;
    
    // Get number of registered companies
    int getCompanyCount() const;
    
    // Get all buses belonging to a specific company
    // Parameters: companyName, buses array (output), count (output)
    // Note: Caller must allocate buses array with sufficient size
    void getBusesByCompany(const string& companyName, Bus** buses, int& count) const;
    
    // Display all registered companies
    void displayAllCompanies() const;
    
    // School bus management
    // Add a school bus to the manager
    // Returns: true if successful, false if bus already exists or invalid
    bool addSchoolBus(SchoolBus* bus);
    
    // Get all school buses for a specific school
    // Parameters: schoolID, buses array (output), count (output)
    // Note: Caller must allocate buses array with sufficient size
    void getSchoolBuses(const string& schoolID, Bus** buses, int& count) const;
    
    // Get number of school buses
    int getSchoolBusCount() const;
    
    // Get number of school buses for a specific school
    int getSchoolBusCountForSchool(const string& schoolID) const;
    
    // Student tracking on school buses
    // Track a student boarding a school bus
    // Returns: true if successful, false if bus not found, bus full, or student already on bus
    bool trackStudentOnBus(const string& busNo, const string& studentID);
    
    // Remove a student from a school bus (alighting)
    // Returns: true if successful, false if bus not found or student not on bus
    bool removeStudentFromBus(const string& busNo, const string& studentID);
    
    // Check if a student is on any bus
    // Parameters: studentID, busNo (output) - returns the bus number if found
    // Returns: true if student found on any bus, false otherwise
    bool isStudentOnAnyBus(const string& studentID, string& busNo) const;
    
    // Get all students on a specific school bus
    // Parameters: busNo, students array (output), count (output)
    // Note: Caller must allocate students array with sufficient size
    void getStudentsOnBus(const string& busNo, string* students, int& count) const;
    
    // Find nearest school bus for a specific school
    // Returns: Pointer to nearest SchoolBus, nullptr if no buses or graph unavailable
    Bus* findNearestSchoolBus(const string& schoolID, const string& fromStopID);
    
    // Find nearest school bus for a specific school from coordinates
    // Returns: Pointer to nearest SchoolBus, nullptr if no buses or graph unavailable
    Bus* findNearestSchoolBus(const string& schoolID, double latitude, double longitude);
    
    // Display all school buses
    void displayAllSchoolBuses() const;
    
    // Display school buses for a specific school
    void displaySchoolBusesForSchool(const string& schoolID) const;
    
    // Route management
    // Add a stop to a bus route
    // Returns: true if successful, false if bus not found or stop already in route
    bool addStopToBusRoute(const string& busNo, const string& stopID);
    
    // Remove a stop from a bus route
    // Returns: true if successful, false if bus not found or stop not in route
    bool removeStopFromBusRoute(const string& busNo, const string& stopID);
    
    // Get route for a specific bus
    // Parameters: busNo, stops array (output), count (output)
    // Note: Caller must allocate stops array with sufficient size
    void getBusRoute(const string& busNo, string* stops, int& count);
    
    // Pathfinding
    // Find shortest path between two stops using Dijkstra's algorithm
    // Parameters: startStopID, endStopID, path array (output), pathLength (output), totalDistance (output)
    // Returns: true if path found, false otherwise
    // Note: Caller must allocate path array with sufficient size
    bool findShortestPath(const string& startStopID, const string& endStopID, 
                         string* path, int& pathLength, double& totalDistance);
    
    // Find nearest bus to a given stop ID
    // Returns: Pointer to nearest Bus, nullptr if no buses or graph unavailable
    Bus* findNearestBus(const string& fromStopID);
    
    // Find nearest bus to given coordinates
    // Returns: Pointer to nearest Bus, nullptr if no buses or graph unavailable
    Bus* findNearestBus(double latitude, double longitude);
    
    // Find buses passing through a specific stop
    // Parameters: stopID, buses array (output), count (output)
    // Note: Caller must allocate buses array with sufficient size
    void findBusesAtStop(const string& stopID, Bus** buses, int& count);
    
    // Bus location tracking
    // Update bus location (move to next stop)
    // Returns: true if successful, false if bus not found
    bool moveBusToNextStop(const string& busNo);
    
    // Set bus location to a specific stop
    // Returns: true if successful, false if bus not found or stop not in route
    bool setBusLocation(const string& busNo, const string& stopID);
    
    // Get current location of a bus
    // Returns: stopID if bus found, empty string otherwise
    string getBusLocation(const string& busNo) const;
    
    // Passenger queue management (optional)
    // Add passenger to queue
    // Returns: true if successful, false if queue is full
    bool enqueuePassenger(const string& passengerID);
    
    // Remove passenger from queue
    // Returns: passengerID if successful, empty string if queue is empty
    string dequeuePassenger();
    
    // Get passenger queue size
    int getPassengerQueueSize() const;
    
    // Route history management (optional)
    // Push route to history
    void pushRouteToHistory(const string& routeInfo);
    
    // Pop route from history
    // Returns: routeInfo if successful, empty string if stack is empty
    string popRouteFromHistory();
    
    // Get route history size
    int getRouteHistorySize() const;
    
    // CSV data loading
    // Load buses from CSV data
    // Parameters: busData - array of CSVRow from DataLoader, busCount - number of rows
    // Format expected: [0]=BusNo, [1]=Company, [2]=CurrentStop, [3]=Route (e.g., "Stop1 > Stop2 > Stop3")
    // Returns: true if successful, false otherwise
    bool loadBusesFromCSVData(CSVRow* busData, int busCount);
    
    // Load stops from CSV data and add to graph
    // Parameters: stopData - array of CSVRow from DataLoader, stopCount - number of rows
    // Format expected: [0]=StopID, [1]=Name, [2]=Coordinates (e.g., "33.684, 73.025")
    // Returns: true if successful, false otherwise
    bool loadStopsFromCSVData(CSVRow* stopData, int stopCount);
    
    // Load school buses from CSV data
    // Parameters: schoolBusData - array of CSVRow from DataLoader, schoolBusCount - number of rows
    // Format expected: [0]=BusNo, [1]=Company, [2]=CurrentStop, [3]=SchoolID, [4]=MaxCapacity, [5]=Route (e.g., "Stop1 > Stop2 > Stop3")
    // Returns: true if successful, false otherwise
    bool loadSchoolBusesFromCSVData(CSVRow* schoolBusData, int schoolBusCount);
    
    // Clear all buses and reset queues
    void clear();
};

#endif // TRANSPORTMANAGER_H

