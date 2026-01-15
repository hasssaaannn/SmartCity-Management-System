#include "PopulationHousing.h"
#include <iostream>
#include <string>
#include <fstream>
using namespace std;

//family tree implementation 
//----------------------------
famtree::famtree() : root(nullptr) {}

famtree::~famtree() { 
    deltree(root);
}


void famtree::deltree(famnode* node) {
    if (!node) {
        return;
    }

    deltree(node->child);
    deltree(node->sibling);

    delete node->citizen;
    delete node;

}

void famtree::sethead(citizeninfo* headfam) {
    if (root) {
        deltree(root);
    }

    root = new famnode(headfam);
}

void famtree::addnode(string parcnic, citizeninfo* child) {
    famnode* parent = findNode(root, parcnic);

    if (!parent) {
        return;
    }

    famnode* tempchild = new famnode(child);
    if (!parent->child)
        parent->child = tempchild;
    else {
        famnode* curr = parent->child;
        while (curr->sibling) {
            curr = curr->sibling;
        }

        curr->sibling = tempchild;

    }
}


void famtree::displaytree(famnode* node, int level) {

    if (!node) {
        return;
    }

    for (int i = 0; i < level; ++i) {

        cout << "  ";
    }

    cout << "- " << node->citizen->name << " (" << node->citizen->cnic << ")\n";
    displaytree(node->child, level + 1);
    displaytree(node->sibling, level);
}

void famtree::displayfam() { 
    displaytree(root, 0); 
}

int famtree::countmem(famnode* node) {
    if (!node) {
        return 0;
    }
    int cnt = 1;

    cnt += countmem(node->child);
    cnt += countmem(node->sibling);

    return cnt;

}

int famtree::getmembers() { 
    return countmem(root); 
}



famnode* famtree::findNode(famnode* node, string cnic) {
    if (!node) {
        return nullptr;
    }

    if (node->citizen->cnic == cnic) {
        return node;
    }

    famnode* found = findNode(node->child, cnic);
    if (found) {
        return found;
    }
    return findNode(node->sibling, cnic);
}


//Street implementation class
void Street::addHouse(int houseNum) {
    if (findHouse(houseNum)) {
        return;
    }
    housenode* temphouse = new housenode(houseNum);
    if (!houses) houses = temphouse;
    else {
        housenode* curr = houses;
        while (curr->next) curr = curr->next;
        curr->next = temphouse;
    }
}

housenode* Street::findHouse(int houseNum) {
    housenode* curr = houses;
    while (curr) {
        if (curr->houseNumber == houseNum) {
            return curr;
        }
        curr = curr->next;
    }
    return nullptr;
}

// -------------------------- Sector Implementation --------------------------
void Sector::addStreet(int streetNum) {
    if (findStreet(streetNum)) return;
    Street* newStreet = new Street(streetNum);
    if (!streets) streets = newStreet;
    else {
        Street* curr = streets;
        while (curr->next) curr = curr->next;
        curr->next = newStreet;
    }
}

Street* Sector::findStreet(int streetNum) {
    Street* curr = streets;
    while (curr) {
        if (curr->streetNumber == streetNum) return curr;
        curr = curr->next;
    }
    return nullptr;
}

// -------------------------- City Implementation --------------------------
City::City(string name) : cityName(name), sectors(nullptr) {}

City::~City() {
    Sector* curr = sectors;
    while (curr) {
        Sector* temp = curr;
        curr = curr->next;
        delete temp;
    }
}

void City::addSector(string sectorName) {
    if (findSector(sectorName)) return;
    Sector* newSector = new Sector(sectorName);
    if (!sectors) sectors = newSector;
    else {
        Sector* curr = sectors;
        while (curr->next) curr = curr->next;
        curr->next = newSector;
    }
}

Sector* City::findSector(string sectorName) {
    Sector* curr = sectors;
    while (curr) {
        if (curr->sectorName == sectorName) return curr;
        curr = curr->next;
    }
    return nullptr;
}

void City::printHierarchy(Sector* sector, int level) {
    if (!sector) return;
    for (int i = 0; i < level; ++i) cout << "  ";
    cout << "[Sector] " << sector->sectorName << endl;
    Street* st = sector->streets;
    while (st) {
        for (int i = 0; i < level + 1; ++i) cout << "  ";
        cout << "[Street #" << st->streetNumber << "]\n";
        housenode* hs = st->houses;
        while (hs) {
            for (int i = 0; i < level + 2; ++i) cout << "  ";
            cout << "[House #" << hs->houseNumber << "]";
            if (hs->family && hs->family->getroot()) {
                cout << " - Family Head: " << hs->family->getroot()->citizen->name;
            }
            cout << endl;
            hs = hs->next;
        }
        st = st->next;
    }
    printHierarchy(sector->next, level);
}

