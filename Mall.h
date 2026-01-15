#ifndef MALL_H
#define MALL_H

#include <string>
#include "core_classes/HashTable.h"
#include "core_classes/SinglyLinkedList.h"
#include "Product.h"
using namespace std;

// Mall class representing a shopping mall with products
// Used for: Commercial sector mall management
class Mall {
private:
    string mallID;                  // Unique identifier (e.g., "MALL01")
    string name;                     // Mall name (e.g., "Centaurus Mall")
    string sector;                   // Sector location (e.g., "F-7", "Blue Area")
    string vertexID;                 // Graph vertex ID this mall is associated with (e.g., "Stop7")
    HashTable* productsTable;       // Hash table for fast product lookup by name (key = product name, value = Product*)
    SinglyLinkedList* allProductsList; // Linked list for iteration/category search (stores product names)
    int productCount;                // Current number of products in this mall
    
public:
    // Constructor
    // Parameters: id, name, sector, vertexID
    Mall(const string& id = "", 
         const string& n = "", 
         const string& sec = "", 
         const string& vertex = "");
    
    // Destructor
    ~Mall();
    
    // Getters
    string getMallID() const;
    string getName() const;
    string getSector() const;
    string getVertexID() const;
    int getProductCount() const;
    
    // Setters
    void setMallID(const string& id);
    void setName(const string& n);
    void setSector(const string& sec);
    void setVertexID(const string& vertex);
    
    // Product management
    // Add a product to this mall
    // Returns: true if successful, false if product already exists
    bool addProduct(Product* product);
    
    // Find a product by name (fast lookup using hash table)
    // Returns: Pointer to Product if found, nullptr otherwise
    Product* findProduct(const string& productName) const;
    
    // Find all products in a specific category
    // Parameters: category, results array (output), count (output)
    // Note: Caller must allocate results array with sufficient size
    void findProductsByCategory(const string& category, Product** results, int& count);
    
    // Display all products in this mall
    void displayProducts() const;
    
    // Display mall information
    void display() const;
    
    // Check if mall is valid
    bool isValid() const;
    
    // Get all product names as array (for iteration)
    // Parameters: names array (output), count (output)
    void getAllProductNames(string* names, int& count) const;
};

#endif // MALL_H

