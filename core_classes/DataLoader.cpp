#include "DataLoader.h"
#include <iostream>
#include <fstream>
using namespace std;

// Helper function to check if character is whitespace (replaces isspace from cctype)
static bool isWhitespace(char c) {
    return (c == ' ' || c == '\t' || c == '\n' || c == '\r' || c == '\f' || c == '\v');
}


// CSVRow Implementation
CSVRow::CSVRow() : fields(nullptr), fieldCount(0) {
}

CSVRow::CSVRow(int count) : fieldCount(count) {
    fields = new string[fieldCount];
    for (int i = 0; i < fieldCount; i++) {
        fields[i] = "";
    }
}

CSVRow::CSVRow(const CSVRow& other) : fieldCount(other.fieldCount) {
    if (other.fieldCount > 0 && other.fields != nullptr) {
        fields = new string[fieldCount];
        for (int i = 0; i < fieldCount; i++) {
            fields[i] = other.fields[i];
        }
    } else {
        fields = nullptr;
        fieldCount = 0;
    }
}

CSVRow& CSVRow::operator=(const CSVRow& other) {
    if (this == &other) {
        return *this;  // Self-assignment check
    }
    
    // Delete old data
    if (fields != nullptr) {
        delete[] fields;
        fields = nullptr;
    }
    
    // Copy new data
    fieldCount = other.fieldCount;
    if (other.fieldCount > 0 && other.fields != nullptr) {
        fields = new string[fieldCount];
        for (int i = 0; i < fieldCount; i++) {
            fields[i] = other.fields[i];
        }
    } else {
        fields = nullptr;
        fieldCount = 0;
    }
    
    return *this;
}

CSVRow::~CSVRow() {
    if (fields != nullptr) {
        delete[] fields;
        fields = nullptr;
    }
    fieldCount = 0;
}

// DataLoader Implementation
DataLoader::DataLoader() {
}

DataLoader::~DataLoader() {
}

string DataLoader::trim(const string& str) {
    if (str.empty()) return "";
    
    size_t start = 0;
    size_t end = str.length() - 1;
    
    // Find first non-whitespace character
    while (start < str.length() && isWhitespace(str[start])) {
        start++;
    }
    
    // Find last non-whitespace character
    while (end > start && isWhitespace(str[end])) {
        end--;
    }
    
    if (start > end) return "";
    
    return str.substr(start, end - start + 1);
}

bool DataLoader::fileExists(const string& filename) {
    ifstream file(filename.c_str());
    bool exists = file.good();
    file.close();
    return exists;
}

int DataLoader::countFields(const string& line) {
    if (line.empty()) return 0;
    
    int count = 1;
    bool inQuotes = false;
    
    for (size_t i = 0; i < line.length(); i++) {
        if (line[i] == '"') {
            inQuotes = !inQuotes;
        } else if (line[i] == ',' && !inQuotes) {
            count++;
        }
    }
    
    return count;
}

CSVRow DataLoader::parseCSVLine(const string& line) {
    CSVRow row;
    
    if (line.empty()) {
        return row;
    }
    
    // Count fields first
    int fieldCount = countFields(line);
    row = CSVRow(fieldCount);
    
    string currentField = "";
    bool inQuotes = false;
    int currentIndex = 0;
    
    for (size_t i = 0; i < line.length(); i++) {
        char c = line[i];
        
        if (c == '"') {
            if (inQuotes && i + 1 < line.length() && line[i + 1] == '"') {
                // Escaped quote (double quotes)
                currentField += '"';
                i++; // Skip next quote
            } else {
                // Toggle quote state
                inQuotes = !inQuotes;
            }
        } else if (c == ',' && !inQuotes) {
            // End of field
            row.fields[currentIndex] = trim(currentField);
            currentField = "";
            currentIndex++;
        } else {
            // Regular character
            currentField += c;
        }
    }
    
    // Add last field
    if (currentIndex < fieldCount) {
        row.fields[currentIndex] = trim(currentField);
    }
    
    return row;
}