void City::printCityHierarchy() {
    cout << cityName << " City Population Structure:\n";
    printHierarchy(sectors, 0);
}

// -------------------------- PopulationManager Implementation --------------------------
PopulationManager::PopulationManager() {
    city = new City("Islamabad");
    citizenHash = new HashTable(101);
}

PopulationManager::~PopulationManager() {
    delete city;
    delete citizenHash;
}

void PopulationManager::addSector(string sectorName) {
    city->addSector(sectorName);
}

void PopulationManager::addStreet(string sectorName, int streetNum) {
    Sector* s = city->findSector(sectorName);
    if (!s) return;
    s->addStreet(streetNum);
}

void PopulationManager::addHouse(string sectorName, int streetNum, int houseNum) {
    Sector* s = city->findSector(sectorName);
    if (!s) return;
    Street* st = s->findStreet(streetNum);
    if (!st) return;
    st->addHouse(houseNum);
}

void PopulationManager::addFamily(string sectorName, int streetNum, int houseNum, citizeninfo* headOfFamily) {
    Sector* s = city->findSector(sectorName);
    if (!s) return;
    Street* st = s->findStreet(streetNum);
    if (!st) return;
    housenode* house = st->findHouse(houseNum);
    if (!house) return;
    house->family->sethead(headOfFamily);
    citizenHash->insert(headOfFamily->cnic, headOfFamily);
}

void PopulationManager::addFamilyMember(string parentCNIC, citizeninfo* member) {
    // Traverse all houses to find parent and add child
    Sector* curr_sector = city->sectors;
    while (curr_sector) {
        Street* st = curr_sector->streets;
        while (st) {
            housenode* h = st->houses;
            while (h) {
                h->family->addnode(parentCNIC, member);
                h = h->next;
            }
            st = st->next;
        }
        curr_sector = curr_sector->next;
    }
    citizenHash->insert(member->cnic, member);
}

citizeninfo* PopulationManager::searchCitizen(string cnic) {
    return static_cast<citizeninfo*>(citizenHash->search(cnic));
}

void PopulationManager::displayCitizenInfo(string cnic) {
    citizeninfo* cit = searchCitizen(cnic);
    if (!cit) {
        cout << "Citizen not found!\n";
        return;
    }
    cout << "CNIC: " << cit->cnic << ", Name: " << cit->name << ", Age: " << cit->age
        << ", Gender: " << cit->gender << ", Occupation: " << cit->occupation << endl;
}

void PopulationManager::displayCityHierarchy() {
    city->printCityHierarchy();
}

void PopulationManager::displayAllCitizens() {
    citizenHash->display();
}

// Helper: collect all citizens from family tree
void PopulationManager::collectAllCitizens(famnode* node, citizeninfo** citizens, int& count) {
    if (!node) return;
    citizens[count++] = node->citizen;
    collectAllCitizens(node->child, citizens, count);
    collectAllCitizens(node->sibling, citizens, count);
}

void PopulationManager::traverseHousesForCitizens(housenode* house, citizeninfo** citizens, int& count) {
    while (house) {
        if (house->family && house->family->getroot())
            collectAllCitizens(house->family->getroot(), citizens, count);
        house = house->next;
    }
}

void PopulationManager::traverseStreetsForCitizens(Street* street, citizeninfo** citizens, int& count) {
    while (street) {
        traverseHousesForCitizens(street->houses, citizens, count);
        street = street->next;
    }
}

void PopulationManager::traverseSectorsForCitizens(Sector* sector, citizeninfo** citizens, int& count) {
    while (sector) {
        traverseStreetsForCitizens(sector->streets, citizens, count);
        sector = sector->next;
    }
}

// Report: Age Distribution
void PopulationManager::generateAgeDistribution() {
    const int MAX_POP = 10000;
    citizeninfo* citizens[MAX_POP];
    int count = 0;

    Sector* curr_sector = city->sectors;
    traverseSectorsForCitizens(curr_sector, citizens, count);

    int under18 = 0, adults = 0, seniors = 0;
    for (int i = 0; i < count; ++i) {
        if (citizens[i]->age < 18) under18++;
        else if (citizens[i]->age < 60) adults++;
        else seniors++;
    }
    cout << "Age Distribution:\n";
    cout << "Under 18: " << under18 << ", Adults (18-59): " << adults << ", Seniors (60+): " << seniors << endl;
}

