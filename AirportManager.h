#ifndef AIRPORTMANAGER_H
#define AIRPORTMANAGER_H

#include <string>
#include "core_classes/Graph.h"
#include "core_classes/HashTable.h"
#include "core_classes/SinglyLinkedList.h"
#include "core_classes/DataLoader.h"
#include "Airport.h"
using namespace std;

// AirportManager class - Central manager for all airports
// Used for: Airport/Rail integration (bonus module), airport management, airport-to-airport travel
class AirportManager {
private:
    HashTable* airportsByID;          // Hash table for airport lookup by ID (key = airportID, value = Airport*)
    HashTable* airportsByName;         // Hash table for airport lookup by name (key = airport name, value = Airport*)
    HashTable* airportsByCode;         // Hash table for airport lookup by IATA code (key = code, value = Airport*)
    SinglyLinkedList* allAirportsList; // Linked list to track all airports for iteration (stores airportID)
    Graph* cityGraph;                  // Pointer to shared city graph (for nearest bus stop connection)
    int airportCount;                   // Current number of airports
    
    // Helper function to get all airports as array
    void getAllAirports(Airport** airports, int& count) const;
    
public:
    // Constructor
    // Parameters: graph - pointer to shared city graph
    AirportManager(Graph* graph = nullptr);
    
    // Destructor
    ~AirportManager();
    
    // Set the city graph (if not provided in constructor)
    void setCityGraph(Graph* graph);
    
    // Get the city graph
    Graph* getCityGraph() const;
    
    // Airport management
    // Add an airport to the manager
    // Returns: true if successful, false if airport already exists
    // Note: Automatically connects airport to nearest bus stop in graph
    bool addAirport(Airport* airport);
    
    // Find an airport by ID
    // Returns: Pointer to Airport if found, nullptr otherwise
    Airport* findAirportByID(const string& airportID) const;
    
    // Find an airport by name
    // Returns: Pointer to Airport if found, nullptr otherwise
    Airport* findAirportByName(const string& name) const;
    
    // Find an airport by IATA code
    // Returns: Pointer to Airport if found, nullptr otherwise
    Airport* findAirportByCode(const string& code) const;
    
    // Get number of airports
    int getAirportCount() const;
    
    // Display all airports
    void displayAllAirports() const;
    
    // Nearest airport queries
    // Find nearest airport to a given stop ID
    // Returns: Pointer to nearest Airport, nullptr if no airports or graph unavailable
    Airport* findNearestAirport(const string& fromStopID);
    
    // Find nearest airport to given coordinates
    // Returns: Pointer to nearest Airport, nullptr if no airports or graph unavailable
    Airport* findNearestAirport(double latitude, double longitude);
    
    // Airport-to-airport travel
    // Calculate air distance between two airports (straight line distance)
    // Parameters: fromAirportID, toAirportID, distance (output)
    // Returns: true if both airports found, false otherwise
    bool calculateAirDistance(const string& fromAirportID, const string& toAirportID, double& distance);
    
    // Find path from one airport to another (direct flight)
    // Parameters: fromAirportID, toAirportID, path array (output), pathLength (output), distance (output)
    // Returns: true if path found, false otherwise
    // Note: For airports, path is always direct (2 stops: source airport -> destination airport)
    // Note: Caller must allocate path array with sufficient size (at least 2)
    bool findAirportPath(const string& fromAirportID, const string& toAirportID, 
                        string* path, int& pathLength, double& distance);
    
    // CSV data loading
    // Load airports from CSV data
    // Parameters: airportData - array of CSVRow from DataLoader, airportCount - number of rows
    // Format expected: [0]=airportID, [1]=name, [2]=code, [3]=city, [4]=coordinates (e.g., "33.684, 73.025")
    // Returns: true if successful, false otherwise
    bool loadAirportsFromCSVData(CSVRow* airportData, int airportCount);
    
    // Clear all airports
    void clear();
};

#endif // AIRPORTMANAGER_H

