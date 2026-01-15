#include "MedicalSector.h"
#include <iostream>
using namespace std;

// Helper function to get approximate coordinates for Islamabad sectors
void MedicalSector::getSectorCoordinates(const string& sector, double& latitude, double& longitude) {
    // Base coordinates for Islamabad (approximate center)
    double baseLat = 33.684;
    double baseLon = 73.047;
    
    // Default to base coordinates
    latitude = baseLat;
    longitude = baseLon;
    
    // Map common sectors to approximate coordinates (Islamabad layout)
    if (sector == "G-10" || sector == "G10") {
        latitude = 33.684; longitude = 73.025;
    } else if (sector == "F-8" || sector == "F8") {
        latitude = 33.700; longitude = 73.037;
    } else if (sector == "F-6" || sector == "F6") {
        latitude = 33.715; longitude = 73.045;
    } else if (sector == "G-9" || sector == "G9") {
        latitude = 33.690; longitude = 73.030;
    } else if (sector == "F-7" || sector == "F7") {
        latitude = 33.707; longitude = 73.040;
    } else if (sector == "G-8" || sector == "G8") {
        latitude = 33.694; longitude = 73.032;
    } else if (sector == "H-8" || sector == "H8") {
        latitude = 33.710; longitude = 73.042;
    } else if (sector == "I-8" || sector == "I8") {
        latitude = 33.720; longitude = 73.048;
    } else if (sector == "G-6" || sector == "G6") {
        latitude = 33.709; longitude = 73.044;
    } else if (sector == "F-10" || sector == "F10") {
        latitude = 33.705; longitude = 73.038;
    } else if (sector == "Blue Area") {
        latitude = 33.697; longitude = 73.039;
    } else {
        // For unknown sectors, use base coordinates
        latitude = baseLat;
        longitude = baseLon;
    }
}

// Helper function to convert string to double (replaces stod from STL)
double MedicalSector::stringToDouble(const string& str) {
    if (str.empty()) return 0.0;
    
    double result = 0.0;
    double fraction = 0.1;
    bool hasDecimal = false;
    bool isNegative = false;
    int i = 0;
    
    // Skip whitespace
    while (i < (int)str.length() && (str[i] == ' ' || str[i] == '\t')) {
        i++;
    }
    
    // Check for negative sign
    if (i < (int)str.length() && str[i] == '-') {
        isNegative = true;
        i++;
    }
    
    // Parse digits
    while (i < (int)str.length()) {
        if (str[i] >= '0' && str[i] <= '9') {
            if (!hasDecimal) {
                result = result * 10.0 + (str[i] - '0');
            } else {
                result += (str[i] - '0') * fraction;
                fraction *= 0.1;
            }
        } else if (str[i] == '.' && !hasDecimal) {
            hasDecimal = true;
        } else {
            break;
        }
        i++;
    }
    
    return isNegative ? -result : result;
}

// Helper function to convert string to int (replaces stoi from STL)
int MedicalSector::stringToInt(const string& str) {
    if (str.empty()) return 0;
    
    int result = 0;
    bool isNegative = false;
    int i = 0;
    
    // Skip whitespace
    while (i < (int)str.length() && (str[i] == ' ' || str[i] == '\t')) {
        i++;
    }
    
    // Check for negative sign
    if (i < (int)str.length() && str[i] == '-') {
        isNegative = true;
        i++;
    }
    
    // Parse digits
    while (i < (int)str.length() && str[i] >= '0' && str[i] <= '9') {
        result = result * 10 + (str[i] - '0');
        i++;
    }
    
    return isNegative ? -result : result;
}

// Constructor
MedicalSector::MedicalSector(int maxHospitals, int maxPharmacies, int maxMedicines, int hashTableSize)
    : maxHospitals(maxHospitals), maxPharmacies(maxPharmacies), maxMedicines(maxMedicines),
      hospitalCount(0), pharmacyCount(0), medicineCount(0), ownsLocationGraph(true) {
    
    // Initialize hospital data structures
    hospitalTable = new HashTable(hashTableSize);
    emergencyBedHeap = new MinHeap(maxHospitals); // Will use inverted priority for MaxHeap behavior
    locationGraph = new Graph(maxHospitals * 2, false); // Undirected graph for locations
    hospitalIDList = new string[maxHospitals];
    
    // Initialize pharmacy data structures
    pharmacyTable = new HashTable(hashTableSize);
    pharmacyIDList = new string[maxPharmacies];
    
    // Initialize medicine data structures
    medicineByNameTable = new HashTable(hashTableSize * 2); // Larger table for medicines
    medicineByFormulaTable = new HashTable(hashTableSize * 2);
    medicineNameList = new string[maxMedicines];
    
    // Initialize doctor data structures
    doctorTable = new HashTable(hashTableSize);
    doctorSpecializationTable = new HashTable(hashTableSize);
    maxDoctors = 100; // Default max doctors
    doctorCount = 0;
    doctorIDList = new string[maxDoctors];
}

