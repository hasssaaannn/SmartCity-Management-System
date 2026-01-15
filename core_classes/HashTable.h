#ifndef HASHTABLE_H
#define HASHTABLE_H

#include <string>
using namespace std;

// Structure for a node in the hash table chain (separate chaining)
struct HashNode {
    string key;          // The key for hashing (e.g., BusNo, CNIC, MedicineName)
    void* data;               // Pointer to the actual data (flexible for different data types)
    HashNode* next;           // Pointer to next node in the chain
    
    HashNode(const string& k, void* d);
    ~HashNode();
};

// HashTable class implementing separate chaining collision resolution
// Used for: Buses, Citizens (CNIC lookup), Medicines, Schools, Doctors, Products
class HashTable {
private:
    HashNode** buckets;       // Array of pointers to hash chains
    int tableSize;            // Size of the hash table (should be prime number)
    int itemCount;            // Current number of items in the table
    
    // Hash function using polynomial rolling hash
    // Complexity: O(k) where k is the length of the key
    int hashFunction(const string& key);
    
    // Helper function to find the next prime number (for resizing)
    bool isPrime(int num);
    int nextPrime(int num);
    
public:
    // Constructor: Initializes hash table with given size
    // Complexity: O(1)
    HashTable(int size = 101);  // Default size 101 (prime)
    
    // Destructor: Deallocates all memory
    // Complexity: O(n) where n is total items
    ~HashTable();
    
    // Insert a key-value pair into the hash table
    // Complexity: Average O(1), Worst case O(n) if all items hash to same bucket
    bool insert(const string& key, void* data);
    
    // Search for a value by key
    // Complexity: Average O(1), Worst case O(n) if all items hash to same bucket
    void* search(const string& key);
    
    // Remove a key-value pair from the hash table
    // Complexity: Average O(1), Worst case O(n)
    bool remove(const string& key);
    
    // Check if the hash table is empty
    // Complexity: O(1)
    bool isEmpty();
    
    // Get the number of items in the hash table
    // Complexity: O(1)
    int getSize();
    
    // Display all entries in the hash table (for debugging)
    // Complexity: O(n)
    void display();
    
    // Clear all entries from the hash table
    // Complexity: O(n)
    void clear();
    
    // Get load factor of the hash table
    // Complexity: O(1)
    double getLoadFactor();
    
    // Resize the hash table (rehashing)
    // Complexity: O(n) where n is current number of items
    void resize(int newSize);
};

#endif // HASHTABLE_H

