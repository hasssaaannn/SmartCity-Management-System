#ifndef MALLMANAGER_H
#define MALLMANAGER_H

#include <string>
#include "core_classes/Graph.h"
#include "core_classes/HashTable.h"
#include "core_classes/SinglyLinkedList.h"
#include "core_classes/DataLoader.h"
#include "Mall.h"
#include "Product.h"
using namespace std;

// MallManager class - Central manager for all malls and commercial operations
// Used for: Commercial sector management, mall operations, product searches, nearest mall queries
class MallManager {
private:
    HashTable* mallsByID;              // Hash table for mall lookup by ID (key = mallID, value = Mall*)
    HashTable* mallsByName;            // Hash table for mall lookup by name (key = mall name, value = Mall*)
    SinglyLinkedList* allMallsList;    // Linked list to track all malls for iteration (stores mallID)
    Graph* cityGraph;                  // Pointer to shared city graph (for nearest mall queries)
    HashTable* globalProductsByName;    // Global hash table for cross-mall product search (key = product name, value = Product*)
    int mallCount;                     // Current number of malls
    
    // Helper function to get all malls as array
    void getAllMalls(Mall** malls, int& count) const;
    
public:
    // Constructor
    // Parameters: graph - pointer to shared city graph
    MallManager(Graph* graph = nullptr);
    
    // Destructor
    ~MallManager();
    
    // Set the city graph (if not provided in constructor)
    void setCityGraph(Graph* graph);
    
    // Get the city graph
    Graph* getCityGraph() const;
    
    // Mall management
    // Add a mall to the manager
    // Returns: true if successful, false if mall already exists
    bool addMall(Mall* mall);
    
    // Find a mall by ID
    // Returns: Pointer to Mall if found, nullptr otherwise
    Mall* findMallByID(const string& mallID) const;
    
    // Find a mall by name
    // Returns: Pointer to Mall if found, nullptr otherwise
    Mall* findMallByName(const string& name) const;
    
    // Get number of malls
    int getMallCount() const;
    
    // Display all malls
    void displayAllMalls() const;
    
    // Product management
    // Add a product to a specific mall
    // Returns: true if successful, false if mall not found or product already exists
    bool addProductToMall(const string& mallID, Product* product);
    
    // Find a product in a specific mall
    // Returns: Pointer to Product if found, nullptr otherwise
    Product* findProductInMall(const string& mallID, const string& productName);
    
    // Find a product anywhere (searches all malls)
    // Returns: Pointer to Product if found, nullptr otherwise
    Product* findProductAnywhere(const string& productName);
    
    // Find all products in a specific category across all malls
    // Parameters: category, results array (output), count (output)
    // Note: Caller must allocate results array with sufficient size
    void findProductsByCategory(const string& category, Product** results, int& count);
    
    // Nearest mall queries
    // Find nearest mall to a given stop ID
    // Returns: Pointer to nearest Mall, nullptr if no malls or graph unavailable
    Mall* findNearestMall(const string& fromStopID);
    
    // Find nearest mall to given coordinates
    // Returns: Pointer to nearest Mall, nullptr if no malls or graph unavailable
    Mall* findNearestMall(double latitude, double longitude);
    
    // CSV data loading (stubs for DataLoader integration)
    // Load malls from CSV data
    // Parameters: mallData - array of CSVRow from DataLoader, mallCount - number of rows
    // Format expected: [0]=mallID, [1]=name, [2]=sector, [3]=vertexID (or similar)
    // Returns: true if successful, false otherwise
    bool loadMallsFromCSVData(CSVRow* mallData, int mallCount);
    
    // Load products from CSV data
    // Parameters: productData - array of CSVRow from DataLoader, productCount - number of rows
    // Format expected: [0]=productID, [1]=name, [2]=category, [3]=price, [4]=mallID (or similar)
    // Returns: true if successful, false otherwise
    bool loadProductsFromCSVData(CSVRow* productData, int productCount);
    
    // Clear all malls and products
    void clear();
};

#endif // MALLMANAGER_H

