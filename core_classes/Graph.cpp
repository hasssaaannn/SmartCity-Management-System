#include "Graph.h"
#include <iostream>
#include <cmath>
using namespace std;


const double M_PI = 3.14159265358979323846;

// Maximum distance value for Dijkstra's algorithm (represents "infinity")
const double MAX_DISTANCE = 1000000.0;

// Edge Implementation
Edge::Edge(int dest, double w) : destination(dest), weight(w), next(nullptr) {
}

// Vertex Implementation
Vertex::Vertex() : vertexID(""), name(""), latitude(0.0), longitude(0.0), 
                   edgeList(nullptr), data(nullptr) {
}

Vertex::Vertex(const string& id, const string& n, double lat, double lon) 
    : vertexID(id), name(n), latitude(lat), longitude(lon), edgeList(nullptr), data(nullptr) {
}

Vertex::~Vertex() {
    Edge* current = edgeList;
    while (current != nullptr) {
        Edge* next = current->next;
        delete current;
        current = next;
    }
    edgeList = nullptr;
}

// Graph Implementation
Graph::Graph(int maxV, bool isDirected) : maxVertices(maxV), vertexCount(0), directed(isDirected) {
    vertices = new Vertex[maxVertices];
    for (int i = 0; i < maxVertices; i++) {
        vertices[i] = Vertex();
    }
}

Graph::~Graph() {
    clear();
    delete[] vertices;
    vertices = nullptr;
}

int Graph::findVertexIndex(const string& vertexID) {
    for (int i = 0; i < vertexCount; i++) {
        if (vertices[i].vertexID == vertexID) {
            return i;
        }
    }
    return -1;
}

double Graph::calculateDistance(double lat1, double lon1, double lat2, double lon2) {
    // Haversine formula to calculate distance between two coordinates
    const double R = 6371.0; // Earth radius in kilometers
    
    double dLat = (lat2 - lat1) * M_PI / 180.0;
    double dLon = (lon2 - lon1) * M_PI / 180.0;
    
    double a = sin(dLat / 2.0) * sin(dLat / 2.0) +
               cos(lat1 * M_PI / 180.0) * cos(lat2 * M_PI / 180.0) *
               sin(dLon / 2.0) * sin(dLon / 2.0);
    
    double c = 2.0 * atan2(sqrt(a), sqrt(1.0 - a));
    double distance = R * c;
    
    return distance;
}

bool Graph::addVertex(const string& vertexID, const string& name, 
                      double latitude, double longitude, void* data) {
    if (vertexCount >= maxVertices) return false;
    if (findVertexIndex(vertexID) != -1) return false; // Vertex already exists
    
    vertices[vertexCount].vertexID = vertexID;
    vertices[vertexCount].name = name;
    vertices[vertexCount].latitude = latitude;
    vertices[vertexCount].longitude = longitude;
    vertices[vertexCount].data = data;
    vertices[vertexCount].edgeList = nullptr;
    
    vertexCount++;
    return true;
}

bool Graph::addEdge(const string& fromID, const string& toID, double weight) {
    int fromIndex = findVertexIndex(fromID);
    int toIndex = findVertexIndex(toID);
    
    if (fromIndex == -1 || toIndex == -1) return false;
    
    return addEdge(fromIndex, toIndex, weight);
}

bool Graph::addEdge(int fromIndex, int toIndex, double weight) {
    if (fromIndex < 0 || fromIndex >= vertexCount) return false;
    if (toIndex < 0 || toIndex >= vertexCount) return false;
    
    // Check if edge already exists
    Edge* current = vertices[fromIndex].edgeList;
    while (current != nullptr) {
        if (current->destination == toIndex) {
            // Update weight of existing edge
            current->weight = weight;
            return true;
        }
        current = current->next;
    }
    
    // Add new edge at the end to maintain insertion order
    Edge* newEdge = new Edge(toIndex, weight);
    newEdge->next = nullptr;
    
    if (vertices[fromIndex].edgeList == nullptr) {
        vertices[fromIndex].edgeList = newEdge;
    } else {
        Edge* current = vertices[fromIndex].edgeList;
        while (current->next != nullptr) {
            current = current->next;
        }
        current->next = newEdge;
    }
    
    // If undirected, add reverse edge at the end
    if (!directed) {
        Edge* reverseEdge = new Edge(fromIndex, weight);
        reverseEdge->next = nullptr;
        
        if (vertices[toIndex].edgeList == nullptr) {
            vertices[toIndex].edgeList = reverseEdge;
        } else {
            Edge* current = vertices[toIndex].edgeList;
            while (current->next != nullptr) {
                current = current->next;
            }
            current->next = reverseEdge;
        }
    }
    
    return true;
}

