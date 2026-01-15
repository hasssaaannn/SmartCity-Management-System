#include "Pharmacy.h"
using namespace std;

// Default constructor
Pharmacy::Pharmacy() : pharmacyID(""), name(""), sector(""), medicines(nullptr), 
                       medicineCount(0), medicineCapacity(0) {
}

// Parameterized constructor
Pharmacy::Pharmacy(const string& id, const string& n, const string& sec)
    : pharmacyID(id), name(n), sector(sec), medicines(nullptr), 
      medicineCount(0), medicineCapacity(0) {
}

// Destructor
Pharmacy::~Pharmacy() {
    if (medicines != nullptr) {
        delete[] medicines;
        medicines = nullptr;
    }
    medicineCount = 0;
    medicineCapacity = 0;
}

// Copy constructor
Pharmacy::Pharmacy(const Pharmacy& other) 
    : pharmacyID(other.pharmacyID), name(other.name), sector(other.sector),
      medicineCount(other.medicineCount), medicineCapacity(other.medicineCapacity) {
    
    if (other.medicines != nullptr && medicineCount > 0) {
        medicines = new Medicine[medicineCapacity];
        for (int i = 0; i < medicineCount; i++) {
            medicines[i] = other.medicines[i];
        }
    } else {
        medicines = nullptr;
    }
}

// Assignment operator
Pharmacy& Pharmacy::operator=(const Pharmacy& other) {
    if (this == &other) {
        return *this;  // Self-assignment check
    }
    
    // Clean up existing medicines
    if (medicines != nullptr) {
        delete[] medicines;
        medicines = nullptr;
    }
    
    // Copy basic fields
    pharmacyID = other.pharmacyID;
    name = other.name;
    sector = other.sector;
    medicineCount = other.medicineCount;
    medicineCapacity = other.medicineCapacity;
    
    // Copy medicines array
    if (other.medicines != nullptr && medicineCount > 0) {
        medicines = new Medicine[medicineCapacity];
        for (int i = 0; i < medicineCount; i++) {
            medicines[i] = other.medicines[i];
        }
    } else {
        medicines = nullptr;
    }
    
    return *this;
}

// Helper: Resize medicines array when needed
void Pharmacy::resizeMedicines() {
    if (medicineCapacity == 0) {
        medicineCapacity = 5;  // Initial capacity
        medicines = new Medicine[medicineCapacity];
    } else if (medicineCount >= medicineCapacity) {
        int newCapacity = medicineCapacity * 2;
        Medicine* newMedicines = new Medicine[newCapacity];
        
        // Copy existing medicines
        for (int i = 0; i < medicineCount; i++) {
            newMedicines[i] = medicines[i];
        }
        
        // Delete old array
        delete[] medicines;
        medicines = newMedicines;
        medicineCapacity = newCapacity;
    }
}

// Getters
string Pharmacy::getPharmacyID() const {
    return pharmacyID;
}

string Pharmacy::getName() const {
    return name;
}

string Pharmacy::getSector() const {
    return sector;
}

int Pharmacy::getMedicineCount() const {
    return medicineCount;
}

Medicine* Pharmacy::getMedicines() const {
    return medicines;
}

// Setters
void Pharmacy::setPharmacyID(const string& id) {
    pharmacyID = id;
}

void Pharmacy::setName(const string& n) {
    name = n;
}

void Pharmacy::setSector(const string& sec) {
    sector = sec;
}

// Add a medicine to the pharmacy
void Pharmacy::addMedicine(const Medicine& medicine) {
    // Check if medicine already exists
    for (int i = 0; i < medicineCount; i++) {
        if (medicines[i].getMedicineName() == medicine.getMedicineName()) {
            // Update existing medicine
            medicines[i] = medicine;
            return;
        }
    }
    
    // Add new medicine
    resizeMedicines();
    medicines[medicineCount] = medicine;
    medicineCount++;
}

// Search for a medicine by name
Medicine* Pharmacy::findMedicineByName(const string& name) {
    for (int i = 0; i < medicineCount; i++) {
        if (medicines[i].matchesName(name)) {
            return &medicines[i];
        }
    }
    return nullptr;
}

// Search for a medicine by formula
Medicine* Pharmacy::findMedicineByFormula(const string& formula) {
    for (int i = 0; i < medicineCount; i++) {
        if (medicines[i].matchesFormula(formula)) {
            return &medicines[i];
        }
    }
    return nullptr;
}

// Check if pharmacy has a specific medicine
bool Pharmacy::hasMedicine(const string& name) const {
    for (int i = 0; i < medicineCount; i++) {
        if (medicines[i].matchesName(name)) {
            return true;
        }
    }
    return false;
}

