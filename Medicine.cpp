#include "Medicine.h"
using namespace std;

// Default constructor
Medicine::Medicine() : medicineName(""), formula(""), price(0.0) {
}

// Parameterized constructor
Medicine::Medicine(const string& name, const string& form, double pr) 
    : medicineName(name), formula(form), price(pr >= 0.0 ? pr : 0.0) {
}

// Destructor
Medicine::~Medicine() {
}

// Getters
string Medicine::getMedicineName() const {
    return medicineName;
}

string Medicine::getFormula() const {
    return formula;
}

double Medicine::getPrice() const {
    return price;
}

// Setters
void Medicine::setMedicineName(const string& name) {
    medicineName = name;
}

void Medicine::setFormula(const string& form) {
    formula = form;
}

void Medicine::setPrice(double pr) {
    price = pr >= 0.0 ? pr : 0.0;  // Ensure non-negative
}

// Check if medicine matches a given name (case-sensitive)
bool Medicine::matchesName(const string& name) const {
    return medicineName == name;
}

// Check if medicine matches a given formula (case-sensitive)
bool Medicine::matchesFormula(const string& form) const {
    return formula == form;
}