// Report: Gender Ratio
void PopulationManager::generateGenderRatio() {
    const int MAX_POP = 10000;
    citizeninfo* citizens[MAX_POP];
    int count = 0;

    Sector* curr_sector = city->sectors;
    traverseSectorsForCitizens(curr_sector, citizens, count);

    int male = 0, female = 0, other = 0;
    for (int i = 0; i < count; ++i) {
        if (citizens[i]->gender == "Male") male++;
        else if (citizens[i]->gender == "Female") female++;
        else other++;
    }
    cout << "Gender Ratio:\n";
    cout << "Males: " << male << ", Females: " << female << ", Others: " << other << endl;
}

// Report: Occupation Breakdown
void PopulationManager::generateOccupationBreakdown() {
    const int MAX_POP = 10000;
    const int MAX_OCCUPATIONS = 100;
    citizeninfo* citizens[MAX_POP];
    int count = 0;

    Sector* curr_sector = city->sectors;
    traverseSectorsForCitizens(curr_sector, citizens, count);

    // Simple occupation counting
    string occupations[MAX_OCCUPATIONS];
    int occ_counts[MAX_OCCUPATIONS];
    int occ_total = 0;

    for (int i = 0; i < count; ++i) {
        string curr_occ = citizens[i]->occupation;
        bool found = false;
        for (int j = 0; j < occ_total; ++j) {
            if (occupations[j] == curr_occ) {
                occ_counts[j]++;
                found = true;
                break;
            }
        }
        if (!found && occ_total < MAX_OCCUPATIONS) {
            occupations[occ_total] = curr_occ;
            occ_counts[occ_total] = 1;
            occ_total++;
        }
    }

    cout << "Occupation Breakdown:\n";
    for (int i = 0; i < occ_total; ++i) {
        cout << occupations[i] << " : " << occ_counts[i] << endl;
    }
}

// Report: Population Density per Sector
void PopulationManager::generatePopulationDensity() {
    cout << "Population Density per Sector:\n";
    Sector* s = city->sectors;
    while (s) {
        int sectorCount = 0;
        Street* st = s->streets;
        while (st) {
            housenode* h = st->houses;
            while (h) {
                sectorCount += h->family->getmembers();
                h = h->next;
            }
            st = st->next;
        }
        cout << "Sector " << s->sectorName << " : " << sectorCount << " people\n";
        s = s->next;
    }
}

int PopulationManager::getTotalPopulation() {
    int total = 0;
    Sector* s = city->sectors;
    while (s) {
        Street* st = s->streets;
        while (st) {
            housenode* h = st->houses;
            while (h) {
                total += h->family->getmembers();
                h = h->next;
            }
            st = st->next;
        }
        s = s->next;
    }
    return total;
}

// ============================================
// HEATMAP FUNCTIONS (BONUS FEATURES)
// ============================================

