#include "MinHeap.h"
#include <iostream>
using namespace std;

// HeapNode Implementation
HeapNode::HeapNode() : identifier(""), priority(0.0), data(nullptr) {
}

HeapNode::HeapNode(const string& id, double prio, void* d) 
    : identifier(id), priority(prio), data(d) {
}

// MinHeap Implementation
MinHeap::MinHeap(int cap) : capacity(cap), heapSize(0) {
    heapArray = new HeapNode[capacity];
}

MinHeap::~MinHeap() {
    delete[] heapArray;
    heapArray = nullptr;
}

int MinHeap::parent(int index) {
    if (index <= 0) return -1;
    return (index - 1) / 2;
}

int MinHeap::leftChild(int index) {
    int left = 2 * index + 1;
    return (left < heapSize) ? left : -1;
}

int MinHeap::rightChild(int index) {
    int right = 2 * index + 2;
    return (right < heapSize) ? right : -1;
}

void MinHeap::swap(int index1, int index2) {
    HeapNode temp = heapArray[index1];
    heapArray[index1] = heapArray[index2];
    heapArray[index2] = temp;
}

void MinHeap::heapifyUp(int index) {
    if (index <= 0) return;
    
    int parentIndex = parent(index);
    if (parentIndex >= 0 && heapArray[index].priority < heapArray[parentIndex].priority) {
        swap(index, parentIndex);
        heapifyUp(parentIndex);
    }
}

void MinHeap::heapifyDown(int index) {
    int smallest = index;
    int left = leftChild(index);
    int right = rightChild(index);
    
    if (left != -1 && heapArray[left].priority < heapArray[smallest].priority) {
        smallest = left;
    }
    
    if (right != -1 && heapArray[right].priority < heapArray[smallest].priority) {
        smallest = right;
    }
    
    if (smallest != index) {
        swap(index, smallest);
        heapifyDown(smallest);
    }
}

void MinHeap::resize() {
    int newCapacity = capacity * 2;
    HeapNode* newArray = new HeapNode[newCapacity];
    
    for (int i = 0; i < heapSize; i++) {
        newArray[i] = heapArray[i];
    }
    
    delete[] heapArray;
    heapArray = newArray;
    capacity = newCapacity;
}

bool MinHeap::insert(const string& identifier, double priority, void* data) {
    if (isFull()) {
        resize();
    }
    
    heapArray[heapSize] = HeapNode(identifier, priority, data);
    heapifyUp(heapSize);
    heapSize++;
    return true;
}

HeapNode MinHeap::extractMin() {
    if (isEmpty()) {
        return HeapNode(); // Return empty node
    }
    
    HeapNode minNode = heapArray[0];
    heapArray[0] = heapArray[heapSize - 1];
    heapSize--;
    
    if (heapSize > 0) {
        heapifyDown(0);
    }
    
    return minNode;
}

HeapNode MinHeap::peekMin() {
    if (isEmpty()) {
        return HeapNode(); // Return empty node
    }
    return heapArray[0];
}

bool MinHeap::decreasePriority(const string& identifier, double newPriority) {
    for (int i = 0; i < heapSize; i++) {
        if (heapArray[i].identifier == identifier) {
            if (newPriority > heapArray[i].priority) {
                return false; // New priority is not smaller
            }
            heapArray[i].priority = newPriority;
            heapifyUp(i);
            return true;
        }
    }
    return false; // Identifier not found
}

bool MinHeap::isEmpty() {
    return heapSize == 0;
}

bool MinHeap::isFull() {
    return heapSize >= capacity;
}

int MinHeap::getSize() {
    return heapSize;
}

int MinHeap::getCapacity() {
    return capacity;
}

void MinHeap::clear() {
    heapSize = 0;
}

bool MinHeap::contains(const string& identifier) {
    for (int i = 0; i < heapSize; i++) {
        if (heapArray[i].identifier == identifier) {
            return true;
        }
    }
    return false;
}

void MinHeap::display() {
    cout << "MinHeap Contents (Size: " << heapSize << "):" << endl;
    for (int i = 0; i < heapSize; i++) {
        cout << "[" << i << "] " << heapArray[i].identifier 
             << " (Priority: " << heapArray[i].priority << ")" << endl;
    }
}

