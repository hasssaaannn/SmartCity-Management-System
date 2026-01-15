#ifndef RAILWAYMANAGER_H
#define RAILWAYMANAGER_H

#include <string>
#include "core_classes/Graph.h"
#include "core_classes/HashTable.h"
#include "core_classes/SinglyLinkedList.h"
#include "core_classes/DataLoader.h"
#include "RailwayStation.h"
using namespace std;

// RailwayManager class - Central manager for all railway stations
// Used for: Airport/Rail integration (bonus module), railway station management, station-to-station travel
class RailwayManager {
private:
    HashTable* stationsByID;          // Hash table for station lookup by ID (key = stationID, value = RailwayStation*)
    HashTable* stationsByName;        // Hash table for station lookup by name (key = station name, value = RailwayStation*)
    HashTable* stationsByCode;        // Hash table for station lookup by code (key = code, value = RailwayStation*)
    SinglyLinkedList* allStationsList; // Linked list to track all stations for iteration (stores stationID)
    Graph* cityGraph;                  // Pointer to shared city graph (for nearest bus stop connection)
    int stationCount;                  // Current number of stations
    
    // Helper function to get all stations as array
    void getAllStations(RailwayStation** stations, int& count) const;
    
public:
    // Constructor
    // Parameters: graph - pointer to shared city graph
    RailwayManager(Graph* graph = nullptr);
    
    // Destructor
    ~RailwayManager();
    
    // Set the city graph (if not provided in constructor)
    void setCityGraph(Graph* graph);
    
    // Get the city graph
    Graph* getCityGraph() const;
    
    // Station management
    // Add a station to the manager
    // Returns: true if successful, false if station already exists
    // Note: Automatically connects station to nearest bus stop in graph
    bool addStation(RailwayStation* station);
    
    // Find a station by ID
    // Returns: Pointer to RailwayStation if found, nullptr otherwise
    RailwayStation* findStationByID(const string& stationID) const;
    
    // Find a station by name
    // Returns: Pointer to RailwayStation if found, nullptr otherwise
    RailwayStation* findStationByName(const string& name) const;
    
    // Find a station by code
    // Returns: Pointer to RailwayStation if found, nullptr otherwise
    RailwayStation* findStationByCode(const string& code) const;
    
    // Get number of stations
    int getStationCount() const;
    
    // Display all stations
    void displayAllStations() const;
    
    // Nearest station queries
    // Find nearest station to a given stop ID
    // Returns: Pointer to nearest RailwayStation, nullptr if no stations or graph unavailable
    RailwayStation* findNearestStation(const string& fromStopID);
    
    // Find nearest station to given coordinates
    // Returns: Pointer to nearest RailwayStation, nullptr if no stations or graph unavailable
    RailwayStation* findNearestStation(double latitude, double longitude);
    
    // Station-to-station travel
    // Calculate rail distance between two stations (straight line distance)
    // Parameters: fromStationID, toStationID, distance (output)
    // Returns: true if both stations found, false otherwise
    bool calculateRailDistance(const string& fromStationID, const string& toStationID, double& distance);
    
    // Find path from one station to another (direct train route)
    // Parameters: fromStationID, toStationID, path array (output), pathLength (output), distance (output)
    // Returns: true if path found, false otherwise
    // Note: For stations, path is always direct (2 stops: source station -> destination station)
    // Note: Caller must allocate path array with sufficient size (at least 2)
    bool findStationPath(const string& fromStationID, const string& toStationID, 
                        string* path, int& pathLength, double& distance);
    
    // CSV data loading
    // Load stations from CSV data
    // Parameters: stationData - array of CSVRow from DataLoader, stationCount - number of rows
    // Format expected: [0]=stationID, [1]=name, [2]=code, [3]=city, [4]=coordinates (e.g., "33.684, 73.025")
    // Returns: true if successful, false otherwise
    bool loadStationsFromCSVData(CSVRow* stationData, int stationCount);
    
    // Clear all stations
    void clear();
};

#endif // RAILWAYMANAGER_H

