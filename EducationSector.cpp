#include "EducationSector.h"
#include <iostream>
using namespace std;

// Helper function to get approximate coordinates for Islamabad sectors
static void getSectorCoordinates(const string& sector, double& latitude, double& longitude) {
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
    } else if (sector == "Blue Area") {
        latitude = 33.697; longitude = 73.039;
    } else {
        // For unknown sectors, use base coordinates with slight variation
        latitude = baseLat;
        longitude = baseLon;
    }
}

// Helper function to convert string to double (replaces stod from STL)
static double stringToDouble(const string& str) {
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

EducationSector::EducationSector(int maxSchools, int hashTableSize) 
    : maxSchools(maxSchools), schoolCount(0), ownsLocationGraph(true) {
    schoolTable = new HashTable(hashTableSize);
    schoolRanking = new MinHeap(maxSchools);
    locationGraph = new Graph(maxSchools * 2, false); // Undirected graph for locations
    schoolIDList = new string[maxSchools];
}

EducationSector::~EducationSector() {
    // Delete all School objects
    for (int i = 0; i < schoolCount; i++) {
        School* school = (School*)schoolTable->search(schoolIDList[i]);
        if (school != nullptr) {
            delete school;
        }
    }
    
    if (schoolTable != nullptr) {
        delete schoolTable;
        schoolTable = nullptr;
    }
    
    if (schoolRanking != nullptr) {
        delete schoolRanking;
        schoolRanking = nullptr;
    }
    
    if (locationGraph != nullptr && ownsLocationGraph) {
        delete locationGraph;
        locationGraph = nullptr;
    }
    
    if (schoolIDList != nullptr) {
        delete[] schoolIDList;
        schoolIDList = nullptr;
    }
    
    schoolCount = 0;
}

void EducationSector::parseSubjects(const string& subjectsStr, string*& subjects, int& count) {
    if (subjectsStr.empty()) {
        subjects = nullptr;
        count = 0;
        return;
    }
    
    // Count commas to determine number of subjects
    count = 1;
    for (size_t i = 0; i < subjectsStr.length(); i++) {
        if (subjectsStr[i] == ',') {
            count++;
        }
    }
    
    // Allocate array
    subjects = new string[count];
    
    // Parse subjects
    string current = "";
    int index = 0;
    
    for (size_t i = 0; i < subjectsStr.length(); i++) {
        if (subjectsStr[i] == ',') {
            // Trim whitespace
            size_t start = 0;
            size_t end = current.length() - 1;
            while (start < current.length() && current[start] == ' ') start++;
            while (end > start && current[end] == ' ') end--;
            
            if (start <= end) {
                subjects[index] = current.substr(start, end - start + 1);
            } else {
                subjects[index] = "";
            }
            
            current = "";
            index++;
        } else {
            current += subjectsStr[i];
        }
    }
    
    // Add last subject
    if (!current.empty()) {
        size_t start = 0;
        size_t end = current.length() - 1;
        while (start < current.length() && current[start] == ' ') start++;
        while (end > start && current[end] == ' ') end--;
        
        if (start <= end) {
            subjects[index] = current.substr(start, end - start + 1);
        } else {
            subjects[index] = "";
        }
    }
}

bool EducationSector::registerSchool(const string& schoolID, const string& name, 
                                     const string& sector, double rating, const string& subjectsStr) {
    // Input validation
    if (schoolID.empty() || name.empty() || sector.empty()) {
        return false; // Invalid input: empty strings
    }
    
    if (rating < 0.0 || rating > 10.0) {
        return false; // Invalid rating: must be between 0.0 and 10.0
    }
    
    // Check if school already exists
    if (searchSchool(schoolID) != nullptr) {
        return false; // School already exists
    }
    
    if (schoolCount >= maxSchools) {
        return false; // Maximum schools reached
    }
    
    // Create new school
    School* school = new School(schoolID, name, sector, rating);
    
    // Parse subjects
    string* subjects = nullptr;
    int subjectCount = 0;
    parseSubjects(subjectsStr, subjects, subjectCount);
    school->setSubjects(subjects, subjectCount);
    if (subjects != nullptr) {
        delete[] subjects;
    }
    
    // Initialize hierarchy tree
    school->initializeHierarchy();
    
    // Add to HashTable
    schoolTable->insert(schoolID, school);
    
    // Add to ranking (use inverted priority: lower rating = higher priority in min-heap)
    // For ranking, we want highest rating first, so use (10.0 - rating) as priority
    double priority = 10.0 - rating; // Inverted so higher ratings come first when extracted
    schoolRanking->insert(schoolID, priority, school);
    
    // Add to school ID list for iteration
    schoolIDList[schoolCount] = schoolID;
    
    // Add school location to Graph for nearest school lookup
    double lat, lon;
    getSectorCoordinates(sector, lat, lon);
    locationGraph->addVertex(schoolID, name, lat, lon, school);
    
    // Connect school to nearest bus stop for pathfinding
    locationGraph->connectToNearestStop(schoolID);
    
    schoolCount++;
    return true;
}

int EducationSector::loadSchoolsFromCSV(const string& filename) {
    DataLoader loader;
    CSVRow schoolData[100];
    int actualRows = 0;
    
    if (!loader.loadSchools(filename, schoolData, 100, actualRows)) {
        return 0;
    }
    
    int loadedCount = 0;
    
    for (int i = 0; i < actualRows; i++) {
        if (schoolData[i].fieldCount >= 4) {
            string schoolID = schoolData[i].fields[0];
            string name = schoolData[i].fields[1];
            string sector = schoolData[i].fields[2];
            double rating = 0.0;
            
            // Parse rating
            if (schoolData[i].fieldCount > 3) {
                rating = stringToDouble(schoolData[i].fields[3]);
            }
            
            // Get subjects
            string subjectsStr = "";
            if (schoolData[i].fieldCount > 4) {
                subjectsStr = schoolData[i].fields[4];
            }
            
            if (registerSchool(schoolID, name, sector, rating, subjectsStr)) {
                loadedCount++;
                // School location already added to graph in registerSchool()
            }
        }
    }
    
    return loadedCount;
}

School* EducationSector::searchSchool(const string& schoolID) {
    return (School*)schoolTable->search(schoolID);
}

School** EducationSector::findSchoolsBySubject(const string& subject, int& count) {
    count = 0;
    School** result = new School*[maxSchools];
    
    // Iterate through all registered schools using ID list
    for (int i = 0; i < schoolCount; i++) {
        School* school = (School*)schoolTable->search(schoolIDList[i]);
        if (school != nullptr && school->offersSubject(subject)) {
            result[count++] = school;
        }
    }
    
    return result;
}

School** EducationSector::getTopRankedSchools(int topN, int& count) {
    count = 0;
    // Input validation: ensure topN is positive and reasonable
    if (topN <= 0) {
        return nullptr; // Invalid input: topN must be positive
    }
    
    if (schoolCount == 0 || schoolRanking->isEmpty()) {
        return nullptr; // No schools available
    }
    
    // Bound topN to available schools
    if (topN > schoolCount) {
        topN = schoolCount; // Limit to available schools
    }
    
    // Use MinHeap properly: Extract top N schools from the priority queue
    School** result = new School*[topN];
    
    // Temporary storage to rebuild heap after extraction
    HeapNode* extractedNodes = new HeapNode[schoolRanking->getSize()];
    int extractedCount = 0;
    
    // Extract top N schools from MinHeap
    while (!schoolRanking->isEmpty() && count < topN) {
        HeapNode node = schoolRanking->extractMin();
        if (!node.identifier.empty() && node.data != nullptr) {
            result[count++] = (School*)node.data;
            extractedNodes[extractedCount++] = node;
        }
    }
    
    // Rebuild the heap by reinserting all extracted elements
    for (int i = 0; i < extractedCount; i++) {
        schoolRanking->insert(extractedNodes[i].identifier, 
                             extractedNodes[i].priority, 
                             extractedNodes[i].data);
    }
    
    delete[] extractedNodes;
    
    // NOTE: Caller is responsible for deleting[] the returned array
    return result;
}

// Helper function to calculate simple squared distance (Euclidean, no sqrt needed for comparison)
static double calculateDistanceSquared(double lat1, double lon1, double lat2, double lon2) {
    double dLat = lat2 - lat1;
    double dLon = lon2 - lon1;
    return dLat * dLat + dLon * dLon;
}

string EducationSector::findNearestSchool(double latitude, double longitude) {
    if (schoolCount == 0 || locationGraph == nullptr) {
        return "";
    }
    
    // Iterate through all registered schools and find the nearest one
    double minDistance = 1000000.0; // Large initial distance
    string nearestSchoolID = "";
    
    for (int i = 0; i < schoolCount; i++) {
        School* school = searchSchool(schoolIDList[i]);
        if (school == nullptr) continue;
        
        // Get school coordinates from its sector
        double schoolLat, schoolLon;
        getSectorCoordinates(school->getSector(), schoolLat, schoolLon);
        
        // Calculate squared distance (no sqrt needed for comparison)
        double distSquared = calculateDistanceSquared(latitude, longitude, schoolLat, schoolLon);
        
        if (distSquared < minDistance) {
            minDistance = distSquared;
            nearestSchoolID = schoolIDList[i];
        }
    }
    
    return nearestSchoolID;
}

bool EducationSector::addDepartment(const string& schoolID, const string& deptID, 
                                   const string& deptName) {
    School* school = searchSchool(schoolID);
    if (school == nullptr) return false;
    
    Tree* tree = school->getHierarchyTree();
    if (tree == nullptr) {
        school->initializeHierarchy();
        tree = school->getHierarchyTree();
    }
    
    // Add department as child of school root
    return tree->addChild(schoolID, deptID, deptName, nullptr);
}

bool EducationSector::addClass(const string& schoolID, const string& deptID, 
                               const string& classID, const string& className) {
    School* school = searchSchool(schoolID);
    if (school == nullptr) return false;
    
    Tree* tree = school->getHierarchyTree();
    if (tree == nullptr) return false;
    
    // Add class as child of department
    return tree->addChild(deptID, classID, className, nullptr);
}

bool EducationSector::registerFaculty(const string& schoolID, const string& deptID, 
                                     const string& facultyID, const string& facultyName, void* facultyData) {
    School* school = searchSchool(schoolID);
    if (school == nullptr) return false;
    
    Tree* tree = school->getHierarchyTree();
    if (tree == nullptr) return false;
    
    // Find department node
    TreeNode* deptNode = tree->findNode(deptID);
    if (deptNode == nullptr) return false;
    
    // Add faculty as child of department
    return tree->addChild(deptID, facultyID, facultyName, facultyData);
}

bool EducationSector::registerStudent(const string& schoolID, const string& deptID, 
                                     const string& classID, const string& studentID, 
                                     const string& studentName, void* studentData) {
    School* school = searchSchool(schoolID);
    if (school == nullptr) return false;
    
    Tree* tree = school->getHierarchyTree();
    if (tree == nullptr) return false;
    
    // Add student as child of class
    return tree->addChild(classID, studentID, studentName, studentData);
}

void EducationSector::displaySchoolOrganogram(const string& schoolID) {
    School* school = searchSchool(schoolID);
    if (school == nullptr) {
        cout << "School not found: " << schoolID << endl;
        return;
    }
    
    cout << "\n=== School Organogram: " << school->getName() << " ===" << endl;
    Tree* tree = school->getHierarchyTree();
    if (tree != nullptr) {
        tree->display();
    } else {
        cout << "No hierarchy defined for this school." << endl;
    }
}

void EducationSector::displayAllSchools() {
    cout << "\n=== All Registered Schools (" << schoolCount << ") ===" << endl;
    
    if (schoolCount == 0) {
        cout << "No schools registered." << endl;
        return;
    }
    
    // Display all schools using ID list
    for (int i = 0; i < schoolCount; i++) {
        School* school = (School*)schoolTable->search(schoolIDList[i]);
        if (school != nullptr) {
            cout << "\n" << (i + 1) << ". ";
            school->display();
        }
    }
}

int EducationSector::getSchoolCount() const {
    return schoolCount;
}

void EducationSector::setCityGraph(Graph* graph) {
    if (graph == nullptr) {
        return; // Invalid graph
    }
    
    // Delete old graph if we own it
    if (locationGraph != nullptr && ownsLocationGraph) {
        delete locationGraph;
    }
    
    // Use external shared graph
    locationGraph = graph;
    ownsLocationGraph = false; // We don't own it, caller does
}

Graph* EducationSector::getCityGraph() const {
    return locationGraph;
}

