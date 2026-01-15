#include "School.h"
#include "core_classes/Tree.h"
#include "core_classes/TreeNode.h"
#include <iostream>
using namespace std;

School::School() : schoolID(""), name(""), sector(""), rating(0.0), 
                   subjects(nullptr), subjectCount(0), hierarchyTree(nullptr) {
}

School::School(const string& id, const string& n, const string& sec, double r)
    : schoolID(id), name(n), sector(sec), rating(r), 
      subjects(nullptr), subjectCount(0), hierarchyTree(nullptr) {
}

School::~School() {
    if (subjects != nullptr) {
        delete[] subjects;
        subjects = nullptr;
    }
    
    if (hierarchyTree != nullptr) {
        delete hierarchyTree;
        hierarchyTree = nullptr;
    }
    
    subjectCount = 0;
}

string School::getSchoolID() const {
    return schoolID;
}

string School::getName() const {
    return name;
}

string School::getSector() const {
    return sector;
}

double School::getRating() const {
    return rating;
}

string* School::getSubjects() const {
    return subjects;
}

int School::getSubjectCount() const {
    return subjectCount;
}

Tree* School::getHierarchyTree() const {
    return hierarchyTree;
}

void School::setSchoolID(const string& id) {
    schoolID = id;
}

void School::setName(const string& n) {
    name = n;
}

void School::setSector(const string& sec) {
    sector = sec;
}

void School::setRating(double r) {
    rating = r;
}

void School::setSubjects(std::string* subj, int count) {
    // Delete old subjects
    if (subjects != nullptr) {
        delete[] subjects;
        subjects = nullptr;
    }
    
    subjectCount = count;
    if (count > 0 && subj != nullptr) {
        subjects = new string[count];
        for (int i = 0; i < count; i++) {
            subjects[i] = subj[i];
        }
    }
}

void School::setHierarchyTree(Tree* tree) {
    if (hierarchyTree != nullptr) {
        delete hierarchyTree;
    }
    hierarchyTree = tree;
}

void School::addSubject(const std::string& subject) {
    // Create new array with one more element
    string* newSubjects = new string[subjectCount + 1];
    
    // Copy old subjects
    for (int i = 0; i < subjectCount; i++) {
        newSubjects[i] = subjects[i];
    }
    
    // Add new subject
    newSubjects[subjectCount] = subject;
    subjectCount++;
    
    // Delete old array and update pointer
    if (subjects != nullptr) {
        delete[] subjects;
    }
    subjects = newSubjects;
}

void School::initializeHierarchy() {
    if (hierarchyTree == nullptr) {
        hierarchyTree = new Tree();
        // Create root node for this school
        TreeNode* root = new TreeNode(schoolID, name, this);
        hierarchyTree->setRoot(root);
    }
}

bool School::offersSubject(const string& subject) const {
    if (subjects == nullptr) return false;
    
    for (int i = 0; i < subjectCount; i++) {
        // Case-insensitive comparison (simple)
        if (subjects[i] == subject) {
            return true;
        }
    }
    return false;
}

void School::display() const {
    cout << "School ID: " << schoolID << endl;
    cout << "Name: " << name << endl;
    cout << "Sector: " << sector << endl;
    cout << "Rating: " << rating << endl;
    cout << "Subjects: ";
    if (subjects != nullptr && subjectCount > 0) {
        for (int i = 0; i < subjectCount; i++) {
            cout << subjects[i];
            if (i < subjectCount - 1) cout << ", ";
        }
    } else {
        cout << "None";
    }
    cout << endl;
}

