#ifndef TREE_H
#define TREE_H

#include <string>
#include "TreeNode.h"
using namespace std;

// Tree class for general N-ary tree implementation
// Used for: 
// - Islamabad sector hierarchy (N-ary tree)
// - School → Department → Class (3-level tree)
// - House → Family → Individuals (general tree)
class Tree {
private:
    TreeNode* root;           // Root of the tree
    
    // Helper function for finding node by ID (recursive)
    // Complexity: O(n) where n is total nodes
    TreeNode* findNodeHelper(TreeNode* node, const string& nodeID);
    
    // Helper function for counting nodes (recursive)
    // Complexity: O(n)
    int countNodesHelper(TreeNode* node);
    
    // Helper function for getting tree height (recursive)
    // Complexity: O(n)
    int getHeightHelper(TreeNode* node);
    
    // Helper function for displaying tree (recursive)
    // Complexity: O(n)
    void displayHelper(TreeNode* node, int level, const string& prefix);
    
    // Helper function for deleting subtree (recursive)
    // Complexity: O(n) where n is nodes in subtree
    void deleteSubtree(TreeNode* node);
    
public:
    // Constructor: Creates an empty tree
    // Complexity: O(1)
    Tree();
    
    // Destructor: Deletes entire tree
    // Complexity: O(n) where n is total nodes
    ~Tree();
    
    // Set the root of the tree
    // Complexity: O(1)
    void setRoot(TreeNode* rootNode);
    
    // Get the root of the tree
    // Complexity: O(1)
    TreeNode* getRoot();
    
    // Find a node by ID
    // Complexity: O(n) where n is total nodes
    TreeNode* findNode(const string& nodeID);
    
    // Add a child to a parent node
    // Complexity: O(n) to find parent, O(1) to add child
    bool addChild(const string& parentID, const string& childID, 
                  const string& childName, void* childData = nullptr);
    
    // Remove a node and its entire subtree
    // Complexity: O(n) where n is nodes in subtree
    bool removeNode(const string& nodeID);
    
    // Get the total number of nodes in the tree
    // Complexity: O(n)
    int getNodeCount();
    
    // Get the height of the tree
    // Complexity: O(n)
    int getTreeHeight();
    
    // Get the depth of a node
    // Complexity: O(h) where h is height of tree
    int getNodeDepth(const string& nodeID);
    
    // Display the tree structure (hierarchical view)
    // Complexity: O(n)
    void display();
    
    // Check if the tree is empty
    // Complexity: O(1)
    bool isEmpty();
    
    // Clear the entire tree
    // Complexity: O(n)
    void clear();
    
    // Get all leaf nodes (nodes with no children)
    // Complexity: O(n)
    void getLeafNodes(TreeNode** leafArray, int& count);
    
    // Check if a node is an ancestor of another node
    // Complexity: O(h) where h is height
    bool isAncestor(const string& ancestorID, const string& descendantID);
    
    // Get the path from root to a given node
    // Returns: Array of node IDs, pathLength is output parameter
    // Complexity: O(h) where h is height
    string* getPathToNode(const string& nodeID, int& pathLength);
};

#endif // TREE_H