void PopulationManager::displayColorHeatmap() {
    // ANSI color codes for Windows console
    const char* COLOR_RED_BG = "\033[41m";      // Red background
    const char* COLOR_YELLOW_BG = "\033[43m";   // Yellow background
    const char* COLOR_GREEN_BG = "\033[42m";    // Green background
    const char* COLOR_BLUE_BG = "\033[44m";     // Blue background
    const char* COLOR_CYAN_BG = "\033[46m";     // Cyan background
    const char* COLOR_RESET = "\033[0m";        // Reset color

    cout << "\n========================================\n";
    cout << "             POPULATION HEATMAP\n";
    cout << "========================================\n";

    if (!city->sectors) {
        cout << "No sectors available for heatmap.\n";
        return;
    }

    const int MAX_SECTORS = 100;
    string sectorNames[MAX_SECTORS];
    int sectorPops[MAX_SECTORS];
    int sectorCount = 0;
    int maxPop = 0;
    int totalPop = 0;

    Sector* tempSector = city->sectors;
    while (tempSector && sectorCount < MAX_SECTORS) {
        int pop = 0;
        Street* tempStreet = tempSector->streets;
        while (tempStreet) {
            housenode* tempHouse = tempStreet->houses;
            while (tempHouse) {
                pop += tempHouse->family->getmembers();
                tempHouse = tempHouse->next;
            }
            tempStreet = tempStreet->next;
        }

        sectorNames[sectorCount] = tempSector->sectorName;
        sectorPops[sectorCount] = pop;
        totalPop += pop;

        if (pop > maxPop) maxPop = pop;

        sectorCount++;
        tempSector = tempSector->next;
    }

    if (maxPop == 0) {
        cout << "No population data available.\n";
        return;
    }

    cout << "\nColor Legend:\n";
    cout << COLOR_RED_BG << "   " << COLOR_RESET << " Very High (80-100%)\n";
    cout << COLOR_YELLOW_BG << "   " << COLOR_RESET << " High (60-80%)\n";
    cout << COLOR_GREEN_BG << "   " << COLOR_RESET << " Medium (40-60%)\n";
    cout << COLOR_BLUE_BG << "   " << COLOR_RESET << " Low (20-40%)\n";
    cout << COLOR_CYAN_BG << "   " << COLOR_RESET << " Very Low (0-20%)\n";
    cout << "\n";

    cout << "Sector Name          Population  Density\n";
    cout << "-------------------------------------------\n";

    for (int i = 0; i < sectorCount; i++) {
        double percentage = (double)sectorPops[i] / maxPop * 100.0;

        const char* colorCode = COLOR_RESET;
        if (percentage >= 80.0) colorCode = COLOR_RED_BG;
        else if (percentage >= 60.0) colorCode = COLOR_YELLOW_BG;
        else if (percentage >= 40.0) colorCode = COLOR_GREEN_BG;
        else if (percentage >= 20.0) colorCode = COLOR_BLUE_BG;
        else colorCode = COLOR_CYAN_BG;

        cout << colorCode << " " << sectorNames[i];
        for (int j = (int)sectorNames[i].length(); j < 18; j++) {
            cout << " ";
        }
        cout << "  " << sectorPops[i];
        for (int j = 0; j < 10 - (sectorPops[i] >= 10 ? 2 : 1); j++) cout << " ";
        cout << (int)percentage << "%" << COLOR_RESET << "\n";
    }

    cout << "-------------------------------------------\n";
    cout << "Total Population: " << totalPop << "\n";
    cout << "========================================\n";
}

string PopulationManager::getHeatmapSymbol(int population, int maxPop) {
    if (maxPop == 0 || population == 0) return "     ";

    double percentage = (double)population / maxPop * 100.0;

    if (percentage >= 80.0) return "*****";
    else if (percentage >= 60.0) return "**** ";
    else if (percentage >= 40.0) return "***  ";
    else if (percentage >= 20.0) return "**   ";
    else if (percentage > 0.0) return "*    ";
    else return "     ";
}

void PopulationManager::displayHeatmapLegend() {
    cout << "\n========================================\n";
    cout << "     POPULATION DENSITY LEGEND\n";
    cout << "========================================\n";
    cout << "***** = Very High (80-100%)\n";
    cout << "****  = High (60-80%)\n";
    cout << "***   = Medium (40-60%)\n";
    cout << "**    = Low (20-40%)\n";
    cout << "*     = Very Low (0-20%)\n";
    cout << "      = Empty (0%)\n";
    cout << "========================================\n";
}