void DataLoader::splitCommaList(const string& list, string* result, int& count) {
    if (list.empty()) {
        count = 0;
        return;
    }
    
    string trimmed = trim(list);
    // Remove surrounding quotes if present
    if (trimmed.length() >= 2 && trimmed[0] == '"' && trimmed[trimmed.length() - 1] == '"') {
        trimmed = trimmed.substr(1, trimmed.length() - 2);
    }
    
    count = 1;
    // Count items
    for (size_t i = 0; i < trimmed.length(); i++) {
        if (trimmed[i] == ',') {
            count++;
        }
    }
    
    // Split
    string current = "";
    int index = 0;
    
    for (size_t i = 0; i < trimmed.length(); i++) {
        if (trimmed[i] == ',') {
            if (result != nullptr && index < count) {
                result[index] = trim(current);
            }
            current = "";
            index++;
        } else {
            current += trimmed[i];
        }
    }
    
    // Add last item
    if (result != nullptr && index < count) {
        result[index] = trim(current);
    }
}

bool DataLoader::loadSchools(const string& filename, CSVRow* schoolData, int maxRows, int& actualRows) {
    ifstream file(filename.c_str());
    if (!file.is_open()) {
        cout << "[ERROR] Could not open file '" << filename << "'!" << endl;
        cout << "Possible reasons:" << endl;
        cout << "  - File does not exist at the specified path" << endl;
        cout << "  - Incorrect file path or filename" << endl;
        cout << "  - Insufficient file permissions" << endl;
        return false;
    }
    
    string line;
    actualRows = 0;
    
    // Skip header row
    if (getline(file, line)) {
        // Header line - we can validate it if needed
    }
    
    // Read data rows
    while (getline(file, line) && actualRows < maxRows) {
        if (line.empty()) continue;
        
        CSVRow row = parseCSVLine(line);
        if (row.fieldCount >= 4) { // At least SchoolID, Name, Sector, Rating
            schoolData[actualRows] = row;
            actualRows++;
        }
    }
    
    file.close();
    return true;
}

bool DataLoader::loadHospitals(const string& filename, CSVRow* hospitalData, int maxRows, int& actualRows) {
    ifstream file(filename.c_str());
    if (!file.is_open()) {
        cout << "[ERROR] Could not open file '" << filename << "'!" << endl;
        cout << "Possible reasons:" << endl;
        cout << "  - File does not exist at the specified path" << endl;
        cout << "  - Incorrect file path or filename" << endl;
        cout << "  - Insufficient file permissions" << endl;
        return false;
    }
    
    string line;
    actualRows = 0;
    
    // Skip header row
    if (getline(file, line)) {
        // Header line
    }
    
    // Read data rows
    while (getline(file, line) && actualRows < maxRows) {
        if (line.empty()) continue;
        
        CSVRow row = parseCSVLine(line);
        if (row.fieldCount >= 4) { // At least HospitalID, Name, Sector, EmergencyBeds
            hospitalData[actualRows] = row;
            actualRows++;
        }
    }
    
    file.close();
    return true;
}

bool DataLoader::loadPharmacies(const string& filename, CSVRow* pharmacyData, int maxRows, int& actualRows) {
    ifstream file(filename.c_str());
    if (!file.is_open()) {
        cout << "[ERROR] Could not open file '" << filename << "'!" << endl;
        cout << "Possible reasons:" << endl;
        cout << "  - File does not exist at the specified path" << endl;
        cout << "  - Incorrect file path or filename" << endl;
        cout << "  - Insufficient file permissions" << endl;
        return false;
    }
    
    string line;
    actualRows = 0;
    
    // Skip header row
    if (getline(file, line)) {
        // Header line
    }
    
    // Read data rows
    while (getline(file, line) && actualRows < maxRows) {
        if (line.empty()) continue;
        
        CSVRow row = parseCSVLine(line);
        if (row.fieldCount >= 3) { // At least PharmacyID, Name, Sector
            pharmacyData[actualRows] = row;
            actualRows++;
        }
    }
    
    file.close();
    return true;
}

