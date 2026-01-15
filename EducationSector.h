#ifndef EDUCATION_SECTOR_H
#define EDUCATION_SECTOR_H

#include <string>
#include "core_classes/HashTable.h"
#include "core_classes/Tree.h"
#include "core_classes/MinHeap.h"
#include "core_classes/Graph.h"
#include "School.h"
#include "core_classes/DataLoader.h"
using namespace std;

// Education Sector Manager Class
// Manages schools, their hierarchy, ranking, and search operations
class EducationSector {
private:
    HashTable* schoolTable;        // HashTable for SchoolID → School* lookup
    MinHeap* schoolRanking;        // Priority Queue for school ranking by rating
    Graph* locationGraph;          // Graph for nearest school queries
    bool ownsLocationGraph;        // Flag to track if we own the graph (for shared graph support)
    string* schoolIDList;     // List of all school IDs for iteration
    int maxSchools;                // Maximum number of schools
    int schoolCount;               // Current number of schools
    
    // Helper: Parse subjects from comma-separated string
    void parseSubjects(const string& subjectsStr, string*& subjects, int& count);
    
public:
    // Constructor
    EducationSector(int maxSchools = 100, int hashTableSize = 11);
    
    // Destructor
    ~EducationSector();
    
    // Register a new school
    // Returns: true if successful, false if school already exists
    bool registerSchool(const string& schoolID, const string& name, 
                       const string& sector, double rating, const string& subjectsStr);
    
    // Load schools from CSV file using DataLoader
    // Returns: number of schools loaded
    int loadSchoolsFromCSV(const string& filename);
    
    // Search school by ID (O(1) lookup using HashTable)
    // Returns: Pointer to School, or nullptr if not found
    School* searchSchool(const string& schoolID);
    
    // Search schools offering a specific subject
    // Returns: Array of School pointers, and count
    School** findSchoolsBySubject(const string& subject, int& count);
    
    // Get top-ranked schools (by rating)
    // Returns: Array of School pointers sorted by rating (best first)
    School** getTopRankedSchools(int topN, int& count);
    
    // Find nearest school to given location (using Graph)
    // Returns: SchoolID of nearest school, or empty string if not found
    string findNearestSchool(double latitude, double longitude);
    
    // Add department to a school (Tree structure: School → Department)
    // Returns: true if successful
    bool addDepartment(const string& schoolID, const string& deptID, 
                      const string& deptName);
    
    // Add class to a department (Tree structure: School → Department → Class)
    // Returns: true if successful
    bool addClass(const string& schoolID, const string& deptID, 
                 const string& classID, const string& className);
    
    // Register faculty member (store in tree node data)
    bool registerFaculty(const string& schoolID, const string& deptID, 
                        const string& facultyID, const string& facultyName, void* facultyData);
    
    // Register student (store in class node data)
    bool registerStudent(const string& schoolID, const string& deptID, 
                        const string& classID, const string& studentID, 
                        const string& studentName, void* studentData);
    
    // Display school organogram (tree structure)
    void displaySchoolOrganogram(const string& schoolID);
    
    // Display all schools
    void displayAllSchools();
    
    // Get number of registered schools
    int getSchoolCount() const;
    
    // Set city graph (for shared graph support - use single graph for entire city)
    // If external graph is provided, it will be used instead of internal graph
    // NOTE: Caller must ensure graph exists for lifetime of EducationSector
    void setCityGraph(Graph* graph);
    
    // Get city graph (for integration with other modules)
    Graph* getCityGraph() const;
};

#endif // EDUCATION_SECTOR_H