// Destructor
MedicalSector::~MedicalSector() {
    // Delete all Hospital objects
    for (int i = 0; i < hospitalCount; i++) {
        Hospital* hospital = (Hospital*)hospitalTable->search(hospitalIDList[i]);
        if (hospital != nullptr) {
            delete hospital;
        }
    }
    
    // Delete all Pharmacy objects
    for (int i = 0; i < pharmacyCount; i++) {
        Pharmacy* pharmacy = (Pharmacy*)pharmacyTable->search(pharmacyIDList[i]);
        if (pharmacy != nullptr) {
            delete pharmacy;
        }
    }
    
    // Delete all Medicine objects
    
    // Delete data structures
    if (hospitalTable != nullptr) {
        delete hospitalTable;
        hospitalTable = nullptr;
    }
    
    if (emergencyBedHeap != nullptr) {
        delete emergencyBedHeap;
        emergencyBedHeap = nullptr;
    }
    
    if (locationGraph != nullptr && ownsLocationGraph) {
        delete locationGraph;
        locationGraph = nullptr;
    }
    
    if (pharmacyTable != nullptr) {
        delete pharmacyTable;
        pharmacyTable = nullptr;
    }
    
    if (medicineByNameTable != nullptr) {
        delete medicineByNameTable;
        medicineByNameTable = nullptr;
    }
    
    if (medicineByFormulaTable != nullptr) {
        delete medicineByFormulaTable;
        medicineByFormulaTable = nullptr;
    }
    
    if (doctorTable != nullptr) {
        delete doctorTable;
        doctorTable = nullptr;
    }
    
    if (doctorSpecializationTable != nullptr) {
        delete doctorSpecializationTable;
        doctorSpecializationTable = nullptr;
    }
    
    if (doctorIDList != nullptr) {
        delete[] doctorIDList;
        doctorIDList = nullptr;
    }
    
    if (hospitalIDList != nullptr) {
        delete[] hospitalIDList;
        hospitalIDList = nullptr;
    }
    
    if (pharmacyIDList != nullptr) {
        delete[] pharmacyIDList;
        pharmacyIDList = nullptr;
    }
    
    if (medicineNameList != nullptr) {
        delete[] medicineNameList;
        medicineNameList = nullptr;
    }
    
    hospitalCount = 0;
    pharmacyCount = 0;
    medicineCount = 0;
    doctorCount = 0;
}

// ========== HOSPITAL OPERATIONS ==========

bool MedicalSector::registerHospital(const string& hospitalID, const string& name,
                                     const string& sector, int emergencyBeds, const string& specialization) {
    // Input validation
    if (hospitalID.empty() || name.empty() || sector.empty()) {
        return false; // Invalid input: empty strings
    }
    
    if (emergencyBeds < 0) {
        return false; // Invalid input: negative emergency beds
    }
    
    // Check if hospital already exists
    if (searchHospital(hospitalID) != nullptr) {
        return false; // Hospital already exists
    }
    
    if (hospitalCount >= maxHospitals) {
        return false; // Maximum hospitals reached
    }
    
    // Create new hospital
    Hospital* hospital = new Hospital(hospitalID, name, sector, emergencyBeds, specialization);
    
    // Add to HashTable
    hospitalTable->insert(hospitalID, hospital);
    
    // Add to emergency bed heap
    double priority = -(double)emergencyBeds; // Negative for max-heap behavior
    emergencyBedHeap->insert(hospitalID, priority, hospital);
    
    // Add to hospital ID list for iteration
    hospitalIDList[hospitalCount] = hospitalID;
    
    // Add hospital location to Graph for nearest hospital lookup
    double lat, lon;
    getSectorCoordinates(sector, lat, lon);
    locationGraph->addVertex(hospitalID, name, lat, lon, hospital);
    
    // Connect hospital to nearest bus stop for pathfinding
    locationGraph->connectToNearestStop(hospitalID);
    
    hospitalCount++;
    return true;
}

