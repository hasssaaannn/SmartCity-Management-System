#ifndef SCHOOLBUS_H
#define SCHOOLBUS_H

#include <string>
#include "Bus.h"
#include "core_classes/SinglyLinkedList.h"
using namespace std;

// SchoolBus class - extends Bus for school-specific functionality
// Used for: School bus tracking, student management
class SchoolBus : public Bus {
private:
    string schoolID;                    // School ID this bus serves (e.g., "S01", "S02")
    SinglyLinkedList* studentsOnBus;     // Linked list of student IDs currently on the bus
    int maxCapacity;                    // Maximum number of students the bus can carry
    
public:
    // Constructor
    // Parameters: busNo, company, currentStopID, schoolID, maxCapacity
    SchoolBus(const string& busNo = "", 
              const string& comp = "", 
              const string& currentStop = "",
              const string& school = "",
              int capacity = 50);
    
    // Destructor
    ~SchoolBus();
    
    // Getters
    string getSchoolID() const;
    int getMaxCapacity() const;
    int getCurrentStudentCount() const;
    SinglyLinkedList* getStudentsOnBus() const;
    
    // Setters
    void setSchoolID(const string& schoolID);
    void setMaxCapacity(int capacity);
    
    // Student management
    // Add a student to the bus (boarding)
    // Returns: true if successful, false if bus full or student already on bus
    bool addStudent(const string& studentID);
    
    // Remove a student from the bus (alighting)
    // Returns: true if successful, false if student not on bus
    bool removeStudent(const string& studentID);
    
    // Check if a student is on the bus
    // Returns: true if student is on bus
    bool isStudentOnBus(const string& studentID) const;
    
    // Get all students on the bus as array
    // Parameters: students array (output), count (output)
    // Note: Caller must allocate students array with sufficient size
    void getStudentsOnBus(string* students, int& count) const;
    
    // Check if bus is full
    // Returns: true if bus has reached max capacity
    bool isFull() const;
    
    // Display school bus information (overrides Bus::display)
    void display();
    
    // Check if school bus is valid
    bool isValid() const;
};

#endif // SCHOOLBUS_H

