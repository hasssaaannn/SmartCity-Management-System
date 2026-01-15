#ifndef STACK_H
#define STACK_H

#include <string>
using namespace std;

// Structure for an element in the stack
struct StackNode {
    string data;         // Data stored in the stack (e.g., RouteID, StopID)
    void* additionalData;     // Pointer to additional data (optional)
    
    StackNode();
    StackNode(const string& d, void* addData = nullptr);
};

// Stack class implemented using array
// Used for: Route travel history
class Stack {
private:
    StackNode* stackArray;    // Array to store stack elements
    int capacity;             // Maximum capacity of the stack
    int topIndex;             // Index of the top element (-1 when empty)
    
    // Resize the stack array when capacity is reached
    // Complexity: O(n) where n is current stack size
    void resize();
    
public:
    // Constructor: Initializes stack with given capacity
    // Complexity: O(1)
    Stack(int cap = 100);
    
    // Destructor: Deallocates memory
    // Complexity: O(1)
    ~Stack();
    
    // Push an element onto the stack
    // Complexity: O(1) amortized, O(n) when resizing
    bool push(const string& data, void* addData = nullptr);
    
    // Pop an element from the stack
    // Complexity: O(1)
    StackNode pop();
    
    // Peek at the top element without removing it
    // Complexity: O(1)
    StackNode peek();
    
    // Check if the stack is empty
    // Complexity: O(1)
    bool isEmpty();
    
    // Check if the stack is full
    // Complexity: O(1)
    bool isFull();
    
    // Get the current size of the stack
    // Complexity: O(1)
    int getSize();
    
    // Get the capacity of the stack
    // Complexity: O(1)
    int getCapacity();
    
    // Clear all elements from the stack
    // Complexity: O(1)
    void clear();
    
    // Display all elements in the stack (top to bottom)
    // Complexity: O(n) where n is stack size
    void display();
    
    // Display all elements in the stack (bottom to top, chronological order)
    // Complexity: O(n) where n is stack size
    void displayReverse();
    
    // Search for an element in the stack
    // Complexity: O(n) where n is stack size
    bool search(const string& data);
    
    // Get all elements as an array (from bottom to top)
    // Complexity: O(n) where n is stack size
    void toArray(StackNode* array, int& count);
};

#endif // STACK_H

