#ifndef MEDICAL_SECTOR_H
#define MEDICAL_SECTOR_H

#include <string>
#include "core_classes/HashTable.h"
#include "core_classes/MinHeap.h"
#include "core_classes/Graph.h"
#include "Hospital.h"
#include "Pharmacy.h"
#include "Medicine.h"
#include "core_classes/DataLoader.h"
using namespace std;

// Medical Sector Manager Class
// Manages hospitals, pharmacies, medicines, and doctors in the smart city system
class MedicalSector {
private:
    // Hospital Management
    HashTable* hospitalTable;         // HashTable for HospitalID → Hospital* lookup
    MinHeap* emergencyBedHeap;        // MinHeap with inverted priority for MaxHeap behavior (hospitals with most beds first)
    Graph* locationGraph;             // Graph for nearest hospital queries
    bool ownsLocationGraph;           // Flag to track if we own the graph (for shared graph support)
    
    // Pharmacy Management
    HashTable* pharmacyTable;         // HashTable for PharmacyID → Pharmacy* lookup
    
    // Medicine Management (dual indexing for fast lookup)
    HashTable* medicineByNameTable;   // HashTable for MedicineName → Medicine* lookup
    HashTable* medicineByFormulaTable;// HashTable for Formula → Medicine* lookup
    
    // Doctor Management
    HashTable* doctorTable;           // HashTable for DoctorID → Doctor data lookup
    HashTable* doctorSpecializationTable; // HashTable for DoctorID → Specialization lookup
    
    // Helper lists for iteration
    string* hospitalIDList;      // List of all hospital IDs
    string* pharmacyIDList;      // List of all pharmacy IDs
    string* medicineNameList;    // List of all medicine names
    string* doctorIDList;        // List of all doctor IDs for iteration
    
    // Capacity tracking
    int maxHospitals;
    int maxPharmacies;
    int maxMedicines;
    int maxDoctors;
    int hospitalCount;
    int pharmacyCount;
    int medicineCount;
    int doctorCount;
    
    // Helper: Get sector coordinates for Graph
    static void getSectorCoordinates(const string& sector, double& latitude, double& longitude);
    
    // Helper: Convert string to double (replaces stod)
    static double stringToDouble(const string& str);
    
    // Helper: Convert string to int (replaces stoi)
    static int stringToInt(const string& str);
    
public:
    // Constructor
    MedicalSector(int maxHospitals = 50, int maxPharmacies = 100, int maxMedicines = 500, int hashTableSize = 11);
    
    // Destructor
    ~MedicalSector();
    
    // ========== HOSPITAL OPERATIONS ==========
    
    // Register a new hospital
    // Returns: true if successful, false if hospital already exists
    bool registerHospital(const string& hospitalID, const string& name,
                         const string& sector, int emergencyBeds, const string& specialization);
    
    // Load hospitals from CSV file using DataLoader
    // Returns: number of hospitals loaded
    int loadHospitalsFromCSV(const string& filename);
    
    // Search hospital by ID (O(1) lookup using HashTable)
    // Returns: Pointer to Hospital, or nullptr if not found
    Hospital* searchHospital(const string& hospitalID);
    
    // Get hospital with most emergency beds available (MaxHeap behavior)
    // Returns: Hospital pointer, or nullptr if no hospitals available
    Hospital* getHospitalWithMostBeds();
    
    // Find nearest hospital to given location (using Graph)
    // Returns: HospitalID of nearest hospital, or empty string if not found
    string findNearestHospital(double latitude, double longitude);
    
    // Search hospitals by specialization
    // Returns: Array of Hospital pointers, and count
    Hospital** findHospitalsBySpecialization(const string& specialization, int& count);
    
    // Display all hospitals
    void displayAllHospitals();
    
    // ========== PHARMACY OPERATIONS ==========
    
    // Register a new pharmacy
    // Returns: true if successful, false if pharmacy already exists
    bool registerPharmacy(const string& pharmacyID, const string& name, const string& sector);
    
    // Load pharmacies from CSV file using DataLoader
    // Returns: number of pharmacies loaded
    int loadPharmaciesFromCSV(const string& filename);
    
    // Search pharmacy by ID (O(1) lookup using HashTable)
    // Returns: Pointer to Pharmacy, or nullptr if not found
    Pharmacy* searchPharmacy(const string& pharmacyID);
    
    // Add medicine to a pharmacy
    // Returns: true if successful
    bool addMedicineToPharmacy(const string& pharmacyID, const string& medicineName,
                               const string& formula, double price);
    
    // Display all pharmacies
    void displayAllPharmacies();
    
    // Find nearest pharmacy to given location (using Graph)
    // Returns: PharmacyID of nearest pharmacy, or empty string if not found
    string findNearestPharmacy(double latitude, double longitude);
    
    // ========== MEDICINE OPERATIONS ==========
    
    // Search medicine by name (O(1) lookup using HashTable)
    // Returns: Pointer to Medicine, or nullptr if not found
    Medicine* searchMedicineByName(const string& medicineName);
    
    // Search medicine by formula (O(1) lookup using HashTable)
    // Returns: Pointer to Medicine, or nullptr if not found
    Medicine* searchMedicineByFormula(const string& formula);
    
    // Find pharmacies that have a specific medicine
    // Returns: Array of Pharmacy pointers, and count
    Pharmacy** findPharmaciesWithMedicine(const string& medicineName, int& count);
    
    // ========== DOCTOR OPERATIONS ==========
    
    // Register a doctor
    // Returns: true if successful
    bool registerDoctor(const string& doctorID, const string& name,
                       const string& specialization, void* doctorData = nullptr);
    
    // Search doctor by ID (O(1) lookup using HashTable)
    // Returns: Doctor data pointer, or nullptr if not found
    void* searchDoctor(const string& doctorID);
    
    // Search doctors by specialization
    // Returns: Array of doctor IDs, and count (requires iteration)
    string* findDoctorsBySpecialization(const string& specialization, int& count);
    
    // ========== UTILITY OPERATIONS ==========
    
    // Get counts
    int getHospitalCount() const;
    int getPharmacyCount() const;
    int getMedicineCount() const;
    
    // Set city graph (for shared graph support - use single graph for entire city)
    // If external graph is provided, it will be used instead of internal graph
    // NOTE: Caller must ensure graph exists for lifetime of MedicalSector
    void setCityGraph(Graph* graph);
    
    // Get city graph (for integration with other modules)
    Graph* getCityGraph() const;
};

#endif // MEDICAL_SECTOR_H

