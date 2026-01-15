#include "SinglyLinkedList.h"
#include <iostream>
using namespace std;

// ListNode Implementation
ListNode::ListNode(const string& d, void* addData) 
    : data(d), additionalData(addData), next(nullptr) {
}

// SinglyLinkedList Implementation
SinglyLinkedList::SinglyLinkedList() : head(nullptr), tail(nullptr), listSize(0) {
}

SinglyLinkedList::~SinglyLinkedList() {
    clear();
}

void SinglyLinkedList::insertAtHead(const string& data, void* addData) {
    ListNode* newNode = new ListNode(data, addData);
    newNode->next = head;
    head = newNode;
    
    if (tail == nullptr) {
        tail = newNode;
    }
    
    listSize++;
}

void SinglyLinkedList::insertAtTail(const string& data, void* addData) {
    ListNode* newNode = new ListNode(data, addData);
    
    if (tail == nullptr) {
        head = newNode;
        tail = newNode;
    } else {
        tail->next = newNode;
        tail = newNode;
    }
    
    listSize++;
}

bool SinglyLinkedList::insertAtPosition(int position, const string& data, void* addData) {
    if (position < 0 || position > listSize) return false;
    
    if (position == 0) {
        insertAtHead(data, addData);
        return true;
    }
    
    if (position == listSize) {
        insertAtTail(data, addData);
        return true;
    }
    
    ListNode* current = head;
    for (int i = 0; i < position - 1; i++) {
        current = current->next;
    }
    
    ListNode* newNode = new ListNode(data, addData);
    newNode->next = current->next;
    current->next = newNode;
    
    listSize++;
    return true;
}

bool SinglyLinkedList::insertAfter(const string& afterValue, const string& data, void* addData) {
    ListNode* current = head;
    
    while (current != nullptr) {
        if (current->data == afterValue) {
            ListNode* newNode = new ListNode(data, addData);
            newNode->next = current->next;
            current->next = newNode;
            
            if (current == tail) {
                tail = newNode;
            }
            
            listSize++;
            return true;
        }
        current = current->next;
    }
    
    return false;
}

bool SinglyLinkedList::remove(const string& data) {
    if (head == nullptr) return false;
    
    if (head->data == data) {
        return removeFromHead();
    }
    
    ListNode* current = head;
    while (current->next != nullptr) {
        if (current->next->data == data) {
            ListNode* toDelete = current->next;
            current->next = toDelete->next;
            
            if (toDelete == tail) {
                tail = current;
            }
            
            delete toDelete;
            listSize--;
            return true;
        }
        current = current->next;
    }
    
    return false;
}

bool SinglyLinkedList::removeAtPosition(int position) {
    if (position < 0 || position >= listSize) return false;
    
    if (position == 0) {
        return removeFromHead();
    }
    
    ListNode* current = head;
    for (int i = 0; i < position - 1; i++) {
        current = current->next;
    }
    
    ListNode* toDelete = current->next;
    current->next = toDelete->next;
    
    if (toDelete == tail) {
        tail = current;
    }
    
    delete toDelete;
    listSize--;
    return true;
}

bool SinglyLinkedList::removeFromHead() {
    if (head == nullptr) return false;
    
    ListNode* toDelete = head;
    head = head->next;
    
    if (head == nullptr) {
        tail = nullptr;
    }
    
    delete toDelete;
    listSize--;
    return true;
}

bool SinglyLinkedList::removeFromTail() {
    if (head == nullptr) return false;
    
    if (head == tail) {
        delete head;
        head = nullptr;
        tail = nullptr;
        listSize--;
        return true;
    }
    
    ListNode* current = head;
    while (current->next != tail) {
        current = current->next;
    }
    
    delete tail;
    tail = current;
    tail->next = nullptr;
    listSize--;
    return true;
}

bool SinglyLinkedList::search(const string& data) {
    ListNode* current = head;
    while (current != nullptr) {
        if (current->data == data) {
            return true;
        }
        current = current->next;
    }
    return false;
}

string SinglyLinkedList::getAtPosition(int position) {
    if (position < 0 || position >= listSize) return "";
    
    ListNode* current = head;
    for (int i = 0; i < position; i++) {
        current = current->next;
    }
    
    return current->data;
}

string SinglyLinkedList::getHead() {
    if (head == nullptr) return "";
    return head->data;
}

string SinglyLinkedList::getTail() {
    if (tail == nullptr) return "";
    return tail->data;
}

bool SinglyLinkedList::updateAtPosition(int position, const string& newData) {
    if (position < 0 || position >= listSize) return false;
    
    ListNode* current = head;
    for (int i = 0; i < position; i++) {
        current = current->next;
    }
    
    current->data = newData;
    return true;
}

void SinglyLinkedList::display() {
    ListNode* current = head;
    while (current != nullptr) {
        cout << current->data;
        if (current->next != nullptr) {
            cout << " -> ";
        }
        current = current->next;
    }
    cout << endl;
}

void SinglyLinkedList::displayFormatted(const string& separator) {
    ListNode* current = head;
    bool first = true;
    while (current != nullptr) {
        if (!first) {
            cout << separator;
        }
        cout << current->data;
        first = false;
        current = current->next;
    }
    cout << endl;
}

int SinglyLinkedList::getSize() const {
    return listSize;
}

bool SinglyLinkedList::isEmpty() {
    return head == nullptr;
}

void SinglyLinkedList::clear() {
    while (head != nullptr) {
        ListNode* toDelete = head;
        head = head->next;
        delete toDelete;
    }
    tail = nullptr;
    listSize = 0;
}

void SinglyLinkedList::reverse() {
    if (head == nullptr || head->next == nullptr) return;
    
    ListNode* previous = nullptr;
    ListNode* current = head;
    ListNode* next = nullptr;
    
    tail = head;
    
    while (current != nullptr) {
        next = current->next;
        current->next = previous;
        previous = current;
        current = next;
    }
    
    head = previous;
}

void SinglyLinkedList::toArray(string* array, int& count) {
    count = listSize;
    if (array == nullptr) return;
    
    ListNode* current = head;
    int index = 0;
    while (current != nullptr) {
        array[index++] = current->data;
        current = current->next;
    }
}