void PopulationManager::displaySectorHeatmap() {
    cout << "\n========================================\n";
    cout << "   POPULATION DENSITY HEATMAP\n";
    cout << "========================================\n";

    if (!city->sectors) {
        cout << "No sectors available for heatmap.\n";
        return;
    }

    const int MAX_SECTORS = 100;
    string sectorNames[MAX_SECTORS];
    int sectorPops[MAX_SECTORS];
    int sectorCount = 0;
    int maxPop = 0;
    int totalPop = 0;

    Sector* s = city->sectors;
    while (s && sectorCount < MAX_SECTORS) {
        int pop = 0;
        Street* st = s->streets;
        while (st) {
            housenode* h = st->houses;
            while (h) {
                pop += h->family->getmembers();
                h = h->next;
            }
            st = st->next;
        }

        sectorNames[sectorCount] = s->sectorName;
        sectorPops[sectorCount] = pop;
        totalPop += pop;

        if (pop > maxPop) maxPop = pop;

        sectorCount++;
        s = s->next;
    }

    if (maxPop == 0) {
        cout << "No population data available for heatmap.\n";
        return;
    }

    // ANSI color codes
    const char* COLOR_RED = "\033[91m";      // Bright red text
    const char* COLOR_YELLOW = "\033[93m";   // Bright yellow text
    const char* COLOR_GREEN = "\033[92m";    // Bright green text
    const char* COLOR_BLUE = "\033[94m";     // Bright blue text
    const char* COLOR_CYAN = "\033[96m";     // Bright cyan text
    const char* COLOR_RESET = "\033[0m";     // Reset color

    cout << "\nSector Name          | Population | Density Bar\n";
    cout << "---------------------+------------+------------------\n";

    for (int i = 0; i < sectorCount; i++) {
        double percentage = (double)sectorPops[i] / maxPop * 100.0;

        const char* colorCode = COLOR_RESET;
        if (percentage >= 80.0) colorCode = COLOR_RED;
        else if (percentage >= 60.0) colorCode = COLOR_YELLOW;
        else if (percentage >= 40.0) colorCode = COLOR_GREEN;
        else if (percentage >= 20.0) colorCode = COLOR_BLUE;
        else colorCode = COLOR_CYAN;

        cout << sectorNames[i];
        for (int j = (int)sectorNames[i].length(); j < 20; j++) cout << " ";

        cout << " | ";
        if (sectorPops[i] < 10) cout << "   ";
        else if (sectorPops[i] < 100) cout << "  ";
        else if (sectorPops[i] < 1000) cout << " ";
        cout << sectorPops[i] << "      ";

        cout << " | " << colorCode << getHeatmapSymbol(sectorPops[i], maxPop) << COLOR_RESET;

        cout << " (" << (int)percentage << "%)\n";
    }

    cout << "---------------------+------------+------------------\n";
    cout << "Total Population: " << totalPop << " | Max: " << maxPop << "\n";
    cout << "========================================\n";
}

void PopulationManager::generatePopulationHeatmap() {
    displayHeatmapLegend();
    displaySectorHeatmap();

    cout << "\n--- Density Analysis ---\n";

    const int MAX_SECTORS = 100;
    string sectorNames[MAX_SECTORS];
    int sectorPops[MAX_SECTORS];
    int sectorCount = 0;

    Sector* s = city->sectors;
    while (s && sectorCount < MAX_SECTORS) {
        int pop = 0;
        Street* st = s->streets;
        while (st) {
            housenode* h = st->houses;
            while (h) {
                pop += h->family->getmembers();
                h = h->next;
            }
            st = st->next;
        }

        sectorNames[sectorCount] = s->sectorName;
        sectorPops[sectorCount] = pop;
        sectorCount++;
        s = s->next;
    }

    if (sectorCount == 0) {
        cout << "No sectors available.\n";
        return;
    }

    int maxIdx = 0, minIdx = 0;
    for (int i = 1; i < sectorCount; i++) {
        if (sectorPops[i] > sectorPops[maxIdx]) maxIdx = i;
        if (sectorPops[i] < sectorPops[minIdx]) minIdx = i;
    }

    cout << "Most Populated: " << sectorNames[maxIdx]
        << " (" << sectorPops[maxIdx] << " people)\n";
    cout << "Least Populated: " << sectorNames[minIdx]
        << " (" << sectorPops[minIdx] << " people)\n";

    int total = 0;
    for (int i = 0; i < sectorCount; i++) {
        total += sectorPops[i];
    }
    double avg = (double)total / sectorCount;
    cout << "Average per Sector: " << (int)avg << " people\n";

    int veryhigh = 0, high = 0, medium = 0, low = 0, verylow = 0, empty = 0;
    int maxPop = sectorPops[maxIdx];
    for (int i = 0; i < sectorCount; i++) {
        double percentage = (double)sectorPops[i] / maxPop * 100.0;

        if (percentage >= 80.0) veryhigh++;
        else if (percentage >= 60.0) high++;
        else if (percentage >= 40.0) medium++;
        else if (percentage >= 20.0) low++;
        else if (percentage > 0.0) verylow++;
        else empty++;
    }

    cout << "\nDensity Distribution:\n";
    cout << "  Very High Density: " << veryhigh << " sectors\n";
    cout << "  High Density: " << high << " sectors\n";
    cout << "  Medium Density: " << medium << " sectors\n";
    cout << "  Low Density: " << low << " sectors\n";
    cout << "  Very Low Density: " << verylow << " sectors\n";
    cout << "  Empty: " << empty << " sectors\n";
}

