#ifndef AIRPORT_H
#define AIRPORT_H

#include <string>
using namespace std;

// Airport class representing an airport
// Used for: Airport/Rail integration (bonus module)
class Airport {
private:
    string airportID;      // Unique identifier (e.g., "AIR01")
    string name;            // Airport name (e.g., "Islamabad International Airport")
    string code;            // IATA code (e.g., "ISB")
    string city;            // City location (e.g., "Islamabad")
    string vertexID;        // Graph vertex ID this airport is associated with (same as airportID)
    double latitude;        // Airport latitude
    double longitude;       // Airport longitude
    
public:
    // Constructor
    // Parameters: id, name, code, cityName, vertexID, lat, lon
    Airport(const string& id = "", 
            const string& n = "", 
            const string& c = "", 
            const string& cityName = "",
            const string& vertex = "",
            double lat = 0.0,
            double lon = 0.0);
    
    // Destructor
    ~Airport();
    
    // Getters
    string getAirportID() const;
    string getName() const;
    string getCode() const;
    string getCity() const;
    string getVertexID() const;
    double getLatitude() const;
    double getLongitude() const;
    
    // Setters
    void setAirportID(const string& id);
    void setName(const string& n);
    void setCode(const string& c);
    void setCity(const string& city);
    void setVertexID(const string& vertex);
    void setCoordinates(double lat, double lon);
    
    // Display airport information
    void display() const;
    
    // Check if airport is valid (has required fields)
    bool isValid() const;
};

#endif // AIRPORT_H

