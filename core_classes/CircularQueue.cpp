#include "CircularQueue.h"
#include <iostream>
using namespace std;

// QueueNode Implementation
QueueNode::QueueNode() : data(""), additionalData(nullptr) {
}

QueueNode::QueueNode(const string& d, void* addData) 
    : data(d), additionalData(addData) {
}

// CircularQueue Implementation
CircularQueue::CircularQueue(int cap) : capacity(cap), front(0), rear(-1), queueSize(0) {
    queueArray = new QueueNode[capacity];
}

CircularQueue::~CircularQueue() {
    delete[] queueArray;
    queueArray = nullptr;
}

int CircularQueue::nextIndex(int index) {
    return (index + 1) % capacity;
}

bool CircularQueue::enqueue(const string& data, void* addData) {
    if (isFull()) return false;
    
    rear = nextIndex(rear);
    queueArray[rear] = QueueNode(data, addData);
    queueSize++;
    return true;
}

QueueNode CircularQueue::dequeue() {
    if (isEmpty()) {
        return QueueNode(); // Return empty node
    }
    
    QueueNode item = queueArray[front];
    front = nextIndex(front);
    queueSize--;
    
    // Reset rear if queue becomes empty
    if (queueSize == 0) {
        rear = -1;
        front = 0;
    }
    
    return item;
}

QueueNode CircularQueue::peekFront() {
    if (isEmpty()) {
        return QueueNode(); // Return empty node
    }
    return queueArray[front];
}

QueueNode CircularQueue::peekRear() {
    if (isEmpty()) {
        return QueueNode(); // Return empty node
    }
    return queueArray[rear];
}

bool CircularQueue::isEmpty() {
    return queueSize == 0;
}

bool CircularQueue::isFull() {
    return queueSize == capacity;
}

int CircularQueue::getSize() {
    return queueSize;
}

int CircularQueue::getCapacity() {
    return capacity;
}

void CircularQueue::clear() {
    front = 0;
    rear = -1;
    queueSize = 0;
}

void CircularQueue::display() {
    if (isEmpty()) {
        cout << "Queue is empty." << endl;
        return;
    }
    
    cout << "Queue Contents (Size: " << queueSize << "): ";
    int count = 0;
    int index = front;
    
    while (count < queueSize) {
        cout << queueArray[index].data;
        if (count < queueSize - 1) {
            cout << " -> ";
        }
        index = nextIndex(index);
        count++;
    }
    cout << endl;
}

bool CircularQueue::search(const std::string& data) {
    if (isEmpty()) return false;
    
    int count = 0;
    int index = front;
    
    while (count < queueSize) {
        if (queueArray[index].data == data) {
            return true;
        }
        index = nextIndex(index);
        count++;
    }
    
    return false;
}

void CircularQueue::toArray(QueueNode* array, int& count) {
    count = queueSize;
    if (array == nullptr || isEmpty()) return;
    
    int index = front;
    for (int i = 0; i < queueSize; i++) {
        array[i] = queueArray[index];
        index = nextIndex(index);
    }
}

