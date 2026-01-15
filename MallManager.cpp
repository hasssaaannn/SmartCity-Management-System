#include "MallManager.h"
#include <iostream>
using namespace std;

// Constructor
MallManager::MallManager(Graph* graph)
    : cityGraph(graph), mallCount(0) {
    mallsByID = new HashTable(101);
    mallsByName = new HashTable(101);
    allMallsList = new SinglyLinkedList();
    globalProductsByName = new HashTable(503); // Larger size for global product search
}

// Destructor
MallManager::~MallManager() {
    clear();
    
    // Delete hash tables and list
    if (mallsByID != nullptr) {
        delete mallsByID;
        mallsByID = nullptr;
    }
    if (mallsByName != nullptr) {
        delete mallsByName;
        mallsByName = nullptr;
    }
    if (allMallsList != nullptr) {
        delete allMallsList;
        allMallsList = nullptr;
    }
    if (globalProductsByName != nullptr) {
        delete globalProductsByName;
        globalProductsByName = nullptr;
    }
}

// Set the city graph
void MallManager::setCityGraph(Graph* graph) {
    cityGraph = graph;
}

// Get the city graph
Graph* MallManager::getCityGraph() const {
    return cityGraph;
}

// Helper function to get all malls as array
void MallManager::getAllMalls(Mall** malls, int& count) const {
    count = 0;
    if (malls == nullptr || mallCount == 0) return;
    
    // Get all mall IDs from linked list
    int idCount = allMallsList->getSize();
    if (idCount == 0) return;
    
    string* mallIDs = new string[idCount];
    int actualCount = 0;
    allMallsList->toArray(mallIDs, actualCount);
    
    // Convert IDs to Mall pointers
    for (int i = 0; i < actualCount; i++) {
        Mall* mall = findMallByID(mallIDs[i]);
        if (mall != nullptr) {
            malls[count] = mall;
            count++;
        }
    }
    
    delete[] mallIDs;
}

// Add a mall to the manager
bool MallManager::addMall(Mall* mall) {
    if (mall == nullptr) return false;
    if (!mall->isValid()) return false;
    
    // Check if mall already exists
    if (findMallByID(mall->getMallID()) != nullptr) {
        return false; // Mall already exists
    }
    
    // Add to both hash tables
    mallsByID->insert(mall->getMallID(), (void*)mall);
    mallsByName->insert(mall->getName(), (void*)mall);
    
    // Add mall ID to list for iteration
    allMallsList->insertAtTail(mall->getMallID());
    
    mallCount++;
    return true;
}

// Find a mall by ID
Mall* MallManager::findMallByID(const string& mallID) const {
    if (mallID.empty()) return nullptr;
    
    void* data = mallsByID->search(mallID);
    if (data != nullptr) {
        return (Mall*)data;
    }
    return nullptr;
}

// Find a mall by name
Mall* MallManager::findMallByName(const string& name) const {
    if (name.empty()) return nullptr;
    
    void* data = mallsByName->search(name);
    if (data != nullptr) {
        return (Mall*)data;
    }
    return nullptr;
}

// Get number of malls
int MallManager::getMallCount() const {
    return mallCount;
}

// Display all malls
void MallManager::displayAllMalls() const {
    if (mallCount == 0) {
        cout << "No malls registered." << endl;
        return;
    }
    
    cout << "Registered Malls (" << mallCount << " malls):" << endl;
    
    // Get all malls
    Mall** malls = new Mall*[mallCount];
    int count = 0;
    getAllMalls(malls, count);
    
    // Display each mall
    for (int i = 0; i < count; i++) {
        cout << "  [" << (i + 1) << "] ";
        malls[i]->display();
        cout << endl;
    }
    
    delete[] malls;
}

// Add a product to a specific mall
bool MallManager::addProductToMall(const string& mallID, Product* product) {
    if (mallID.empty() || product == nullptr) return false;
    
    Mall* mall = findMallByID(mallID);
    if (mall == nullptr) return false;
    
    // Add product to the mall
    if (!mall->addProduct(product)) {
        return false; // Product already exists in mall
    }
    
    // Also add to global products hash table for cross-mall search
    globalProductsByName->insert(product->getName(), (void*)product);
    
    return true;
}

