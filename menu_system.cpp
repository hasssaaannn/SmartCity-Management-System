#include "EducationSector.h"
#include "MedicalSector.h"
#include "TransportManager.h"
#include "MallManager.h"
#include "FacilityManager.h"
#include "PopulationHousing.h"
#include "AirportManager.h"
#include "RailwayManager.h"
#include "EmergencyManager.h"
#include "EmergencyVehicle.h"
#include "core_classes/Graph.h"
#include "core_classes/DataLoader.h"
#include "Bus.h"
#include "SchoolBus.h"
#include "Mall.h"
#include "Product.h"
#include "Facility.h"
#include "Hospital.h"
#include "Pharmacy.h"
#include "School.h"
#include "Airport.h"
#include "RailwayStation.h"
#include <iostream>
#include <cmath>
#include <thread>
#include <chrono>
#include <cstdlib>
#include <ctime>
using namespace std;

#ifndef M_PI
#define M_PI 3.1415
#endif

// Helper function to calculate distance between two coordinates (Haversine formula)
static double calculateDistance(double lat1, double lon1, double lat2, double lon2) {
    const double R = 6371.0; // Earth radius in kilometers
    
    double dLat = (lat2 - lat1) * M_PI / 180.0;
    double dLon = (lon2 - lon1) * M_PI / 180.0;
    
    double a = sin(dLat / 2.0) * sin(dLat / 2.0) +
               cos(lat1 * M_PI / 180.0) * cos(lat2 * M_PI / 180.0) *
               sin(dLon / 2.0) * sin(dLon / 2.0);
    
    double c = 2.0 * atan2(sqrt(a), sqrt(1.0 - a));
    double distance = R * c;
    
    return distance;
}

// Helper function to calculate complete distance from source coordinates to destination coordinates
// Returns: total distance (source coords -> source stop -> destination stop -> destination coords)
// Parameters: sourceLat, sourceLon, destLat, destLon, graph, sourceStopID (output), destStopID (output), totalDistance (output)
// Returns: true if successful, false otherwise
static bool calculateCompleteDistance(double sourceLat, double sourceLon, double destLat, double destLon,
                                      Graph* graph, string& sourceStopID, string& destStopID, double& totalDistance) {
    if (graph == nullptr) return false;
    
    totalDistance = 0.0;
    
    // Step 1: Find nearest stop to source coordinates
    sourceStopID = graph->findNearestStop(sourceLat, sourceLon);
    if (sourceStopID.empty()) return false;
    
    // Step 2: Calculate distance from source coordinates to source stop (straight-line)
    double sourceStopLat, sourceStopLon;
    if (!graph->getVertexCoordinates(sourceStopID, sourceStopLat, sourceStopLon)) return false;
    double walkToSourceStop = calculateDistance(sourceLat, sourceLon, sourceStopLat, sourceStopLon);
    totalDistance += walkToSourceStop;
    
    // Step 3: Find nearest stop to destination coordinates
    destStopID = graph->findNearestStop(destLat, destLon);
    if (destStopID.empty()) return false;
    
    // Step 4: Calculate edge-based distance from source stop to destination stop (graph edges)
    int pathLength = 0;
    double edgeDistance = 0.0;
    string* path = graph->findShortestPath(sourceStopID, destStopID, pathLength, edgeDistance);
    if (path == nullptr) {
        return false;
    }
    delete[] path;
    totalDistance += edgeDistance;
    
    // Step 5: Calculate distance from destination stop to destination coordinates (straight-line)
    double destStopLat, destStopLon;
    if (!graph->getVertexCoordinates(destStopID, destStopLat, destStopLon)) {
        return false;
    }
    double walkFromDestStop = calculateDistance(destStopLat, destStopLon, destLat, destLon);
    totalDistance += walkFromDestStop;
    
    return true;
}

// Helper function to calculate complete distance from source coordinates to destination vertex ID
// Returns: total distance (source coords -> source stop -> destination vertex -> destination coords)
// Parameters: sourceLat, sourceLon, destVertexID, graph, sourceStopID (output), totalDistance (output)
// Returns: true if successful, false otherwise
static bool calculateCompleteDistanceToVertex(double sourceLat, double sourceLon, const string& destVertexID,
                                               Graph* graph, string& sourceStopID, double& totalDistance) {
    if (graph == nullptr || destVertexID.empty()) return false;
    
    totalDistance = 0.0;
    
    // Step 1: Find nearest stop to source coordinates
    sourceStopID = graph->findNearestStop(sourceLat, sourceLon);
    if (sourceStopID.empty()) return false;
    
    // Step 2: Calculate distance from source coordinates to source stop (straight-line)
    double sourceStopLat, sourceStopLon;
    if (!graph->getVertexCoordinates(sourceStopID, sourceStopLat, sourceStopLon)) return false;
    double walkToSourceStop = calculateDistance(sourceLat, sourceLon, sourceStopLat, sourceStopLon);
    totalDistance += walkToSourceStop;
    
    // Step 3: Get destination vertex coordinates
    double destLat, destLon;
    if (!graph->getVertexCoordinates(destVertexID, destLat, destLon)) return false;
    
    // Step 4: Find nearest stop to destination coordinates
    string destStopID = graph->findNearestStop(destLat, destLon);
    if (destStopID.empty()) return false;
    
    // Step 5: Calculate edge-based distance from source stop to destination stop (graph edges)
    int pathLength = 0;
    double edgeDistance = 0.0;
    string* path = graph->findShortestPath(sourceStopID, destStopID, pathLength, edgeDistance);
    if (path == nullptr) {
        return false;
    }
    delete[] path;
    totalDistance += edgeDistance;
    
    // Step 6: Calculate distance from destination stop to destination coordinates (straight-line)
    double destStopLat, destStopLon;
    if (!graph->getVertexCoordinates(destStopID, destStopLat, destStopLon)) {
        return false;
    }
    double walkFromDestStop = calculateDistance(destStopLat, destStopLon, destLat, destLon);
    totalDistance += walkFromDestStop;
    
    return true;
}

// Helper function to parse coordinates from string "lat, lon"
static bool parseCoordinates(const string& coordStr, double& lat, double& lon) {
    if (coordStr.empty()) return false;
    
    string trimmed = coordStr;
    if (trimmed.length() >= 2 && trimmed[0] == '"' && trimmed[trimmed.length() - 1] == '"') {
        trimmed = trimmed.substr(1, trimmed.length() - 2);
    }
    
    size_t commaPos = trimmed.find(',');
    if (commaPos == string::npos) return false;
    
    string latStr = trimmed.substr(0, commaPos);
    string lonStr = trimmed.substr(commaPos + 1);
    
    // Trim whitespace
    size_t start = 0, end = latStr.length() - 1;
    while (start < latStr.length() && (latStr[start] == ' ' || latStr[start] == '\t')) start++;
    while (end > start && (latStr[end] == ' ' || latStr[end] == '\t')) end--;
    if (start <= end) latStr = latStr.substr(start, end - start + 1);
    
    start = 0; end = lonStr.length() - 1;
    while (start < lonStr.length() && (lonStr[start] == ' ' || lonStr[start] == '\t')) start++;
    while (end > start && (lonStr[end] == ' ' || lonStr[end] == '\t')) end--;
    if (start <= end) lonStr = lonStr.substr(start, end - start + 1);
    
    // Convert to double (simple conversion)
    lat = 0.0; lon = 0.0;
    bool hasDecimal = false;
    double fraction = 0.1;
    
    // Parse latitude
    bool isNegative = false;
    int i = 0;
    if (!latStr.empty() && latStr[0] == '-') {
        isNegative = true;
        i = 1;
    }
    for (; i < (int)latStr.length(); i++) {
        if (latStr[i] >= '0' && latStr[i] <= '9') {
            if (!hasDecimal) {
                lat = lat * 10.0 + (latStr[i] - '0');
            } else {
                lat += (latStr[i] - '0') * fraction;
                fraction *= 0.1;
            }
        } else if (latStr[i] == '.' && !hasDecimal) {
            hasDecimal = true;
        }
    }
    if (isNegative) lat = -lat;
    
    // Parse longitude
    hasDecimal = false;
    fraction = 0.1;
    isNegative = false;
    i = 0;
    if (!lonStr.empty() && lonStr[0] == '-') {
        isNegative = true;
        i = 1;
    }
    for (; i < (int)lonStr.length(); i++) {
        if (lonStr[i] >= '0' && lonStr[i] <= '9') {
            if (!hasDecimal) {
                lon = lon * 10.0 + (lonStr[i] - '0');
            } else {
                lon += (lonStr[i] - '0') * fraction;
                fraction *= 0.1;
            }
        } else if (lonStr[i] == '.' && !hasDecimal) {
            hasDecimal = true;
        }
    }
    if (isNegative) lon = -lon;
    
    return true;
}

// Helper function to read a double from user input
static double readDouble(const string& prompt) {
    cout << prompt;
    string input;
    getline(cin, input);
    
    double result = 0.0;
    double fraction = 0.1;
    bool hasDecimal = false;
    bool isNegative = false;
    int i = 0;
    
    // Skip whitespace
    while (i < (int)input.length() && (input[i] == ' ' || input[i] == '\t')) i++;
    
    // Check for negative
    if (i < (int)input.length() && input[i] == '-') {
        isNegative = true;
        i++;
    }
    
    // Parse digits
    while (i < (int)input.length()) {
        if (input[i] >= '0' && input[i] <= '9') {
            if (!hasDecimal) {
                result = result * 10.0 + (input[i] - '0');
            } else {
                result += (input[i] - '0') * fraction;
                fraction *= 0.1;
            }
        } else if (input[i] == '.' && !hasDecimal) {
            hasDecimal = true;
        } else {
            break;
        }
        i++;
    }
    
    return isNegative ? -result : result;
}

// Helper function to read an integer from user input
static int readInt(const string& prompt) {
    cout << prompt;
    string input;
    getline(cin, input);
    
    int result = 0;
    bool isNegative = false;
    int i = 0;
    
    // Skip whitespace
    while (i < (int)input.length() && (input[i] == ' ' || input[i] == '\t')) i++;
    
    // Check for negative
    if (i < (int)input.length() && input[i] == '-') {
        isNegative = true;
        i++;
    }
    
    // Parse digits
    while (i < (int)input.length() && input[i] >= '0' && input[i] <= '9') {
        result = result * 10 + (input[i] - '0');
        i++;
    }
    
    return isNegative ? -result : result;
}

// Helper function to read a string from user input
static string readString(const string& prompt) {
    cout << prompt;
    string input;
    getline(cin, input);
    
    // Trim leading/trailing whitespace
    size_t start = 0;
    while (start < input.length() && (input[start] == ' ' || input[start] == '\t')) start++;
    size_t end = input.length();
    while (end > start && (input[end - 1] == ' ' || input[end - 1] == '\t')) end--;
    
    if (start < end) {
        return input.substr(start, end - start);
    }
    return "";
}

// Helper function to pause and wait for user input
static void pause() {
    cout << "\nPress Enter to continue...";
    cin.ignore();
    cin.get();
}

// Helper function for delay in real-time simulation
static void delay() {
    std::this_thread::sleep_for(std::chrono::milliseconds(250));
}



// Global system components (initialized in main)
static Graph* sharedCityGraph = nullptr;
static EducationSector* education = nullptr;
static MedicalSector* medical = nullptr;
static TransportManager* transport = nullptr;
static MallManager* malls = nullptr;
static FacilityManager* facilities = nullptr;
static PopulationManager* population = nullptr;
static AirportManager* airports = nullptr;
static RailwayManager* railways = nullptr;
static EmergencyManager* emergency = nullptr;
static DataLoader* loader = nullptr;

// ========================================================================
// MENU DISPLAY FUNCTIONS
// ========================================================================

void displayMainMenu() {
    cout << "\n";
    cout << "================================================================\n";
    cout << "         SMART CITY MANAGEMENT SYSTEM - MAIN MENU\n";
    cout << "================================================================\n";
    cout << "\n";
    cout << "   1. Education Sector\n";
    cout << "   2. Medical Sector\n";
    cout << "   3. Transport Manager\n";
    cout << "   4. Mall Manager\n";
    cout << "   5. Facility Manager\n";
    cout << "   6. Population/Housing\n";
    cout << "   7. Airport Manager\n";
    cout << "   8. Railway Manager\n";
    cout << "   9. Emergency Transport Routing\n";
    cout << "  10. Cross-Module Queries (Find Nearest Services)\n";
    cout << "  11. System Statistics\n";
    cout << "  12. Load/Reload Data\n";
    cout << "   0. Exit\n";
    cout << "\n";
    cout << "================================================================\n";
    cout << "Enter your choice (0-12): ";
}

void displayEducationMenu() {
    cout << "\n";
    cout << "================================================================\n";
    cout << "              EDUCATION SECTOR MENU\n";
    cout << "================================================================\n";
    cout << "\n";
    cout << "   1. Search School by ID\n";
    cout << "   2. Search Schools by Subject\n";
    cout << "   3. Get Top Ranked Schools\n";
    cout << "   4. Find Nearest School (by coordinates)\n";
    cout << "   5. Display All Schools\n";
    cout << "   6. View School Organogram (Hierarchy Tree)\n";
    cout << "   7. Add Department to School\n";
    cout << "   8. Add Class to Department\n";
    cout << "   9. Add New School\n";
    cout << "   0. Back to Main Menu\n";
    cout << "\n";
    cout << "================================================================\n";
    cout << "Enter your choice (0-9): ";
}

void displayMedicalMenu() {
    cout << "\n";
    cout << "================================================================\n";
    cout << "              MEDICAL SECTOR MENU\n";
    cout << "================================================================\n";
    cout << "\n";
    cout << "   1. Search Hospital by ID\n";
    cout << "   2. Search Pharmacy by ID\n";
    cout << "   3. Search Medicine by Name\n";
    cout << "   4. Search Medicine by Formula\n";
    cout << "   5. Search Doctor by Specialization\n";
    cout << "   6. Search Hospitals by Specialization\n";
    cout << "   7. Get Hospital with Most Emergency Beds\n";
    cout << "   8. Find Nearest Hospital (by coordinates)\n";
    cout << "   9. Display All Hospitals\n";
    cout << "  10. Display All Pharmacies\n";
    cout << "  11. Add New Hospital\n";
    cout << "  12. Add New Pharmacy\n";
    cout << "  13. Add New Doctor\n";
    cout << "  14. Add Medicine to Pharmacy\n";
    cout << "   0. Back to Main Menu\n";
    cout << "\n";
    cout << "================================================================\n";
    cout << "Enter your choice (0-14): ";
}

void displayTransportMenu() {
    cout << "\n";
    cout << "================================================================\n";
    cout << "            TRANSPORT MANAGER MENU\n";
    cout << "================================================================\n";
    cout << "\n";
    cout << "   1. Search Bus by Bus Number\n";
    cout << "   2. Find Buses at a Stop\n";
    cout << "   3. Find Shortest Path (Between Two Stops)\n";
    cout << "   4. Find Nearest Bus (by coordinates)\n";
    cout << "   5. Display All Buses\n";
    cout << "   6. Display All Companies\n";
    cout << "   7. View Bus Route\n";
    cout << "   8. Get Buses by Company\n";
    cout << "   9. Set Bus Location\n";
    cout << "  10. Get Bus Current Location\n";
    cout << "  11. Add New Bus\n";
    cout << "\n";
    cout << "  12. Display All School Buses (Grouped by School)\n";
    cout << "  13. Find Nearest School Bus\n";
    cout << "  14. Track Student on Bus\n";
    cout << "  15. Add New School Bus\n";
    cout << "   0. Back to Main Menu\n";
    cout << "\n";
    cout << "================================================================\n";
    cout << "Enter your choice (0-15): ";
}

void displayMallMenu() {
    cout << "\n";
    cout << "================================================================\n";
    cout << "               MALL MANAGER MENU\n";
    cout << "================================================================\n";
    cout << "\n";
    cout << "   1. Search Mall by ID\n";
    cout << "   2. Search Mall by Name\n";
    cout << "   3. Search Product (Anywhere/Any Mall)\n";
    cout << "   4. Search Products by Category\n";
    cout << "   5. Find Nearest Mall (by stop ID)\n";
    cout << "   6. Find Nearest Mall (by coordinates)\n";
    cout << "   7. Display All Malls\n";
    cout << "   8. Display Products in a Mall\n";
    cout << "   9. Add New Mall\n";
    cout << "  10. Add Product to Mall\n";
    cout << "   0. Back to Main Menu\n";
    cout << "\n";
    cout << "================================================================\n";
    cout << "Enter your choice (0-10): ";
}

void displayFacilityMenu() {
    cout << "\n";
    cout << "================================================================\n";
    cout << "            FACILITY MANAGER MENU\n";
    cout << "================================================================\n";
    cout << "\n";
    cout << "   1. Search Facility by ID\n";
    cout << "   2. Search Facility by Name\n";
    cout << "   3. Search Facilities by Type\n";
    cout << "   4. Find Nearest Facility (by coordinates)\n";
    cout << "   5. Display All Facilities\n";
    cout << "   6. Add New Facility\n";
    cout << "   0. Back to Main Menu\n";
    cout << "\n";
    cout << "================================================================\n";
    cout << "Enter your choice (0-6): ";
}

void displayAirportMenu() {
    cout << "\n";
    cout << "================================================================\n";
    cout << "            AIRPORT MANAGER MENU\n";
    cout << "================================================================\n";
    cout << "\n";
    cout << "   1. Search Airport by ID\n";
    cout << "   2. Search Airport by Name\n";
    cout << "   3. Search Airport by IATA Code\n";
    cout << "   4. Find Nearest Airport (by coordinates)\n";
    cout << "   5. Calculate Air Distance (Between Two Airports)\n";
    cout << "   6. Display All Airports\n";
    cout << "   7. Add New Airport\n";
    cout << "   0. Back to Main Menu\n";
    cout << "\n";
    cout << "================================================================\n";
    cout << "Enter your choice (0-7): ";
}

void displayPopulationMenu() {
    cout << "\n";
    cout << "================================================================\n";
    cout << "           POPULATION/HOUSING MENU\n";
    cout << "================================================================\n";
    cout << "\n";
    cout << "   [SEARCH & VIEW]\n";
    cout << "   1. Search Citizen by CNIC\n";
    cout << "   2. Display Citizen Information\n";
    cout << "\n";
    cout << "   [ADD LOCATIONS]\n";
    cout << "   3. Add Sector\n";
    cout << "   4. Add Street\n";
    cout << "   5. Add House\n";
    cout << "\n";
    cout << "   [ADD PEOPLE]\n";
    cout << "   6. Add Family (Family Head)\n";
    cout << "   7. Add Family Member\n";
    cout << "\n";
    cout << "   [REPORTS]\n";
    cout << "   8. View Age Distribution Report\n";
    cout << "   9. View Gender Ratio Report\n";
    cout << "  10. View Occupation Breakdown Report\n";
    cout << "  11. View Population Density Report\n";
    cout << "  12. View Total Population\n";
    cout << "\n";
    cout << "   [HEATMAPS]\n";
    cout << "  13. Display Color-Coded Heatmap\n";
    cout << "  14. Display Bar Chart Heatmap\n";
    cout << "  15. Generate Full Population Heatmap (with Analysis)\n";
    cout << "  16. Export Heatmap to File\n";
    cout << "\n";
    cout << "   0. Back to Main Menu\n";
    cout << "\n";
    cout << "================================================================\n";
    cout << "Enter your choice (0-16): ";
}

void displayCrossModuleMenu() {
    cout << "\n";
    cout << "================================================================\n";
    cout << "         CROSS-MODULE QUERIES MENU\n";
    cout << "================================================================\n";
    cout << "\n";
    cout << "   1. Find All Nearest Services (from coordinates)\n";
    cout << "   2. Student Scenario: Find Best School\n";
    cout << "   3. Medical Emergency Scenario\n";
    cout << "   4. Shopping Trip Planning\n";
    cout << "   5. Universal Pathfinding (Any Location -> Any Location)\n";
    cout << "   6. Real-Time Route Simulation\n";
    cout << "   0. Back to Main Menu\n";
    cout << "\n";
    cout << "================================================================\n";
    cout << "Enter your choice (0-6): ";
}

void displayStatisticsMenu() {
    cout << "\n";
    cout << "================================================================\n";
    cout << "           SYSTEM STATISTICS MENU\n";
    cout << "================================================================\n";
    cout << "\n";
    cout << "   1. View Module Statistics\n";
    cout << "   2. View Shared Graph Statistics\n";
    cout << "   3. View Data Structure Usage Summary\n";
    cout << "   4. Back to Main Menu\n";
    cout << "\n";
    cout << "================================================================\n";
    cout << "Enter your choice (1-4): ";
}