void PopulationManager::exportHeatmapToFile(const string& filename) {
    cout << "\n=== Exporting Heatmap to File ===\n";
    cout << "Exporting to: " << filename << "\n";

    const int MAX_SECTORS = 100;
    string sectorNames[MAX_SECTORS];
    int sectorPops[MAX_SECTORS];
    int sectorCount = 0;
    int maxPop = 0;

    Sector* s = city->sectors;
    while (s && sectorCount < MAX_SECTORS) {
        int pop = 0;
        Street* st = s->streets;
        while (st) {
            housenode* h = st->houses;
            while (h) {
                pop += h->family->getmembers();
                h = h->next;
            }
            st = st->next;
        }

        sectorNames[sectorCount] = s->sectorName;
        sectorPops[sectorCount] = pop;
        if (pop > maxPop) maxPop = pop;

        sectorCount++;
        s = s->next;
    }

    if (sectorCount == 0) {
        cout << "[ERROR] No sectors available for export.\n";
        return;
    }

    // Open file for writing
    ofstream outFile(filename.c_str());
    if (!outFile.is_open()) {
        cout << "[ERROR] Failed to open file: " << filename << "\n";
        cout << "Make sure the directory exists and you have write permissions.\n";
        return;
    }

    // Write CSV header
    outFile << "Sector,Population,DensityLevel,DensityPercentage\n";

    // Write data rows
    for (int i = 0; i < sectorCount; i++) {
        double percentage = maxPop > 0 ? (double)sectorPops[i] / maxPop * 100.0 : 0;

        string level;
        if (percentage >= 80.0) level = "VeryHigh";
        else if (percentage >= 60.0) level = "High";
        else if (percentage >= 40.0) level = "Medium";
        else if (percentage >= 20.0) level = "Low";
        else if (percentage > 0.0) level = "VeryLow";
        else level = "Empty";

        // Write to file
        outFile << sectorNames[i] << "," << sectorPops[i] << "," << level << "," << (int)percentage << "\n";
    }

    // Close file
    outFile.close();

    cout << "[SUCCESS] Heatmap exported successfully!\n";
    cout << "File: " << filename << "\n";
    cout << "Records exported: " << sectorCount << " sectors\n";
    cout << "\nFile Format:\n";
    cout << "  Column 1: Sector Name\n";
    cout << "  Column 2: Population Count\n";
    cout << "  Column 3: Density Level (VeryHigh/High/Medium/Low/VeryLow/Empty)\n";
    cout << "  Column 4: Density Percentage (0-100)\n";
}

// Menu
void PopulationManager::displayMenu() {
    cout << "==== Population & Housing Menu ====\n";
    cout << "1. Add Sector\n2. Add Street\n3. Add House\n4. Add Family\n5. Add Family Member\n"
        << "6. Print City Hierarchy\n7. Search Citizen by CNIC\n8. Generate Age Report\n"
        << "9. Generate Gender Ratio\n10. Occupation Breakdown\n11. Population Density\n12. Total Population\n0. Exit\n";
}

void PopulationManager::run() {
    int choice = -1;

    // Declare ALL variables BEFORE switch statement
    string sector;
    int sn, hn;
    string cnic, name, gender, occupation;
    int age;
    string mcnic;

    while (choice != 0) {
        displayMenu();
        cout << "Enter choice: ";
        cin >> choice;

        if (!cin) {
            cin.clear();
            cin.ignore(10000, '\n');
            continue;
        }

        switch (choice) {
        case 1:
            cout << "Sector name: ";
            cin >> sector;
            addSector(sector);
            cout << "Sector added successfully!\n";
            break;

        case 2:
            cout << "Sector name: ";
            cin >> sector;
            cout << "Street #: ";
            cin >> sn;
            addStreet(sector, sn);
            cout << "Street added successfully!\n";
            break;

        case 3:
            cout << "Sector name: ";
            cin >> sector;
            cout << "Street #: ";
            cin >> sn;
            cout << "House #: ";
            cin >> hn;
            addHouse(sector, sn, hn);
            cout << "House added successfully!\n";
            break;

        case 4:
            cout << "Sector name: ";
            cin >> sector;
            cout << "Street #: ";
            cin >> sn;
            cout << "House #: ";
            cin >> hn;
            cout << "Family head CNIC: ";
            cin >> cnic;
            cout << "Name: ";
            cin.ignore();
            getline(cin, name);
            cout << "Age: ";
            cin >> age;
            cout << "Gender: ";
            cin >> gender;
            cout << "Occupation: ";
            cin.ignore();
            getline(cin, occupation);
            addFamily(sector, sn, hn, new citizeninfo(cnic, name, age, gender, occupation));
            cout << "Family added successfully!\n";
            break;

        case 5:
            cout << "Parent CNIC: ";
            cin >> cnic;
            cout << "Member CNIC: ";
            cin >> mcnic;
            cout << "Name: ";
            cin.ignore();
            getline(cin, name);
            cout << "Age: ";
            cin >> age;
            cout << "Gender: ";
            cin >> gender;
            cout << "Occupation: ";
            cin.ignore();
            getline(cin, occupation);
            addFamilyMember(cnic, new citizeninfo(mcnic, name, age, gender, occupation));
            cout << "Family member added successfully!\n";
            break;

        case 6:
            displayCityHierarchy();
            break;

        case 7:
            cout << "CNIC: ";
            cin >> cnic;
            displayCitizenInfo(cnic);
            break;

        case 8:
            generateAgeDistribution();
            break;

        case 9:
            generateGenderRatio();
            break;

        case 10:
            generateOccupationBreakdown();
            break;

        case 11:
            generatePopulationDensity();
            break;

        case 12:
            cout << "Total Population: " << getTotalPopulation() << " people\n";
            break;

        case 0:
            cout << "Exiting...\n";
            break;

        default:
            cout << "Invalid option!\n";
        }
    }
}