// Find a product in a specific mall
Product* MallManager::findProductInMall(const string& mallID, const string& productName) {
    if (mallID.empty() || productName.empty()) return nullptr;
    
    Mall* mall = findMallByID(mallID);
    if (mall == nullptr) return nullptr;
    
    return mall->findProduct(productName);
}

// Find a product anywhere (searches all malls)
Product* MallManager::findProductAnywhere(const string& productName) {
    if (productName.empty()) return nullptr;
    
    void* data = globalProductsByName->search(productName);
    if (data != nullptr) {
        return (Product*)data;
    }
    return nullptr;
}

// Find all products in a specific category across all malls
void MallManager::findProductsByCategory(const string& category, Product** results, int& count) {
    count = 0;
    if (results == nullptr || category.empty()) return;
    
    // Get all malls
    Mall** malls = new Mall*[mallCount];
    int mallCountActual = 0;
    getAllMalls(malls, mallCountActual);
    
    // Search each mall for products in the category
    for (int i = 0; i < mallCountActual; i++) {
        Product* mallResults[100]; // Temporary array for each mall's results
        int mallResultCount = 0;
        
        malls[i]->findProductsByCategory(category, mallResults, mallResultCount);
        
        // Add to overall results
        for (int j = 0; j < mallResultCount; j++) {
            results[count] = mallResults[j];
            count++;
        }
    }
    
    delete[] malls;
}

// Find nearest mall to a given stop ID
Mall* MallManager::findNearestMall(const string& fromStopID) {
    if (fromStopID.empty() || cityGraph == nullptr || mallCount == 0) {
        return nullptr;
    }
    
    Mall* nearestMall = nullptr;
    double minDistance = -1.0;
    
    // Get all malls
    Mall** malls = new Mall*[mallCount];
    int count = 0;
    getAllMalls(malls, count);
    
    // For each mall, calculate distance from fromStopID to mall's vertexID
    for (int i = 0; i < count; i++) {
        string mallVertexID = malls[i]->getVertexID();
        if (mallVertexID.empty()) continue;
        
        // Use graph to find shortest path
        int pathLength = 0;
        double distance = 0.0;
        string* path = cityGraph->findShortestPath(fromStopID, mallVertexID, pathLength, distance);
        
        if (path != nullptr && distance >= 0.0) {
            // Valid path found
            if (minDistance < 0.0 || distance < minDistance) {
                minDistance = distance;
                nearestMall = malls[i];
            }
            delete[] path;
        }
    }
    
    delete[] malls;
    return nearestMall;
}

// Helper function to calculate simple squared distance (Euclidean, no sqrt needed for comparison)
static double calculateDistanceSquared(double lat1, double lon1, double lat2, double lon2) {
    double dLat = lat2 - lat1;
    double dLon = lon2 - lon1;
    return dLat * dLat + dLon * dLon;
}