int MedicalSector::loadHospitalsFromCSV(const string& filename) {
    DataLoader loader;
    CSVRow hospitalData[100];
    int actualRows = 0;
    
    if (!loader.loadHospitals(filename, hospitalData, 100, actualRows)) {
        return 0;
    }
    
    int loadedCount = 0;
    
    for (int i = 0; i < actualRows; i++) {
        if (hospitalData[i].fieldCount >= 5) {
            string hospitalID = hospitalData[i].fields[0];
            string name = hospitalData[i].fields[1];
            string sector = hospitalData[i].fields[2];
            int emergencyBeds = 0;
            
            // Parse emergency beds
            if (hospitalData[i].fieldCount > 3) {
                emergencyBeds = stringToInt(hospitalData[i].fields[3]);
            }
            
            // Get specialization
            string specialization = "";
            if (hospitalData[i].fieldCount > 4) {
                specialization = hospitalData[i].fields[4];
            }
            
            if (registerHospital(hospitalID, name, sector, emergencyBeds, specialization)) {
                loadedCount++;
            }
        }
    }
    
    return loadedCount;
}

Hospital* MedicalSector::searchHospital(const string& hospitalID) {
    return (Hospital*)hospitalTable->search(hospitalID);
}

Hospital* MedicalSector::getHospitalWithMostBeds() {
    if (emergencyBedHeap->isEmpty()) {
        return nullptr;
    }
    
    // Peek at the top (min priority = most negative = most beds)
    HeapNode top = emergencyBedHeap->peekMin();
    if (top.identifier.empty()) {
        return nullptr;
    }
    
    return (Hospital*)top.data;
}

// Helper function to calculate simple squared distance
static double calculateDistanceSquared(double lat1, double lon1, double lat2, double lon2) {
    double dLat = lat2 - lat1;
    double dLon = lon2 - lon1;
    return dLat * dLat + dLon * dLon;
}

string MedicalSector::findNearestHospital(double latitude, double longitude) {
    if (hospitalCount == 0 || locationGraph == nullptr) {
        return "";
    }
    
    // Iterate through all registered hospitals and find the nearest one
    double minDistance = 1000000.0; // Large initial distance
    string nearestHospitalID = "";
    
    for (int i = 0; i < hospitalCount; i++) {
        Hospital* hospital = searchHospital(hospitalIDList[i]);
        if (hospital == nullptr) continue;
        
        // Get hospital coordinates from its sector
        double hospLat, hospLon;
        getSectorCoordinates(hospital->getSector(), hospLat, hospLon);
        
        // Calculate squared distance (no sqrt needed for comparison)
        double distSquared = calculateDistanceSquared(latitude, longitude, hospLat, hospLon);
        
        if (distSquared < minDistance) {
            minDistance = distSquared;
            nearestHospitalID = hospitalIDList[i];
        }
    }
    
    return nearestHospitalID;
}

Hospital** MedicalSector::findHospitalsBySpecialization(const string& specialization, int& count) {
    count = 0;
    Hospital** result = new Hospital*[maxHospitals];
    
    // Iterate through all registered hospitals
    for (int i = 0; i < hospitalCount; i++) {
        Hospital* hospital = (Hospital*)hospitalTable->search(hospitalIDList[i]);
        if (hospital != nullptr) {
            // Check if specialization matches (substring search)
            string hospitalSpec = hospital->getSpecialization();
            if (hospitalSpec.find(specialization) != string::npos) {
                result[count++] = hospital;
            }
        }
    }
    
    return result;
}

void MedicalSector::displayAllHospitals() {
    cout << "=== All Registered Hospitals (" << hospitalCount << ") ===" << endl;
    for (int i = 0; i < hospitalCount; i++) {
        Hospital* hospital = (Hospital*)hospitalTable->search(hospitalIDList[i]);
        if (hospital != nullptr) {
            cout << (i + 1) << ". Hospital ID: " << hospital->getHospitalID()
                 << "  Name: " << hospital->getName()
                 << "  Sector: " << hospital->getSector()
                 << "  Emergency Beds: " << hospital->getEmergencyBeds()
                 << "  Specialization: " << hospital->getSpecialization() << endl;
        }
    }
}

// ========== PHARMACY OPERATIONS ==========

