#ifndef FACILITYMANAGER_H
#define FACILITYMANAGER_H

#include <string>
#include "core_classes/Graph.h"
#include "core_classes/HashTable.h"
#include "core_classes/SinglyLinkedList.h"
#include "core_classes/MinHeap.h"
#include "core_classes/DataLoader.h"
#include "Facility.h"
using namespace std;

// FacilityManager class - Central manager for all public facilities
// Used for: Public facilities management, facility searches, nearest facility queries
class FacilityManager {
private:
    HashTable* facilitiesByID;          // Hash table for facility lookup by ID (key = facilityID, value = Facility*)
    HashTable* facilitiesByName;        // Hash table for facility lookup by name (key = facility name, value = Facility*)
    HashTable* facilitiesByType;        // Hash table for facility lookup by type (key = type, value = Facility*)
    SinglyLinkedList* allFacilitiesList; // Linked list to track all facilities for iteration (stores facilityID)
    Graph* cityGraph;                    // Pointer to shared city graph (for nearest facility queries)
    int facilityCount;                   // Current number of facilities
    
    // Helper function to get all facilities as array
    void getAllFacilities(Facility** facilities, int& count) const;
    
    // Helper: Get sector coordinates for coordinate-based searches
    static void getSectorCoordinates(const string& sector, double& latitude, double& longitude);
    
public:
    // Constructor
    // Parameters: graph - pointer to shared city graph
    FacilityManager(Graph* graph = nullptr);
    
    // Destructor
    ~FacilityManager();
    
    // Set the city graph (if not provided in constructor)
    void setCityGraph(Graph* graph);
    
    // Get the city graph
    Graph* getCityGraph() const;
    
    // Facility management
    // Add a facility to the manager
    // Returns: true if successful, false if facility already exists
    bool addFacility(Facility* facility);
    
    // Find a facility by ID
    // Returns: Pointer to Facility if found, nullptr otherwise
    Facility* findFacilityByID(const string& facilityID) const;
    
    // Find a facility by name
    // Returns: Pointer to Facility if found, nullptr otherwise
    Facility* findFacilityByName(const string& name) const;
    
    // Find all facilities of a specific type
    // Parameters: type, results array (output), count (output)
    // Note: Caller must allocate results array with sufficient size
    void findFacilitiesByType(const string& type, Facility** results, int& count);
    
    // Get number of facilities
    int getFacilityCount() const;
    
    // Display all facilities
    void displayAllFacilities() const;
    
    // Nearest facility queries
    // Find nearest facility to a given stop ID
    // Returns: Pointer to nearest Facility, nullptr if no facilities or graph unavailable
    Facility* findNearestFacility(const string& fromStopID);
    
    // Find nearest facility to given coordinates
    // Returns: Pointer to nearest Facility, nullptr if no facilities or graph unavailable
    Facility* findNearestFacility(double latitude, double longitude);
    
    // Find nearest facility of a specific type
    // Returns: Pointer to nearest Facility of given type, nullptr if not found
    Facility* findNearestFacilityByType(const string& type, const string& fromStopID);
    
    // CSV data loading
    // Load facilities from CSV data
    // Parameters: facilityData - array of CSVRow from DataLoader, facilityCount - number of rows
    // Format expected: [0]=facilityID, [1]=name, [2]=type, [3]=sector, [4]=vertexID (or similar)
    // Returns: true if successful, false otherwise
    bool loadFacilitiesFromCSVData(CSVRow* facilityData, int facilityCount);
    
    // Clear all facilities
    void clear();
};

#endif // FACILITYMANAGER_H