void displayDataMenu() {
    cout << "\n";
    cout << "================================================================\n";
    cout << "            DATA MANAGEMENT MENU\n";
    cout << "================================================================\n";
    cout << "\n";
    cout << "   1. Load All Data (from CSV files)\n";
    cout << "   2. Load Schools Only\n";
    cout << "   3. Load Hospitals Only\n";
    cout << "   4. Load Pharmacies Only\n";
    cout << "   5. Load Buses Only\n";
    cout << "   6. Load Population Only\n";
    cout << "   7. Load Malls Only\n";
    cout << "   8. Load Products Only\n";
    cout << "   9. Load Facilities Only\n";
    cout << "  10. Load Airports Only\n";
    cout << "  11. Load Railway Stations Only\n";
    cout << "  12. Load School Buses Only\n";
    cout << "  13. View Data Status\n";
    cout << "   0. Back to Main Menu\n";
    cout << "\n";
    cout << "================================================================\n";
    cout << "Enter your choice (0-13): ";
}

// ========================================================================
// EDUCATION SECTOR MENU HANDLERS
// ========================================================================

void handleEducationMenu() {
    int choice;
    do {
        displayEducationMenu();
        choice = readInt("");
        
        switch (choice) {
            case 1: {
                string schoolID = readString("Enter School ID: ");
                School* school = education->searchSchool(schoolID);
                if (school != nullptr) {
                    cout << "\n[RESULT]\n";
                    cout << "----------------------------\n";
                    cout << "School Found!\n";
                    cout << "----------------------------\n";
                    cout << "ID: " << school->getSchoolID() << "\n";
                    cout << "Name: " << school->getName() << "\n";
                    cout << "Sector: " << school->getSector() << "\n";
                    cout << "Rating: " << school->getRating() << "/10.0\n";
                    cout << "Subjects: ";
                    string* subjects = school->getSubjects();
                    int subjectCount = school->getSubjectCount();
                    if (subjects != nullptr && subjectCount > 0) {
                        for (int i = 0; i < subjectCount; i++) {
                            cout << subjects[i];
                            if (i < subjectCount - 1) cout << ", ";
                        }
                    } else {
                        cout << "None";
                    }
                    cout << "\n";
                } else {
                    cout << "\n[ERROR] School not found with ID: " << schoolID << "\n";
                }
                pause();
                break;
            }
            
            case 2: {
                string subject = readString("Enter Subject Name: ");
                int count = 0;
                School** schools = education->findSchoolsBySubject(subject, count);
                if (count > 0) {
                    cout << "\n[RESULT]\n";
                    cout << "----------------------------\n";
                    cout << "Found " << count << " school(s) offering " << subject << ":\n";
                    cout << "----------------------------\n";
                    for (int i = 0; i < count; i++) {
                        cout << (i + 1) << ". " << schools[i]->getName() 
                             << " (ID: " << schools[i]->getSchoolID() 
                             << ", Rating: " << schools[i]->getRating() << ")\n";
                    }
                    delete[] schools;
                } else {
                    cout << "\n[INFO] No schools found offering " << subject << "\n";
                }
                pause();
                break;
            }
            
            case 3: {
                int topN = readInt("Enter number of top schools to display: ");
                int count = 0;
                School** topSchools = education->getTopRankedSchools(topN, count);
                if (count > 0) {
                    cout << "\n[RESULT]\n";
                    cout << "----------------------------\n";
                    cout << "Top " << count << " Ranked School(s):\n";
                    cout << "----------------------------\n";
                    for (int i = 0; i < count; i++) {
                        cout << (i + 1) << ". " << topSchools[i]->getName() 
                             << " (Rating: " << topSchools[i]->getRating() << "/10.0)\n";
                    }
                    delete[] topSchools;
                } else {
                    cout << "\n[INFO] No schools available\n";
                }
                pause();
                break;
            }
            
            case 4: {
                double lat = readDouble("Enter latitude (-90 to 90): ");
                double lon = readDouble("Enter longitude (-180 to 180): ");
                
                // Validate coordinate ranges
                if (lat < -90.0 || lat > 90.0) {
                    cout << "\n[ERROR] Invalid latitude: " << lat << "\n";
                    cout << "Latitude must be between -90 and 90 degrees.\n";
                    pause();
                    break;
                }
                
                if (lon < -180.0 || lon > 180.0) {
                    cout << "\n[ERROR] Invalid longitude: " << lon << "\n";
                    cout << "Longitude must be between -180 and 180 degrees.\n";
                    pause();
                    break;
                }
                
                string nearestID = education->findNearestSchool(lat, lon);
                if (!nearestID.empty()) {
                    School* nearest = education->searchSchool(nearestID);
                    if (nearest != nullptr) {
                        cout << "\n[RESULT]\n";
                        cout << "----------------------------\n";
                        cout << "Nearest School:\n";
                        cout << "----------------------------\n";
                        cout << "Name: " << nearest->getName() << "\n";
                        cout << "ID: " << nearest->getSchoolID() << "\n";
                        cout << "Sector: " << nearest->getSector() << "\n";
                        cout << "Rating: " << nearest->getRating() << "/10.0\n";
                    }
                } else {
                    cout << "\n[ERROR] No schools found near the specified location\n";
                }
                pause();
                break;
            }
            
            case 5: {
                cout << "\n";
                education->displayAllSchools();
                pause();
                break;
            }
            
            case 6: {
                string schoolID = readString("Enter School ID: ");
                cout << "\n";
                education->displaySchoolOrganogram(schoolID);
                pause();
                break;
            }
            
            case 7: {
                string schoolID = readString("Enter School ID: ");
                string deptID = readString("Enter Department ID: ");
                string deptName = readString("Enter Department Name: ");
                if (schoolID.empty() || deptID.empty() || deptName.empty()) {
                    cout << "\n[ERROR] School ID, Department ID, and Department Name are required.\n";
                    pause();
                    break;
                }
                
                // Check if school exists
                if (education->searchSchool(schoolID) == nullptr) {
                    cout << "\n[ERROR] School with ID '" << schoolID << "' not found.\n";
                    cout << "Please register the school first or check the school ID.\n";
                    pause();
                    break;
                }
                
                if (education->addDepartment(schoolID, deptID, deptName)) {
                    cout << "\n[SUCCESS] Department added successfully!\n";
                } else {
                    cout << "\n[ERROR] Failed to add department.\n";
                    cout << "Possible reasons:" << endl;
                    cout << "  - Department ID already exists for this school" << endl;
                    cout << "  - Invalid department data" << endl;
                }
                pause();
                break;
            }
            
            case 8: {
                string schoolID = readString("Enter School ID: ");
                string deptID = readString("Enter Department ID: ");
                string classID = readString("Enter Class ID: ");
                string className = readString("Enter Class Name: ");
                if (schoolID.empty() || deptID.empty() || classID.empty() || className.empty()) {
                    cout << "\n[ERROR] School ID, Department ID, Class ID, and Class Name are required.\n";
                    pause();
                    break;
                }
                
                // Check if school exists
                if (education->searchSchool(schoolID) == nullptr) {
                    cout << "\n[ERROR] School with ID '" << schoolID << "' not found.\n";
                    cout << "Please register the school first or check the school ID.\n";
                    pause();
                    break;
                }
                
                if (education->addClass(schoolID, deptID, classID, className)) {
                    cout << "\n[SUCCESS] Class added successfully!\n";
                } else {
                    cout << "\n[ERROR] Failed to add class.\n";
                    cout << "Possible reasons:" << endl;
                    cout << "  - Department '" << deptID << "' not found in school '" << schoolID << "'" << endl;
                    cout << "  - Class ID already exists in this department" << endl;
                    cout << "  - Invalid class data" << endl;
                }
                pause();
                break;
            }
            
            case 9: {
                cout << "\n[ADD NEW SCHOOL]\n";
                cout << "----------------------------\n";
                string schoolID = readString("Enter School ID: ");
                string name = readString("Enter School Name: ");
                string sector = readString("Enter Sector: ");
                double rating = readDouble("Enter Rating (0.0-10.0): ");
                string subjectsStr = readString("Enter Subjects (comma-separated, e.g., Math,Science,English): ");
                
                if (schoolID.empty()) {
                    cout << "\n[ERROR] School ID cannot be empty.\n";
                    cout << "Please enter a valid school ID.\n";
                    pause();
                    break;
                }
                
                if (name.empty()) {
                    cout << "\n[ERROR] School name cannot be empty.\n";
                    pause();
                    break;
                }
                
                if (sector.empty()) {
                    cout << "\n[ERROR] Sector cannot be empty.\n";
                    pause();
                    break;
                }
                
                if (subjectsStr.empty()) {
                    cout << "\n[ERROR] Subjects cannot be empty.\n";
                    cout << "Please enter at least one subject (comma-separated).\n";
                    pause();
                    break;
                }
                
                if (rating < 0.0 || rating > 10.0) {
                    cout << "\n[ERROR] Rating must be between 0.0 and 10.0\n";
                    pause();
                    break;
                }
                
                // Check if school already exists
                if (education->searchSchool(schoolID) != nullptr) {
                    cout << "\n[ERROR] School with ID " << schoolID << " already exists\n";
                    pause();
                    break;
                }
                
                // Register school (adds with sector coordinates automatically)
                if (!education->registerSchool(schoolID, name, sector, rating, subjectsStr)) {
                    cout << "\n[ERROR] Failed to add school\n";
                    pause();
                    break;
                }
                
                // Ask for custom coordinates
                cout << "\nEnter coordinates (or press Enter to use sector coordinates):\n";
                string coordStr = readString("Coordinates (format: lat, lon, e.g., 33.708, 73.041): ");
                
                Graph* cityGraph = education->getCityGraph();
                if (cityGraph != nullptr && !coordStr.empty()) {
                    double lat = 0.0, lon = 0.0;
                    if (parseCoordinates(coordStr, lat, lon)) {
                        // Remove vertex (to update with custom coordinates)
                        if (cityGraph->removeVertex(schoolID)) {
                            // Re-add with custom coordinates
                            School* school = education->searchSchool(schoolID);
                            if (school != nullptr) {
                                if (cityGraph->addVertex(schoolID, name, lat, lon, school)) {
                                    cityGraph->connectToNearestStop(schoolID);
                                }
                            }
                        }
                    }
                }
                
                cout << "\n[SUCCESS] School added successfully!\n";
                cout << "ID: " << schoolID << "\n";
                cout << "Name: " << name << "\n";
                cout << "Sector: " << sector << "\n";
                cout << "Rating: " << rating << "/10.0\n";
                cout << "Subjects: " << subjectsStr << "\n";
                
                // Display connected stops
                if (cityGraph != nullptr) {
                    string connectedStops[10];
                    int stopCount = 0;
                    if (cityGraph->getConnectedStops(schoolID, connectedStops, 10, stopCount)) {
                        if (stopCount > 0) {
                            cout << "Connected Stop(s): ";
                            for (int i = 0; i < stopCount; i++) {
                                cout << connectedStops[i];
                                if (i < stopCount - 1) cout << ", ";
                            }
                            cout << "\n";
                        }
                    }
                }
                
                pause();
                break;
            }
            
            case 0:
                return;
            
            default:
                cout << "\n[ERROR] Invalid choice. Please try again.\n";
                pause();
                break;
        }
    } while (choice != 0);
}

// ========================================================================
// MEDICAL SECTOR MENU HANDLERS (Partial - continuing in next part)
// ========================================================================

void handleMedicalMenu() {
    int choice;
    do {
        displayMedicalMenu();
        choice = readInt("");
        
        switch (choice) {
            case 1: {
                string hospitalID = readString("Enter Hospital ID: ");
                Hospital* hospital = medical->searchHospital(hospitalID);
                if (hospital != nullptr) {
                    cout << "\n[RESULT]\n";
                    cout << "----------------------------\n";
                    cout << "Hospital Found!\n";
                    cout << "----------------------------\n";
                    cout << "ID: " << hospital->getHospitalID() << "\n";
                    cout << "Name: " << hospital->getName() << "\n";
                    cout << "Sector: " << hospital->getSector() << "\n";
                    cout << "Emergency Beds: " << hospital->getEmergencyBeds() << "\n";
                    cout << "Specialization: " << hospital->getSpecialization() << "\n";
                } else {
                    cout << "\n[ERROR] Hospital not found with ID: " << hospitalID << "\n";
                }
                pause();
                break;
            }
            
            case 2: {
                string pharmacyID = readString("Enter Pharmacy ID: ");
                Pharmacy* pharmacy = medical->searchPharmacy(pharmacyID);
                if (pharmacy != nullptr) {
                    cout << "\n[RESULT]\n";
                    cout << "----------------------------\n";
                    cout << "Pharmacy Found!\n";
                    cout << "----------------------------\n";
                    cout << "ID: " << pharmacy->getPharmacyID() << "\n";
                    cout << "Name: " << pharmacy->getName() << "\n";
                    cout << "Sector: " << pharmacy->getSector() << "\n";
                } else {
                    cout << "\n[ERROR] Pharmacy not found with ID: " << pharmacyID << "\n";
                }
                pause();
                break;
            }
            
            case 3: {
                string medicineName = readString("Enter Medicine Name: ");
                Medicine* medicine = medical->searchMedicineByName(medicineName);
                if (medicine != nullptr) {
                    cout << "\n[RESULT]\n";
                    cout << "----------------------------\n";
                    cout << "Medicine Found!\n";
                    cout << "----------------------------\n";
                    cout << "Name: " << medicine->getMedicineName() << "\n";
                    cout << "Formula: " << medicine->getFormula() << "\n";
                    cout << "Price: " << medicine->getPrice() << " PKR\n";
                    int count = 0;
                    Pharmacy** pharmacies = medical->findPharmaciesWithMedicine(medicineName, count);
                    if (count > 0) {
                        cout << "Available at " << count << " pharmacy(ies)\n";
                    }
                    if (pharmacies != nullptr) delete[] pharmacies;
                } else {
                    cout << "\n[ERROR] Medicine not found: " << medicineName << "\n";
                }
                pause();
                break;
            }
            
            case 4: {
                string formula = readString("Enter Medicine Formula: ");
                Medicine* medicine = medical->searchMedicineByFormula(formula);
                if (medicine != nullptr) {
                    cout << "\n[RESULT]\n";
                    cout << "----------------------------\n";
                    cout << "Medicine Found!\n";
                    cout << "----------------------------\n";
                    cout << "Name: " << medicine->getMedicineName() << "\n";
                    cout << "Formula: " << medicine->getFormula() << "\n";
                    cout << "Price: " << medicine->getPrice() << " PKR\n";
                } else {
                    cout << "\n[ERROR] Medicine not found with formula: " << formula << "\n";
                }
                pause();
                break;
            }
            
            case 5: {
                string specialization = readString("Enter Doctor Specialization: ");
                int count = 0;
                string* doctorIDs = medical->findDoctorsBySpecialization(specialization, count);
                if (count > 0) {
                    cout << "\n[RESULT]\n";
                    cout << "----------------------------\n";
                    cout << "Found " << count << " doctor(s) with specialization: " << specialization << "\n";
                    cout << "----------------------------\n";
                    for (int i = 0; i < count; i++) {
                        cout << (i + 1) << ". Doctor ID: " << doctorIDs[i] << "\n";
                    }
                    delete[] doctorIDs;
                } else {
                    cout << "\n[INFO] No doctors found with specialization: " << specialization << "\n";
                }
                pause();
                break;
            }
            
            case 6: {
                string specialization = readString("Enter Hospital Specialization: ");
                int count = 0;
                Hospital** hospitals = medical->findHospitalsBySpecialization(specialization, count);
                if (count > 0) {
                    cout << "\n[RESULT]\n";
                    cout << "----------------------------\n";
                    cout << "Found " << count << " hospital(s) with specialization: " << specialization << "\n";
                    cout << "----------------------------\n";
                    for (int i = 0; i < count; i++) {
                        cout << (i + 1) << ". " << hospitals[i]->getName() 
                             << " (Beds: " << hospitals[i]->getEmergencyBeds() << ")\n";
                    }
                    delete[] hospitals;
                } else {
                    cout << "\n[INFO] No hospitals found with specialization: " << specialization << "\n";
                }
                pause();
                break;
            }
            
            case 7: {
                Hospital* mostBeds = medical->getHospitalWithMostBeds();
                if (mostBeds != nullptr) {
                    cout << "\n[RESULT]\n";
                    cout << "----------------------------\n";
                    cout << "Hospital with Most Emergency Beds:\n";
                    cout << "----------------------------\n";
                    cout << "Name: " << mostBeds->getName() << "\n";
                    cout << "ID: " << mostBeds->getHospitalID() << "\n";
                    cout << "Emergency Beds: " << mostBeds->getEmergencyBeds() << "\n";
                    cout << "Specialization: " << mostBeds->getSpecialization() << "\n";
                } else {
                    cout << "\n[INFO] No hospitals available\n";
                }
                pause();
                break;
            }
            
            case 8: {
                double lat = readDouble("Enter latitude: ");
                double lon = readDouble("Enter longitude: ");
                string nearestID = medical->findNearestHospital(lat, lon);
                if (!nearestID.empty()) {
                    Hospital* nearest = medical->searchHospital(nearestID);
                    if (nearest != nullptr) {
                        cout << "\n[RESULT]\n";
                        cout << "----------------------------\n";
                        cout << "Nearest Hospital:\n";
                        cout << "----------------------------\n";
                        cout << "Name: " << nearest->getName() << "\n";
                        cout << "ID: " << nearest->getHospitalID() << "\n";
                        cout << "Emergency Beds: " << nearest->getEmergencyBeds() << "\n";
                        cout << "Specialization: " << nearest->getSpecialization() << "\n";
                    }
                } else {
                    cout << "\n[ERROR] No hospitals found near the specified location\n";
                }
                pause();
                break;
            }
            
            case 9: {
                cout << "\n";
                medical->displayAllHospitals();
                pause();
                break;
            }
            
            case 10: {
                cout << "\n";
                medical->displayAllPharmacies();
                pause();
                break;
            }
            
            case 11: {
                cout << "\n[ADD NEW HOSPITAL]\n";
                cout << "----------------------------\n";
                string hospitalID = readString("Enter Hospital ID: ");
                string name = readString("Enter Hospital Name: ");
                string sector = readString("Enter Sector: ");
                int emergencyBeds = readInt("Enter Number of Emergency Beds: ");
                string specialization = readString("Enter Specialization (comma-separated): ");
                
                if (hospitalID.empty() || name.empty() || sector.empty() || specialization.empty()) {
                    cout << "\n[ERROR] All fields are required\n";
                    pause();
                    break;
                }
                
                if (emergencyBeds < 0) {
                    cout << "\n[ERROR] Emergency beds must be non-negative\n";
                    pause();
                    break;
                }
                
                // Check if hospital already exists
                if (medical->searchHospital(hospitalID) != nullptr) {
                    cout << "\n[ERROR] Hospital with ID " << hospitalID << " already exists\n";
                    pause();
                    break;
                }
                
                // Register hospital (adds with sector coordinates automatically)
                if (!medical->registerHospital(hospitalID, name, sector, emergencyBeds, specialization)) {
                    cout << "\n[ERROR] Failed to add hospital\n";
                    pause();
                    break;
                }
                
                // Ask for custom coordinates
                cout << "\nEnter coordinates (or press Enter to use sector coordinates):\n";
                string coordStr = readString("Coordinates (format: lat, lon, e.g., 33.708, 73.041): ");
                
                Graph* cityGraph = medical->getCityGraph();
                if (cityGraph != nullptr && !coordStr.empty()) {
                    double lat = 0.0, lon = 0.0;
                    if (parseCoordinates(coordStr, lat, lon)) {
                        // Remove vertex (to update with custom coordinates)
                        if (cityGraph->removeVertex(hospitalID)) {
                            // Re-add with custom coordinates
                            Hospital* hospital = medical->searchHospital(hospitalID);
                            if (hospital != nullptr) {
                                if (cityGraph->addVertex(hospitalID, name, lat, lon, hospital)) {
                                    cityGraph->connectToNearestStop(hospitalID);
                                }
                            }
                        }
                    }
                }
                
                cout << "\n[SUCCESS] Hospital added successfully!\n";
                cout << "ID: " << hospitalID << "\n";
                cout << "Name: " << name << "\n";
                cout << "Sector: " << sector << "\n";
                cout << "Emergency Beds: " << emergencyBeds << "\n";
                cout << "Specialization: " << specialization << "\n";
                
                // Display connected stops
                if (cityGraph != nullptr) {
                    string connectedStops[10];
                    int stopCount = 0;
                    if (cityGraph->getConnectedStops(hospitalID, connectedStops, 10, stopCount)) {
                        if (stopCount > 0) {
                            cout << "Connected Stop(s): ";
                            for (int i = 0; i < stopCount; i++) {
                                cout << connectedStops[i];
                                if (i < stopCount - 1) cout << ", ";
                            }
                            cout << "\n";
                        }
                    }
                }
                
                pause();
                break;
            }
            
            case 12: {
                cout << "\n[ADD NEW PHARMACY]\n";
                cout << "----------------------------\n";
                string pharmacyID = readString("Enter Pharmacy ID: ");
                string name = readString("Enter Pharmacy Name: ");
                string sector = readString("Enter Sector: ");
                
                if (pharmacyID.empty() || name.empty() || sector.empty()) {
                    cout << "\n[ERROR] All fields are required\n";
                    pause();
                    break;
                }
                
                // Check if pharmacy already exists
                if (medical->searchPharmacy(pharmacyID) != nullptr) {
                    cout << "\n[ERROR] Pharmacy with ID " << pharmacyID << " already exists\n";
                    pause();
                    break;
                }
                
                // Register pharmacy (adds with sector coordinates automatically)
                if (!medical->registerPharmacy(pharmacyID, name, sector)) {
                    cout << "\n[ERROR] Failed to add pharmacy\n";
                    pause();
                    break;
                }
                
                // Ask for custom coordinates
                cout << "\nEnter coordinates (or press Enter to use sector coordinates):\n";
                string coordStr = readString("Coordinates (format: lat, lon, e.g., 33.708, 73.041): ");
                
                Graph* cityGraph = medical->getCityGraph();
                if (cityGraph != nullptr && !coordStr.empty()) {
                    double lat = 0.0, lon = 0.0;
                    if (parseCoordinates(coordStr, lat, lon)) {
                        // Remove vertex (to update with custom coordinates)
                        if (cityGraph->removeVertex(pharmacyID)) {
                            // Re-add with custom coordinates
                            Pharmacy* pharmacy = medical->searchPharmacy(pharmacyID);
                            if (pharmacy != nullptr) {
                                if (cityGraph->addVertex(pharmacyID, name, lat, lon, pharmacy)) {
                                    cityGraph->connectToNearestStop(pharmacyID);
                                }
                            }
                        }
                    }
                }
                
                cout << "\n[SUCCESS] Pharmacy added successfully!\n";
                cout << "ID: " << pharmacyID << "\n";
                cout << "Name: " << name << "\n";
                cout << "Sector: " << sector << "\n";
                
                // Display connected stops
                if (cityGraph != nullptr) {
                    string connectedStops[10];
                    int stopCount = 0;
                    if (cityGraph->getConnectedStops(pharmacyID, connectedStops, 10, stopCount)) {
                        if (stopCount > 0) {
                            cout << "Connected Stop(s): ";
                            for (int i = 0; i < stopCount; i++) {
                                cout << connectedStops[i];
                                if (i < stopCount - 1) cout << ", ";
                            }
                            cout << "\n";
                        }
                    }
                }
                
                pause();
                break;
            }
            
            case 13: {
                cout << "\n[ADD NEW DOCTOR]\n";
                cout << "----------------------------\n";
                string doctorID = readString("Enter Doctor ID: ");
                string name = readString("Enter Doctor Name: ");
                string specialization = readString("Enter Specialization: ");
                string hospitalID = readString("Enter Associated Hospital ID (optional, press Enter to skip): ");
                
                if (doctorID.empty() || name.empty() || specialization.empty()) {
                    cout << "\n[ERROR] Doctor ID, Name, and Specialization are required\n";
                    pause();
                    break;
                }
                
                // Check if doctor already exists
                if (medical->searchDoctor(doctorID) != nullptr) {
                    cout << "\n[ERROR] Doctor with ID " << doctorID << " already exists\n";
                    pause();
                    break;
                }
                
                // Validate hospital ID if provided
                if (!hospitalID.empty()) {
                    Hospital* hospital = medical->searchHospital(hospitalID);
                    if (hospital == nullptr) {
                        cout << "\n[WARNING] Hospital ID " << hospitalID << " not found. Doctor will be added without hospital association.\n";
                    }
                }
                
                // Register doctor
                if (!medical->registerDoctor(doctorID, name, specialization, nullptr)) {
                    cout << "\n[ERROR] Failed to add doctor\n";
                    pause();
                    break;
                }
                
                cout << "\n[SUCCESS] Doctor added successfully!\n";
                cout << "ID: " << doctorID << "\n";
                cout << "Name: " << name << "\n";
                cout << "Specialization: " << specialization << "\n";
                if (!hospitalID.empty()) {
                    cout << "Associated Hospital: " << hospitalID << "\n";
                }
                
                pause();
                break;
            }
            
            case 14: {
                cout << "\n[ADD MEDICINE TO PHARMACY]\n";
                cout << "----------------------------\n";
                string pharmacyID = readString("Enter Pharmacy ID: ");
                string medicineName = readString("Enter Medicine Name: ");
                string formula = readString("Enter Medicine Formula: ");
                double price = readDouble("Enter Price (PKR): ");
                
                if (pharmacyID.empty() || medicineName.empty() || formula.empty()) {
                    cout << "\n[ERROR] Pharmacy ID, Medicine Name, and Formula are required\n";
                    pause();
                    break;
                }
                
                if (price < 0.0) {
                    cout << "\n[ERROR] Price must be non-negative\n";
                    pause();
                    break;
                }
                
                // Check if pharmacy exists
                if (medical->searchPharmacy(pharmacyID) == nullptr) {
                    cout << "\n[ERROR] Pharmacy with ID " << pharmacyID << " not found\n";
                    pause();
                    break;
                }
                
                // Add medicine to pharmacy
                if (!medical->addMedicineToPharmacy(pharmacyID, medicineName, formula, price)) {
                    cout << "\n[ERROR] Failed to add medicine to pharmacy.\n";
                    cout << "Possible reasons:" << endl;
                    cout << "  - Medicine with this name already exists in the pharmacy" << endl;
                    cout << "  - Invalid medicine data" << endl;
                    pause();
                    break;
                }
                
                cout << "\n[SUCCESS] Medicine added to pharmacy successfully!\n";
                cout << "Pharmacy ID: " << pharmacyID << "\n";
                cout << "Medicine Name: " << medicineName << "\n";
                cout << "Formula: " << formula << "\n";
                cout << "Price: " << price << " PKR\n";
                
                pause();
                break;
            }
            
            case 0:
                return;
            
            default:
                cout << "\n[ERROR] Invalid choice. Please try again.\n";
                pause();
                break;
        }
    } while (choice != 0);
}

