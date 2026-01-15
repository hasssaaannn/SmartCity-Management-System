#ifndef EMERGENCYVEHICLE_H
#define EMERGENCYVEHICLE_H

#include <string>
#include "Bus.h"
using namespace std;

// EmergencyVehicle class representing an emergency vehicle (ambulance, fire truck, etc.)
// Used for: Emergency transport routing (bonus module)
// Extends Bus class to inherit route and location management
class EmergencyVehicle : public Bus {
private:
    string vehicleType;      // Type: "Ambulance", "FireTruck", "Police"
    string vehicleID;         // Unique vehicle ID (e.g., "AMB01", "FIRE01")
    bool isAvailable;         // Currently available for dispatch
    int priorityLevel;        // 1=Critical, 2=Urgent, 3=Normal (for vehicle priority)
    string currentEmergencyID; // ID of current emergency being handled (empty if available)
    
public:
    // Constructor
    // Parameters: vehicleID, vehicleType, busNo, company, currentStopID
    EmergencyVehicle(const string& vehID = "",
                     const string& type = "",
                     const string& busNo = "",
                     const string& comp = "",
                     const string& currentStop = "");
    
    // Destructor
    virtual ~EmergencyVehicle();
    
    // Getters
    string getVehicleType() const;
    string getVehicleID() const;
    bool getIsAvailable() const;
    int getPriorityLevel() const;
    string getCurrentEmergencyID() const;
    
    // Setters
    void setVehicleType(const string& type);
    void setVehicleID(const string& vehID);
    void setIsAvailable(bool available);
    void setPriorityLevel(int level);
    void setCurrentEmergencyID(const string& emergencyID);
    
    // Emergency operations
    // Dispatch vehicle to an emergency
    // Returns: true if successful, false if vehicle not available
    bool dispatchToEmergency(const string& emergencyID);
    
    // Complete emergency and make vehicle available again
    // Returns: true if successful
    bool completeEmergency();
    
    // Check if vehicle is valid
    bool isValid() const;
    
    // Display vehicle information (override Bus::display())
    void display() const;
};

#endif // EMERGENCYVEHICLE_H

