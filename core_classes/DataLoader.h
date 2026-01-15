#ifndef DATALOADER_H
#define DATALOADER_H

#include <string>
using namespace std;

// Structure to store a CSV row (array of strings)
struct CSVRow {
    string* fields;  // Array of field strings
    int fieldCount;       // Number of fields in this row
    
    CSVRow();
    CSVRow(int count);
    CSVRow(const CSVRow& other);  // Copy constructor
    CSVRow& operator=(const CSVRow& other);  // Assignment operator
    ~CSVRow();
};

// DataLoader class for parsing CSV files
// Used for: Loading all data from SmartCity_dataset folder
class DataLoader {
private:
    // Helper function to trim whitespace from string
    string trim(const string& str);
    
    // Helper function to check if file exists
    bool fileExists(const string& filename);
    
    // Parse a single CSV line (handles quoted fields with commas)
    // Returns: CSVRow with parsed fields
    CSVRow parseCSVLine(const string& line);
    
    // Count number of fields in a CSV line
    int countFields(const string& line);
    
    // Split comma-separated list (for Subjects, Specialization)
    void splitCommaList(const string& list, string* result, int& count);
    
public:
    // Constructor
    DataLoader();
    
    // Destructor
    ~DataLoader();
    
    // Load schools from CSV file
    // Parameters: filename, array to store school data, maxRows, actualRows (output)
    // Returns: true if successful, false otherwise
    // Format: SchoolID,Name,Sector,Rating,Subjects
    bool loadSchools(const string& filename, CSVRow* schoolData, int maxRows, int& actualRows);
    
    // Load hospitals from CSV file
    // Parameters: filename, array to store hospital data, maxRows, actualRows (output)
    // Returns: true if successful, false otherwise
    // Format: HospitalID,Name,Sector,EmergencyBeds,Specialization
    bool loadHospitals(const string& filename, CSVRow* hospitalData, int maxRows, int& actualRows);
    
    // Load pharmacies from CSV file
    // Parameters: filename, array to store pharmacy data, maxRows, actualRows (output)
    // Returns: true if successful, false otherwise
    // Format: PharmacyID,Name,Sector,MedicineName,Formula,Price
    bool loadPharmacies(const string& filename, CSVRow* pharmacyData, int maxRows, int& actualRows);
    
    // Load bus stops from CSV file
    // Parameters: filename, array to store stop data, maxRows, actualRows (output)
    // Returns: true if successful, false otherwise
    // Format: StopID,Name,Coordinates
    bool loadStops(const string& filename, CSVRow* stopData, int maxRows, int& actualRows);
    
    // Load buses from CSV file
    // Parameters: filename, array to store bus data, maxRows, actualRows (output)
    // Returns: true if successful, false otherwise
    // Format: BusNo,Company,CurrentStop,Route
    bool loadBuses(const string& filename, CSVRow* busData, int maxRows, int& actualRows);
    
    // Load school buses from CSV file
    // Parameters: filename, array to store school bus data, maxRows, actualRows (output)
    // Returns: true if successful, false otherwise
    // Format: BusNo,Company,CurrentStop,SchoolID,MaxCapacity,Route
    bool loadSchoolBuses(const string& filename, CSVRow* schoolBusData, int maxRows, int& actualRows);
    
    // Load population from CSV file
    // Parameters: filename, array to store population data, maxRows, actualRows (output)
    // Returns: true if successful, false otherwise
    // Format: CNIC,Name,Gender,Age,Sector,Street,HouseNo,Occupation
    bool loadPopulation(const string& filename, CSVRow* populationData, int maxRows, int& actualRows);
    
    // Load malls from CSV file
    // Parameters: filename, array to store mall data, maxRows, actualRows (output)
    // Returns: true if successful, false otherwise
    // Format: MallID,Name,Sector,Coordinates,Categories
    bool loadMalls(const string& filename, CSVRow* mallData, int maxRows, int& actualRows);
    
    // Load products from CSV file
    // Parameters: filename, array to store product data, maxRows, actualRows (output)
    // Returns: true if successful, false otherwise
    // Format: ProductID,MallID,ProductName,Category,Price
    bool loadProducts(const string& filename, CSVRow* productData, int maxRows, int& actualRows);
    
    // Load facilities from CSV file
    // Parameters: filename, array to store facility data, maxRows, actualRows (output)
    // Returns: true if successful, false otherwise
    // Format: FacilityID,Name,Type,Sector,Coordinates
    bool loadFacilities(const string& filename, CSVRow* facilityData, int maxRows, int& actualRows);
    
    // Load airports from CSV file
    // Parameters: filename, array to store airport data, maxRows, actualRows (output)
    // Returns: true if successful, false otherwise
    // Format: AirportID,Name,Code,City,Coordinates
    bool loadAirports(const string& filename, CSVRow* airportData, int maxRows, int& actualRows);
    
    // Load railway stations from CSV file
    // Parameters: filename, array to store station data, maxRows, actualRows (output)
    // Returns: true if successful, false otherwise
    // Format: StationID,Name,Code,City,Coordinates
    bool loadRailways(const string& filename, CSVRow* stationData, int maxRows, int& actualRows);
    
    // Display loaded data (for debugging)
    void displayCSVData(const CSVRow* data, int rowCount, const string& title);
};

#endif // DATALOADER_H