// ========================================================================
// TRANSPORT MANAGER MENU HANDLERS
// ========================================================================

void handleTransportMenu() {
    int choice;
    do {
        displayTransportMenu();
        choice = readInt("");
        
        switch (choice) {
            case 1: {
                string busNo = readString("Enter Bus Number: ");
                Bus* bus = transport->findBusByNumber(busNo);
                if (bus != nullptr) {
                    cout << "\n[RESULT]\n";
                    cout << "----------------------------\n";
                    cout << "Bus Found!\n";
                    cout << "----------------------------\n";
                    cout << "Bus Number: " << bus->getBusNo() << "\n";
                    cout << "Company: " << bus->getCompany() << "\n";
                    cout << "Current Stop: " << bus->getCurrentStopID() << "\n";
                } else {
                    cout << "\n[ERROR] Bus not found with number: " << busNo << "\n";
                }
                pause();
                break;
            }
            
            case 2: {
                string stopID = readString("Enter Stop ID: ");
                Bus* buses[20];
                int count = 0;
                transport->findBusesAtStop(stopID, buses, count);
                if (count > 0) {
                    cout << "\n[RESULT]\n";
                    cout << "----------------------------\n";
                    cout << "Found " << count << " bus(es) at " << stopID << ":\n";
                    cout << "----------------------------\n";
                    for (int i = 0; i < count; i++) {
                        cout << (i + 1) << ". " << buses[i]->getBusNo() 
                             << " (" << buses[i]->getCompany() << ")\n";
                    }
                } else {
                    cout << "\n[INFO] No buses found at stop: " << stopID << "\n";
                }
                pause();
                break;
            }
            
            case 3: {
                string fromStop = readString("Enter starting stop ID: ");
                string toStop = readString("Enter destination stop ID: ");
                string* path = new string[50];
                int pathLength = 0;
                double distance = 0.0;
                
                if (transport->findShortestPath(fromStop, toStop, path, pathLength, distance)) {
                    cout << "\n[RESULT]\n";
                    cout << "----------------------------\n";
                    cout << "Shortest Path Found!\n";
                    cout << "----------------------------\n";
                    cout << "From: " << fromStop << "\n";
                    cout << "To: " << toStop << "\n";
                    cout << "Distance: " << distance << " km\n";
                    cout << "Stops: " << pathLength << "\n";
                    cout << "\nRoute: ";
                    for (int i = 0; i < pathLength; i++) {
                        cout << path[i];
                        if (i < pathLength - 1) cout << " -> ";
                    }
                    cout << "\n";
                } else {
                    cout << "\n[ERROR] No path found between " << fromStop << " and " << toStop << "\n";
                }
                delete[] path;
                pause();
                break;
            }
            
            case 4: {
                double lat = readDouble("Enter latitude: ");
                double lon = readDouble("Enter longitude: ");
                Bus* nearest = transport->findNearestBus(lat, lon);
                if (nearest != nullptr) {
                    cout << "\n[RESULT]\n";
                    cout << "----------------------------\n";
                    cout << "Nearest Bus:\n";
                    cout << "----------------------------\n";
                    cout << "Bus Number: " << nearest->getBusNo() << "\n";
                    cout << "Company: " << nearest->getCompany() << "\n";
                    cout << "Current Stop: " << nearest->getCurrentStopID() << "\n";
                } else {
                    cout << "\n[ERROR] No buses found near the specified location\n";
                }
                pause();
                break;
            }
            
            case 5: {
                cout << "\n";
                transport->displayAllBuses();
                pause();
                break;
            }
            
            case 6: {
                cout << "\n";
                transport->displayAllCompanies();
                pause();
                break;
            }
            
            case 7: {
                string busNo = readString("Enter Bus Number: ");
                string* stops = new string[50];
                int count = 0;
                transport->getBusRoute(busNo, stops, count);
                if (count > 0) {
                    cout << "\n[RESULT]\n";
                    cout << "----------------------------\n";
                    cout << "Bus Route for " << busNo << ":\n";
                    cout << "----------------------------\n";
                    for (int i = 0; i < count; i++) {
                        cout << (i + 1) << ". " << stops[i] << "\n";
                    }
                } else {
                    cout << "\n[ERROR] Bus not found or route is empty\n";
                }
                delete[] stops;
                pause();
                break;
            }
            
            case 8: {
                string company = readString("Enter Company Name: ");
                Bus* buses[50];
                int count = 0;
                transport->getBusesByCompany(company, buses, count);
                if (count > 0) {
                    cout << "\n[RESULT]\n";
                    cout << "----------------------------\n";
                    cout << "Found " << count << " bus(es) for " << company << ":\n";
                    cout << "----------------------------\n";
                    for (int i = 0; i < count; i++) {
                        cout << (i + 1) << ". " << buses[i]->getBusNo() << "\n";
                    }
                } else {
                    cout << "\n[INFO] No buses found for company: " << company << "\n";
                }
                pause();
                break;
            }
            
            case 9: {
                string busNo = readString("Enter Bus Number: ");
                string stopID = readString("Enter Stop ID: ");
                if (transport->setBusLocation(busNo, stopID)) {
                    cout << "\n[SUCCESS] Bus location updated successfully\n";
                } else {
                    cout << "\n[ERROR] Failed to update bus location\n";
                }
                pause();
                break;
            }
            
            case 10: {
                string busNo = readString("Enter Bus Number: ");
                string location = transport->getBusLocation(busNo);
                if (!location.empty()) {
                    cout << "\n[RESULT]\n";
                    cout << "----------------------------\n";
                    cout << "Bus " << busNo << " is currently at: " << location << "\n";
                    cout << "----------------------------\n";
                } else {
                    cout << "\n[ERROR] Bus not found\n";
                }
                pause();
                break;
            }
            
            case 11: {
                cout << "\n[ADD NEW BUS]\n";
                cout << "----------------------------\n";
                string busNo = readString("Enter Bus Number: ");
                string company = readString("Enter Company Name: ");
                string currentStop = readString("Enter Current Stop ID: ");
                
                if (busNo.empty() || company.empty() || currentStop.empty()) {
                    cout << "\n[ERROR] Bus Number, Company, and Current Stop are required\n";
                    pause();
                    break;
                }
                
                // Check if bus already exists
                if (transport->findBusByNumber(busNo) != nullptr) {
                    cout << "\n[ERROR] Bus with number " << busNo << " already exists\n";
                    pause();
                    break;
                }
                
                // Validate stop exists in graph
                Graph* cityGraph = transport->getCityGraph();
                if (cityGraph != nullptr) {
                    double dummyLat, dummyLon;
                    if (!cityGraph->getVertexCoordinates(currentStop, dummyLat, dummyLon)) {
                        cout << "\n[ERROR] Stop ID " << currentStop << " not found in city graph\n";
                        pause();
                        break;
                    }
                }
                
                // Create new bus
                Bus* newBus = new Bus(busNo, company, currentStop);
                
                // Add route stops
                cout << "\nEnter route stops (one per line, enter empty line to finish):\n";
                string stopID;
                int stopCount = 0;
                do {
                    stopID = readString("Stop ID (or press Enter to finish): ");
                    if (!stopID.empty()) {
                        // Validate stop exists
                        if (cityGraph != nullptr) {
                            double dummyLat, dummyLon;
                            if (!cityGraph->getVertexCoordinates(stopID, dummyLat, dummyLon)) {
                                cout << "[WARNING] Stop ID " << stopID << " not found, skipping...\n";
                                continue;
                            }
                        }
                        if (newBus->addStopToRoute(stopID)) {
                            stopCount++;
                        } else {
                            cout << "[WARNING] Failed to add stop " << stopID << " to route\n";
                        }
                    }
                } while (!stopID.empty());
                
                // Add bus to transport manager
                if (!transport->addBus(newBus)) {
                    cout << "\n[ERROR] Failed to add bus\n";
                    delete newBus;
                    pause();
                    break;
                }
                
                cout << "\n[SUCCESS] Bus added successfully!\n";
                cout << "Bus Number: " << busNo << "\n";
                cout << "Company: " << company << "\n";
                cout << "Current Stop: " << currentStop << "\n";
                cout << "Route Stops: " << stopCount << " stop(s) added\n";
                
                pause();
                break;
            }
            
            case 12: {
                cout << "\n";
                transport->displayAllSchoolBuses();
                pause();
                break;
            }
            
            case 13: {
                string schoolID = readString("Enter School ID: ");
                string fromStop = readString("Enter starting stop ID: ");
                Bus* nearest = transport->findNearestSchoolBus(schoolID, fromStop);
                if (nearest != nullptr) {
                    cout << "\n[RESULT]\n";
                    cout << "----------------------------\n";
                    cout << "Nearest School Bus:\n";
                    cout << "----------------------------\n";
                    cout << "Bus Number: " << nearest->getBusNo() << "\n";
                    cout << "Company: " << nearest->getCompany() << "\n";
                    cout << "Current Stop: " << nearest->getCurrentStopID() << "\n";
                } else {
                    cout << "\n[ERROR] No school bus found for school " << schoolID << "\n";
                }
                pause();
                break;
            }
            
            case 14: {
                string busNo = readString("Enter Bus Number: ");
                string studentID = readString("Enter Student ID: ");
                
                // Check if bus exists and is a school bus
                Bus* bus = transport->findBusByNumber(busNo);
                if (bus == nullptr) {
                    cout << "\n[ERROR] Bus " << busNo << " not found.\n";
                    pause();
                    break;
                }
                
                // Check if it's a school bus by trying to get it
                SchoolBus* schoolBus = transport->getSchoolBusByNumber(busNo);
                if (schoolBus == nullptr) {
                    cout << "\n[ERROR] Bus " << busNo << " is not a school bus.\n";
                    pause();
                    break;
                }
                
                // Check if student is already on bus
                if (schoolBus->isStudentOnBus(studentID)) {
                    cout << "\n[ERROR] Student " << studentID << " is already on bus " << busNo << ".\n";
                    pause();
                    break;
                }
                
                // Check if bus is full
                if (schoolBus->isFull()) {
                    cout << "\n[ERROR] Bus " << busNo << " is full (capacity: " << schoolBus->getMaxCapacity() << ").\n";
                    pause();
                    break;
                }
                
                // Try to track student
                if (transport->trackStudentOnBus(busNo, studentID)) {
                    // Verify student was actually added
                    if (schoolBus->isStudentOnBus(studentID)) {
                        cout << "\n[SUCCESS] Student " << studentID << " tracked on bus " << busNo << "\n";
                        cout << "Current students on bus: " << schoolBus->getCurrentStudentCount() << " / " << schoolBus->getMaxCapacity() << "\n";
                    } else {
                        cout << "\n[ERROR] Student tracking reported success but student was not added to bus.\n";
                    }
                } else {
                    cout << "\n[ERROR] Failed to track student on bus.\n";
                }
                pause();
                break;
            }
            
            case 15: {
                cout << "\n[ADD NEW SCHOOL BUS]\n";
                cout << "----------------------------\n";
                string busNo = readString("Enter Bus Number: ");
                string company = readString("Enter Company Name: ");
                string currentStop = readString("Enter Current Stop ID: ");
                string schoolID = readString("Enter School ID: ");
                
                if (busNo.empty() || company.empty() || currentStop.empty() || schoolID.empty()) {
                    cout << "\n[ERROR] All fields are required\n";
                    pause();
                    break;
                }
                
                // Check if bus already exists
                if (transport->findBusByNumber(busNo) != nullptr) {
                    cout << "\n[ERROR] Bus with number " << busNo << " already exists\n";
                    pause();
                    break;
                }
                
                // Validate stop exists in graph
                Graph* cityGraph = transport->getCityGraph();
                if (cityGraph != nullptr) {
                    double dummyLat, dummyLon;
                    if (!cityGraph->getVertexCoordinates(currentStop, dummyLat, dummyLon)) {
                        cout << "\n[ERROR] Stop ID " << currentStop << " not found in city graph\n";
                        pause();
                        break;
                    }
                }
                
                // Create new school bus with max capacity 20
                SchoolBus* newSchoolBus = new SchoolBus(busNo, company, currentStop, schoolID, 20);
                
                // Add route stops
                cout << "\nEnter route stops (one per line, enter empty line to finish):\n";
                string stopID;
                int stopCount = 0;
                do {
                    stopID = readString("Stop ID (or press Enter to finish): ");
                    if (!stopID.empty()) {
                        // Validate stop exists
                        if (cityGraph != nullptr) {
                            double dummyLat, dummyLon;
                            if (!cityGraph->getVertexCoordinates(stopID, dummyLat, dummyLon)) {
                                cout << "[WARNING] Stop ID " << stopID << " not found, skipping...\n";
                                continue;
                            }
                        }
                        if (newSchoolBus->addStopToRoute(stopID)) {
                            stopCount++;
                        } else {
                            cout << "[WARNING] Failed to add stop " << stopID << " to route\n";
                        }
                    }
                } while (!stopID.empty());
                
                // Add random students to the bus
                srand(static_cast<unsigned int>(time(nullptr)) + busNo.length());
                int randomStudentCount = rand() % 21; // Random number 0-20
                for (int i = 1; i <= randomStudentCount; i++) {
                    string studentID = schoolID + "_STU" + to_string(i);
                    newSchoolBus->addStudent(studentID);
                }
                
                // Add school bus to transport manager
                if (!transport->addSchoolBus(newSchoolBus)) {
                    cout << "\n[ERROR] Failed to add school bus\n";
                    delete newSchoolBus;
                    pause();
                    break;
                }
                
                cout << "\n[SUCCESS] School bus added successfully!\n";
                cout << "Bus Number: " << busNo << "\n";
                cout << "Company: " << company << "\n";
                cout << "School ID: " << schoolID << "\n";
                cout << "Current Stop: " << currentStop << "\n";
                cout << "Max Capacity: 20\n";
                cout << "Route Stops: " << stopCount << " stop(s) added\n";
                cout << "Students Added: " << randomStudentCount << " (random)\n";
                
                pause();
                break;
            }
            
            case 0:
                return;
            
            default:
                cout << "\n[ERROR] Invalid choice. Please try again.\n";
                pause();
                break;
        }
    } while (choice != 0);
}

