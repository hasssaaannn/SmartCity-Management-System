#ifndef RAILWAYSTATION_H
#define RAILWAYSTATION_H

#include <string>
using namespace std;

// RailwayStation class representing a railway station
// Used for: Airport/Rail integration (bonus module)
class RailwayStation {
private:
    string stationID;      // Unique identifier (e.g., "RLY01")
    string name;          // Station name (e.g., "Islamabad Railway Station")
    string code;          // Station code (e.g., "ISB-RLY" or station number)
    string city;          // City location (e.g., "Islamabad")
    string vertexID;      // Graph vertex ID this station is associated with (same as stationID)
    double latitude;      // Station latitude
    double longitude;     // Station longitude
    
public:
    // Constructor
    RailwayStation(const string& id = "", 
                   const string& n = "", 
                   const string& c = "", 
                   const string& cityName = "",
                   const string& vertex = "",
                   double lat = 0.0,
                   double lon = 0.0);
    
    // Destructor
    ~RailwayStation();
    
    // Getters
    string getStationID() const;
    string getName() const;
    string getCode() const;
    string getCity() const;
    string getVertexID() const;
    double getLatitude() const;
    double getLongitude() const;
    
    // Setters
    void setStationID(const string& id);
    void setName(const string& n);
    void setCode(const string& c);
    void setCity(const string& cityName);
    void setVertexID(const string& vertex);
    void setCoordinates(double lat, double lon);
    
    // Display station information
    void display() const;
    
    // Check if station is valid (has required fields)
    bool isValid() const;
};

#endif // RAILWAYSTATION_H

