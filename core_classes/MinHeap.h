#ifndef MINHEAP_H
#define MINHEAP_H

#include <string>
using namespace std;

// Structure for an element in the min-heap
struct HeapNode {
    string identifier;   // Identifier (e.g., HospitalID, FacilityID)
    double priority;          // Priority value (e.g., distance, emergency level)
    void* data;               // Pointer to additional data
    
    HeapNode();
    HeapNode(const string& id, double prio, void* d = nullptr);
};

// MinHeap class implementing binary min-heap (Priority Queue)
// Used for: Nearest facility search, Emergency hospital ranking
class MinHeap {
private:
    HeapNode* heapArray;      // Array to store heap elements
    int capacity;             // Maximum capacity of the heap
    int heapSize;             // Current number of elements in the heap
    
    // Helper function to get parent index
    // Complexity: O(1)
    int parent(int index);
    
    // Helper function to get left child index
    // Complexity: O(1)
    int leftChild(int index);
    
    // Helper function to get right child index
    // Complexity: O(1)
    int rightChild(int index);
    
    // Helper function to swap two elements
    // Complexity: O(1)
    void swap(int index1, int index2);
    
    // Heapify up (bubble up) after insertion
    // Complexity: O(log n) where n is heap size
    void heapifyUp(int index);
    
    // Heapify down (bubble down) after extraction
    // Complexity: O(log n) where n is heap size
    void heapifyDown(int index);
    
    // Resize the heap array when capacity is reached
    // Complexity: O(n) where n is current heap size
    void resize();
    
public:
    // Constructor: Initializes min-heap with given capacity
    // Complexity: O(1)
    MinHeap(int cap = 100);
    
    // Destructor: Deallocates memory
    // Complexity: O(1)
    ~MinHeap();
    
    // Insert an element into the min-heap
    // Complexity: O(log n) where n is heap size
    bool insert(const string& identifier, double priority, void* data = nullptr);
    
    // Extract the minimum element (root)
    // Complexity: O(log n) where n is heap size
    HeapNode extractMin();
    
    // Peek at the minimum element without removing it
    // Complexity: O(1)
    HeapNode peekMin();
    
    // Decrease the priority of an element
    // Complexity: O(n) to find + O(log n) to heapify = O(n)
    bool decreasePriority(const string& identifier, double newPriority);
    
    // Check if the heap is empty
    // Complexity: O(1)
    bool isEmpty();
    
    // Check if the heap is full
    // Complexity: O(1)
    bool isFull();
    
    // Get the current size of the heap
    // Complexity: O(1)
    int getSize();
    
    // Get the capacity of the heap
    // Complexity: O(1)
    int getCapacity();
    
    // Clear all elements from the heap
    // Complexity: O(1)
    void clear();
    
    // Check if an element exists in the heap
    // Complexity: O(n)
    bool contains(const string& identifier);
    
    // Display the heap structure (for debugging)
    // Complexity: O(n)
    void display();
};

#endif // MINHEAP_H

