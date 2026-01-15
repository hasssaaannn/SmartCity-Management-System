#ifndef TREENODE_H
#define TREENODE_H

#include <string>
using namespace std;

// Structure for a node in an N-ary tree (general tree)
// Used for: Islamabad sector hierarchy, School→Department→Class, House→Family→Individuals
class TreeNode {
public:
    string nodeID;       // Unique identifier for the node
    string name;         // Name of the node (e.g., "G-10", "Math Department")
    void* data;               // Pointer to additional data associated with node
    TreeNode* parent;         // Pointer to parent node
    TreeNode* firstChild;     // Pointer to first child (leftmost child)
    TreeNode* nextSibling;    // Pointer to next sibling (right sibling)
    int childCount;           // Number of children
    
    // Constructor
    // Complexity: O(1)
    TreeNode(const string& id, const string& n, void* d = nullptr);
    
    // Destructor: Recursively deletes all children
    // Complexity: O(n) where n is number of nodes in subtree
    ~TreeNode();
    
    // Add a child to this node
    // Complexity: O(1)
    void addChild(TreeNode* child);
    
    // Remove a child from this node by ID
    // Complexity: O(C) where C is number of children
    bool removeChild(const string& childID);
    
    // Find a child by ID
    // Complexity: O(C) where C is number of children
    TreeNode* findChild(const string& childID);
    
    // Check if this node has children
    // Complexity: O(1)
    bool hasChildren();
    
    // Check if this node is a leaf
    // Complexity: O(1)
    bool isLeaf();
    
    // Get depth of this node (distance from root)
    // Complexity: O(h) where h is height from root
    int getDepth();
    
    // Get height of subtree rooted at this node
    // Complexity: O(n) where n is number of nodes in subtree
    int getHeight();
};

#endif // TREENODE_H