// ========================================================================
// MALL MANAGER MENU HANDLERS
// ========================================================================

void handleMallMenu() {
    int choice;
    do {
        displayMallMenu();
        choice = readInt("");
        
        switch (choice) {
            case 1: {
                string mallID = readString("Enter Mall ID: ");
                Mall* mall = malls->findMallByID(mallID);
                if (mall != nullptr) {
                    cout << "\n[RESULT]\n";
                    cout << "----------------------------\n";
                    cout << "Mall Found!\n";
                    cout << "----------------------------\n";
                    cout << "ID: " << mall->getMallID() << "\n";
                    cout << "Name: " << mall->getName() << "\n";
                    cout << "Sector: " << mall->getSector() << "\n";
                    
                    // Get coordinates from graph
                    Graph* cityGraph = malls->getCityGraph();
                    if (cityGraph != nullptr) {
                        double lat, lon;
                        if (cityGraph->getVertexCoordinates(mall->getVertexID(), lat, lon)) {
                            cout << "Coordinates: " << lat << ", " << lon << "\n";
                        }
                        
                        // Get connected stops
                        string connectedStops[10];
                        int stopCount = 0;
                        if (cityGraph->getConnectedStops(mall->getVertexID(), connectedStops, 10, stopCount)) {
                            if (stopCount > 0) {
                                cout << "Connected Stop(s): ";
                                for (int i = 0; i < stopCount; i++) {
                                    cout << connectedStops[i];
                                    if (i < stopCount - 1) cout << ", ";
                                }
                                cout << "\n";
                            } else {
                                cout << "Connected Stop: None (not connected to road network)\n";
                            }
                        }
                    }
                } else {
                    cout << "\n[ERROR] Mall not found with ID: " << mallID << "\n";
                }
                pause();
                break;
            }
            
            case 2: {
                string name = readString("Enter Mall Name: ");
                Mall* mall = malls->findMallByName(name);
                if (mall != nullptr) {
                    cout << "\n[RESULT]\n";
                    cout << "----------------------------\n";
                    cout << "Mall Found!\n";
                    cout << "----------------------------\n";
                    cout << "ID: " << mall->getMallID() << "\n";
                    cout << "Name: " << mall->getName() << "\n";
                    cout << "Sector: " << mall->getSector() << "\n";
                    
                    // Get coordinates from graph
                    Graph* cityGraph = malls->getCityGraph();
                    if (cityGraph != nullptr) {
                        double lat, lon;
                        if (cityGraph->getVertexCoordinates(mall->getVertexID(), lat, lon)) {
                            cout << "Coordinates: " << lat << ", " << lon << "\n";
                        }
                        
                        // Get connected stops
                        string connectedStops[10];
                        int stopCount = 0;
                        if (cityGraph->getConnectedStops(mall->getVertexID(), connectedStops, 10, stopCount)) {
                            if (stopCount > 0) {
                                cout << "Connected Stop(s): ";
                                for (int i = 0; i < stopCount; i++) {
                                    cout << connectedStops[i];
                                    if (i < stopCount - 1) cout << ", ";
                                }
                                cout << "\n";
                            } else {
                                cout << "Connected Stop: None (not connected to road network)\n";
                            }
                        }
                    }
                } else {
                    cout << "\n[ERROR] Mall not found with name: " << name << "\n";
                }
                pause();
                break;
            }
            
            case 3: {
                string productName = readString("Enter Product Name: ");
                Product* product = malls->findProductAnywhere(productName);
                if (product != nullptr) {
                    cout << "\n[RESULT]\n";
                    cout << "----------------------------\n";
                    cout << "Product Found!\n";
                    cout << "----------------------------\n";
                    cout << "Name: " << product->getName() << "\n";
                    cout << "Category: " << product->getCategory() << "\n";
                    cout << "Price: " << product->getPrice() << " PKR\n";
                    
                    // Get and display mall information
                    string mallID = product->getMallID();
                    if (!mallID.empty()) {
                        Mall* mall = malls->findMallByID(mallID);
                        if (mall != nullptr) {
                            cout << "Mall: " << mall->getName() << " (" << mallID << ")\n";
                            cout << "Location: " << mall->getSector() << " - " << mall->getVertexID() << "\n";
                        } else {
                            cout << "Mall ID: " << mallID << " (Mall not found)\n";
                        }
                    }
                } else {
                    cout << "\n[ERROR] Product not found: " << productName << "\n";
                }
                pause();
                break;
            }
            
            case 4: {
                string category = readString("Enter Product Category: ");
                Product* results[100];
                int count = 0;
                malls->findProductsByCategory(category, results, count);
                if (count > 0) {
                    cout << "\n[RESULT]\n";
                    cout << "----------------------------\n";
                    cout << "Found " << count << " product(s) in category " << category << ":\n";
                    cout << "----------------------------\n";
                    for (int i = 0; i < count && i < 20; i++) {
                        cout << (i + 1) << ". " << results[i]->getName() 
                             << " - " << results[i]->getPrice() << " PKR\n";
                    }
                } else {
                    cout << "\n[INFO] No products found in category: " << category << "\n";
                }
                pause();
                break;
            }
            
            case 5: {
                string stopID = readString("Enter Stop ID: ");
                Mall* nearest = malls->findNearestMall(stopID);
                if (nearest != nullptr) {
                    cout << "\n[RESULT]\n";
                    cout << "----------------------------\n";
                    cout << "Nearest Mall:\n";
                    cout << "----------------------------\n";
                    cout << "Name: " << nearest->getName() << "\n";
                    cout << "ID: " << nearest->getMallID() << "\n";
                    cout << "Sector: " << nearest->getSector() << "\n";
                    
                    // Get connected stops
                    Graph* cityGraph = malls->getCityGraph();
                    if (cityGraph != nullptr) {
                        string connectedStops[10];
                        int stopCount = 0;
                        if (cityGraph->getConnectedStops(nearest->getVertexID(), connectedStops, 10, stopCount)) {
                            if (stopCount > 0) {
                                cout << "Connected Stop(s): ";
                                for (int i = 0; i < stopCount; i++) {
                                    cout << connectedStops[i];
                                    if (i < stopCount - 1) cout << ", ";
                                }
                                cout << "\n";
                            }
                        }
                    }
                } else {
                    cout << "\n[ERROR] No malls found near stop: " << stopID << "\n";
                }
                pause();
                break;
            }
            
            case 6: {
                double lat = readDouble("Enter latitude: ");
                double lon = readDouble("Enter longitude: ");
                Mall* nearest = malls->findNearestMall(lat, lon);
                if (nearest != nullptr) {
                    cout << "\n[RESULT]\n";
                    cout << "----------------------------\n";
                    cout << "Nearest Mall:\n";
                    cout << "----------------------------\n";
                    cout << "Name: " << nearest->getName() << "\n";
                    cout << "ID: " << nearest->getMallID() << "\n";
                    cout << "Sector: " << nearest->getSector() << "\n";
                    
                    // Get coordinates and connected stops
                    Graph* cityGraph = malls->getCityGraph();
                    if (cityGraph != nullptr) {
                        double mallLat, mallLon;
                        if (cityGraph->getVertexCoordinates(nearest->getVertexID(), mallLat, mallLon)) {
                            cout << "Coordinates: " << mallLat << ", " << mallLon << "\n";
                        }
                        
                        string connectedStops[10];
                        int stopCount = 0;
                        if (cityGraph->getConnectedStops(nearest->getVertexID(), connectedStops, 10, stopCount)) {
                            if (stopCount > 0) {
                                cout << "Connected Stop(s): ";
                                for (int i = 0; i < stopCount; i++) {
                                    cout << connectedStops[i];
                                    if (i < stopCount - 1) cout << ", ";
                                }
                                cout << "\n";
                            }
                        }
                    }
                } else {
                    cout << "\n[ERROR] No malls found near the specified location\n";
                }
                pause();
                break;
            }
            
            case 7: {
                cout << "\n";
                if (malls->getMallCount() == 0) {
                    cout << "No malls registered.\n";
                } else {
                    cout << "Registered Malls (" << malls->getMallCount() << " malls):\n";
                    cout << "----------------------------\n";
                    
                    // Get all malls and display with connected stops
                    Mall** allMalls = new Mall*[malls->getMallCount()];
                    int count = 0;
                    // We need to get all malls - let's use a workaround
                    // Since we don't have direct access, we'll create a custom display
                    
                    // For now, use the existing display but add connected stops info
                    malls->displayAllMalls();
                    
                    // Note: To show connected stops for each mall in displayAllMalls,
                    // we would need to modify MallManager::displayAllMalls to accept graph
                    // or create a custom display here that iterates through malls
                    // For simplicity, connected stops are shown in individual search results
                }
                pause();
                break;
            }
            
            case 8: {
                string mallID = readString("Enter Mall ID: ");
                Mall* mall = malls->findMallByID(mallID);
                if (mall != nullptr) {
                    cout << "\n";
                    mall->displayProducts();
                } else {
                    cout << "\n[ERROR] Mall not found\n";
                }
                pause();
                break;
            }
            
            case 9: {
                cout << "\n[ADD NEW MALL]\n";
                cout << "----------------------------\n";
                string mallID = readString("Enter Mall ID: ");
                string name = readString("Enter Mall Name: ");
                string sector = readString("Enter Sector: ");
                string coordStr = readString("Enter Coordinates (format: lat, lon, e.g., 33.708, 73.041): ");
                
                if (mallID.empty() || name.empty() || sector.empty() || coordStr.empty()) {
                    cout << "\n[ERROR] All fields are required\n";
                    pause();
                    break;
                }
                
                // Check if mall already exists
                if (malls->findMallByID(mallID) != nullptr) {
                    cout << "\n[ERROR] Mall with ID " << mallID << " already exists\n";
                    pause();
                    break;
                }
                
                // Parse coordinates
                double lat = 0.0, lon = 0.0;
                if (!parseCoordinates(coordStr, lat, lon)) {
                    cout << "\n[ERROR] Invalid coordinate format. Use: lat, lon (e.g., 33.708, 73.041)\n";
                    pause();
                    break;
                }
                
                // Create new mall (use mallID as vertexID)
                Mall* newMall = new Mall(mallID, name, sector, mallID);
                
                // Add to manager first
                if (!malls->addMall(newMall)) {
                    cout << "\n[ERROR] Failed to add mall\n";
                    delete newMall;
                    pause();
                    break;
                }
                
                // Add mall as a vertex to the graph with coordinates
                Graph* cityGraph = malls->getCityGraph();
                if (cityGraph != nullptr) {
                    cityGraph->addVertex(mallID, name, lat, lon, newMall);
                    cityGraph->connectToNearestStop(mallID);
                    cout << "\n[SUCCESS] Mall added successfully!\n";
                    cout << "ID: " << mallID << "\n";
                    cout << "Name: " << name << "\n";
                    cout << "Sector: " << sector << "\n";
                    cout << "Coordinates: " << lat << ", " << lon << "\n";
                    cout << "Connected to nearest bus stop for pathfinding\n";
                } else {
                    cout << "\n[SUCCESS] Mall added (but graph unavailable for coordinates)\n";
                    cout << "ID: " << mallID << "\n";
                    cout << "Name: " << name << "\n";
                    cout << "Sector: " << sector << "\n";
                }
                pause();
                break;
            }
            
            case 10: {
                cout << "\n[ADD PRODUCT TO MALL]\n";
                cout << "----------------------------\n";
                string mallID = readString("Enter Mall ID: ");
                Mall* mall = malls->findMallByID(mallID);
                if (mall == nullptr) {
                    cout << "\n[ERROR] Mall not found\n";
                    pause();
                    break;
                }
                
                string productID = readString("Enter Product ID: ");
                string productName = readString("Enter Product Name: ");
                string category = readString("Enter Product Category: ");
                double price = readDouble("Enter Product Price: ");
                
                if (productID.empty() || productName.empty() || category.empty() || price < 0) {
                    cout << "\n[ERROR] All fields are required and price must be >= 0\n";
                    pause();
                    break;
                }
                
                // Check if product already exists in this mall
                if (mall->findProduct(productName) != nullptr) {
                    cout << "\n[ERROR] Product " << productName << " already exists in this mall\n";
                    pause();
                    break;
                }
                
                // Create and add product
                Product* newProduct = new Product(productID, productName, category, price, mallID);
                if (malls->addProductToMall(mallID, newProduct)) {
                    cout << "\n[SUCCESS] Product added successfully!\n";
                    cout << "Product ID: " << productID << "\n";
                    cout << "Name: " << productName << "\n";
                    cout << "Category: " << category << "\n";
                    cout << "Price: " << price << " PKR\n";
                    cout << "Mall: " << mall->getName() << "\n";
                } else {
                    cout << "\n[ERROR] Failed to add product\n";
                    delete newProduct;
                }
                pause();
                break;
            }
            
            case 0:
                return;
            
            default:
                cout << "\n[ERROR] Invalid choice. Please try again.\n";
                pause();
                break;
        }
    } while (choice != 0);
}

// ========================================================================
// FACILITY MANAGER MENU HANDLERS
// ========================================================================

void handleFacilityMenu() {
    int choice;
    do {
        displayFacilityMenu();
        choice = readInt("");
        
        switch (choice) {
            case 1: {
                string facilityID = readString("Enter Facility ID: ");
                Facility* facility = facilities->findFacilityByID(facilityID);
                if (facility != nullptr) {
                    cout << "\n[RESULT]\n";
                    cout << "----------------------------\n";
                    cout << "Facility Found!\n";
                    cout << "----------------------------\n";
                    cout << "ID: " << facility->getFacilityID() << "\n";
                    cout << "Name: " << facility->getName() << "\n";
                    cout << "Type: " << facility->getType() << "\n";
                    cout << "Sector: " << facility->getSector() << "\n";
                    
                    // Get coordinates from graph
                    Graph* cityGraph = facilities->getCityGraph();
                    if (cityGraph != nullptr) {
                        double lat, lon;
                        if (cityGraph->getVertexCoordinates(facility->getVertexID(), lat, lon)) {
                            cout << "Coordinates: " << lat << ", " << lon << "\n";
                        }
                        
                        // Get connected stops
                        string connectedStops[10];
                        int stopCount = 0;
                        if (cityGraph->getConnectedStops(facility->getVertexID(), connectedStops, 10, stopCount)) {
                            if (stopCount > 0) {
                                cout << "Connected Stop(s): ";
                                for (int i = 0; i < stopCount; i++) {
                                    cout << connectedStops[i];
                                    if (i < stopCount - 1) cout << ", ";
                                }
                                cout << "\n";
                            } else {
                                cout << "Connected Stop: None (not connected to road network)\n";
                            }
                        }
                    }
                } else {
                    cout << "\n[ERROR] Facility not found with ID: " << facilityID << "\n";
                }
                pause();
                break;
            }
            
            case 2: {
                string name = readString("Enter Facility Name: ");
                Facility* facility = facilities->findFacilityByName(name);
                if (facility != nullptr) {
                    cout << "\n[RESULT]\n";
                    cout << "----------------------------\n";
                    cout << "Facility Found!\n";
                    cout << "----------------------------\n";
                    cout << "ID: " << facility->getFacilityID() << "\n";
                    cout << "Name: " << facility->getName() << "\n";
                    cout << "Type: " << facility->getType() << "\n";
                    cout << "Sector: " << facility->getSector() << "\n";
                    
                    // Get coordinates from graph
                    Graph* cityGraph = facilities->getCityGraph();
                    if (cityGraph != nullptr) {
                        double lat, lon;
                        if (cityGraph->getVertexCoordinates(facility->getVertexID(), lat, lon)) {
                            cout << "Coordinates: " << lat << ", " << lon << "\n";
                        }
                        
                        // Get connected stops
                        string connectedStops[10];
                        int stopCount = 0;
                        if (cityGraph->getConnectedStops(facility->getVertexID(), connectedStops, 10, stopCount)) {
                            if (stopCount > 0) {
                                cout << "Connected Stop(s): ";
                                for (int i = 0; i < stopCount; i++) {
                                    cout << connectedStops[i];
                                    if (i < stopCount - 1) cout << ", ";
                                }
                                cout << "\n";
                            } else {
                                cout << "Connected Stop: None (not connected to road network)\n";
                            }
                        }
                    }
                } else {
                    cout << "\n[ERROR] Facility not found with name: " << name << "\n";
                }
                pause();
                break;
            }
            
            case 3: {
                string type = readString("Enter Facility Type: ");
                Facility* results[50];
                int count = 0;
                facilities->findFacilitiesByType(type, results, count);
                if (count > 0) {
                    cout << "\n[RESULT]\n";
                    cout << "----------------------------\n";
                    cout << "Found " << count << " facility(ies) of type " << type << ":\n";
                    cout << "----------------------------\n";
                    
                    Graph* cityGraph = facilities->getCityGraph();
                    for (int i = 0; i < count; i++) {
                        cout << "\n" << (i + 1) << ". ";
                        cout << "ID: " << results[i]->getFacilityID() << "\n";
                        cout << "   Name: " << results[i]->getName() << "\n";
                        cout << "   Type: " << results[i]->getType() << "\n";
                        cout << "   Sector: " << results[i]->getSector() << "\n";
                        
                        // Get connected stops
                        if (cityGraph != nullptr) {
                            string connectedStops[10];
                            int stopCount = 0;
                            if (cityGraph->getConnectedStops(results[i]->getVertexID(), connectedStops, 10, stopCount)) {
                                if (stopCount > 0) {
                                    cout << "   Connected Stop(s): ";
                                    for (int j = 0; j < stopCount; j++) {
                                        cout << connectedStops[j];
                                        if (j < stopCount - 1) cout << ", ";
                                    }
                                    cout << "\n";
                                }
                            }
                        }
                    }
                } else {
                    cout << "\n[INFO] No facilities found of type: " << type << "\n";
                }
                pause();
                break;
            }
            
            case 4: {
                double lat = readDouble("Enter latitude: ");
                double lon = readDouble("Enter longitude: ");
                Facility* nearest = facilities->findNearestFacility(lat, lon);
                if (nearest != nullptr) {
                    cout << "\n[RESULT]\n";
                    cout << "----------------------------\n";
                    cout << "Nearest Facility:\n";
                    cout << "----------------------------\n";
                    cout << "ID: " << nearest->getFacilityID() << "\n";
                    cout << "Name: " << nearest->getName() << "\n";
                    cout << "Type: " << nearest->getType() << "\n";
                    cout << "Sector: " << nearest->getSector() << "\n";
                    
                    // Get coordinates and connected stops
                    Graph* cityGraph = facilities->getCityGraph();
                    if (cityGraph != nullptr) {
                        double facilityLat, facilityLon;
                        if (cityGraph->getVertexCoordinates(nearest->getVertexID(), facilityLat, facilityLon)) {
                            cout << "Coordinates: " << facilityLat << ", " << facilityLon << "\n";
                        }
                        
                        string connectedStops[10];
                        int stopCount = 0;
                        if (cityGraph->getConnectedStops(nearest->getVertexID(), connectedStops, 10, stopCount)) {
                            if (stopCount > 0) {
                                cout << "Connected Stop(s): ";
                                for (int i = 0; i < stopCount; i++) {
                                    cout << connectedStops[i];
                                    if (i < stopCount - 1) cout << ", ";
                                }
                                cout << "\n";
                            }
                        }
                    }
                } else {
                    cout << "\n[ERROR] No facilities found near the specified location\n";
                }
                pause();
                break;
            }
            
            case 5: {
                cout << "\n";
                facilities->displayAllFacilities();
                pause();
                break;
            }
            
            case 6: {
                cout << "\n[ADD NEW FACILITY]\n";
                cout << "----------------------------\n";
                string facilityID = readString("Enter Facility ID: ");
                string name = readString("Enter Facility Name: ");
                string type = readString("Enter Facility Type (e.g., Mosque, Park, Water Cooler): ");
                string sector = readString("Enter Sector: ");
                string coordStr = readString("Enter Coordinates (format: lat, lon, e.g., 33.708, 73.041): ");
                
                if (facilityID.empty() || name.empty() || type.empty() || sector.empty() || coordStr.empty()) {
                    cout << "\n[ERROR] All fields are required\n";
                    pause();
                    break;
                }
                
                // Check if facility already exists
                if (facilities->findFacilityByID(facilityID) != nullptr) {
                    cout << "\n[ERROR] Facility with ID " << facilityID << " already exists\n";
                    pause();
                    break;
                }
                
                // Parse coordinates
                double lat = 0.0, lon = 0.0;
                if (!parseCoordinates(coordStr, lat, lon)) {
                    cout << "\n[ERROR] Invalid coordinate format. Use: lat, lon (e.g., 33.708, 73.041)\n";
                    pause();
                    break;
                }
                
                // Create new facility (use facilityID as vertexID)
                Facility* newFacility = new Facility(facilityID, name, type, sector, facilityID);
                
                // Add to manager first
                if (!facilities->addFacility(newFacility)) {
                    cout << "\n[ERROR] Failed to add facility\n";
                    delete newFacility;
                    pause();
                    break;
                }
                
                // Add facility as a vertex to the graph with coordinates
                Graph* cityGraph = facilities->getCityGraph();
                if (cityGraph != nullptr) {
                    cityGraph->addVertex(facilityID, name, lat, lon, newFacility);
                    cityGraph->connectToNearestStop(facilityID);
                    cout << "\n[SUCCESS] Facility added successfully!\n";
                    cout << "ID: " << facilityID << "\n";
                    cout << "Name: " << name << "\n";
                    cout << "Type: " << type << "\n";
                    cout << "Sector: " << sector << "\n";
                    cout << "Coordinates: " << lat << ", " << lon << "\n";
                    cout << "Connected to nearest bus stop for pathfinding\n";
                } else {
                    cout << "\n[SUCCESS] Facility added (but graph unavailable for coordinates)\n";
                    cout << "ID: " << facilityID << "\n";
                    cout << "Name: " << name << "\n";
                    cout << "Type: " << type << "\n";
                    cout << "Sector: " << sector << "\n";
                }
                pause();
                break;
            }
            
            case 0:
                return;
            
            default:
                cout << "\n[ERROR] Invalid choice. Please try again.\n";
                pause();
                break;
        }
    } while (choice != 0);
}

