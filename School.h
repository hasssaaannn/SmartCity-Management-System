#ifndef SCHOOL_H
#define SCHOOL_H

#include <string>
using namespace std;

// Forward declaration
class Tree;

// School entity class
// Represents a school with its basic information and hierarchy
class School {
private:
    string schoolID;          // Unique identifier (e.g., "S01")
    string name;              // School name (e.g., "City School")
    string sector;            // Sector location (e.g., "G-10")
    double rating;                 // School rating (e.g., 4.5)
    string* subjects;         // Array of subjects offered
    int subjectCount;              // Number of subjects
    Tree* hierarchyTree;           // Tree structure: School → Department → Class
    
public:
    // Constructor
    School();
    School(const string& id, const string& n, const string& sec, double r);
    
    // Destructor
    ~School();
    
    // Getters
    string getSchoolID() const;
    string getName() const;
    string getSector() const;
    double getRating() const;
    string* getSubjects() const;
    int getSubjectCount() const;
    Tree* getHierarchyTree() const;
    
    // Setters
    void setSchoolID(const string& id);
    void setName(const string& n);
    void setSector(const string& sec);
    void setRating(double r);
    void setSubjects(string* subj, int count);
    void setHierarchyTree(Tree* tree);
    
    // Add a single subject
    void addSubject(const string& subject);
    
    // Initialize hierarchy tree (creates root node)
    void initializeHierarchy();
    
    // Check if school offers a specific subject
    bool offersSubject(const string& subject) const;
    
    // Display school information
    void display() const;
};

#endif // SCHOOL_H

