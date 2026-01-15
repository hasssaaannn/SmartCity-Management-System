#include "Product.h"
#include <iostream>
using namespace std;

// Constructor
Product::Product(const string& id, const string& n, 
                 const string& cat, double p, const string& mall)
    : productID(id), name(n), category(cat), price(p), mallID(mall) {
}

// Destructor
Product::~Product() {
}

// Getters
string Product::getProductID() const {
    return productID;
}

string Product::getName() const {
    return name;
}

string Product::getCategory() const {
    return category;
}

double Product::getPrice() const {
    return price;
}

string Product::getMallID() const {
    return mallID;
}

// Setters
void Product::setProductID(const string& id) {
    productID = id;
}

void Product::setName(const string& n) {
    name = n;
}

void Product::setCategory(const string& cat) {
    category = cat;
}

void Product::setPrice(double p) {
    price = p;
}

void Product::setMallID(const string& mall) {
    mallID = mall;
}

// Display product information
void Product::display() const {
    cout << "Product ID: " << productID << endl;
    cout << "Name: " << name << endl;
    cout << "Category: " << category << endl;
    cout << "Price: " << price << endl;
    cout << "Mall ID: " << mallID << endl;
}

// Check if product is valid
bool Product::isValid() const {
    return !productID.empty() && !name.empty() && price >= 0.0;
}

