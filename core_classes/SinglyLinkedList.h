#ifndef SINGLYLINKEDLIST_H
#define SINGLYLINKEDLIST_H

#include <string>
using namespace std;

// Structure for a node in the singly linked list
struct ListNode {
    string data;         // Data stored in the node (e.g., StopID)
    void* additionalData;     // Pointer to additional data (optional)
    ListNode* next;           // Pointer to next node
    
    ListNode(const string& d, void* addData = nullptr);
};

// SinglyLinkedList class
// Used for: Bus routes (storing order of stops)
class SinglyLinkedList {
private:
    ListNode* head;           // Pointer to the first node
    ListNode* tail;           // Pointer to the last node (for O(1) insertion at end)
    int listSize;             // Current number of nodes in the list
    
public:
    // Constructor: Creates an empty list
    // Complexity: O(1)
    SinglyLinkedList();
    
    // Destructor: Deallocates all nodes
    // Complexity: O(n) where n is list size
    ~SinglyLinkedList();
    
    // Insert at the beginning of the list
    // Complexity: O(1)
    void insertAtHead(const string& data, void* addData = nullptr);
    
    // Insert at the end of the list
    // Complexity: O(1) with tail pointer
    void insertAtTail(const string& data, void* addData = nullptr);
    
    // Insert at a specific position (0-indexed)
    // Complexity: O(n) worst case
    bool insertAtPosition(int position, const string& data, void* addData = nullptr);
    
    // Insert after a specific value
    // Complexity: O(n) to find position
    bool insertAfter(const string& afterValue, const string& data, void* addData = nullptr);
    
    // Remove the first occurrence of a value
    // Complexity: O(n) to find node
    bool remove(const string& data);
    
    // Remove node at a specific position
    // Complexity: O(n) to reach position
    bool removeAtPosition(int position);
    
    // Remove from the beginning
    // Complexity: O(1)
    bool removeFromHead();
    
    // Remove from the end
    // Complexity: O(n) to find second-to-last node
    bool removeFromTail();
    
    // Search for a value in the list
    // Complexity: O(n)
    bool search(const string& data);
    
    // Get data at a specific position
    // Complexity: O(n) to reach position
    string getAtPosition(int position);
    
    // Get the first node's data
    // Complexity: O(1)
    string getHead();
    
    // Get the last node's data
    // Complexity: O(1) with tail pointer
    string getTail();
    
    // Update data at a specific position
    // Complexity: O(n) to reach position
    bool updateAtPosition(int position, const string& newData);
    
    // Display the entire list
    // Complexity: O(n)
    void display();
    
    // Display the list in a formatted way (e.g., "Stop1 → Stop2 → Stop3")
    // Complexity: O(n)
    void displayFormatted(const string& separator = " → ");
    
    // Get the size of the list
    // Complexity: O(1)
    int getSize() const;
    
    // Check if the list is empty
    // Complexity: O(1)
    bool isEmpty();
    
    // Clear all nodes from the list
    // Complexity: O(n)
    void clear();
    
    // Reverse the list
    // Complexity: O(n)
    void reverse();
    
    // Get all elements as an array (for easier traversal)
    // Complexity: O(n)
    void toArray(string* array, int& count);
};

#endif // SINGLYLINKEDLIST_H