// ========================================================================
// AIRPORT MANAGER MENU HANDLERS
// ========================================================================

void handleAirportMenu() {
    int choice;
    do {
        displayAirportMenu();
        choice = readInt("");
        
        switch (choice) {
            case 1: {
                string airportID = readString("Enter Airport ID: ");
                Airport* airport = airports->findAirportByID(airportID);
                if (airport != nullptr) {
                    cout << "\n[RESULT]\n";
                    cout << "----------------------------\n";
                    cout << "Airport Found!\n";
                    cout << "----------------------------\n";
                    airport->display();
                    
                    // Get connected stops
                    Graph* cityGraph = airports->getCityGraph();
                    if (cityGraph != nullptr) {
                        string connectedStops[10];
                        int stopCount = 0;
                        if (cityGraph->getConnectedStops(airport->getVertexID(), connectedStops, 10, stopCount)) {
                            if (stopCount > 0) {
                                cout << "Connected Stop(s): ";
                                for (int i = 0; i < stopCount; i++) {
                                    cout << connectedStops[i];
                                    if (i < stopCount - 1) cout << ", ";
                                }
                                cout << "\n";
                            }
                        }
                    }
                } else {
                    cout << "\n[ERROR] Airport not found with ID: " << airportID << "\n";
                }
                pause();
                break;
            }
            
            case 2: {
                string name = readString("Enter Airport Name: ");
                Airport* airport = airports->findAirportByName(name);
                if (airport != nullptr) {
                    cout << "\n[RESULT]\n";
                    cout << "----------------------------\n";
                    cout << "Airport Found!\n";
                    cout << "----------------------------\n";
                    airport->display();
                    
                    // Get connected stops
                    Graph* cityGraph = airports->getCityGraph();
                    if (cityGraph != nullptr) {
                        string connectedStops[10];
                        int stopCount = 0;
                        if (cityGraph->getConnectedStops(airport->getVertexID(), connectedStops, 10, stopCount)) {
                            if (stopCount > 0) {
                                cout << "Connected Stop(s): ";
                                for (int i = 0; i < stopCount; i++) {
                                    cout << connectedStops[i];
                                    if (i < stopCount - 1) cout << ", ";
                                }
                                cout << "\n";
                            }
                        }
                    }
                } else {
                    cout << "\n[ERROR] Airport not found with name: " << name << "\n";
                }
                pause();
                break;
            }
            
            case 3: {
                string code = readString("Enter IATA Code: ");
                Airport* airport = airports->findAirportByCode(code);
                if (airport != nullptr) {
                    cout << "\n[RESULT]\n";
                    cout << "----------------------------\n";
                    cout << "Airport Found!\n";
                    cout << "----------------------------\n";
                    airport->display();
                    
                    // Get connected stops
                    Graph* cityGraph = airports->getCityGraph();
                    if (cityGraph != nullptr) {
                        string connectedStops[10];
                        int stopCount = 0;
                        if (cityGraph->getConnectedStops(airport->getVertexID(), connectedStops, 10, stopCount)) {
                            if (stopCount > 0) {
                                cout << "Connected Stop(s): ";
                                for (int i = 0; i < stopCount; i++) {
                                    cout << connectedStops[i];
                                    if (i < stopCount - 1) cout << ", ";
                                }
                                cout << "\n";
                            }
                        }
                    }
                } else {
                    cout << "\n[ERROR] Airport not found with IATA code: " << code << "\n";
                }
                pause();
                break;
            }
            
            case 4: {
                double lat = readDouble("Enter latitude: ");
                double lon = readDouble("Enter longitude: ");
                Airport* nearest = airports->findNearestAirport(lat, lon);
                if (nearest != nullptr) {
                    cout << "\n[RESULT]\n";
                    cout << "----------------------------\n";
                    cout << "Nearest Airport:\n";
                    cout << "----------------------------\n";
                    nearest->display();
                    
                    // Calculate distance
                    double distance = calculateDistance(lat, lon, nearest->getLatitude(), nearest->getLongitude());
                    cout << "Distance: " << distance << " km\n";
                    
                    // Get connected stops
                    Graph* cityGraph = airports->getCityGraph();
                    if (cityGraph != nullptr) {
                        string connectedStops[10];
                        int stopCount = 0;
                        if (cityGraph->getConnectedStops(nearest->getVertexID(), connectedStops, 10, stopCount)) {
                            if (stopCount > 0) {
                                cout << "Connected Stop(s): ";
                                for (int i = 0; i < stopCount; i++) {
                                    cout << connectedStops[i];
                                    if (i < stopCount - 1) cout << ", ";
                                }
                                cout << "\n";
                            }
                        }
                    }
                } else {
                    cout << "\n[ERROR] No airports found\n";
                }
                pause();
                break;
            }
            
            case 5: {
                string fromID = readString("Enter Source Airport ID: ");
                string toID = readString("Enter Destination Airport ID: ");
                double distance = 0.0;
                if (airports->calculateAirDistance(fromID, toID, distance)) {
                    cout << "\n[RESULT]\n";
                    cout << "----------------------------\n";
                    cout << "Air Distance Calculation:\n";
                    cout << "----------------------------\n";
                    Airport* from = airports->findAirportByID(fromID);
                    Airport* to = airports->findAirportByID(toID);
                    if (from != nullptr && to != nullptr) {
                        cout << "From: " << from->getName() << " (" << from->getCode() << ")\n";
                        cout << "To: " << to->getName() << " (" << to->getCode() << ")\n";
                        cout << "Air Distance: " << distance << " km\n";
                    }
                } else {
                    cout << "\n[ERROR] Could not calculate distance. Make sure both airports exist.\n";
                }
                pause();
                break;
            }
            
            case 6: {
                cout << "\n";
                airports->displayAllAirports();
                pause();
                break;
            }
            
            case 7: {
                cout << "\n[ADD NEW AIRPORT]\n";
                cout << "----------------------------\n";
                string airportID = readString("Enter Airport ID: ");
                string name = readString("Enter Airport Name: ");
                string code = readString("Enter IATA Code: ");
                string city = readString("Enter City: ");
                string coordStr = readString("Enter Coordinates (format: lat, lon, e.g., 33.6167, 73.0992): ");
                
                if (airportID.empty() || name.empty() || code.empty() || city.empty() || coordStr.empty()) {
                    cout << "\n[ERROR] All fields are required\n";
                    pause();
                    break;
                }
                
                // Check if airport already exists
                if (airports->findAirportByID(airportID) != nullptr) {
                    cout << "\n[ERROR] Airport with ID " << airportID << " already exists\n";
                    pause();
                    break;
                }
                
                // Parse coordinates
                double lat = 0.0, lon = 0.0;
                if (!parseCoordinates(coordStr, lat, lon)) {
                    cout << "\n[ERROR] Invalid coordinate format. Use: lat, lon (e.g., 33.6167, 73.0992)\n";
                    pause();
                    break;
                }
                
                // Create new airport (use airportID as vertexID)
                Airport* newAirport = new Airport(airportID, name, code, city, airportID, lat, lon);
                
                // Add to manager (this will automatically connect to nearest bus stop and other airports)
                if (airports->addAirport(newAirport)) {
                    cout << "\n[SUCCESS] Airport added successfully!\n";
                    cout << "ID: " << airportID << "\n";
                    cout << "Name: " << name << "\n";
                    cout << "IATA Code: " << code << "\n";
                    cout << "City: " << city << "\n";
                    cout << "Coordinates: " << lat << ", " << lon << "\n";
                    cout << "Connected to nearest bus stop for pathfinding\n";
                    cout << "Connected to all other airports for direct flights\n";
                } else {
                    cout << "\n[ERROR] Failed to add airport\n";
                    delete newAirport;
                }
                pause();
                break;
            }
            
            case 0:
                return;
            
            default:
                cout << "\n[ERROR] Invalid choice. Please try again.\n";
                pause();
                break;
        }
    } while (choice != 0);
}

// ========================================================================
// POPULATION/HOUSING MENU HANDLERS
// ========================================================================

void handlePopulationMenu() {
    int choice;
    do {
        displayPopulationMenu();
        choice = readInt("");
        
        switch (choice) {
            case 1: {
                string cnic = readString("Enter Citizen CNIC: ");
                citizeninfo* citizen = population->searchCitizen(cnic);
                if (citizen != nullptr) {
                    cout << "\n[RESULT]\n";
                    cout << "----------------------------\n";
                    cout << "Citizen Found!\n";
                    cout << "----------------------------\n";
                    population->displayCitizenInfo(cnic);
                } else {
                    cout << "\n[ERROR] Citizen not found with CNIC: " << cnic << "\n";
                }
                pause();
                break;
            }
            
            case 2: {
                string cnic = readString("Enter Citizen CNIC: ");
                population->displayCitizenInfo(cnic);
                pause();
                break;
            }
            
            case 3: {
                cout << "\n[ADD SECTOR]\n";
                cout << "----------------------------\n";
                string sectorName = readString("Enter Sector Name (e.g., G-10, F-8): ");
                if (sectorName.empty()) {
                    cout << "\n[ERROR] Sector name cannot be empty\n";
                    pause();
                    break;
                }
                population->addSector(sectorName);
                cout << "\n[SUCCESS] Sector " << sectorName << " added successfully!\n";
                pause();
                break;
            }
            
            case 4: {
                cout << "\n[ADD STREET]\n";
                cout << "----------------------------\n";
                string sectorName = readString("Enter Sector Name: ");
                int streetNum = readInt("Enter Street Number: ");
                if (sectorName.empty() || streetNum <= 0) {
                    cout << "\n[ERROR] Invalid input. Sector name and street number are required\n";
                    pause();
                    break;
                }
                population->addStreet(sectorName, streetNum);
                cout << "\n[SUCCESS] Street " << streetNum << " added to sector " << sectorName << "!\n";
                pause();
                break;
            }
            
            case 5: {
                cout << "\n[ADD HOUSE]\n";
                cout << "----------------------------\n";
                string sectorName = readString("Enter Sector Name: ");
                int streetNum = readInt("Enter Street Number: ");
                int houseNum = readInt("Enter House Number: ");
                if (sectorName.empty() || streetNum <= 0 || houseNum <= 0) {
                    cout << "\n[ERROR] Invalid input. All fields are required\n";
                    pause();
                    break;
                }
                population->addHouse(sectorName, streetNum, houseNum);
                cout << "\n[SUCCESS] House " << houseNum << " added to Street " << streetNum 
                     << ", Sector " << sectorName << "!\n";
                pause();
                break;
            }
            
            case 6: {
                cout << "\n[ADD FAMILY (FAMILY HEAD)]\n";
                cout << "----------------------------\n";
                string sectorName = readString("Enter Sector Name: ");
                int streetNum = readInt("Enter Street Number: ");
                int houseNum = readInt("Enter House Number: ");
                
                if (sectorName.empty() || streetNum <= 0 || houseNum <= 0) {
                    cout << "\n[ERROR] Invalid location. All fields are required\n";
                    pause();
                    break;
                }
                
                string cnic = readString("Enter CNIC: ");
                string name = readString("Enter Name: ");
                int age = readInt("Enter Age: ");
                string gender = readString("Enter Gender (Male/Female/Other): ");
                string occupation = readString("Enter Occupation: ");
                
                if (cnic.empty() || name.empty() || age <= 0 || gender.empty() || occupation.empty()) {
                    cout << "\n[ERROR] All citizen information is required\n";
                    pause();
                    break;
                }
                
                // Check if citizen already exists
                if (population->searchCitizen(cnic) != nullptr) {
                    cout << "\n[ERROR] Citizen with CNIC " << cnic << " already exists\n";
                    pause();
                    break;
                }
                
                citizeninfo* headOfFamily = new citizeninfo(cnic, name, age, gender, occupation);
                population->addFamily(sectorName, streetNum, houseNum, headOfFamily);
                
                cout << "\n[SUCCESS] Family added successfully!\n";
                cout << "Family Head: " << name << " (CNIC: " << cnic << ")\n";
                cout << "Location: Sector " << sectorName << ", Street " << streetNum 
                     << ", House " << houseNum << "\n";
                pause();
                break;
            }
            
            case 7: {
                cout << "\n[ADD FAMILY MEMBER]\n";
                cout << "----------------------------\n";
                string parentCNIC = readString("Enter Parent/Family Head CNIC: ");
                
                // Verify parent exists
                citizeninfo* parent = population->searchCitizen(parentCNIC);
                if (parent == nullptr) {
                    cout << "\n[ERROR] Parent with CNIC " << parentCNIC << " not found\n";
                    cout << "Please add the family head first using option 6\n";
                    pause();
                    break;
                }
                
                cout << "Parent Found: " << parent->name << "\n";
                cout << "\nEnter Family Member Details:\n";
                
                string memberCNIC = readString("Enter Member CNIC: ");
                string name = readString("Enter Name: ");
                int age = readInt("Enter Age: ");
                string gender = readString("Enter Gender (Male/Female/Other): ");
                string occupation = readString("Enter Occupation: ");
                
                if (memberCNIC.empty() || name.empty() || age <= 0 || gender.empty() || occupation.empty()) {
                    cout << "\n[ERROR] All member information is required\n";
                    pause();
                    break;
                }
                
                // Check if member already exists
                if (population->searchCitizen(memberCNIC) != nullptr) {
                    cout << "\n[ERROR] Citizen with CNIC " << memberCNIC << " already exists\n";
                    pause();
                    break;
                }
                
                citizeninfo* member = new citizeninfo(memberCNIC, name, age, gender, occupation);
                population->addFamilyMember(parentCNIC, member);
                
                cout << "\n[SUCCESS] Family member added successfully!\n";
                cout << "Member: " << name << " (CNIC: " << memberCNIC << ")\n";
                cout << "Added to family of: " << parent->name << "\n";
                pause();
                break;
            }
            
            case 8: {
                cout << "\n";
                cout << "[REPORT] Age Distribution:\n";
                cout << "----------------------------\n";
                population->generateAgeDistribution();
                pause();
                break;
            }
            
            case 9: {
                cout << "\n";
                cout << "[REPORT] Gender Ratio:\n";
                cout << "----------------------------\n";
                population->generateGenderRatio();
                pause();
                break;
            }
            
            case 10: {
                cout << "\n";
                cout << "[REPORT] Occupation Breakdown:\n";
                cout << "----------------------------\n";
                population->generateOccupationBreakdown();
                pause();
                break;
            }
            
            case 11: {
                cout << "\n";
                cout << "[REPORT] Population Density:\n";
                cout << "----------------------------\n";
                population->generatePopulationDensity();
                pause();
                break;
            }
            
            case 12: {
                cout << "\n";
                cout << "[REPORT] Total Population:\n";
                cout << "----------------------------\n";
                int total = population->getTotalPopulation();
                cout << "Total Population: " << total << " people\n";
                pause();
                break;
            }
            
            case 13: {
                cout << "\n";
                population->displayColorHeatmap();
                pause();
                break;
            }
            
            case 14: {
                cout << "\n";
                population->displaySectorHeatmap();
                pause();
                break;
            }
            
            case 15: {
                cout << "\n";
                population->generatePopulationHeatmap();
                pause();
                break;
            }
            
            case 16: {
                cout << "\n[EXPORT HEATMAP]\n";
                cout << "----------------------------\n";
                string filename = readString("Enter filename (e.g., heatmap.csv): ");
                if (!filename.empty()) {
                    population->exportHeatmapToFile(filename);
                } else {
                    cout << "\n[ERROR] Filename cannot be empty\n";
                }
                pause();
                break;
            }
            
            case 0:
                return;
            
            default:
                cout << "\n[ERROR] Invalid choice. Please try again.\n";
                pause();
                break;
        }
    } while (choice != 0);
}

// ========================================================================
// CROSS-MODULE QUERIES MENU HANDLERS
// ========================================================================

