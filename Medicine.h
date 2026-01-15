#ifndef MEDICINE_H
#define MEDICINE_H

#include <string>
using namespace std;

// Medicine entity class
// Represents a medicine in the pharmacy system
class Medicine {
private:
    string medicineName;    // Medicine name (e.g., Panadol, Augmentin)
    string formula;         // Chemical formula/compound (e.g., Paracetamol, Co-Amoxiclav)
    double price;                // Price in PKR
    
public:
    // Constructor
    Medicine();
    Medicine(const string& name, const string& form, double pr);
    
    // Destructor
    ~Medicine();
    
    // Getters
    string getMedicineName() const;
    string getFormula() const;
    double getPrice() const;
    
    // Setters
    void setMedicineName(const string& name);
    void setFormula(const string& form);
    void setPrice(double pr);
    
    // Check if medicine matches a given name or formula
    bool matchesName(const string& name) const;
    bool matchesFormula(const string& form) const;
};

#endif // MEDICINE_H