bool Graph::addEdgeWithDistance(const string& fromID, const string& toID) {
    int fromIndex = findVertexIndex(fromID);
    int toIndex = findVertexIndex(toID);
    
    if (fromIndex == -1 || toIndex == -1) return false;
    
    double distance = calculateDistance(
        vertices[fromIndex].latitude, vertices[fromIndex].longitude,
        vertices[toIndex].latitude, vertices[toIndex].longitude
    );
    
    return addEdge(fromIndex, toIndex, distance);
}

bool Graph::removeEdge(const string& fromID, const string& toID) {
    int fromIndex = findVertexIndex(fromID);
    int toIndex = findVertexIndex(toID);
    
    if (fromIndex == -1 || toIndex == -1) return false;
    
    // Remove edge from fromIndex
    Edge* current = vertices[fromIndex].edgeList;
    Edge* previous = nullptr;
    
    while (current != nullptr) {
        if (current->destination == toIndex) {
            if (previous == nullptr) {
                vertices[fromIndex].edgeList = current->next;
            } else {
                previous->next = current->next;
            }
            delete current;
            
            // If undirected, remove reverse edge
            if (!directed) {
                current = vertices[toIndex].edgeList;
                previous = nullptr;
                while (current != nullptr) {
                    if (current->destination == fromIndex) {
                        if (previous == nullptr) {
                            vertices[toIndex].edgeList = current->next;
                        } else {
                            previous->next = current->next;
                        }
                        delete current;
                        break;
                    }
                    previous = current;
                    current = current->next;
                }
            }
            
            return true;
        }
        previous = current;
        current = current->next;
    }
    
    return false;
}

bool Graph::removeVertex(const string& vertexID) {
    int index = findVertexIndex(vertexID);
    if (index == -1) return false;
    
    // Remove all edges connected to this vertex
    for (int i = 0; i < vertexCount; i++) {
        Edge* current = vertices[i].edgeList;
        Edge* previous = nullptr;
        
        while (current != nullptr) {
            if (current->destination == index) {
                if (previous == nullptr) {
                    vertices[i].edgeList = current->next;
                } else {
                    previous->next = current->next;
                }
                Edge* toDelete = current;
                current = current->next;
                delete toDelete;
            } else {
                // Update destination indices for vertices after removed one
                if (current->destination > index) {
                    current->destination--;
                }
                previous = current;
                current = current->next;
            }
        }
    }
    
    // Shift vertices array
    for (int i = index; i < vertexCount - 1; i++) {
        vertices[i] = vertices[i + 1];
    }
    
    vertexCount--;
    return true;
}

