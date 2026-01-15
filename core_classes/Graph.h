#ifndef GRAPH_H
#define GRAPH_H

#include <string>
using namespace std;

// Structure for an edge in the graph (weighted edge)
struct Edge {
    int destination;          // Destination vertex ID
    double weight;            // Weight/distance of the edge
    Edge* next;               // Pointer to next edge in the adjacency list
    
    Edge(int dest, double w);
};

// Structure for a vertex (node) in the graph
struct Vertex {
    string vertexID;     // Unique identifier (e.g., StopID, LocationID)
    string name;         // Name of the location (e.g., "G-10 Markaz")
    double latitude;          // Latitude coordinate
    double longitude;         // Longitude coordinate
    Edge* edgeList;           // Pointer to the list of edges (adjacency list)
    void* data;               // Additional data associated with vertex (optional)
    
    Vertex();
    Vertex(const string& id, const string& n, double lat, double lon);
    ~Vertex();
};

// Graph class implementing weighted graph using adjacency list
// Used for: Bus routes, Road network, Nearest location queries, Shortest paths (Dijkstra)
class Graph {
private:
    Vertex* vertices;         // Array of vertices
    int maxVertices;          // Maximum number of vertices
    int vertexCount;          // Current number of vertices
    bool directed;            // true for directed graph, false for undirected
    
    // Helper function to find vertex index by ID
    // Complexity: O(V) where V is number of vertices
    int findVertexIndex(const string& vertexID);
    
    // Helper function to calculate distance between two coordinates (Haversine or Euclidean)
    // Complexity: O(1)
    double calculateDistance(double lat1, double lon1, double lat2, double lon2);
    
public:
    // Constructor: Initializes graph with maximum number of vertices
    // Complexity: O(V)
    Graph(int maxV = 100, bool isDirected = false);
    
    // Destructor: Deallocates all memory
    // Complexity: O(V + E) where V is vertices, E is edges
    ~Graph();
    
    // Add a vertex to the graph
    // Complexity: O(1) amortized, O(V) if resizing needed
    bool addVertex(const string& vertexID, const string& name, 
                   double latitude, double longitude, void* data = nullptr);
    
    // Add an edge between two vertices
    // Complexity: O(1) if using coordinates, O(V) if finding by ID
    bool addEdge(const string& fromID, const string& toID, double weight);
    bool addEdge(int fromIndex, int toIndex, double weight);
    
    // Add an edge with automatic weight calculation from coordinates
    // Complexity: O(1)
    bool addEdgeWithDistance(const string& fromID, const string& toID);
    
    // Remove an edge between two vertices
    // Complexity: O(E) where E is average edges per vertex
    bool removeEdge(const string& fromID, const string& toID);
    
    // Remove a vertex and all its edges
    // Complexity: O(V + E)
    bool removeVertex(const string& vertexID);
    
    // Find shortest path using Dijkstra's Algorithm
    // Complexity: O(V^2) for basic implementation, can be optimized to O(V log V + E)
    // Returns: Array of vertex IDs representing the shortest path
    // Parameters: startID, endID, pathLength (output), totalDistance (output)
    string* findShortestPath(const string& startID, const string& endID, 
                                   int& pathLength, double& totalDistance);
    
    // Find nearest location to given coordinates
    // Complexity: O(V)
    string findNearestLocation(double latitude, double longitude);
    
    // Find nearest location to a given vertex
    // Complexity: O(V)
    string findNearestLocation(const string& fromVertexID);
    
    // Find nearest stop to given coordinates (filters by vertex ID starting with "Stop")
    // Complexity: O(V)
    string findNearestStop(double latitude, double longitude);
    
    // Connect a vertex to its nearest bus stop (creates bidirectional edge)
    // Complexity: O(V)
    // Returns: true if connection was made, false otherwise
    bool connectToNearestStop(const string& vertexID);
    
    // Get vertex coordinates by vertex ID
    // Complexity: O(V)
    // Returns: true if vertex found and coordinates set, false otherwise
    bool getVertexCoordinates(const string& vertexID, double& latitude, double& longitude);
    
    // Get connected stops (neighbors that are bus stops) for a vertex
    // Complexity: O(E) where E is edges of the vertex
    // Parameters: vertexID, connectedStops array (output), maxStops, actualCount (output)
    // Returns: true if vertex found, false otherwise
    bool getConnectedStops(const string& vertexID, string* connectedStops, int maxStops, int& actualCount);
    
    // Get degree of a vertex (number of connected edges)
    // Complexity: O(E) where E is edges of that vertex
    int getDegree(const string& vertexID);
    
    // Check if two vertices are adjacent
    // Complexity: O(E) where E is edges of fromVertex
    bool areAdjacent(const string& fromID, const string& toID);
    
    // Display the graph structure (for debugging)
    // Complexity: O(V + E)
    void display();
    
    // Get number of vertices
    // Complexity: O(1)
    int getVertexCount();
    
    // Get number of edges
    // Complexity: O(V + E)
    int getEdgeCount();
    
    // Clear all vertices and edges
    // Complexity: O(V + E)
    void clear();
    
    // Get vertex at index (for iteration/visualization)
    // Complexity: O(1)
    // Returns: Pointer to Vertex, or nullptr if index invalid
    Vertex* getVertexAt(int index);
    
    // Get all edges for a vertex at index
    // Complexity: O(E) where E is edges of vertex
    // Parameters: vertexIndex, destinations array (output), weights array (output), maxEdges, actualCount (output)
    // Returns: true if vertex exists, false otherwise
    bool getEdgesForVertex(int vertexIndex, int* destinations, double* weights, int maxEdges, int& actualCount);
};

#endif // GRAPH_H

