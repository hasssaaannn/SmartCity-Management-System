#ifndef FACILITY_H
#define FACILITY_H

#include <string>
using namespace std;

// Facility class representing a public facility (mosque, park, water cooler, etc.)
// Used for: Public facilities sector management
class Facility {
private:
    string facilityID;      // Unique identifier (e.g., "FCL01")
    string name;            // Facility name (e.g., "Central Park", "Faisal Mosque")
    string type;            // Facility type (e.g., "Mosque", "Park", "Water Cooler")
    string sector;          // Sector location (e.g., "F-7", "G-10")
    string vertexID;        // Graph vertex ID this facility is associated with (e.g., "Stop7")
    
public:
    // Constructor
    // Parameters: id, name, type, sector, vertexID
    Facility(const string& id = "", 
             const string& n = "", 
             const string& t = "", 
             const string& sec = "", 
             const string& vertex = "");
    
    // Destructor
    ~Facility();
    
    // Getters
    string getFacilityID() const;
    string getName() const;
    string getType() const;
    string getSector() const;
    string getVertexID() const;
    
    // Setters
    void setFacilityID(const string& id);
    void setName(const string& name);
    void setType(const string& type);
    void setSector(const string& sector);
    void setVertexID(const string& vertexID);
    
    // Display facility information
    void display() const;
    
    // Check if facility is valid (has required fields)
    bool isValid() const;
};

#endif // FACILITY_H