void handleCrossModuleMenu() {
    int choice;
    do {
        displayCrossModuleMenu();
        choice = readInt("");
        
        switch (choice) {
            case 1: {
                double lat = readDouble("Enter latitude (-90 to 90): ");
                double lon = readDouble("Enter longitude (-180 to 180): ");
                
                // Validate coordinate ranges
                if (lat < -90.0 || lat > 90.0) {
                    cout << "\n[ERROR] Invalid latitude: " << lat << "\n";
                    cout << "Latitude must be between -90 and 90 degrees.\n";
                    cout << "Please enter a valid latitude value.\n";
                    pause();
                    break;
                }
                
                if (lon < -180.0 || lon > 180.0) {
                    cout << "\n[ERROR] Invalid longitude: " << lon << "\n";
                    cout << "Longitude must be between -180 and 180 degrees.\n";
                    cout << "Please enter a valid longitude value.\n";
                    pause();
                    break;
                }
                
                cout << "\n[SEARCHING NEAREST SERVICES USING DIJKSTRA'S ALGORITHM...]\n";
                
                if (sharedCityGraph == nullptr) {
                    cout << "\n[ERROR] City graph not initialized.\n";
                    cout << "Please load data first using the Data Management menu.\n";
                    pause();
                    break;
                }
                
                // Step 1: Find nearest bus stop to user's coordinates (using Euclidean distance)
                string nearestStopID = sharedCityGraph->findNearestStop(lat, lon);
                if (nearestStopID.empty()) {
                    cout << "\n[ERROR] No bus stops found in the graph.\n";
                    pause();
                    break;
                }
                
                cout << "\n[RESULT]\n";
                cout << "================================================================\n";
                cout << "Nearest Services from Your Location (" << lat << ", " << lon << ")\n";
                cout << "Nearest Bus Stop: " << nearestStopID << "\n";
                cout << "================================================================\n";
                cout << "Note: Distances calculated using Dijkstra's Algorithm (road network)\n";
                cout << "================================================================\n";
                
                // ========== FIND NEAREST SCHOOL (using Dijkstra) ==========
                {
                    School* nearestSchool = nullptr;
                    double minRoadDistance = -1.0;
                    string nearestSchoolID = "";
                    int bestPathLength = 0;
                    string* bestPath = nullptr;
                    
                    // Get all schools by checking school count and searching by common ID pattern
                    // Since we don't have direct access to schoolIDList, we'll iterate through possible IDs
                    int schoolCount = education->getSchoolCount();
                    if (schoolCount > 0) {
                        // Try common school ID patterns (S01, S02, etc.)
                        for (int i = 1; i <= 100 && i <= schoolCount * 3; i++) {
                            string schoolID = "S";
                            if (i < 10) schoolID += "0";
                            schoolID += to_string(i);
                            
                            School* school = education->searchSchool(schoolID);
                            if (school == nullptr) continue;
                            
                            // Calculate complete distance: source coords -> source stop -> dest stop -> dest coords
                            double completeDistance = 0.0;
                            string tempSourceStop, tempDestStop;
                            if (calculateCompleteDistanceToVertex(lat, lon, schoolID, sharedCityGraph, tempSourceStop, completeDistance)) {
                                if (minRoadDistance < 0.0 || completeDistance < minRoadDistance) {
                                    minRoadDistance = completeDistance;
                                    nearestSchool = school;
                                    nearestSchoolID = schoolID;
                                    
                                    // Get path for display (from source stop to destination stop)
                                    int pathLength = 0;
                                    double edgeDistance = 0.0;
                                    string destLat, destLon;
                                    double schoolLat, schoolLon;
                                    if (sharedCityGraph->getVertexCoordinates(schoolID, schoolLat, schoolLon)) {
                                        string destStopID = sharedCityGraph->findNearestStop(schoolLat, schoolLon);
                                        if (!destStopID.empty()) {
                                            string* path = sharedCityGraph->findShortestPath(tempSourceStop, destStopID, pathLength, edgeDistance);
                                            if (path != nullptr) {
                                                bestPathLength = pathLength;
                                                if (bestPath != nullptr) delete[] bestPath;
                                                bestPath = new string[pathLength];
                                                for (int j = 0; j < pathLength; j++) {
                                                    bestPath[j] = path[j];
                                                }
                                                delete[] path;
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                    
                    if (nearestSchool != nullptr) {
                        cout << "\n[NEAREST SCHOOL] (by road network)\n";
                        cout << "  Name: " << nearestSchool->getName() << "\n";
                        cout << "  Rating: " << nearestSchool->getRating() << "/10.0\n";
                        cout << "  Sector: " << nearestSchool->getSector() << "\n";
                        
                        double schoolLat, schoolLon;
                        if (sharedCityGraph->getVertexCoordinates(nearestSchoolID, schoolLat, schoolLon)) {
                            cout << "  Coordinates: " << schoolLat << ", " << schoolLon << "\n";
                            cout << "  Total Distance: " << minRoadDistance << " km (includes walking + road network)\n";
                            cout << "  Route (" << bestPathLength << " locations): ";
                            for (int i = 0; i < bestPathLength; i++) {
                                cout << bestPath[i];
                                if (i < bestPathLength - 1) cout << " -> ";
                            }
                            cout << "\n";
                            
                            string connectedStops[10];
                            int stopCount = 0;
                            if (sharedCityGraph->getConnectedStops(nearestSchoolID, connectedStops, 10, stopCount)) {
                                if (stopCount > 0) {
                                    cout << "  Connected Stop(s): ";
                                    for (int i = 0; i < stopCount; i++) {
                                        cout << connectedStops[i];
                                        if (i < stopCount - 1) cout << ", ";
                                    }
                                    cout << "\n";
                                }
                            }
                        }
                        if (bestPath != nullptr) delete[] bestPath;
                    }
                }
                
                // ========== FIND NEAREST HOSPITAL (using Dijkstra) ==========
                {
                    Hospital* nearestHospital = nullptr;
                    double minRoadDistance = -1.0;
                    string nearestHospitalID = "";
                    int bestPathLength = 0;
                    string* bestPath = nullptr;
                    
                    int hospitalCount = medical->getHospitalCount();
                    if (hospitalCount > 0) {
                        for (int i = 1; i <= 100 && i <= hospitalCount * 3; i++) {
                            string hospitalID = "H";
                            if (i < 10) hospitalID += "0";
                            hospitalID += to_string(i);
                            
                            Hospital* hospital = medical->searchHospital(hospitalID);
                            if (hospital == nullptr) continue;
                            
                            // Calculate complete distance: source coords -> source stop -> dest stop -> dest coords
                            double completeDistance = 0.0;
                            string tempSourceStop, tempDestStop;
                            if (calculateCompleteDistanceToVertex(lat, lon, hospitalID, sharedCityGraph, tempSourceStop, completeDistance)) {
                                if (minRoadDistance < 0.0 || completeDistance < minRoadDistance) {
                                    minRoadDistance = completeDistance;
                                    nearestHospital = hospital;
                                    nearestHospitalID = hospitalID;
                                    
                                    // Get path for display (from source stop to destination stop)
                                    int pathLength = 0;
                                    double edgeDistance = 0.0;
                                    double hospLat, hospLon;
                                    if (sharedCityGraph->getVertexCoordinates(hospitalID, hospLat, hospLon)) {
                                        string destStopID = sharedCityGraph->findNearestStop(hospLat, hospLon);
                                        if (!destStopID.empty()) {
                                            string* path = sharedCityGraph->findShortestPath(tempSourceStop, destStopID, pathLength, edgeDistance);
                                            if (path != nullptr) {
                                                bestPathLength = pathLength;
                                                if (bestPath != nullptr) delete[] bestPath;
                                                bestPath = new string[pathLength];
                                                for (int j = 0; j < pathLength; j++) {
                                                    bestPath[j] = path[j];
                                                }
                                                delete[] path;
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                    
                    if (nearestHospital != nullptr) {
                        cout << "\n[NEAREST HOSPITAL] (by road network)\n";
                        cout << "  Name: " << nearestHospital->getName() << "\n";
                        cout << "  Emergency Beds: " << nearestHospital->getEmergencyBeds() << "\n";
                        cout << "  Sector: " << nearestHospital->getSector() << "\n";
                        
                        double hospLat, hospLon;
                        if (sharedCityGraph->getVertexCoordinates(nearestHospitalID, hospLat, hospLon)) {
                            cout << "  Coordinates: " << hospLat << ", " << hospLon << "\n";
                            cout << "  Total Distance: " << minRoadDistance << " km (includes walking + road network)\n";
                            cout << "  Route (" << bestPathLength << " locations): ";
                            for (int i = 0; i < bestPathLength; i++) {
                                cout << bestPath[i];
                                if (i < bestPathLength - 1) cout << " -> ";
                            }
                            cout << "\n";
                            
                            string connectedStops[10];
                            int stopCount = 0;
                            if (sharedCityGraph->getConnectedStops(nearestHospitalID, connectedStops, 10, stopCount)) {
                                if (stopCount > 0) {
                                    cout << "  Connected Stop(s): ";
                                    for (int i = 0; i < stopCount; i++) {
                                        cout << connectedStops[i];
                                        if (i < stopCount - 1) cout << ", ";
                                    }
                                    cout << "\n";
                                }
                            }
                        }
                        if (bestPath != nullptr) delete[] bestPath;
                    }
                }
                
                // ========== FIND NEAREST BUS (using Dijkstra) ==========
                {
                    // Use existing findNearestBus which already uses Dijkstra from nearest stop
                    Bus* nearestBus = transport->findNearestBus(nearestStopID);
                    
                    if (nearestBus != nullptr) {
                        string busLocation = transport->getBusLocation(nearestBus->getBusNo());
                        if (!busLocation.empty()) {
                            // Calculate complete distance: source coords -> source stop -> bus location stop -> bus location coords
                            double completeDistance = 0.0;
                            string tempSourceStop;
                            if (calculateCompleteDistanceToVertex(lat, lon, busLocation, sharedCityGraph, tempSourceStop, completeDistance)) {
                                cout << "\n[NEAREST BUS] (by road network)\n";
                                cout << "  Bus Number: " << nearestBus->getBusNo() << "\n";
                                cout << "  Company: " << nearestBus->getCompany() << "\n";
                                cout << "  Current Location: " << busLocation << "\n";
                                
                                double stopLat, stopLon;
                                if (sharedCityGraph->getVertexCoordinates(busLocation, stopLat, stopLon)) {
                                    cout << "  Stop Coordinates: " << stopLat << ", " << stopLon << "\n";
                                    cout << "  Total Distance: " << completeDistance << " km (includes walking + road network)\n";
                                    
                                    // Get path for display
                                    string destStopID = sharedCityGraph->findNearestStop(stopLat, stopLon);
                                    if (!destStopID.empty()) {
                                        int pathLength = 0;
                                        double edgeDistance = 0.0;
                                        string* path = sharedCityGraph->findShortestPath(tempSourceStop, destStopID, pathLength, edgeDistance);
                                        if (path != nullptr && pathLength > 0) {
                                            cout << "  Route (" << pathLength << " locations): ";
                                            for (int i = 0; i < pathLength; i++) {
                                                cout << path[i];
                                                if (i < pathLength - 1) cout << " -> ";
                                            }
                                            cout << "\n";
                                            delete[] path;
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
                
                // ========== FIND NEAREST MALL (using Dijkstra) ==========
                {
                    // Use existing findNearestMall which already uses Dijkstra from nearest stop
                    Mall* nearestMall = malls->findNearestMall(nearestStopID);
                    
                    if (nearestMall != nullptr) {
                        string mallID = nearestMall->getVertexID();
                        if (!mallID.empty()) {
                            // Calculate complete distance: source coords -> source stop -> dest stop -> dest coords
                            double completeDistance = 0.0;
                            string tempSourceStop;
                            if (calculateCompleteDistanceToVertex(lat, lon, mallID, sharedCityGraph, tempSourceStop, completeDistance)) {
                                cout << "\n[NEAREST MALL] (by road network)\n";
                                cout << "  Name: " << nearestMall->getName() << "\n";
                                cout << "  Sector: " << nearestMall->getSector() << "\n";
                                
                                double mallLat, mallLon;
                                if (sharedCityGraph->getVertexCoordinates(mallID, mallLat, mallLon)) {
                                    cout << "  Coordinates: " << mallLat << ", " << mallLon << "\n";
                                    cout << "  Total Distance: " << completeDistance << " km (includes walking + road network)\n";
                                    
                                    // Get path for display
                                    string destStopID = sharedCityGraph->findNearestStop(mallLat, mallLon);
                                    if (!destStopID.empty()) {
                                        int pathLength = 0;
                                        double edgeDistance = 0.0;
                                        string* path = sharedCityGraph->findShortestPath(tempSourceStop, destStopID, pathLength, edgeDistance);
                                        if (path != nullptr && pathLength > 0) {
                                            cout << "  Route (" << pathLength << " locations): ";
                                            for (int i = 0; i < pathLength; i++) {
                                                cout << path[i];
                                                if (i < pathLength - 1) cout << " -> ";
                                            }
                                            cout << "\n";
                                            delete[] path;
                                        }
                                    }
                                    
                                    string connectedStops[10];
                                    int stopCount = 0;
                                    if (sharedCityGraph->getConnectedStops(mallID, connectedStops, 10, stopCount)) {
                                        if (stopCount > 0) {
                                            cout << "  Connected Stop(s): ";
                                            for (int i = 0; i < stopCount; i++) {
                                                cout << connectedStops[i];
                                                if (i < stopCount - 1) cout << ", ";
                                            }
                                            cout << "\n";
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
                
                // ========== FIND NEAREST FACILITY (using Dijkstra) ==========
                {
                    // Use existing findNearestFacility which already uses Dijkstra from nearest stop
                    Facility* nearestFacility = facilities->findNearestFacility(nearestStopID);
                    
                    if (nearestFacility != nullptr) {
                        string facilityID = nearestFacility->getVertexID();
                        if (!facilityID.empty()) {
                            // Calculate complete distance: source coords -> source stop -> dest stop -> dest coords
                            double completeDistance = 0.0;
                            string tempSourceStop;
                            if (calculateCompleteDistanceToVertex(lat, lon, facilityID, sharedCityGraph, tempSourceStop, completeDistance)) {
                                cout << "\n[NEAREST FACILITY] (by road network)\n";
                                cout << "  Name: " << nearestFacility->getName() << "\n";
                                cout << "  Type: " << nearestFacility->getType() << "\n";
                                cout << "  Sector: " << nearestFacility->getSector() << "\n";
                                
                                double facilityLat, facilityLon;
                                if (sharedCityGraph->getVertexCoordinates(facilityID, facilityLat, facilityLon)) {
                                    cout << "  Coordinates: " << facilityLat << ", " << facilityLon << "\n";
                                    cout << "  Total Distance: " << completeDistance << " km (includes walking + road network)\n";
                                    
                                    // Get path for display
                                    string destStopID = sharedCityGraph->findNearestStop(facilityLat, facilityLon);
                                    if (!destStopID.empty()) {
                                        int pathLength = 0;
                                        double edgeDistance = 0.0;
                                        string* path = sharedCityGraph->findShortestPath(tempSourceStop, destStopID, pathLength, edgeDistance);
                                        if (path != nullptr && pathLength > 0) {
                                            cout << "  Route (" << pathLength << " locations): ";
                                            for (int i = 0; i < pathLength; i++) {
                                                cout << path[i];
                                                if (i < pathLength - 1) cout << " -> ";
                                            }
                                            cout << "\n";
                                            delete[] path;
                                        }
                                    }
                                }
                                
                                string connectedStops[10];
                                int stopCount = 0;
                                if (sharedCityGraph->getConnectedStops(facilityID, connectedStops, 10, stopCount)) {
                                    if (stopCount > 0) {
                                        cout << "  Connected Stop(s): ";
                                        for (int i = 0; i < stopCount; i++) {
                                            cout << connectedStops[i];
                                            if (i < stopCount - 1) cout << ", ";
                                        }
                                        cout << "\n";
                                    }
                                }
                            }
                        }
                    }
                }
                
                cout << "\n================================================================\n";
                pause();
                break;
            }
            
            case 2: {
                string subject = readString("Enter Subject (e.g., Math): ");
                int count = 0;
                School** schools = education->findSchoolsBySubject(subject, count);
                if (count > 0) {
                    cout << "\n[RESULT]\n";
                    cout << "----------------------------\n";
                    cout << "Found " << count << " school(s) offering " << subject << ":\n";
                    cout << "----------------------------\n";
                    
                    int topCount = 0;
                    School** topSchools = education->getTopRankedSchools(1, topCount);
                    if (topCount > 0) {
                        cout << "Best Rated School: " << topSchools[0]->getName() 
                             << " (Rating: " << topSchools[0]->getRating() << ")\n";
                        delete[] topSchools;
                    }
                    
                    for (int i = 0; i < count && i < 5; i++) {
                        cout << (i + 1) << ". " << schools[i]->getName() 
                             << " (Rating: " << schools[i]->getRating() << ")\n";
                    }
                    delete[] schools;
                } else {
                    cout << "\n[INFO] No schools found offering " << subject << "\n";
                }
                pause();
                break;
            }
            
            case 3: {
                Hospital* mostBeds = medical->getHospitalWithMostBeds();
                if (mostBeds != nullptr) {
                    cout << "\n[RESULT]\n";
                    cout << "----------------------------\n";
                    cout << "Hospital with Most Emergency Beds:\n";
                    cout << "----------------------------\n";
                    cout << "Name: " << mostBeds->getName() << "\n";
                    cout << "Emergency Beds: " << mostBeds->getEmergencyBeds() << "\n";
                    cout << "Specialization: " << mostBeds->getSpecialization() << "\n";
                    
                    double lat = 33.709, lon = 73.037;
                    Bus* emergencyBus = transport->findNearestBus(lat, lon);
                    if (emergencyBus != nullptr) {
                        cout << "\nNearest Bus: " << emergencyBus->getBusNo() 
                             << " at " << emergencyBus->getCurrentStopID() << "\n";
                    }
                } else {
                    cout << "\n[ERROR] No hospitals available\n";
                }
                pause();
                break;
            }
            
            case 4: {
                string fromStop = readString("Enter starting stop ID: ");
                Mall* targetMall = malls->findNearestMall(fromStop);
                if (targetMall != nullptr) {
                    cout << "\n[RESULT]\n";
                    cout << "----------------------------\n";
                    cout << "Nearest Mall: " << targetMall->getName() << " at " << targetMall->getVertexID() << "\n";
                    
                    string* path = new string[50];
                    int pathLength = 0;
                    double distance = 0.0;
                    if (transport->findShortestPath(fromStop, targetMall->getVertexID(), path, pathLength, distance)) {
                        cout << "\nShortest Path (" << pathLength << " stops, " << distance << " km):\n";
                        for (int i = 0; i < pathLength; i++) {
                            cout << path[i];
                            if (i < pathLength - 1) cout << " -> ";
                        }
                        cout << "\n";
                    }
                    delete[] path;
                } else {
                    cout << "\n[ERROR] No malls found\n";
                }
                pause();
                break;
            }
            
            case 5: {
                cout << "\n[UNIVERSAL PATHFINDING]\n";
                cout << "Find shortest path between ANY two locations\n";
                cout << "Examples: S01 (School), H01 (Hospital), Stop1 (Bus Stop), P01 (Pharmacy), MALL01 (Mall), FCL01 (Facility)\n\n";
                
                string fromID = readString("Enter starting location ID: ");
                string toID = readString("Enter destination location ID: ");
                
                if (fromID.empty() || toID.empty()) {
                    cout << "\n[ERROR] Invalid location IDs\n";
                    pause();
                    break;
                }
                
                cout << "\n[SEARCHING PATH...]\n";
                
                int pathLength = 0;
                double distance = 0.0;
                string* path = sharedCityGraph->findShortestPath(fromID, toID, pathLength, distance);
                
                if (path != nullptr && pathLength > 0) {
                    cout << "\n[RESULT]\n";
                    cout << "----------------------------\n";
                    cout << "Shortest Path Found!\n";
                    cout << "----------------------------\n";
                    cout << "From: " << fromID << "\n";
                    cout << "To: " << toID << "\n";
                    cout << "Distance: " << distance << " km\n";
                    cout << "Route Length: " << pathLength << " locations\n";
                    cout << "\nRoute:\n";
                    
                    for (int i = 0; i < pathLength; i++) {
                        cout << "  " << (i + 1) << ". " << path[i];
                        if (i < pathLength - 1) {
                            cout << " ->";
                        }
                        cout << "\n";
                    }
                    
                    cout << "\nFull Path: ";
                    for (int i = 0; i < pathLength; i++) {
                        cout << path[i];
                        if (i < pathLength - 1) cout << " -> ";
                    }
                    cout << "\n";
                    
                    delete[] path;
                } else {
                    cout << "\n[ERROR] No path found between " << fromID << " and " << toID << "\n";
                    cout << "Make sure both locations exist in the graph and are connected.\n";
                }
                
                pause();
                break;
            }
            
            case 6: {
                cout << "\n[REAL-TIME ROUTE SIMULATION]\n";
                cout << "========================================\n";
                cout << "Simulate travel from one location to another\n";
                cout << "Examples: Stop1, Stop2, Stop3 (Bus Stops)\n\n";
                
                string fromID = readString("Enter starting stop ID: ");
                string toID = readString("Enter destination stop ID: ");
                
                if (fromID.empty()) {
                    cout << "\n[ERROR] Starting stop ID cannot be empty.\n";
                    cout << "Please enter a valid stop ID (e.g., Stop1, Stop2).\n";
                    pause();
                    break;
                }
                
                if (toID.empty()) {
                    cout << "\n[ERROR] Destination stop ID cannot be empty.\n";
                    cout << "Please enter a valid stop ID (e.g., Stop1, Stop2).\n";
                    pause();
                    break;
                }
                
                if (sharedCityGraph == nullptr) {
                    cout << "\n[ERROR] City Graph not initialized\n";
                    pause();
                    break;
                }
                
                cout << "\n[SEARCHING ROUTE...]\n";
                delay();
                
                // Find shortest path using graph edges (Dijkstra's algorithm)
                int pathLength = 0;
                double totalDistance = 0.0;
                string* path = sharedCityGraph->findShortestPath(fromID, toID, pathLength, totalDistance);
                
                if (path != nullptr && pathLength > 0) {
                    // Calculate distances between consecutive stops
                    double* segmentDistances = new double[pathLength - 1];
                    for (int i = 0; i < pathLength - 1; i++) {
                        double lat1, lon1, lat2, lon2;
                        if (sharedCityGraph->getVertexCoordinates(path[i], lat1, lon1) &&
                            sharedCityGraph->getVertexCoordinates(path[i + 1], lat2, lon2)) {
                            segmentDistances[i] = calculateDistance(lat1, lon1, lat2, lon2);
                        } else {
                            segmentDistances[i] = 0.1; // Default small distance
                        }
                    }
                    
                    cout << "\n[ROUTE FOUND!]\n";
                    cout << "========================================\n";
                    cout << "Starting Location: " << fromID << "\n";
                    cout << "Destination: " << toID << "\n";
                    cout << "Total Distance: " << totalDistance << " km\n";
                    cout << "Total Stops: " << pathLength << "\n";
                    cout << "========================================\n\n";
                    
                    cout << "[STARTING SIMULATION...]\n";
                    cout << "Press any key to begin...";
                    cin.ignore();
                    cin.get();
                    
                    // Simulate travel through each stop with animated progress
                    for (int i = 0; i < pathLength; i++) {
                        // Calculate cumulative distance traveled so far
                        double cumulativeDistance = 0.0;
                        for (int j = 0; j < i; j++) {
                            cumulativeDistance += segmentDistances[j];
                        }
                        
                        // Calculate progress percentage
                        double baseProgress = (totalDistance > 0.0) ? (cumulativeDistance / totalDistance) * 100.0 : 0.0;
                        if (i == pathLength - 1) baseProgress = 100.0;
                        
                        // Animate progress from previous to current, percent by percent
                        double prevProgress = 0.0;
                        if (i > 0) {
                            double prevCumulative = 0.0;
                            for (int j = 0; j < i - 1; j++) {
                                prevCumulative += segmentDistances[j];
                            }
                            prevProgress = (totalDistance > 0.0) ? (prevCumulative / totalDistance) * 100.0 : 0.0;
                        }
                        
                        // Animate progress bar filling percent by percent
                        for (int percent = (int)prevProgress; percent <= (int)baseProgress; percent++) {
                            system("cls");
                            
                            // Display header
                            cout << "================================================================\n";
                            cout << "          REAL-TIME ROUTE SIMULATION\n";
                            cout << "================================================================\n\n";
                            
                            // Display path at top
                            cout << "Route: ";
                            for (int k = 0; k < pathLength; k++) {
                                cout << path[k];
                                if (k < i) {
                                    cout << " [PASSED]";
                                } else if (k == i) {
                                    cout << " [NEXT]";
                                }
                                if (k < pathLength - 1) cout << " -> ";
                            }
                            cout << "\n\n";
                            
                            // Progress bar with 100 | characters
                            cout << "Progress: [";
                            int barLength = 100;
                            int filled = percent;
                            for (int j = 0; j < filled; j++) cout << "|";
                            for (int j = filled; j < barLength; j++) cout << " ";
                            cout << "] " << percent << "%\n\n";
                            
                            // Current stats
                            double currentDistance = (totalDistance * percent) / 100.0;
                            cout << "Next: " << path[i] << "\n";
                            cout << "Distance Traveled: " << currentDistance << " / " << totalDistance << " km\n";
                            cout << "Stop: " << (i + 1) << " / " << pathLength << "\n";
                            
                            if (i < pathLength - 1) {
                                cout << "Distance to next: " << segmentDistances[i] << " km\n";
                            } else {
                                cout << "Status: DESTINATION REACHED\n";
                            }
                            
                            if (percent < (int)baseProgress) {
                                delay();
                            }
                        }
                        
                        // Final update for this stop
                        if (i < pathLength - 1) {
                            delay();
                        }
                    }
                    
                    // Final screen
                    system("cls");
                    cout << "================================================================\n";
                    cout << "                    SIMULATION COMPLETE\n";
                    cout << "================================================================\n\n";
                    cout << "Route: ";
                    for (int i = 0; i < pathLength; i++) {
                        cout << path[i];
                        if (i < pathLength - 1) cout << " -> ";
                    }
                    cout << "\n\n";
                    cout << "Progress: [";
                    for (int j = 0; j < 100; j++) cout << "|";
                    cout << "] 100%\n\n";
                    cout << "Total Distance: " << totalDistance << " km\n";
                    cout << "Stops Visited: " << pathLength << "\n";
                    cout << "Status: JOURNEY COMPLETE\n\n";
                    
                    // Cleanup
                    delete[] path;
                    delete[] segmentDistances;
                } else {
                    cout << "\n[ERROR] No route found between " << fromID << " and " << toID << "\n";
                    cout << "Possible reasons:" << endl;
                    cout << "  - One or both locations do not exist in the city graph" << endl;
                    cout << "  - Locations are not connected by any path" << endl;
                    cout << "  - Graph data has not been loaded" << endl;
                    cout << "\nPlease verify:" << endl;
                    cout << "  - Both stop IDs are correct" << endl;
                    cout << "  - Data has been loaded using the Data Management menu" << endl;
                }
                
                pause();
                break;
            }
            
            case 0:
                return;
            
            default:
                cout << "\n[ERROR] Invalid choice. Please try again.\n";
                pause();
                break;
        }
    } while (choice != 0);
}

// ========================================================================
// RAILWAY MANAGER MENU HANDLERS
// ========================================================================

void displayRailwayMenu() {
    cout << "\n";
    cout << "================================================================\n";
    cout << "            RAILWAY MANAGER MENU\n";
    cout << "================================================================\n";
    cout << "\n";
    cout << "   1. Search Station by ID\n";
    cout << "   2. Search Station by Name\n";
    cout << "   3. Search Station by Code\n";
    cout << "   4. Find Nearest Station (by coordinates)\n";
    cout << "   5. Calculate Rail Distance (Between Two Stations)\n";
    cout << "   6. Display All Stations\n";
    cout << "   7. Add New Station\n";
    cout << "   0. Back to Main Menu\n";
    cout << "\n";
    cout << "================================================================\n";
    cout << "Enter your choice (0-7): ";
}

void handleRailwayMenu() {
    int choice;
    do {
        displayRailwayMenu();
        choice = readInt("");
        
        switch (choice) {
            case 1: {
                string stationID = readString("Enter Station ID: ");
                RailwayStation* station = railways->findStationByID(stationID);
                if (station != nullptr) {
                    cout << "\n[RESULT]\n";
                    cout << "----------------------------\n";
                    cout << "Station Found!\n";
                    cout << "----------------------------\n";
                    station->display();
                    
                    // Get connected stops
                    Graph* cityGraph = railways->getCityGraph();
                    if (cityGraph != nullptr) {
                        string connectedStops[10];
                        int stopCount = 0;
                        if (cityGraph->getConnectedStops(station->getVertexID(), connectedStops, 10, stopCount)) {
                            if (stopCount > 0) {
                                cout << "Connected Stop(s): ";
                                for (int i = 0; i < stopCount; i++) {
                                    cout << connectedStops[i];
                                    if (i < stopCount - 1) cout << ", ";
                                }
                                cout << "\n";
                            }
                        }
                    }
                } else {
                    cout << "\n[ERROR] Station not found with ID: " << stationID << "\n";
                }
                pause();
                break;
            }
            
            case 2: {
                string name = readString("Enter Station Name: ");
                RailwayStation* station = railways->findStationByName(name);
                if (station != nullptr) {
                    cout << "\n[RESULT]\n";
                    cout << "----------------------------\n";
                    cout << "Station Found!\n";
                    cout << "----------------------------\n";
                    station->display();
                    
                    // Get connected stops
                    Graph* cityGraph = railways->getCityGraph();
                    if (cityGraph != nullptr) {
                        string connectedStops[10];
                        int stopCount = 0;
                        if (cityGraph->getConnectedStops(station->getVertexID(), connectedStops, 10, stopCount)) {
                            if (stopCount > 0) {
                                cout << "Connected Stop(s): ";
                                for (int i = 0; i < stopCount; i++) {
                                    cout << connectedStops[i];
                                    if (i < stopCount - 1) cout << ", ";
                                }
                                cout << "\n";
                            }
                        }
                    }
                } else {
                    cout << "\n[ERROR] Station not found with name: " << name << "\n";
                }
                pause();
                break;
            }
            
            case 3: {
                string code = readString("Enter Station Code: ");
                RailwayStation* station = railways->findStationByCode(code);
                if (station != nullptr) {
                    cout << "\n[RESULT]\n";
                    cout << "----------------------------\n";
                    cout << "Station Found!\n";
                    cout << "----------------------------\n";
                    station->display();
                    
                    // Get connected stops
                    Graph* cityGraph = railways->getCityGraph();
                    if (cityGraph != nullptr) {
                        string connectedStops[10];
                        int stopCount = 0;
                        if (cityGraph->getConnectedStops(station->getVertexID(), connectedStops, 10, stopCount)) {
                            if (stopCount > 0) {
                                cout << "Connected Stop(s): ";
                                for (int i = 0; i < stopCount; i++) {
                                    cout << connectedStops[i];
                                    if (i < stopCount - 1) cout << ", ";
                                }
                                cout << "\n";
                            }
                        }
                    }
                } else {
                    cout << "\n[ERROR] Station not found with code: " << code << "\n";
                }
                pause();
                break;
            }
            
            case 4: {
                double lat = readDouble("Enter latitude: ");
                double lon = readDouble("Enter longitude: ");
                RailwayStation* nearest = railways->findNearestStation(lat, lon);
                if (nearest != nullptr) {
                    cout << "\n[RESULT]\n";
                    cout << "----------------------------\n";
                    cout << "Nearest Station:\n";
                    cout << "----------------------------\n";
                    nearest->display();
                    
                    // Calculate distance
                    double distance = calculateDistance(lat, lon, nearest->getLatitude(), nearest->getLongitude());
                    cout << "Distance: " << distance << " km\n";
                    
                    // Get connected stops
                    Graph* cityGraph = railways->getCityGraph();
                    if (cityGraph != nullptr) {
                        string connectedStops[10];
                        int stopCount = 0;
                        if (cityGraph->getConnectedStops(nearest->getVertexID(), connectedStops, 10, stopCount)) {
                            if (stopCount > 0) {
                                cout << "Connected Stop(s): ";
                                for (int i = 0; i < stopCount; i++) {
                                    cout << connectedStops[i];
                                    if (i < stopCount - 1) cout << ", ";
                                }
                                cout << "\n";
                            }
                        }
                    }
                } else {
                    cout << "\n[ERROR] No stations found\n";
                }
                pause();
                break;
            }
            
            case 5: {
                string fromID = readString("Enter Source Station ID: ");
                string toID = readString("Enter Destination Station ID: ");
                double distance = 0.0;
                if (railways->calculateRailDistance(fromID, toID, distance)) {
                    cout << "\n[RESULT]\n";
                    cout << "----------------------------\n";
                    cout << "Rail Distance Calculation:\n";
                    cout << "----------------------------\n";
                    RailwayStation* from = railways->findStationByID(fromID);
                    RailwayStation* to = railways->findStationByID(toID);
                    if (from != nullptr && to != nullptr) {
                        cout << "From: " << from->getName() << " (" << from->getCode() << ")\n";
                        cout << "To: " << to->getName() << " (" << to->getCode() << ")\n";
                        cout << "Rail Distance: " << distance << " km\n";
                    }
                } else {
                    cout << "\n[ERROR] Could not calculate distance.\n";
                    cout << "Possible reasons:" << endl;
                    cout << "  - One or both station IDs do not exist" << endl;
                    cout << "  - Station data has not been loaded" << endl;
                    cout << "\nPlease verify:" << endl;
                    cout << "  - Both station IDs are correct" << endl;
                    cout << "  - Railway stations have been loaded from CSV" << endl;
                }
                pause();
                break;
            }
            
            case 6: {
                cout << "\n";
                railways->displayAllStations();
                pause();
                break;
            }
            
            case 7: {
                cout << "\n[ADD NEW RAILWAY STATION]\n";
                cout << "----------------------------\n";
                string stationID = readString("Enter Station ID: ");
                string name = readString("Enter Station Name: ");
                string code = readString("Enter Station Code: ");
                string city = readString("Enter City: ");
                string coordStr = readString("Enter Coordinates (format: lat, lon, e.g., 33.6167, 73.0992): ");
                
                if (stationID.empty() || name.empty() || code.empty() || city.empty() || coordStr.empty()) {
                    cout << "\n[ERROR] All fields are required\n";
                    pause();
                    break;
                }
                
                // Check if station already exists
                if (railways->findStationByID(stationID) != nullptr) {
                    cout << "\n[ERROR] Station with ID " << stationID << " already exists\n";
                    pause();
                    break;
                }
                
                // Parse coordinates
                double lat = 0.0, lon = 0.0;
                if (!parseCoordinates(coordStr, lat, lon)) {
                    cout << "\n[ERROR] Invalid coordinate format. Use: lat, lon (e.g., 33.6167, 73.0992)\n";
                    pause();
                    break;
                }
                
                // Create new station (use stationID as vertexID)
                RailwayStation* newStation = new RailwayStation(stationID, name, code, city, stationID, lat, lon);
                
                // Add to manager (this will automatically connect to nearest bus stop and other stations)
                if (railways->addStation(newStation)) {
                    cout << "\n[SUCCESS] Station added successfully!\n";
                    cout << "ID: " << stationID << "\n";
                    cout << "Name: " << name << "\n";
                    cout << "Code: " << code << "\n";
                    cout << "City: " << city << "\n";
                    cout << "Coordinates: " << lat << ", " << lon << "\n";
                    cout << "Connected to nearest bus stop for pathfinding\n";
                    cout << "Connected to all other stations for direct train routes\n";
                } else {
                    cout << "\n[ERROR] Failed to add station\n";
                    delete newStation;
                }
                pause();
                break;
            }
            
            case 0:
                return;
            
            default:
                cout << "\n[ERROR] Invalid choice. Please try again.\n";
                pause();
                break;
        }
    } while (choice != 0);
}

// ========================================================================
// EMERGENCY TRANSPORT ROUTING MENU HANDLERS
// ========================================================================

void displayEmergencyMenu() {
    cout << "\n";
    cout << "================================================================\n";
    cout << "        EMERGENCY TRANSPORT ROUTING MENU\n";
    cout << "================================================================\n";
    cout << "\n";
    cout << "   1. Add Emergency Vehicle\n";
    cout << "   2. Display All Vehicles\n";
    cout << "   3. Display Available Vehicles\n";
    cout << "   4. Report Emergency\n";
    cout << "   5. Dispatch to Emergency\n";
    cout << "   6. Complete Emergency\n";
    cout << "   7. Find Best Hospital for Emergency\n";
    cout << "   8. Calculate Emergency Route\n";
    cout << "   9. Display Active Emergencies\n";
    cout << "   0. Back to Main Menu\n";
    cout << "\n";
    cout << "================================================================\n";
    cout << "Enter your choice (0-9): ";
}

void handleEmergencyMenu() {
    int choice;
    do {
        displayEmergencyMenu();
        choice = readInt("");
        
        switch (choice) {
            case 1: {
                cout << "\n[ADD EMERGENCY VEHICLE]\n";
                cout << "----------------------------\n";
                string vehicleID = readString("Enter Vehicle ID (e.g., AMB01): ");
                string vehicleType = readString("Enter Vehicle Type (Ambulance/FireTruck/Police): ");
                string busNo = readString("Enter Bus Number: ");
                string company = readString("Enter Company Name: ");
                string currentStop = readString("Enter Current Stop ID: ");
                int priorityLevel = readInt("Enter Priority Level (1=Critical, 2=Urgent, 3=Normal): ");
                
                if (vehicleID.empty() || vehicleType.empty() || busNo.empty() || company.empty()) {
                    cout << "\n[ERROR] All fields are required\n";
                    pause();
                    break;
                }
                
                if (priorityLevel < 1 || priorityLevel > 3) {
                    cout << "\n[ERROR] Priority level must be 1, 2, or 3\n";
                    pause();
                    break;
                }
                
                // Check if vehicle already exists
                if (emergency->findVehicleByID(vehicleID) != nullptr) {
                    cout << "\n[ERROR] Vehicle with ID " << vehicleID << " already exists\n";
                    pause();
                    break;
                }
                
                // Create new emergency vehicle
                EmergencyVehicle* newVehicle = new EmergencyVehicle(vehicleID, vehicleType, busNo, company, currentStop);
                newVehicle->setPriorityLevel(priorityLevel);
                
                // Note: Emergency vehicles use the city graph edges for navigation
                // No predefined route needed - vehicle navigates using graph connections
                
                // Add to manager
                if (emergency->addVehicle(newVehicle)) {
                    cout << "\n[SUCCESS] Emergency vehicle added successfully!\n";
                    cout << "Vehicle ID: " << vehicleID << "\n";
                    cout << "Type: " << vehicleType << "\n";
                    cout << "Bus Number: " << busNo << "\n";
                    cout << "Company: " << company << "\n";
                    cout << "Current Stop: " << currentStop << "\n";
                    cout << "Priority Level: " << priorityLevel << "\n";
                    cout << "Navigation: Uses city graph edges for automatic pathfinding\n";
                } else {
                    cout << "\n[ERROR] Failed to add vehicle\n";
                    delete newVehicle;
                }
                pause();
                break;
            }
            
            case 2: {
                cout << "\n";
                emergency->displayAllVehicles();
                pause();
                break;
            }
            
            case 3: {
                cout << "\n";
                emergency->displayAvailableVehicles();
                pause();
                break;
            }
            
            case 4: {
                cout << "\n[REPORT EMERGENCY]\n";
                cout << "----------------------------\n";
                string location = readString("Enter Emergency Location (Stop ID): ");
                string useCoords = readString("Use coordinates instead? (y/n): ");
                double lat = 0.0, lon = 0.0;
                
                if (useCoords == "y" || useCoords == "Y") {
                    lat = readDouble("Enter Latitude: ");
                    lon = readDouble("Enter Longitude: ");
                    location = ""; // Clear location if using coordinates
                }
                
                int priority = readInt("Enter Priority (1=Critical, 2=Urgent, 3=Normal): ");
                if (priority < 1 || priority > 3) {
                    cout << "\n[ERROR] Priority must be 1, 2, or 3\n";
                    pause();
                    break;
                }
                
                string type = readString("Enter Emergency Type (Medical/Fire/Police): ");
                string specialization = "";
                if (type == "Medical") {
                    specialization = readString("Enter Required Hospital Specialization (or leave empty): ");
                }
                
                string emergencyID = emergency->reportEmergency(location, lat, lon, priority, type, specialization);
                if (!emergencyID.empty()) {
                    cout << "\n[SUCCESS] Emergency reported successfully!\n";
                    cout << "Emergency ID: " << emergencyID << "\n";
                    cout << "Priority: " << priority;
                    if (priority == 1) cout << " (Critical)";
                    else if (priority == 2) cout << " (Urgent)";
                    else cout << " (Normal)";
                    cout << "\n";
                    cout << "Type: " << type << "\n";
                    if (!specialization.empty()) {
                        cout << "Required Specialization: " << specialization << "\n";
                    }
                } else {
                    cout << "\n[ERROR] Failed to report emergency\n";
                }
                pause();
                break;
            }
            
            case 5: {
                string emergencyID = readString("Enter Emergency ID: ");
                if (emergencyID.empty()) {
                    cout << "\n[ERROR] Emergency ID is required\n";
                    pause();
                    break;
                }
                
                cout << "\n[DISPATCHING TO EMERGENCY...]\n";
                EmergencyRoute* route = emergency->dispatchToEmergency(emergencyID);
                
                if (route != nullptr) {
                    cout << "\n[SUCCESS] Vehicle dispatched!\n";
                    cout << "----------------------------\n";
                    cout << "Emergency ID: " << emergencyID << "\n";
                    cout << "Vehicle ID: " << route->vehicleID << "\n";
                    cout << "Emergency Location: " << route->emergencyLocation << "\n";
                    if (!route->hospitalID.empty()) {
                        cout << "Hospital ID: " << route->hospitalID << "\n";
                    }
                    cout << "Total Distance: " << route->totalDistance << " km\n";
                    cout << "Route Length: " << route->routeLength << " stops\n";
                    cout << "\nRoute:\n";
                    for (int i = 0; i < route->routeLength; i++) {
                        cout << "  " << (i + 1) << ". " << route->route[i];
                        if (i < route->routeLength - 1) {
                            cout << " ->";
                        }
                        cout << "\n";
                    }
                    
                    cout << "\nFull Path: ";
                    for (int i = 0; i < route->routeLength; i++) {
                        cout << route->route[i];
                        if (i < route->routeLength - 1) cout << " -> ";
                    }
                    cout << "\n";
                    
                    // Cleanup
                    delete[] route->route;
                    delete route;
                } else {
                    cout << "\n[ERROR] Failed to dispatch vehicle. Check if:\n";
                    cout << "  - Emergency ID is valid\n";
                    cout << "  - Emergency is still active\n";
                    cout << "  - Available vehicle exists\n";
                }
                pause();
                break;
            }
            
            case 6: {
                string emergencyID = readString("Enter Emergency ID: ");
                if (emergency->completeEmergency(emergencyID)) {
                    cout << "\n[SUCCESS] Emergency completed. Vehicle is now available.\n";
                } else {
                    cout << "\n[ERROR] Failed to complete emergency. Check if emergency ID is valid.\n";
                }
                pause();
                break;
            }
            
            case 7: {
                string emergencyLocation = readString("Enter Emergency Location (Stop ID): ");
                string specialization = readString("Enter Required Specialization (or leave empty): ");
                
                Hospital* bestHospital = emergency->findBestHospital(emergencyLocation, specialization);
                if (bestHospital != nullptr) {
                    cout << "\n[RESULT]\n";
                    cout << "----------------------------\n";
                    cout << "Best Hospital for Emergency:\n";
                    cout << "----------------------------\n";
                    cout << "Hospital ID: " << bestHospital->getHospitalID() << "\n";
                    cout << "Name: " << bestHospital->getName() << "\n";
                    cout << "Sector: " << bestHospital->getSector() << "\n";
                    cout << "Emergency Beds: " << bestHospital->getEmergencyBeds() << "\n";
                    cout << "Specialization: " << bestHospital->getSpecialization() << "\n";
                    
                    // Calculate distance
                    if (sharedCityGraph != nullptr) {
                        int pathLength = 0;
                        double distance = 0.0;
                        string* path = sharedCityGraph->findShortestPath(emergencyLocation, bestHospital->getHospitalID(), pathLength, distance);
                        if (path != nullptr) {
                            cout << "Distance: " << distance << " km\n";
                            delete[] path;
                        }
                    }
                } else {
                    cout << "\n[ERROR] Could not find suitable hospital\n";
                }
                pause();
                break;
            }
            
            case 8: {
                string vehicleID = readString("Enter Vehicle ID: ");
                string emergencyLocation = readString("Enter Emergency Location (Stop ID): ");
                string hospitalID = readString("Enter Hospital ID (or leave empty): ");
                
                EmergencyRoute* route = emergency->calculateEmergencyRoute(vehicleID, emergencyLocation, hospitalID);
                if (route != nullptr) {
                    cout << "\n[RESULT]\n";
                    cout << "----------------------------\n";
                    cout << "Emergency Route Calculated:\n";
                    cout << "----------------------------\n";
                    cout << "Vehicle ID: " << route->vehicleID << "\n";
                    cout << "Emergency Location: " << route->emergencyLocation << "\n";
                    if (!route->hospitalID.empty()) {
                        cout << "Hospital ID: " << route->hospitalID << "\n";
                    }
                    cout << "Total Distance: " << route->totalDistance << " km\n";
                    cout << "Route Length: " << route->routeLength << " stops\n";
                    cout << "\nRoute:\n";
                    for (int i = 0; i < route->routeLength; i++) {
                        cout << "  " << (i + 1) << ". " << route->route[i];
                        if (i < route->routeLength - 1) {
                            cout << " ->";
                        }
                        cout << "\n";
                    }
                    
                    cout << "\nFull Path: ";
                    for (int i = 0; i < route->routeLength; i++) {
                        cout << route->route[i];
                        if (i < route->routeLength - 1) cout << " -> ";
                    }
                    cout << "\n";
                    
                    // Cleanup
                    delete[] route->route;
                    delete route;
                } else {
                    cout << "\n[ERROR] Failed to calculate route. Check if:\n";
                    cout << "  - Vehicle ID is valid\n";
                    cout << "  - Locations exist in graph\n";
                }
                pause();
                break;
            }
            
            case 9: {
                cout << "\n";
                emergency->displayActiveEmergencies();
                pause();
                break;
            }
            
            case 0:
                return;
            
            default:
                cout << "\n[ERROR] Invalid choice. Please try again.\n";
                pause();
                break;
        }
    } while (choice != 0);
}

// ========================================================================
// SYSTEM STATISTICS MENU HANDLERS
// ========================================================================

void handleStatisticsMenu() {
    int choice;
    do {
        displayStatisticsMenu();
        choice = readInt("");
        
        switch (choice) {
            case 1: {
                cout << "\n[STATISTICS]\n";
                cout << "----------------------------\n";
                cout << "MODULE STATISTICS:\n";
                cout << "----------------------------\n";
                cout << "\nEducation Sector:\n";
                cout << "   - Schools: " << education->getSchoolCount() << "\n";
                cout << "\nMedical Sector:\n";
                cout << "   - Hospitals: " << medical->getHospitalCount() << "\n";
                cout << "   - Pharmacies: " << medical->getPharmacyCount() << "\n";
                cout << "   - Medicines: " << medical->getMedicineCount() << "\n";
                cout << "\nTransport Sector:\n";
                cout << "   - Buses: " << transport->getBusCount() << "\n";
                cout << "   - School Buses: " << transport->getSchoolBusCount() << "\n";
                cout << "   - Companies: " << transport->getCompanyCount() << "\n";
                cout << "\nCommercial Sector:\n";
                cout << "   - Malls: " << malls->getMallCount() << "\n";
                cout << "\nFacilities Sector:\n";
                cout << "   - Facilities: " << facilities->getFacilityCount() << "\n";
                cout << "\nPopulation Sector:\n";
                cout << "   - Total Population: " << population->getTotalPopulation() << "\n";
                pause();
                break;
            }
            
            case 2: {
                cout << "\n[STATISTICS]\n";
                cout << "----------------------------\n";
                cout << "SHARED CITY GRAPH:\n";
                cout << "----------------------------\n";
                cout << "Total Vertices: " << sharedCityGraph->getVertexCount() << "\n";
                cout << "Total Edges: " << sharedCityGraph->getEdgeCount() << "\n";
                cout << "Graph Type: Undirected\n";
                cout << "Used by: Education, Medical, Transport, Malls, Facilities\n";
                cout << "NOT used by: Population/Housing (uses Tree structure)\n";
                pause();
                break;
            }
            
            case 3: {
                cout << "\n[STATISTICS]\n";
                cout << "----------------------------\n";
                cout << "DATA STRUCTURES USAGE SUMMARY:\n";
                cout << "----------------------------\n";
                cout << "1. HashTable: School/Hospital/Pharmacy/Citizen lookup (O(1))\n";
                cout << "2. MinHeap: School ranking, Hospital beds (priority queues)\n";
                cout << "3. Graph: Location queries, shortest paths (Dijkstra)\n";
                cout << "4. Tree: School hierarchy (School -> Department -> Class)\n";
                cout << "5. Tree: Population hierarchy (City -> Sector -> Street -> House -> Family)\n";
                cout << "6. SinglyLinkedList: Bus routes, Mall/Facility tracking\n";
                cout << "7. CircularQueue: Passenger queue\n";
                cout << "8. Stack: Route history\n";
                pause();
                break;
            }
            
            case 4:
                return;
            
            default:
                cout << "\n[ERROR] Invalid choice. Please try again.\n";
                pause();
                break;
        }
    } while (choice != 4);
}

// ========================================================================
// DATA MANAGEMENT MENU HANDLERS
// ========================================================================

void handleDataMenu() {
    int choice;
    do {
        displayDataMenu();
        choice = readInt("");
        
        switch (choice) {
            case 1: {
                cout << "\n[LOADING ALL DATA...]\n";
                int schoolsLoaded = education->loadSchoolsFromCSV("../SmartCity_dataset/schools.csv");
                cout << "[OK] Loaded " << schoolsLoaded << " schools\n";
                int hospitalsLoaded = medical->loadHospitalsFromCSV("../SmartCity_dataset/hospitals.csv");
                cout << "[OK] Loaded " << hospitalsLoaded << " hospitals\n";
                int pharmaciesLoaded = medical->loadPharmaciesFromCSV("../SmartCity_dataset/pharmacies.csv");
                cout << "[OK] Loaded " << pharmaciesLoaded << " pharmacies\n";
                CSVRow busData[50];
                int busCount = 0;
                if (loader->loadBuses("../SmartCity_dataset/buses.csv", busData, 50, busCount)) {
                    transport->loadBusesFromCSVData(busData, busCount);
                    cout << "[OK] Loaded " << busCount << " buses\n";
                }
                population->loadPopulationFromCSV("../SmartCity_dataset/population.csv");
                cout << "[OK] Loaded population data\n";
                CSVRow mallData[50];
                int mallCount = 0;
                if (loader->loadMalls("../SmartCity_dataset/malls.csv", mallData, 50, mallCount)) {
                    malls->loadMallsFromCSVData(mallData, mallCount);
                    cout << "[OK] Loaded " << mallCount << " malls\n";
                }
                CSVRow productData[200];
                int productCount = 0;
                if (loader->loadProducts("../SmartCity_dataset/products.csv", productData, 200, productCount)) {
                    malls->loadProductsFromCSVData(productData, productCount);
                    cout << "[OK] Loaded " << productCount << " products\n";
                }
                CSVRow facilityData[100];
                int facilityCount = 0;
                if (loader->loadFacilities("../SmartCity_dataset/facilities.csv", facilityData, 100, facilityCount)) {
                    facilities->loadFacilitiesFromCSVData(facilityData, facilityCount);
                    cout << "[OK] Loaded " << facilityCount << " facilities\n";
                }
                CSVRow airportData[50];
                int airportCount = 0;
                if (loader->loadAirports("../SmartCity_dataset/airports.csv", airportData, 50, airportCount)) {
                    airports->loadAirportsFromCSVData(airportData, airportCount);
                    cout << "[OK] Loaded " << airportCount << " airports\n";
                }
                CSVRow railwayData[50];
                int railwayCount = 0;
                if (loader->loadRailways("../SmartCity_dataset/railways.csv", railwayData, 50, railwayCount)) {
                    railways->loadStationsFromCSVData(railwayData, railwayCount);
                    cout << "[OK] Loaded " << railwayCount << " railway stations\n";
                }
                CSVRow schoolBusData[50];
                int schoolBusCount = 0;
                if (loader->loadSchoolBuses("../SmartCity_dataset/school_buses.csv", schoolBusData, 50, schoolBusCount)) {
                    transport->loadSchoolBusesFromCSVData(schoolBusData, schoolBusCount);
                    cout << "[OK] Loaded " << schoolBusCount << " school buses\n";
                }
                cout << "\n[SUCCESS] All data loaded successfully!\n";
                pause();
                break;
            }
            
            case 2: {
                int count = education->loadSchoolsFromCSV("../SmartCity_dataset/schools.csv");
                cout << "\n[SUCCESS] Loaded " << count << " schools\n";
                pause();
                break;
            }
            
            case 3: {
                int count = medical->loadHospitalsFromCSV("../SmartCity_dataset/hospitals.csv");
                cout << "\n[SUCCESS] Loaded " << count << " hospitals\n";
                pause();
                break;
            }
            
            case 4: {
                int count = medical->loadPharmaciesFromCSV("../SmartCity_dataset/pharmacies.csv");
                cout << "\n[SUCCESS] Loaded " << count << " pharmacies\n";
                pause();
                break;
            }
            
            case 5: {
                CSVRow busData[50];
                int busCount = 0;
                if (loader->loadBuses("../SmartCity_dataset/buses.csv", busData, 50, busCount)) {
                    transport->loadBusesFromCSVData(busData, busCount);
                    cout << "\n[SUCCESS] Loaded " << busCount << " buses\n";
                } else {
                    cout << "\n[ERROR] Failed to load buses\n";
                }
                pause();
                break;
            }
            
            case 6: {
                if (population->loadPopulationFromCSV("../SmartCity_dataset/population.csv")) {
                    cout << "\n[SUCCESS] Loaded population data\n";
                } else {
                    cout << "\n[ERROR] Failed to load population data\n";
                }
                pause();
                break;
            }
            
            case 7: {
                CSVRow mallData[50];
                int mallCount = 0;
                if (loader->loadMalls("../SmartCity_dataset/malls.csv", mallData, 50, mallCount)) {
                    if (malls->loadMallsFromCSVData(mallData, mallCount)) {
                        cout << "\n[SUCCESS] Loaded " << mallCount << " malls\n";
                    } else {
                        cout << "\n[ERROR] Failed to process mall data\n";
                    }
                } else {
                    cout << "\n[ERROR] Failed to load malls from CSV\n";
                }
                pause();
                break;
            }
            
            case 8: {
                CSVRow productData[200];
                int productCount = 0;
                if (loader->loadProducts("../SmartCity_dataset/products.csv", productData, 200, productCount)) {
                    if (malls->loadProductsFromCSVData(productData, productCount)) {
                        cout << "\n[SUCCESS] Loaded " << productCount << " products\n";
                    } else {
                        cout << "\n[ERROR] Failed to process product data\n";
                    }
                } else {
                    cout << "\n[ERROR] Failed to load products from CSV\n";
                }
                pause();
                break;
            }
            
            case 9: {
                CSVRow facilityData[100];
                int facilityCount = 0;
                if (loader->loadFacilities("../SmartCity_dataset/facilities.csv", facilityData, 100, facilityCount)) {
                    if (facilities->loadFacilitiesFromCSVData(facilityData, facilityCount)) {
                        cout << "\n[SUCCESS] Loaded " << facilityCount << " facilities\n";
                    } else {
                        cout << "\n[ERROR] Failed to process facility data\n";
                    }
                } else {
                    cout << "\n[ERROR] Failed to load facilities from CSV\n";
                }
                pause();
                break;
            }
            
            case 10: {
                CSVRow airportData[50];
                int airportCount = 0;
                if (loader->loadAirports("../SmartCity_dataset/airports.csv", airportData, 50, airportCount)) {
                    airports->loadAirportsFromCSVData(airportData, airportCount);
                    cout << "\n[SUCCESS] Loaded " << airportCount << " airports\n";
                } else {
                    cout << "\n[ERROR] Failed to load airports\n";
                }
                pause();
                break;
            }
            
            case 11: {
                CSVRow railwayData[50];
                int railwayCount = 0;
                if (loader->loadRailways("../SmartCity_dataset/railways.csv", railwayData, 50, railwayCount)) {
                    railways->loadStationsFromCSVData(railwayData, railwayCount);
                    cout << "\n[SUCCESS] Loaded " << railwayCount << " railway stations\n";
                } else {
                    cout << "\n[ERROR] Failed to load railway stations\n";
                }
                pause();
                break;
            }
            
            case 12: {
                CSVRow schoolBusData[50];
                int schoolBusCount = 0;
                if (loader->loadSchoolBuses("../SmartCity_dataset/school_buses.csv", schoolBusData, 50, schoolBusCount)) {
                    if (transport->loadSchoolBusesFromCSVData(schoolBusData, schoolBusCount)) {
                        cout << "\n[SUCCESS] Loaded " << schoolBusCount << " school buses\n";
                    } else {
                        cout << "\n[ERROR] Failed to process school bus data\n";
                    }
                } else {
                    cout << "\n[ERROR] Failed to load school buses from CSV\n";
                }
                pause();
                break;
            }
            
            case 13: {
                cout << "\n[DATA STATUS]\n";
                cout << "----------------------------\n";
                cout << "Schools: " << education->getSchoolCount() << "\n";
                cout << "Hospitals: " << medical->getHospitalCount() << "\n";
                cout << "Pharmacies: " << medical->getPharmacyCount() << "\n";
                cout << "Medicines: " << medical->getMedicineCount() << "\n";
                cout << "Buses: " << transport->getBusCount() << "\n";
                cout << "School Buses: " << transport->getSchoolBusCount() << "\n";
                cout << "Malls: " << malls->getMallCount() << "\n";
                cout << "Facilities: " << facilities->getFacilityCount() << "\n";
                cout << "Airports: " << airports->getAirportCount() << "\n";
                cout << "Railways: " << railways->getStationCount() << "\n";
                cout << "Emergency Vehicles: " << emergency->getVehicleCount() << "\n";
                cout << "Population: " << population->getTotalPopulation() << "\n";
                pause();
                break;
            }
            
            case 0:
                return;
            
            default:
                cout << "\n[ERROR] Invalid choice. Please try again.\n";
                pause();
                break;
        }
    } while (choice != 0);
}

// ========================================================================
// INITIALIZATION FUNCTION
// ========================================================================

bool initializeSystem() {
    cout << "\n";
    cout << "================================================================\n";
    cout << "     INITIALIZING SMART CITY MANAGEMENT SYSTEM\n";
    cout << "================================================================\n";
    cout << "\n";
    
    // Create shared graph
    sharedCityGraph = new Graph(200, false);
    cout << "[OK] Created shared city graph\n";
    
    // Create data loader
    loader = new DataLoader();
    
    // Load stops and build road network
    CSVRow stopData[50];
    int stopCount = 0;
    cout << "\nLoading bus stops from CSV...\n";
    if (loader->loadStops("../SmartCity_dataset/stops.csv", stopData, 50, stopCount)) {
        for (int i = 0; i < stopCount; i++) {
            if (stopData[i].fieldCount >= 3) {
                string stopID = stopData[i].fields[0];
                string name = stopData[i].fields[1];
                string coordStr = stopData[i].fields[2];
                
                double lat = 0.0, lon = 0.0;
                if (parseCoordinates(coordStr, lat, lon)) {
                    sharedCityGraph->addVertex(stopID, name, lat, lon);
                }
            }
        }
        cout << "[OK] Loaded " << stopCount << " stops\n";
    }
    
    // Build road network
    sharedCityGraph->addEdgeWithDistance("Stop1", "Stop2");
    sharedCityGraph->addEdgeWithDistance("Stop2", "Stop3");
    sharedCityGraph->addEdgeWithDistance("Stop3", "Stop4");
    sharedCityGraph->addEdgeWithDistance("Stop1", "Stop5");
    sharedCityGraph->addEdgeWithDistance("Stop5", "Stop6");
    sharedCityGraph->addEdgeWithDistance("Stop6", "Stop7");
    sharedCityGraph->addEdgeWithDistance("Stop7", "Stop8");
    sharedCityGraph->addEdgeWithDistance("Stop4", "Stop7");
    sharedCityGraph->addEdgeWithDistance("Stop2", "Stop5");
    sharedCityGraph->addEdgeWithDistance("Stop6", "Stop9");
    sharedCityGraph->addEdgeWithDistance("Stop4", "Stop10");
    sharedCityGraph->addEdgeWithDistance("Stop10", "Stop11");
    sharedCityGraph->addEdgeWithDistance("Stop1", "Stop9");
    cout << "[OK] Built road network (" << sharedCityGraph->getEdgeCount() << " edges)\n";
    
    // Create all modules
    education = new EducationSector(50, 11);
    medical = new MedicalSector(50, 100, 500, 11);
    transport = new TransportManager(sharedCityGraph, 100);
    malls = new MallManager(sharedCityGraph);
    facilities = new FacilityManager(sharedCityGraph);
    population = new PopulationManager();
    airports = new AirportManager(sharedCityGraph);
    railways = new RailwayManager(sharedCityGraph);
    emergency = new EmergencyManager(sharedCityGraph, medical);
    
    // Set shared graph
    education->setCityGraph(sharedCityGraph);
    medical->setCityGraph(sharedCityGraph);
    
    cout << "[OK] All modules initialized\n";
    
    cout << "\n[INFO] CSV data files will be loaded from the Load/Reload Data menu\n";
    cout << "\n[SUCCESS] System initialization complete!\n";
    cout << "================================================================\n";
    
    return true;
}

// ========================================================================
// MAIN FUNCTION
// ========================================================================

int main() {
    // Initialize system
    if (!initializeSystem()) {
        cout << "\n[ERROR] Failed to initialize system. Exiting...\n";
        return 1;
    }
    
    // Main menu loop
    int choice;
    do {
        displayMainMenu();
        choice = readInt("");
        
        switch (choice) {
            case 1:
                handleEducationMenu();
                break;
            case 2:
                handleMedicalMenu();
                break;
            case 3:
                handleTransportMenu();
                break;
            case 4:
                handleMallMenu();
                break;
            case 5:
                handleFacilityMenu();
                break;
            case 6:
                handlePopulationMenu();
                break;
            case 7:
                handleAirportMenu();
                break;
            case 8:
                handleRailwayMenu();
                break;
            case 9:
                handleEmergencyMenu();
                break;
            case 10:
                handleCrossModuleMenu();
                break;
            case 11:
                handleStatisticsMenu();
                break;
            case 12:
                handleDataMenu();
                break;
            case 0:
                cout << "\n[INFO] Exiting Smart City Management System...\n";
                break;
            default:
                cout << "\n[ERROR] Invalid choice. Please try again.\n";
                pause();
                break;
        }
    } while (choice != 0);
    
    // Cleanup
    delete education;
    delete medical;
    delete transport;
    delete malls;
    delete facilities;
    delete population;
    delete airports;
    delete railways;
    delete emergency;
    delete sharedCityGraph;
    delete loader;
    
    cout << "\n[SUCCESS] System shutdown complete. Thank you!\n";
    return 0;
}

