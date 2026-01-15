#ifndef CIRCULARQUEUE_H
#define CIRCULARQUEUE_H

#include <string>
using namespace std;

// Structure for an element in the queue
struct QueueNode {
    string data;         // Data stored in the queue (e.g., PassengerID)
    void* additionalData;     // Pointer to additional data (optional)
    
    QueueNode();
    QueueNode(const string& d, void* addData = nullptr);
};

// CircularQueue class
// Used for: Passenger queue simulation
class CircularQueue {
private:
    QueueNode* queueArray;    // Array to store queue elements
    int capacity;             // Maximum capacity of the queue
    int front;                // Index of the front element
    int rear;                 // Index of the rear element
    int queueSize;            // Current number of elements in the queue
    
    // Helper function to calculate next index (circular increment)
    // Complexity: O(1)
    int nextIndex(int index);
    
public:
    // Constructor: Initializes circular queue with given capacity
    // Complexity: O(1)
    CircularQueue(int cap = 100);
    
    // Destructor: Deallocates memory
    // Complexity: O(1)
    ~CircularQueue();
    
    // Enqueue (add) an element at the rear
    // Complexity: O(1)
    bool enqueue(const string& data, void* addData = nullptr);
    
    // Dequeue (remove) an element from the front
    // Complexity: O(1)
    QueueNode dequeue();
    
    // Peek at the front element without removing it
    // Complexity: O(1)
    QueueNode peekFront();
    
    // Peek at the rear element without removing it
    // Complexity: O(1)
    QueueNode peekRear();
    
    // Check if the queue is empty
    // Complexity: O(1)
    bool isEmpty();
    
    // Check if the queue is full
    // Complexity: O(1)
    bool isFull();
    
    // Get the current size of the queue
    // Complexity: O(1)
    int getSize();
    
    // Get the capacity of the queue
    // Complexity: O(1)
    int getCapacity();
    
    // Clear all elements from the queue
    // Complexity: O(1)
    void clear();
    
    // Display all elements in the queue
    // Complexity: O(n) where n is queue size
    void display();
    
    // Search for an element in the queue
    // Complexity: O(n) where n is queue size
    bool search(const string& data);
    
    // Get all elements as an array (maintaining order)
    // Complexity: O(n) where n is queue size
    void toArray(QueueNode* array, int& count);
};

#endif // CIRCULARQUEUE_H