bool MedicalSector::registerPharmacy(const string& pharmacyID, const string& name, const string& sector) {
    // Input validation
    if (pharmacyID.empty() || name.empty() || sector.empty()) {
        return false; // Invalid input: empty strings
    }
    
    // Check if pharmacy already exists
    if (searchPharmacy(pharmacyID) != nullptr) {
        return false; // Pharmacy already exists
    }
    
    if (pharmacyCount >= maxPharmacies) {
        return false; // Maximum pharmacies reached
    }
    
    // Create new pharmacy
    Pharmacy* pharmacy = new Pharmacy(pharmacyID, name, sector);
    
    // Add to HashTable
    pharmacyTable->insert(pharmacyID, pharmacy);
    
    // Add to pharmacy ID list for iteration
    pharmacyIDList[pharmacyCount] = pharmacyID;
    
    // Add pharmacy location to Graph for nearest pharmacy lookup
    double lat, lon;
    getSectorCoordinates(sector, lat, lon);
    locationGraph->addVertex(pharmacyID, name, lat, lon, pharmacy);
    
    // Connect pharmacy to nearest bus stop for pathfinding
    locationGraph->connectToNearestStop(pharmacyID);
    
    pharmacyCount++;
    return true;
}

int MedicalSector::loadPharmaciesFromCSV(const string& filename) {
    DataLoader loader;
    CSVRow pharmacyData[100];
    int actualRows = 0;
    
    if (!loader.loadPharmacies(filename, pharmacyData, 100, actualRows)) {
        return 0;
    }
    
    int loadedCount = 0;
    string currentPharmacyID = "";
    
    for (int i = 0; i < actualRows; i++) {
        if (pharmacyData[i].fieldCount >= 6) {
            string pharmacyID = pharmacyData[i].fields[0];
            string name = pharmacyData[i].fields[1];
            string sector = pharmacyData[i].fields[2];
            string medicineName = pharmacyData[i].fields[3];
            string formula = pharmacyData[i].fields[4];
            double price = 0.0;
            
            // Parse price
            if (pharmacyData[i].fieldCount > 5) {
                price = stringToDouble(pharmacyData[i].fields[5]);
            }
            
            // Register pharmacy if it's a new one
            if (pharmacyID != currentPharmacyID) {
                // Check if pharmacy already exists (from previous CSV rows)
                Pharmacy* existingPharmacy = searchPharmacy(pharmacyID);
                if (existingPharmacy == nullptr) {
                    // New pharmacy - register it
                    if (registerPharmacy(pharmacyID, name, sector)) {
                        loadedCount++;
                    }
                }
                currentPharmacyID = pharmacyID;
            }
            
            // Add medicine to the pharmacy
            addMedicineToPharmacy(pharmacyID, medicineName, formula, price);
        }
    }
    
    return loadedCount;
}

Pharmacy* MedicalSector::searchPharmacy(const string& pharmacyID) {
    return (Pharmacy*)pharmacyTable->search(pharmacyID);
}

bool MedicalSector::addMedicineToPharmacy(const string& pharmacyID, const string& medicineName,
                                         const string& formula, double price) {
    // Input validation
    if (pharmacyID.empty() || medicineName.empty() || formula.empty()) {
        return false; // Invalid input: empty strings
    }
    
    if (price < 0.0) {
        return false; // Invalid input: negative price
    }
    
    Pharmacy* pharmacy = searchPharmacy(pharmacyID);
    if (pharmacy == nullptr) {
        return false; // Pharmacy not found
    }
    
    // Create medicine
    Medicine medicine(medicineName, formula, price);
    
    // Add to pharmacy
    pharmacy->addMedicine(medicine);
    
    // Add to medicine hash tables for fast lookup
    Medicine* medPtr = pharmacy->findMedicineByName(medicineName);
    if (medPtr != nullptr) {
        // Store pointer in hash tables for O(1) lookup
        medicineByNameTable->insert(medicineName, medPtr);
        medicineByFormulaTable->insert(formula, medPtr);
        
        // Add to medicine name list if not already present
        bool exists = false;
        for (int i = 0; i < medicineCount; i++) {
            if (medicineNameList[i] == medicineName) {
                exists = true;
                break;
            }
        }
        if (!exists && medicineCount < maxMedicines) {
            medicineNameList[medicineCount++] = medicineName;
        }
    }
    
    return true;
}

void MedicalSector::displayAllPharmacies() {
    cout << "=== All Registered Pharmacies (" << pharmacyCount << ") ===" << endl;
    for (int i = 0; i < pharmacyCount; i++) {
        Pharmacy* pharmacy = (Pharmacy*)pharmacyTable->search(pharmacyIDList[i]);
        if (pharmacy != nullptr) {
            cout << (i + 1) << ". Pharmacy ID: " << pharmacy->getPharmacyID()
                 << "  Name: " << pharmacy->getName()
                 << "  Sector: " << pharmacy->getSector()
                 << "  Medicines: " << pharmacy->getMedicineCount() << endl;
        }
    }
}