std::string* Graph::findShortestPath(const std::string& startID, const std::string& endID, 
                                     int& pathLength, double& totalDistance) {
    pathLength = 0;
    totalDistance = 0.0;
    
    int startIndex = findVertexIndex(startID);
    int endIndex = findVertexIndex(endID);
    
    if (startIndex == -1 || endIndex == -1) return nullptr;
    if (startIndex == endIndex) {
        pathLength = 1;
        std::string* path = new std::string[1];
        path[0] = startID;
        return path;
    }
    
    // Dijkstra's Algorithm
    double* distances = new double[vertexCount];
    int* previous = new int[vertexCount];
    bool* visited = new bool[vertexCount];
    
    // Initialize arrays
    for (int i = 0; i < vertexCount; i++) {
        distances[i] = MAX_DISTANCE;
        previous[i] = -1;
        visited[i] = false;
    }
    
    distances[startIndex] = 0.0;
    
    // Dijkstra's main loop
    for (int count = 0; count < vertexCount; count++) {
        // Find unvisited vertex with minimum distance
        int minIndex = -1;
        double minDist = MAX_DISTANCE;
        
        for (int i = 0; i < vertexCount; i++) {
            if (!visited[i] && distances[i] < minDist) {
                minDist = distances[i];
                minIndex = i;
            }
        }
        
        if (minIndex == -1 || minIndex == endIndex) break;
        
        visited[minIndex] = true;
        
        // Update distances to neighbors
        Edge* current = vertices[minIndex].edgeList;
        while (current != nullptr) {
            int neighbor = current->destination;
            if (!visited[neighbor]) {
                double alt = distances[minIndex] + current->weight;
                if (alt < distances[neighbor]) {
                    distances[neighbor] = alt;
                    previous[neighbor] = minIndex;
                }
            }
            current = current->next;
        }
    }
    
    // Check if path exists
    if (distances[endIndex] >= MAX_DISTANCE) {
        delete[] distances;
        delete[] previous;
        delete[] visited;
        return nullptr;
    }
    
    // Reconstruct path
    int* pathIndices = new int[vertexCount];
    int pathIndexCount = 0;
    int current = endIndex;
    
    while (current != -1) {
        pathIndices[pathIndexCount++] = current;
        current = previous[current];
    }
    
    // Reverse path and create string array
    pathLength = pathIndexCount;
    std::string* path = new std::string[pathLength];
    totalDistance = distances[endIndex];
    
    for (int i = 0; i < pathLength; i++) {
        path[i] = vertices[pathIndices[pathLength - 1 - i]].vertexID;
    }
    
    delete[] distances;
    delete[] previous;
    delete[] visited;
    delete[] pathIndices;
    
    return path;
}

std::string Graph::findNearestLocation(double latitude, double longitude) {
    if (vertexCount == 0) return "";
    
    double minDistance = MAX_DISTANCE;
    int nearestIndex = -1;
    
    for (int i = 0; i < vertexCount; i++) {
        double dist = calculateDistance(latitude, longitude, 
                                       vertices[i].latitude, vertices[i].longitude);
        if (dist < minDistance) {
            minDistance = dist;
            nearestIndex = i;
        }
    }
    
    if (nearestIndex == -1) return "";
    return vertices[nearestIndex].vertexID;
}

std::string Graph::findNearestLocation(const std::string& fromVertexID) {
    int fromIndex = findVertexIndex(fromVertexID);
    if (fromIndex == -1) return "";
    
    double minDistance = MAX_DISTANCE;
    int nearestIndex = -1;
    
    Edge* current = vertices[fromIndex].edgeList;
    while (current != nullptr) {
        if (current->weight < minDistance) {
            minDistance = current->weight;
            nearestIndex = current->destination;
        }
        current = current->next;
    }
    
    if (nearestIndex == -1) return "";
    return vertices[nearestIndex].vertexID;
}

std::string Graph::findNearestStop(double latitude, double longitude) {
    if (vertexCount == 0) return "";
    
    double minDistance = MAX_DISTANCE;
    int nearestIndex = -1;
    
    // Only search for vertices with ID starting with "Stop"
    for (int i = 0; i < vertexCount; i++) {
        // Check if vertex ID starts with "Stop" (case-sensitive)
        if (vertices[i].vertexID.length() >= 4 && 
            vertices[i].vertexID.substr(0, 4) == "Stop") {
            double dist = calculateDistance(latitude, longitude, 
                                           vertices[i].latitude, vertices[i].longitude);
            if (dist < minDistance) {
                minDistance = dist;
                nearestIndex = i;
            }
        }
    }
    
    if (nearestIndex == -1) return "";
    return vertices[nearestIndex].vertexID;
}

bool Graph::connectToNearestStop(const std::string& vertexID) {
    int vertexIndex = findVertexIndex(vertexID);
    if (vertexIndex == -1) return false;
    
    // Get vertex coordinates
    double lat = vertices[vertexIndex].latitude;
    double lon = vertices[vertexIndex].longitude;
    
    // Find nearest stop
    string nearestStopID = findNearestStop(lat, lon);
    if (nearestStopID.empty()) return false;
    
    // Connect vertex to nearest stop (bidirectional edge)
    return addEdgeWithDistance(vertexID, nearestStopID);
}

