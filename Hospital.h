#ifndef HOSPITAL_H
#define HOSPITAL_H

#include <string>
using namespace std;

// Hospital entity class
// Represents a hospital in the smart city system
class Hospital {
private:
    string hospitalID;      // Unique identifier (e.g., H01, H02)
    string name;            // Hospital name (e.g., PIMS, Shifa International)
    string sector;          // Location sector (e.g., G-8, H-8)
    int emergencyBeds;           // Number of emergency beds available
    string specialization;  // Medical specialization(s) (e.g., "General, Cardiology")
    
public:
    // Constructor
    Hospital();
    Hospital(const string& id, const string& n, const string& sec, 
             int beds, const string& spec);
    
    // Destructor
    ~Hospital();
    
    // Getters
    string getHospitalID() const;
    string getName() const;
    string getSector() const;
    int getEmergencyBeds() const;
    string getSpecialization() const;
    
    // Setters
    void setHospitalID(const string& id);
    void setName(const string& n);
    void setSector(const string& sec);
    void setEmergencyBeds(int beds);
    void setSpecialization(const string& spec);
    
    // Check if hospital has available emergency beds
    bool hasAvailableBeds() const;
};

#endif // HOSPITAL_H

