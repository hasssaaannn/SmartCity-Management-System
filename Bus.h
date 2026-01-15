#ifndef BUS_H
#define BUS_H

#include <string>
#include "core_classes/SinglyLinkedList.h"
using namespace std;

// Bus class representing a bus with its route
// Used for: Transport sector bus management
class Bus {
private:
    string busNo;                    // Bus number (e.g., "B101", "B102")
    string company;                  // Transport company name (e.g., "MetroBus", "CapitalTrans")
    string currentStopID;            // Current stop where bus is located (e.g., "Stop3")
    SinglyLinkedList* route;         // Singly linked list storing the route (ordered stops)
    
public:
    // Constructor
    // Parameters: busNo, company, currentStopID
    Bus(const string& busNo = "", 
        const string& comp = "", 
        const string& currentStop = "");
    
    // Destructor (virtual for proper inheritance cleanup)
    virtual ~Bus();
    
    // Getters
    string getBusNo() const;
    string getCompany() const;
    string getCurrentStopID() const;
    SinglyLinkedList* getRoute() const;
    int getRouteLength() const;
    
    // Setters
    void setBusNo(const string& busNo);
    void setCompany(const string& company);
    void setCurrentStopID(const string& stopID);
    
    // Route management
    // Add a stop to the route (at the end)
    // Returns: true if successful
    bool addStopToRoute(const string& stopID);
    
    // Add a stop at a specific position in the route
    // Returns: true if successful
    bool addStopAtPosition(int position, const string& stopID);
    
    // Remove a stop from the route
    // Returns: true if stop was found and removed
    bool removeStopFromRoute(const string& stopID);
    
    // Remove stop at a specific position
    // Returns: true if successful
    bool removeStopAtPosition(int position);
    
    // Get stop at a specific position in the route
    // Returns: stopID at position, empty string if invalid position
    string getStopAtPosition(int position);
    
    // Get all stops in route as array
    // Parameters: stops array (output), count (output)
    void getRouteStops(string* stops, int& count);
    
    // Check if a stop is in the route
    // Returns: true if stop exists in route
    bool isStopInRoute(const string& stopID);
    
    // Update current location (move bus to next stop)
    // Returns: true if successfully moved to next stop
    bool moveToNextStop();
    
    // Set current location to a specific stop (if stop is in route)
    // Returns: true if stop is in route and location updated
    bool setCurrentLocation(const string& stopID);
    
    // Display the bus route
    void displayRoute();
    
    // Display bus information
    void display();
    
    // Check if bus is valid
    bool isValid() const;
};

#endif // BUS_H

