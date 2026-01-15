#ifndef PRODUCT_H
#define PRODUCT_H

#include <string>
using namespace std;

// Product class representing a single product/item in a mall
// Used for: Commercial sector product management
class Product {
private:
    string productID;      // Unique identifier (e.g., "PRD001")
    string name;            // Product name (e.g., "Laptop", "Shirt")
    string category;        // Product category (e.g., "Electronics", "Clothing")
    double price;                // Product price
    string mallID;          // Which mall this product belongs to
    
public:
    // Constructor
    // Parameters: id, name, category, price, mallID
    Product(const string& id = "", 
            const string& n = "", 
            const string& cat = "", 
            double p = 0.0, 
            const string& mall = "");
    
    // Destructor
    ~Product();
    
    // Getters
    string getProductID() const;
    string getName() const;
    string getCategory() const;
    double getPrice() const;
    string getMallID() const;
    
    // Setters (optional - for flexibility)
    void setProductID(const string& id);
    void setName(const string& name);
    void setCategory(const string& category);
    void setPrice(double price);
    void setMallID(const string& mallID);
    
    // Display product information
    void display() const;
    
    // Check if product is valid (has required fields)
    bool isValid() const;
};

#endif // PRODUCT_H