// Find nearest mall to given coordinates
Mall* MallManager::findNearestMall(double latitude, double longitude) {
    if (cityGraph == nullptr || mallCount == 0) {
        return nullptr;
    }
    
    // Iterate through all registered malls and find the nearest one using CSV coordinates from graph
    double minDistance = 1000000.0; // Large initial distance
    Mall* nearestMall = nullptr;
    
    // Get all malls
    Mall** malls = new Mall*[mallCount];
    int count = 0;
    getAllMalls(malls, count);
    
    // For each mall, get coordinates from graph vertex and calculate distance
    for (int i = 0; i < count; i++) {
        Mall* mall = malls[i];
        if (mall == nullptr) continue;
        
        // Get mall coordinates from graph vertex (using mallID as vertexID)
        string mallVertexID = mall->getVertexID();
        if (mallVertexID.empty()) continue;
        
        double mallLat, mallLon;
        if (!cityGraph->getVertexCoordinates(mallVertexID, mallLat, mallLon)) {
            // Vertex not found in graph, skip this mall
            continue;
        }
        
        // Calculate squared distance (no sqrt needed for comparison)
        double distSquared = calculateDistanceSquared(latitude, longitude, mallLat, mallLon);
        
        if (distSquared < minDistance) {
            minDistance = distSquared;
            nearestMall = mall;
        }
    }
    
    delete[] malls;
    return nearestMall;
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

// Load malls from CSV data
bool MallManager::loadMallsFromCSVData(CSVRow* mallData, int mallCount) {
    if (mallData == nullptr || mallCount <= 0 || cityGraph == nullptr) return false;
    
    // CSV Format: MallID,Name,Sector,Coordinates,Categories
    
    for (int i = 0; i < mallCount; i++) {
        if (mallData[i].fieldCount < 5) continue; // Skip invalid rows (need at least 5 fields)
        
        string mallID = mallData[i].fields[0];
        string name = mallData[i].fields[1];
        string sector = mallData[i].fields[2];
        string coordStr = mallData[i].fields[3];
        
        // Parse coordinates from CSV
        double lat = 0.0, lon = 0.0;
        
        if (!parseCoordinates(coordStr, lat, lon)) {
            // Invalid coordinates, skip this mall
            continue;
        }
        
        // Create new mall (use mallID as vertexID)
        Mall* mall = new Mall(mallID, name, sector, mallID);
        
        // Add to manager first
        if (!addMall(mall)) {
            // If add failed (duplicate), delete the mall to avoid memory leak
            delete mall;
            continue;
        }
        
        // Add mall as a vertex to the graph with CSV coordinates
        cityGraph->addVertex(mallID, name, lat, lon, mall);
        
        // Connect mall to the nearest bus stop for pathfinding
        cityGraph->connectToNearestStop(mallID);
    }
    
    return true;
}

// Load products from CSV data
bool MallManager::loadProductsFromCSVData(CSVRow* productData, int productCount) {
    if (productData == nullptr || productCount <= 0) return false;
    
    // CSV Format: ProductID,MallID,ProductName,Category,Price
    
    for (int i = 0; i < productCount; i++) {
        if (productData[i].fieldCount < 5) continue; // Skip invalid rows
        
        string productID = productData[i].fields[0];    // ProductID
        string mallID = productData[i].fields[1];       // MallID
        string name = productData[i].fields[2];         // ProductName
        string category = productData[i].fields[3];     // Category
        
        // Parse price (convert string to double)
        double price = 0.0;
        string priceStr = productData[i].fields[4];     // Price
        // Simple string to double conversion (manual parsing)
        bool hasDecimal = false;
        int intPart = 0;
        int decPart = 0;
        int decDigits = 0;
        bool negative = false;
        int startIdx = 0;
        
        if (priceStr.length() > 0 && priceStr[0] == '-') {
            negative = true;
            startIdx = 1;
        }
        
        for (int j = startIdx; j < (int)priceStr.length(); j++) {
            if (priceStr[j] == '.') {
                hasDecimal = true;
                continue;
            }
            if (priceStr[j] >= '0' && priceStr[j] <= '9') {
                if (!hasDecimal) {
                    intPart = intPart * 10 + (priceStr[j] - '0');
                } else {
                    decPart = decPart * 10 + (priceStr[j] - '0');
                    decDigits++;
                }
            }
        }
        
        // Calculate 10^decDigits manually (since we can't use cmath)
        double divisor = 1.0;
        for (int k = 0; k < decDigits; k++) {
            divisor *= 10.0;
        }
        price = intPart + (decPart / divisor);
        if (negative) price = -price;
        
        // Create new product (mallID already extracted above)
        Product* product = new Product(productID, name, category, price, mallID);
        
        // Add to the appropriate mall
        if (!addProductToMall(mallID, product)) {
            // If add failed, delete the product to avoid memory leak
            delete product;
        }
    }
    
    return true;
}

// Clear all malls and products
void MallManager::clear() {
    // Get all malls and delete them
    Mall** malls = new Mall*[mallCount];
    int count = 0;
    getAllMalls(malls, count);
    
    // Delete each mall (which will clean up its products)
    for (int i = 0; i < count; i++) {
        delete malls[i];
    }
    
    delete[] malls;
    
    // Clear hash tables and list
    if (mallsByID != nullptr) {
        mallsByID->clear();
    }
    if (mallsByName != nullptr) {
        mallsByName->clear();
    }
    if (allMallsList != nullptr) {
        allMallsList->clear();
    }
    if (globalProductsByName != nullptr) {
        globalProductsByName->clear();
    }
    
    mallCount = 0;
}

