#ifndef POPULATION_HOUSING_H
#define POPULATION_HOUSING_H

#include <iostream>
#include <string>
#include "core_classes/HashTable.h" 
#include "core_classes/DataLoader.h"
using namespace std;



//this class stores information about a citizen
class citizeninfo {
public:
    string cnic;
    string name;
    int age;
    string gender;
    string occupation;

    citizeninfo(string c = "", string n = "", int a = 0, string g = "", string occ = "")
        : cnic(c), name(n), age(a), gender(g), occupation(occ) {
    }
};


//this is a family node for the family tree made using n-ary tree
class famnode {
public:
    citizeninfo* citizen;
    famnode* child;   
    famnode* sibling; 

    famnode(citizeninfo* c) : citizen(c), child(nullptr), sibling(nullptr) {}
};

// ============================================
// FAMILY TREE - Ek ghar ki family ka tree
// Head of family root hoga
// ============================================

//this is basically the whole family tree structure
class famtree {
private:
    famnode* root;

    void deltree(famnode* node);
    void displaytree(famnode* node, int level);
    int countmem(famnode* node);

public:
    famtree();
    ~famtree();

    void sethead(citizeninfo* headOfFamily);

    void addnode(string parentCNIC, citizeninfo* child);
    void displayfam();
    int getmembers();
    famnode* getroot() { 
        return root; 
    }

    famnode* findNode(famnode* node, string cnic);
};

//house node class 
class housenode {
public:
    int houseNumber;
    famtree* family;
    housenode* next; // Linked list for street

    housenode(int num) : houseNumber(num), family(nullptr), next(nullptr) {
        family = new famtree();
    }

    ~housenode() {
        if (family) delete family;
    }
};

// ============================================
// STREET CLASS - Ek street jisme houses hain
// ============================================
class Street {
public:
    int streetNumber;
    housenode* houses; // Linked list of houses
    Street* next;  // N-ary tree ke liye sibling

    Street(int num) : streetNumber(num), houses(nullptr), next(nullptr) {}

    ~Street() {
        housenode* current = houses;
        while (current) {
            housenode* temp = current;
            current = current->next;
            delete temp;
        }
    }

    void addHouse(int houseNum);
    housenode* findHouse(int houseNum);
};

// ============================================
// SECTOR CLASS - G-10, F-8 jaise sectors
// ============================================
class Sector {
public:
    string sectorName;
    Street* streets; // Linked list of streets
    Sector* next;    // N-ary tree ke liye sibling

    Sector(string name) : sectorName(name), streets(nullptr), next(nullptr) {}

    ~Sector() {
        Street* current = streets;
        while (current) {
            Street* temp = current;
            current = current->next;
            delete temp;
        }
    }

    void addStreet(int streetNum);
    Street* findStreet(int streetNum);
};

// ============================================
// CITY CLASS - Islamabad city (root of tree)
// N-ary tree: City -> Sectors -> Streets -> Houses
// ============================================
class City {
public:
    string cityName;
    Sector* sectors; // Linked list of sectors

    void printHierarchy(Sector* sector, int level);

public:
    City(string name = "Islamabad");
    ~City();
    
    void addSector(string sectorName);
    Sector* findSector(string sectorName);
    void printCityHierarchy();
};

// ============================================
// POPULATION MANAGER - Main module class
// Sab kuch handle karta hai
// ============================================
class PopulationManager {
private:
    City* city;
    HashTable* citizenHash;  // Tumhari existing HashTable use karenge

public:
    PopulationManager();
    ~PopulationManager();

    // Basic operations
    void addSector(string sectorName);
    void addStreet(string sectorName, int streetNum);
    void addHouse(string sectorName, int streetNum, int houseNum);

    // Family & citizen operations
    void addFamily(string sectorName, int streetNum, int houseNum, citizeninfo* headOfFamily);
    void addFamilyMember(string parentCNIC, citizeninfo* member);

    // Search operations
    citizeninfo* searchCitizen(string cnic);
    void displayCitizenInfo(string cnic);
    bool loadPopulationFromCSV(const string& filename);
    // Reports
    void generateAgeDistribution();
    void generateGenderRatio();
    void generateOccupationBreakdown();
    void generatePopulationDensity();
    int getTotalPopulation();

    // Display functions
    void displayCityHierarchy();
    void displayAllCitizens();

    // Menu
    void displayMenu();
    void run();

    // Helper functions for reports
    void collectAllCitizens(famnode* node, citizeninfo** citizens, int& count);
    void traverseHousesForCitizens(housenode* house, citizeninfo** citizens, int& count);
    void traverseStreetsForCitizens(Street* street, citizeninfo** citizens, int& count);
    void traverseSectorsForCitizens(Sector* sector, citizeninfo** citizens, int& count);

    // Heatmap functions (bonus features)
    void displayColorHeatmap();
    void generatePopulationHeatmap();
    void displayHeatmapLegend();
    void displaySectorHeatmap();
    string getHeatmapSymbol(int population, int maxPop);
    void exportHeatmapToFile(const string& filename);

    // Get city pointer for integration with other modules
    City* getCity() { return city; }
    HashTable* getCitizenHash() { return citizenHash; }
};

#endif