bool Graph::getVertexCoordinates(const std::string& vertexID, double& latitude, double& longitude) {
    int index = findVertexIndex(vertexID);
    if (index == -1) return false;
    
    latitude = vertices[index].latitude;
    longitude = vertices[index].longitude;
    return true;
}

bool Graph::getConnectedStops(const std::string& vertexID, std::string* connectedStops, int maxStops, int& actualCount) {
    int index = findVertexIndex(vertexID);
    if (index == -1) {
        actualCount = 0;
        return false;
    }
    
    actualCount = 0;
    Edge* current = vertices[index].edgeList;
    
    while (current != nullptr && actualCount < maxStops) {
        // Get the connected vertex ID
        std::string connectedID = vertices[current->destination].vertexID;
        
        // Check if it's a stop (starts with "Stop")
        if (connectedID.length() >= 4 && connectedID.substr(0, 4) == "Stop") {
            connectedStops[actualCount] = connectedID;
            actualCount++;
        }
        
        current = current->next;
    }
    
    return true;
}

int Graph::getDegree(const std::string& vertexID) {
    int index = findVertexIndex(vertexID);
    if (index == -1) return -1;
    
    int degree = 0;
    Edge* current = vertices[index].edgeList;
    while (current != nullptr) {
        degree++;
        current = current->next;
    }
    
    return degree;
}

bool Graph::areAdjacent(const std::string& fromID, const std::string& toID) {
    int fromIndex = findVertexIndex(fromID);
    int toIndex = findVertexIndex(toID);
    
    if (fromIndex == -1 || toIndex == -1) return false;
    
    Edge* current = vertices[fromIndex].edgeList;
    while (current != nullptr) {
        if (current->destination == toIndex) {
            return true;
        }
        current = current->next;
    }
    
    return false;
}

void Graph::display() {
    cout << "Graph Structure (Vertices: " << vertexCount << "):" << endl;
    for (int i = 0; i < vertexCount; i++) {
        cout << "Vertex[" << i << "]: " << vertices[i].vertexID 
             << " (" << vertices[i].name << ") - ";
        Edge* current = vertices[i].edgeList;
        if (current == nullptr) {
            cout << "No edges";
        } else {
            while (current != nullptr) {
                cout << vertices[current->destination].vertexID 
                     << "(" << current->weight << ") ";
                current = current->next;
                if (current != nullptr) cout << "-> ";
            }
        }
        cout << endl;
    }
}

int Graph::getVertexCount() {
    return vertexCount;
}

int Graph::getEdgeCount() {
    int count = 0;
    for (int i = 0; i < vertexCount; i++) {
        Edge* current = vertices[i].edgeList;
        while (current != nullptr) {
            count++;
            current = current->next;
        }
    }
    
    if (!directed) {
        count = count / 2; // Each edge counted twice in undirected graph
    }
    
    return count;
}

void Graph::clear() {
    for (int i = 0; i < vertexCount; i++) {
        Edge* current = vertices[i].edgeList;
        while (current != nullptr) {
            Edge* next = current->next;
            delete current;
            current = next;
        }
        vertices[i].edgeList = nullptr;
        vertices[i].vertexID = "";
        vertices[i].name = "";
        vertices[i].data = nullptr;
    }
    vertexCount = 0;
}

Vertex* Graph::getVertexAt(int index) {
    if (index < 0 || index >= vertexCount) return nullptr;
    return &vertices[index];
}

bool Graph::getEdgesForVertex(int vertexIndex, int* destinations, double* weights, int maxEdges, int& actualCount) {
    if (vertexIndex < 0 || vertexIndex >= vertexCount) return false;
    if (destinations == nullptr || weights == nullptr) return false;
    
    actualCount = 0;
    Edge* current = vertices[vertexIndex].edgeList;
    
    while (current != nullptr && actualCount < maxEdges) {
        destinations[actualCount] = current->destination;
        weights[actualCount] = current->weight;
        actualCount++;
        current = current->next;
    }
    
    return true;
}