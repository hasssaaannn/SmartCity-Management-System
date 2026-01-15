#include "HashTable.h"
#include <iostream>
#include <cmath>
using namespace std;

// HashNode Implementation
HashNode::HashNode(const string& k, void* d) : key(k), data(d), next(nullptr) {
}

HashNode::~HashNode() {
    // Clean up the chain recursively
    if (next != nullptr) {
        delete next;
        next = nullptr;
    }
}

// HashTable Implementation
HashTable::HashTable(int size) : tableSize(size), itemCount(0) {
    buckets = new HashNode*[tableSize];
    for (int i = 0; i < tableSize; i++) {
        buckets[i] = nullptr;
    }
}

HashTable::~HashTable() {
    clear();
    delete[] buckets;
    buckets = nullptr;
}

int HashTable::hashFunction(const std::string& key) {
    // Polynomial rolling hash function
    // Using a prime base (31) and prime modulus (tableSize)
    const int base = 31;
    long long hashValue = 0;
    long long power = 1;
    
    for (int i = 0; i < (int)key.length(); i++) {
        // Use character value directly (works for any ASCII character)
        hashValue = (hashValue + (unsigned char)key[i] * power) % tableSize;
        power = (power * base) % tableSize;
    }
    
    // Ensure positive hash value
    return (int)((hashValue + tableSize) % tableSize);
}

bool HashTable::isPrime(int num) {
    if (num < 2) return false;
    if (num == 2) return true;
    if (num % 2 == 0) return false;
    
    for (int i = 3; i * i <= num; i += 2) {
        if (num % i == 0) return false;
    }
    return true;
}

int HashTable::nextPrime(int num) {
    if (num <= 2) return 2;
    if (num % 2 == 0) num++;
    
    while (!isPrime(num)) {
        num += 2;
    }
    return num;
}

bool HashTable::insert(const std::string& key, void* data) {
    if (key.empty()) return false;
    
    int index = hashFunction(key);
    
    // Check if key already exists
    HashNode* current = buckets[index];
    while (current != nullptr) {
        if (current->key == key) {
            // Update existing key's data
            current->data = data;
            return true;
        }
        current = current->next;
    }
    
    // Insert new node at the end of the chain to maintain insertion order
    HashNode* newNode = new HashNode(key, data);
    newNode->next = nullptr;
    
    if (buckets[index] == nullptr) {
        buckets[index] = newNode;
    } else {
        HashNode* current = buckets[index];
        while (current->next != nullptr) {
            current = current->next;
        }
        current->next = newNode;
    }
    itemCount++;
    
    // Resize if load factor exceeds 0.75
    if (getLoadFactor() > 0.75) {
        resize(nextPrime(tableSize * 2));
    }
    
    return true;
}

void* HashTable::search(const std::string& key) {
    if (key.empty()) return nullptr;
    
    int index = hashFunction(key);
    HashNode* current = buckets[index];
    
    while (current != nullptr) {
        if (current->key == key) {
            return current->data;
        }
        current = current->next;
    }
    
    return nullptr;
}

bool HashTable::remove(const std::string& key) {
    if (key.empty()) return false;
    
    int index = hashFunction(key);
    HashNode* current = buckets[index];
    HashNode* previous = nullptr;
    
    while (current != nullptr) {
        if (current->key == key) {
            if (previous == nullptr) {
                // Removing first node in chain
                buckets[index] = current->next;
            } else {
                previous->next = current->next;
            }
            current->next = nullptr; // Prevent recursive deletion
            delete current;
            itemCount--;
            return true;
        }
        previous = current;
        current = current->next;
    }
    
    return false;
}

bool HashTable::isEmpty() {
    return itemCount == 0;
}

int HashTable::getSize() {
    return itemCount;
}

void HashTable::display() {
    cout << "HashTable Contents (Size: " << itemCount << "):" << endl;
    for (int i = 0; i < tableSize; i++) {
        if (buckets[i] != nullptr) {
            cout << "Bucket[" << i << "]: ";
            HashNode* current = buckets[i];
            while (current != nullptr) {
                cout << current->key << " -> ";
                current = current->next;
            }
            cout << "null" << endl;
        }
    }
}

void HashTable::clear() {
    for (int i = 0; i < tableSize; i++) {
        if (buckets[i] != nullptr) {
            delete buckets[i];
            buckets[i] = nullptr;
        }
    }
    itemCount = 0;
}

double HashTable::getLoadFactor() {
    if (tableSize == 0) return 0.0;
    return (double)itemCount / (double)tableSize;
}

void HashTable::resize(int newSize) {
    if (newSize <= tableSize) return;
    
    // Save old data
    HashNode** oldBuckets = buckets;
    int oldSize = tableSize;
    
    // Create new buckets
    tableSize = newSize;
    buckets = new HashNode*[tableSize];
    for (int i = 0; i < tableSize; i++) {
        buckets[i] = nullptr;
    }
    
    // Rehash all elements
    int oldCount = itemCount;
    itemCount = 0;
    
    for (int i = 0; i < oldSize; i++) {
        HashNode* current = oldBuckets[i];
        while (current != nullptr) {
            HashNode* next = current->next;
            current->next = nullptr;
            
            // Rehash - add at end to maintain order
            int newIndex = hashFunction(current->key);
            if (buckets[newIndex] == nullptr) {
                buckets[newIndex] = current;
            } else {
                HashNode* chainEnd = buckets[newIndex];
                while (chainEnd->next != nullptr) {
                    chainEnd = chainEnd->next;
                }
                chainEnd->next = current;
            }
            itemCount++;
            
            current = next;
        }
    }
    
    delete[] oldBuckets;
}