// -------------------------- HELPER FUNCTION --------------------------
// Helper function to load sample data for testing
void loadSampleData(PopulationManager* pm) {
    cout << "\n=== Loading Sample Data ===\n";

    // Add sectors
    pm->addSector("G-10");
    pm->addSector("F-8");
    pm->addSector("G-9");
    pm->addSector("F-6");
    pm->addSector("Blue Area");

    // Add streets
    pm->addStreet("G-10", 22);
    pm->addStreet("F-8", 5);
    pm->addStreet("G-9", 17);
    pm->addStreet("F-6", 9);
    pm->addStreet("Blue Area", 1);

    // Add houses
    pm->addHouse("G-10", 22, 180);
    pm->addHouse("F-8", 5, 12);
    pm->addHouse("G-9", 17, 90);
    pm->addHouse("F-6", 9, 33);
    pm->addHouse("Blue Area", 1, 5);

    // Add families (sample data)
    pm->addFamily("G-10", 22, 180, new citizeninfo("61101-1111111-1", "Ahmed Khan", 45, "Male", "Engineer"));
    pm->addFamily("F-8", 5, 12, new citizeninfo("61101-2222222-2", "Fatima Zahra", 38, "Female", "Teacher"));
    pm->addFamily("G-9", 17, 90, new citizeninfo("61101-3333333-3", "Ali Raza", 29, "Male", "Doctor"));
    pm->addFamily("F-6", 9, 33, new citizeninfo("61101-4444444-4", "Sara Malik", 22, "Female", "Student"));
    pm->addFamily("Blue Area", 1, 5, new citizeninfo("61101-5555555-5", "Hamza Noor", 50, "Male", "Business"));

    // Add family members
    pm->addFamilyMember("61101-1111111-1", new citizeninfo("61101-1111111-2", "Ayesha Khan", 20, "Female", "Student"));
    pm->addFamilyMember("61101-1111111-1", new citizeninfo("61101-1111111-3", "Bilal Khan", 18, "Male", "Student"));
    pm->addFamilyMember("61101-2222222-2", new citizeninfo("61101-2222222-3", "Zain Zahra", 15, "Male", "Student"));

    cout << "[OK] Sample data loaded: 5 sectors, 5 houses, 8 citizens\n\n";
}


