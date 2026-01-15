#ifndef PHARMACY_H
#define PHARMACY_H

#include <string>
#include "Medicine.h"
using namespace std;

// Pharmacy entity class
// Represents a pharmacy in the smart city system
class Pharmacy {
private:
    string pharmacyID;      // Unique identifier (e.g., P01, P02)
    string name;            // Pharmacy name (e.g., Sehat Pharmacy)
    string sector;          // Location sector (e.g., F-8, G-10)
    Medicine* medicines;         // Dynamic array of medicines
    int medicineCount;           // Number of medicines
    int medicineCapacity;        // Capacity of medicines array
    
    // Helper: Resize medicines array when needed
    void resizeMedicines();
    
public:
    // Constructor
    Pharmacy();
    Pharmacy(const string& id, const string& n, const string& sec);
    
    // Destructor
    ~Pharmacy();
    
    // Copy constructor
    Pharmacy(const Pharmacy& other);
    
    // Assignment operator
    Pharmacy& operator=(const Pharmacy& other);
    
    // Getters
    string getPharmacyID() const;
    string getName() const;
    string getSector() const;
    int getMedicineCount() const;
    Medicine* getMedicines() const;
    
    // Setters
    void setPharmacyID(const string& id);
    void setName(const string& n);
    void setSector(const string& sec);
    
    // Add a medicine to the pharmacy
    void addMedicine(const Medicine& medicine);
    
    // Search for a medicine by name
    Medicine* findMedicineByName(const string& name);
    
    // Search for a medicine by formula
    Medicine* findMedicineByFormula(const string& formula);
    
    // Check if pharmacy has a specific medicine
    bool hasMedicine(const string& name) const;
};

#endif // PHARMACY_H