bool DataLoader::loadStops(const string& filename, CSVRow* stopData, int maxRows, int& actualRows) {
    ifstream file(filename.c_str());
    if (!file.is_open()) {
        cout << "[ERROR] Could not open file '" << filename << "'!" << endl;
        cout << "Possible reasons:" << endl;
        cout << "  - File does not exist at the specified path" << endl;
        cout << "  - Incorrect file path or filename" << endl;
        cout << "  - Insufficient file permissions" << endl;
        return false;
    }
    
    string line;
    actualRows = 0;
    
    // Skip header row
    if (getline(file, line)) {
        // Header line
    }
    
    // Read data rows
    while (getline(file, line) && actualRows < maxRows) {
        if (line.empty()) continue;
        
        CSVRow row = parseCSVLine(line);
        if (row.fieldCount >= 3) { // StopID, Name, Coordinates
            stopData[actualRows] = row;
            actualRows++;
        }
    }
    
    file.close();
    return true;
}

bool DataLoader::loadBuses(const string& filename, CSVRow* busData, int maxRows, int& actualRows) {
    ifstream file(filename.c_str());
    if (!file.is_open()) {
        cout << "Error: Could not open file '" << filename << "'!" << endl;
        return false;
    }
    
    string line;
    actualRows = 0;
    
    // Skip header row
    if (getline(file, line)) {
        // Header line
    }
    
    // Read data rows
    while (getline(file, line) && actualRows < maxRows) {
        if (line.empty()) continue;
        
        CSVRow row = parseCSVLine(line);
        if (row.fieldCount >= 3) { // At least BusNo, Company, CurrentStop
            busData[actualRows] = row;
            actualRows++;
        }
    }
    
    file.close();
    return true;
}

bool DataLoader::loadSchoolBuses(const string& filename, CSVRow* schoolBusData, int maxRows, int& actualRows) {
    ifstream file(filename.c_str());
    if (!file.is_open()) {
        cout << "[ERROR] Could not open file '" << filename << "'!" << endl;
        cout << "Possible reasons:" << endl;
        cout << "  - File does not exist at the specified path" << endl;
        cout << "  - Incorrect file path or filename" << endl;
        cout << "  - Insufficient file permissions" << endl;
        return false;
    }
    
    string line;
    actualRows = 0;
    
    // Skip header row
    if (getline(file, line)) {
        // Header line
    }
    
    // Read data rows
    while (getline(file, line) && actualRows < maxRows) {
        if (line.empty()) continue;
        
        CSVRow row = parseCSVLine(line);
        if (row.fieldCount >= 6) { // At least BusNo, Company, CurrentStop, SchoolID, MaxCapacity, Route
            schoolBusData[actualRows] = row;
            actualRows++;
        }
    }
    
    file.close();
    return true;
}

bool DataLoader::loadPopulation(const string& filename, CSVRow* populationData, int maxRows, int& actualRows) {
    ifstream file(filename.c_str());
    if (!file.is_open()) {
        cout << "Error: Could not open file '" << filename << "'!" << endl;
        return false;
    }
    
    string line;
    actualRows = 0;
    
    // Skip header row
    if (getline(file, line)) {
        // Header line
    }
    
    // Read data rows
    while (getline(file, line) && actualRows < maxRows) {
        if (line.empty()) continue;
        
        CSVRow row = parseCSVLine(line);
        if (row.fieldCount >= 5) { // At least CNIC, Name, Age, Sector, Street
            populationData[actualRows] = row;
            actualRows++;
        }
    }
    
    file.close();
    return true;
}

bool DataLoader::loadMalls(const string& filename, CSVRow* mallData, int maxRows, int& actualRows) {
    ifstream file(filename.c_str());
    if (!file.is_open()) {
        cout << "[ERROR] Could not open file '" << filename << "'!" << endl;
        cout << "Possible reasons:" << endl;
        cout << "  - File does not exist at the specified path" << endl;
        cout << "  - Incorrect file path or filename" << endl;
        cout << "  - Insufficient file permissions" << endl;
        return false;
    }
    
    string line;
    actualRows = 0;
    
    // Skip header row
    if (getline(file, line)) {
        // Header line
    }
    
    // Read data rows
    while (getline(file, line) && actualRows < maxRows) {
        if (line.empty()) continue;
        
        CSVRow row = parseCSVLine(line);
        if (row.fieldCount >= 4) { // At least MallID, Name, Sector, Coordinates
            mallData[actualRows] = row;
            actualRows++;
        }
    }
    
    file.close();
    return true;
}