// -------------------------- CSV Data Loading --------------------------
bool PopulationManager::loadPopulationFromCSV(const string& filename) {
    const int MAX_ROWS = 1000;
    CSVRow populationData[MAX_ROWS];
    int actualRows = 0;

    DataLoader loader;

    cout << "Loading population data from: " << filename << endl;

    if (!loader.loadPopulation(filename, populationData, MAX_ROWS, actualRows)) {
        cout << "Failed to load population data!" << endl;
        return false;
    }

    cout << "Processing " << actualRows << " population records..." << endl;

    // CSV Format: CNIC, Name, Gender, Age, Sector, Street, HouseNo, Occupation
    for (int i = 0; i < actualRows; i++) {
        CSVRow& row = populationData[i];

        if (row.fieldCount < 8) {
            cout << "Skipping incomplete row " << (i + 1) << " (expected 8 fields, got " << row.fieldCount << ")" << endl;
            continue;
        }

        // Extract data from CSV row according to new format
        string cnic = row.fields[0];      // CNIC
        string name = row.fields[1];      // Name
        string gender = row.fields[2];    // Gender 
        int age = 0;

        // Convert age string to int (manual conversion)
        // Age is now at position 3 (was position 2)
        for (size_t j = 0; j < row.fields[3].length(); j++) {
            if (row.fields[3][j] >= '0' && row.fields[3][j] <= '9') {
                age = age * 10 + (row.fields[3][j] - '0');
            }
        }

        string sector = row.fields[4];    // Sector
        int streetNum = 0;

        // Convert street to int
        // Street is now at position 5 
        for (size_t j = 0; j < row.fields[5].length(); j++) {
            if (row.fields[5][j] >= '0' && row.fields[5][j] <= '9') {
                streetNum = streetNum * 10 + (row.fields[5][j] - '0');
            }
        }

        int houseNum = 0;

        // Convert house to int
        // HouseNo is now at position 6
        for (size_t j = 0; j < row.fields[6].length(); j++) {
            if (row.fields[6][j] >= '0' && row.fields[6][j] <= '9') {
                houseNum = houseNum * 10 + (row.fields[6][j] - '0');
            }
        }

        string occupation = row.fields[7]; // Occupation (now at position 7)

        // Add sector if doesn't exist
        if (!city->findSector(sector)) {
            addSector(sector);
        }

        // Add street if doesn't exist
        Sector* sec = city->findSector(sector);
        if (sec && !sec->findStreet(streetNum)) {
            addStreet(sector, streetNum);
        }

        // Add house if doesn't exist
        if (sec) {
            Street* st = sec->findStreet(streetNum);
            if (st && !st->findHouse(houseNum)) {
                addHouse(sector, streetNum, houseNum);
            }

            // Add family (as family head for now - you can enhance this later)
            if (st) {
                housenode* house = st->findHouse(houseNum);
                if (house && house->family->getroot() == nullptr) {
                    // House is empty, add as family head
                    addFamily(sector, streetNum, houseNum,
                        new citizeninfo(cnic, name, age, gender, occupation));
                }
                else {
                    // House has family, add as member
                    // For simplicity, we'll skip or you can enhance to detect parent CNIC
                    cout << "Note: House " << houseNum << " already has a family. Skipping " << name << endl;
                }
            }
        }
    }

    cout << "Population data loaded successfully!" << endl;
    cout << "Total citizens: " << citizenHash->getSize() << endl;
    return true;
}



//int main() {
//    cout << "\n================================================\n";
//    cout << "   POPULATION & HOUSING MODULE - TESTING\n";
//    cout << "   Smart City Project - Islamabad\n";
//    cout << "================================================\n";
//
//    PopulationManager* pm = new PopulationManager();
//
//    int choice;
//    cout << "\nSelect Mode:\n";
//    cout << "1. Load Sample Data & Test\n";
//    cout << "2. Load from CSV File\n";
//    cout << "3. Start Empty (Manual Entry)\n";
//    cout << "Enter choice: ";
//    cin >> choice;
//
//    if (choice == 1) {
//        loadSampleData(pm);
//
//        // Quick automated tests
//        cout << "--- Testing Features ---\n\n";
//
//        cout << "1. City Hierarchy:\n";
//        pm->displayCityHierarchy();
//
//        cout << "\n2. Search Test (Ahmed Khan):\n";
//        pm->displayCitizenInfo("61101-1111111-1");
//
//        cout << "\n3. Age Distribution:\n";
//        pm->generateAgeDistribution();
//
//        cout << "\n4. Gender Ratio:\n";
//        pm->generateGenderRatio();
//
//        cout << "\n5. Population Density:\n";
//        pm->generatePopulationDensity();
//
//        cout << "\n--- Tests Complete! ---\n";
//    }
//    else if (choice == 2) {
//        string filename;
//        cout << "Enter CSV filename (e.g., population.csv): ";
//        cin >> filename;
//
//        if (pm->loadPopulationFromCSV(filename)) {
//            cout << "\n--- Quick Test of Loaded Data ---\n";
//            pm->displayCityHierarchy();
//            cout << "\nAge Distribution:\n";
//            pm->generateAgeDistribution();
//            cout << "\nGender Ratio:\n";
//            pm->generateGenderRatio();
//        }
//        else {
//            cout << "Failed to load CSV. Starting with empty data.\n";
//        }
//    }
//
//    // Interactive menu
//    cout << "\nStarting Interactive Menu...\n";
//    pm->run();
//
//    delete pm;
//    cout << "\nProgram ended successfully!\n";
//    return 0;
//}
