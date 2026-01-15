#include "Mall.h"
#include <iostream>
using namespace std;

// Constructor
Mall::Mall(const string& id, const string& n, const string& sec, const string& vertex)
    : mallID(id), name(n), sector(sec), vertexID(vertex), productCount(0) {
    // Initialize hash table with a reasonable size (prime number)
    productsTable = new HashTable(101);
    // Initialize linked list for iteration
    allProductsList = new SinglyLinkedList();
}

// Destructor
Mall::~Mall() {
    // Delete hash table
    if (productsTable != nullptr) {
        delete productsTable;
        productsTable = nullptr;
    }
    // Delete linked list
    if (allProductsList != nullptr) {
        delete allProductsList;
        allProductsList = nullptr;
    }
}

// Getters
string Mall::getMallID() const {
    return mallID;
}

string Mall::getName() const {
    return name;
}

string Mall::getSector() const {
    return sector;
}

string Mall::getVertexID() const {
    return vertexID;
}

int Mall::getProductCount() const {
    return productCount;
}

// Setters
void Mall::setMallID(const string& id) {
    mallID = id;
}

void Mall::setName(const string& n) {
    name = n;
}

void Mall::setSector(const string& sec) {
    sector = sec;
}

void Mall::setVertexID(const string& vertex) {
    vertexID = vertex;
}

// Add a product to this mall
bool Mall::addProduct(Product* product) {
    if (product == nullptr) return false;
    if (product->getName().empty()) return false;
    
    // Check if product already exists
    if (findProduct(product->getName()) != nullptr) {
        return false; // Product already exists
    }
    
    // Add to hash table
    productsTable->insert(product->getName(), (void*)product);
    
    // Add product name to linked list for iteration
    allProductsList->insertAtTail(product->getName());
    
    productCount++;
    return true;
}

// Find a product by name
Product* Mall::findProduct(const string& productName) const {
    if (productName.empty()) return nullptr;
    
    void* data = productsTable->search(productName);
    if (data != nullptr) {
        return (Product*)data;
    }
    return nullptr;
}

// Find all products in a specific category
void Mall::findProductsByCategory(const string& category, Product** results, int& count) {
    count = 0;
    if (results == nullptr || category.empty()) return;
    
    // Get all product names from linked list
    int nameCount = allProductsList->getSize();
    if (nameCount == 0) return;
    
    string* productNames = new string[nameCount];
    int actualCount = 0;
    allProductsList->toArray(productNames, actualCount);
    
    // Iterate through all products and filter by category
    for (int i = 0; i < actualCount; i++) {
        Product* product = findProduct(productNames[i]);
        if (product != nullptr && product->getCategory() == category) {
            results[count] = product;
            count++;
        }
    }
    
    delete[] productNames;
}

// Display all products in this mall
void Mall::displayProducts() const {
    if (productCount == 0) {
        cout << "No products in this mall." << endl;
        return;
    }
    
    cout << "Products in " << name << " (" << productCount << " products):" << endl;
    
    // Get all product names from linked list
    int nameCount = allProductsList->getSize();
    if (nameCount == 0) return;
    
    string* productNames = new string[nameCount];
    int actualCount = 0;
    allProductsList->toArray(productNames, actualCount);
    
    // Display each product
    for (int i = 0; i < actualCount; i++) {
        Product* product = findProduct(productNames[i]);
        if (product != nullptr) {
            cout << "  [" << (i + 1) << "] ";
            product->display();
            cout << endl;
        }
    }
    
    delete[] productNames;
}

// Display mall information
void Mall::display() const {
    cout << "Mall ID: " << mallID << endl;
    cout << "Name: " << name << endl;
    cout << "Sector: " << sector << endl;
    cout << "Vertex ID: " << vertexID << endl;
    cout << "Product Count: " << productCount << endl;
}

// Check if mall is valid
bool Mall::isValid() const {
    return !mallID.empty() && !name.empty() && !vertexID.empty();
}

// Get all product names as array
void Mall::getAllProductNames(string* names, int& count) const {
    count = allProductsList->getSize();
    if (names == nullptr || count == 0) return;
    
    allProductsList->toArray(names, count);
}

