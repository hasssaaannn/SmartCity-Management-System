#include "Stack.h"
#include <iostream>
using namespace std;

// StackNode Implementation
StackNode::StackNode() : data(""), additionalData(nullptr) {
}

StackNode::StackNode(const string& d, void* addData) 
    : data(d), additionalData(addData) {
}

// Stack Implementation
Stack::Stack(int cap) : capacity(cap), topIndex(-1) {
    stackArray = new StackNode[capacity];
}

Stack::~Stack() {
    delete[] stackArray;
    stackArray = nullptr;
}

void Stack::resize() {
    int newCapacity = capacity * 2;
    StackNode* newArray = new StackNode[newCapacity];
    
    for (int i = 0; i <= topIndex; i++) {
        newArray[i] = stackArray[i];
    }
    
    delete[] stackArray;
    stackArray = newArray;
    capacity = newCapacity;
}

bool Stack::push(const string& data, void* addData) {
    if (isFull()) {
        resize();
    }
    
    topIndex++;
    stackArray[topIndex] = StackNode(data, addData);
    return true;
}

StackNode Stack::pop() {
    if (isEmpty()) {
        return StackNode(); // Return empty node
    }
    
    StackNode item = stackArray[topIndex];
    topIndex--;
    return item;
}

StackNode Stack::peek() {
    if (isEmpty()) {
        return StackNode(); // Return empty node
    }
    return stackArray[topIndex];
}

bool Stack::isEmpty() {
    return topIndex == -1;
}

bool Stack::isFull() {
    return topIndex >= capacity - 1;
}

int Stack::getSize() {
    return topIndex + 1;
}

int Stack::getCapacity() {
    return capacity;
}

void Stack::clear() {
    topIndex = -1;
}

void Stack::display() {
    if (isEmpty()) {
        cout << "Stack is empty." << endl;
        return;
    }
    
    cout << "Stack Contents (Top to Bottom, Size: " << getSize() << "):" << endl;
    for (int i = topIndex; i >= 0; i--) {
        cout << "[" << i << "] " << stackArray[i].data << endl;
    }
}

void Stack::displayReverse() {
    if (isEmpty()) {
        cout << "Stack is empty." << endl;
        return;
    }
    
    cout << "Stack Contents (Bottom to Top, Chronological, Size: " << getSize() << "):" << endl;
    for (int i = 0; i <= topIndex; i++) {
        cout << "[" << i << "] " << stackArray[i].data;
        if (i < topIndex) {
            cout << " -> ";
        }
    }
    cout << endl;
}

bool Stack::search(const string& data) {
    for (int i = 0; i <= topIndex; i++) {
        if (stackArray[i].data == data) {
            return true;
        }
    }
    return false;
}

void Stack::toArray(StackNode* array, int& count) {
    count = getSize();
    if (array == nullptr || isEmpty()) return;
    
    for (int i = 0; i <= topIndex; i++) {
        array[i] = stackArray[i];
    }
}