bool DataLoader::loadProducts(const string& filename, CSVRow* productData, int maxRows, int& actualRows) {
    ifstream file(filename.c_str());
    if (!file.is_open()) {
        cout << "[ERROR] Could not open file '" << filename << "'!" << endl;
        cout << "Possible reasons:" << endl;
        cout << "  - File does not exist at the specified path" << endl;
        cout << "  - Incorrect file path or filename" << endl;
        cout << "  - Insufficient file permissions" << endl;
        return false;
    }
    
    string line;
    actualRows = 0;
    
    // Skip header row
    if (getline(file, line)) {
        // Header line
    }
    
    // Read data rows
    while (getline(file, line) && actualRows < maxRows) {
        if (line.empty()) continue;
        
        CSVRow row = parseCSVLine(line);
        if (row.fieldCount >= 5) { // At least ProductID, MallID, ProductName, Category, Price
            productData[actualRows] = row;
            actualRows++;
        }
    }
    
    file.close();
    return true;
}

bool DataLoader::loadFacilities(const string& filename, CSVRow* facilityData, int maxRows, int& actualRows) {
    ifstream file(filename.c_str());
    if (!file.is_open()) {
        cout << "Error: Could not open file '" << filename << "'!" << endl;
        return false;
    }
    
    string line;
    actualRows = 0;
    
    // Skip header row
    if (getline(file, line)) {
        // Header line
    }
    
    // Read data rows
    while (getline(file, line) && actualRows < maxRows) {
        if (line.empty()) continue;
        
        CSVRow row = parseCSVLine(line);
        if (row.fieldCount >= 5) { // At least FacilityID, Name, Type, Sector, Coordinates
            facilityData[actualRows] = row;
            actualRows++;
        }
    }
    
    file.close();
    return true;
}

bool DataLoader::loadAirports(const string& filename, CSVRow* airportData, int maxRows, int& actualRows) {
    ifstream file(filename.c_str());
    if (!file.is_open()) {
        cout << "[ERROR] Could not open file '" << filename << "'!" << endl;
        cout << "Possible reasons:" << endl;
        cout << "  - File does not exist at the specified path" << endl;
        cout << "  - Incorrect file path or filename" << endl;
        cout << "  - Insufficient file permissions" << endl;
        return false;
    }
    
    string line;
    actualRows = 0;
    
    // Skip header row
    if (getline(file, line)) {
        // Header line
    }
    
    // Read data rows
    while (getline(file, line) && actualRows < maxRows) {
        if (line.empty()) continue;
        
        CSVRow row = parseCSVLine(line);
        if (row.fieldCount >= 5) { // At least AirportID, Name, Code, City, Coordinates
            airportData[actualRows] = row;
            actualRows++;
        }
    }
    
    file.close();
    return true;
}

bool DataLoader::loadRailways(const string& filename, CSVRow* stationData, int maxRows, int& actualRows) {
    ifstream file(filename.c_str());
    if (!file.is_open()) {
        cout << "[ERROR] Could not open file '" << filename << "'!" << endl;
        cout << "Possible reasons:" << endl;
        cout << "  - File does not exist at the specified path" << endl;
        cout << "  - Incorrect file path or filename" << endl;
        cout << "  - Insufficient file permissions" << endl;
        return false;
    }
    
    string line;
    actualRows = 0;
    
    // Skip header row
    if (getline(file, line)) {
        // Header line
    }
    
    // Read data rows
    while (getline(file, line) && actualRows < maxRows) {
        if (line.empty()) continue;
        
        CSVRow row = parseCSVLine(line);
        if (row.fieldCount >= 5) { // At least StationID, Name, Code, City, Coordinates
            stationData[actualRows] = row;
            actualRows++;
        }
    }
    
    file.close();
    return true;
}

void DataLoader::displayCSVData(const CSVRow* data, int rowCount, const string& title) {
    cout << "\n=== " << title << " (Loaded " << rowCount << " rows) ===" << endl;
    
    if (rowCount == 0) {
        cout << "No data loaded." << endl;
        return;
    }
    
    for (int i = 0; i < rowCount; i++) {
        cout << "Row " << (i + 1) << ": ";
        for (int j = 0; j < data[i].fieldCount; j++) {
            cout << "[" << j << "]=" << data[i].fields[j];
            if (j < data[i].fieldCount - 1) cout << " | ";
        }
        cout << endl;
    }
    cout << endl;
}