string MedicalSector::findNearestPharmacy(double latitude, double longitude) {
    if (pharmacyCount == 0 || locationGraph == nullptr) {
        return "";
    }
    
    // Iterate through all registered pharmacies and find the nearest one
    double minDistance = 1000000.0; // Large initial distance
    string nearestPharmacyID = "";
    
    for (int i = 0; i < pharmacyCount; i++) {
        Pharmacy* pharmacy = searchPharmacy(pharmacyIDList[i]);
        if (pharmacy == nullptr) continue;
        
        // Get pharmacy coordinates from its sector
        double pharmLat, pharmLon;
        getSectorCoordinates(pharmacy->getSector(), pharmLat, pharmLon);
        
        // Calculate squared distance (no sqrt needed for comparison)
        double distSquared = calculateDistanceSquared(latitude, longitude, pharmLat, pharmLon);
        
        if (distSquared < minDistance) {
            minDistance = distSquared;
            nearestPharmacyID = pharmacyIDList[i];
        }
    }
    
    return nearestPharmacyID;
}

// ========== MEDICINE OPERATIONS ==========

Medicine* MedicalSector::searchMedicineByName(const string& medicineName) {
    return (Medicine*)medicineByNameTable->search(medicineName);
}

Medicine* MedicalSector::searchMedicineByFormula(const string& formula) {
    return (Medicine*)medicineByFormulaTable->search(formula);
}

Pharmacy** MedicalSector::findPharmaciesWithMedicine(const string& medicineName, int& count) {
    count = 0;
    Pharmacy** result = new Pharmacy*[maxPharmacies];
    
    // Iterate through all pharmacies
    for (int i = 0; i < pharmacyCount; i++) {
        Pharmacy* pharmacy = (Pharmacy*)pharmacyTable->search(pharmacyIDList[i]);
        if (pharmacy != nullptr && pharmacy->hasMedicine(medicineName)) {
            result[count++] = pharmacy;
        }
    }
    
    return result;
}

// ========== DOCTOR OPERATIONS ==========

bool MedicalSector::registerDoctor(const string& doctorID, const string& name,
                                   const string& specialization, void* doctorData) {
    // Check if doctor already exists
    if (doctorTable->search(doctorID) != nullptr) {
        return false; // Doctor already exists
    }
    
    if (doctorCount >= maxDoctors) {
        return false; // Maximum doctors reached
    }
    
    // Store doctorData directly (user is responsible for the data structure)
    doctorTable->insert(doctorID, doctorData);
    
    // Store specialization for searching by specialization
    string* specializationStr = new string(specialization);
    doctorSpecializationTable->insert(doctorID, specializationStr);
    
    // Add to doctor ID list for iteration
    doctorIDList[doctorCount] = doctorID;
    doctorCount++;
    
    return true;
}

void* MedicalSector::searchDoctor(const string& doctorID) {
    return doctorTable->search(doctorID);
}

string* MedicalSector::findDoctorsBySpecialization(const string& specialization, int& count) {
    count = 0;
    if (doctorCount == 0) {
        return nullptr;
    }
    
    string* result = new string[maxDoctors]; // Allocate max size
    
    // Iterate through all registered doctors using ID list
    for (int i = 0; i < doctorCount; i++) {
        // Check specialization for this doctor
        string* docSpecialization = (string*)doctorSpecializationTable->search(doctorIDList[i]);
        if (docSpecialization != nullptr) {
            // Check if specialization matches (substring search for flexibility)
            if (docSpecialization->find(specialization) != string::npos || 
                specialization.find(*docSpecialization) != string::npos) {
                result[count++] = doctorIDList[i];
            }
        }
    }
    
    return result;
}

// ========== UTILITY OPERATIONS ==========

int MedicalSector::getHospitalCount() const {
    return hospitalCount;
}

int MedicalSector::getPharmacyCount() const {
    return pharmacyCount;
}

int MedicalSector::getMedicineCount() const {
    return medicineCount;
}

void MedicalSector::setCityGraph(Graph* graph) {
    if (graph == nullptr) {
        return; // Invalid graph
    }
    
    // Delete old graph if we own it
    if (locationGraph != nullptr && ownsLocationGraph) {
        delete locationGraph;
    }
    
    // Use external shared graph
    locationGraph = graph;
    ownsLocationGraph = false;
}

Graph* MedicalSector::getCityGraph() const {
    return locationGraph;
}

