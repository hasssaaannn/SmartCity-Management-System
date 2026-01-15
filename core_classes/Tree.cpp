#include "Tree.h"
#include <iostream>
#include <cmath>
using namespace std;

// Static helper function to collect leaf nodes
static void collectLeafNodesHelper(TreeNode* node, TreeNode** leafArray, int& index) {
    if (node == nullptr) return;
    
    if (node->isLeaf()) {
        if (leafArray != nullptr) {
            leafArray[index] = node;
        }
        index++;
        return;
    }
    
    // Traverse all children
    TreeNode* current = node->firstChild;
    while (current != nullptr) {
        collectLeafNodesHelper(current, leafArray, index);
        current = current->nextSibling;
    }
}

// Tree Implementation
Tree::Tree() : root(nullptr) {
}

Tree::~Tree() {
    clear();
}

void Tree::setRoot(TreeNode* rootNode) {
    if (root != nullptr && root != rootNode) {
        delete root;
    }
    root = rootNode;
}

TreeNode* Tree::getRoot() {
    return root;
}

TreeNode* Tree::findNodeHelper(TreeNode* node, const std::string& nodeID) {
    if (node == nullptr) return nullptr;
    if (node->nodeID == nodeID) return node;
    
    // Search in children
    TreeNode* current = node->firstChild;
    while (current != nullptr) {
        TreeNode* found = findNodeHelper(current, nodeID);
        if (found != nullptr) {
            return found;
        }
        current = current->nextSibling;
    }
    
    return nullptr;
}

TreeNode* Tree::findNode(const std::string& nodeID) {
    return findNodeHelper(root, nodeID);
}

bool Tree::addChild(const std::string& parentID, const std::string& childID, 
                    const std::string& childName, void* childData) {
    TreeNode* parentNode = findNode(parentID);
    if (parentNode == nullptr) {
        // If root is null and parentID matches, create root
        if (root == nullptr) {
            root = new TreeNode(parentID, parentID, nullptr);
            parentNode = root;
        } else {
            return false; // Parent not found
        }
    }
    
    // Check if child already exists
    if (findNode(childID) != nullptr) {
        return false; // Child already exists
    }
    
    TreeNode* childNode = new TreeNode(childID, childName, childData);
    parentNode->addChild(childNode);
    return true;
}

bool Tree::removeNode(const std::string& nodeID) {
    TreeNode* node = findNode(nodeID);
    if (node == nullptr) return false;
    
    if (node == root) {
        delete root;
        root = nullptr;
        return true;
    }
    
    // Remove from parent's children
    if (node->parent != nullptr) {
        node->parent->removeChild(nodeID);
    }
    
    return true;
}

int Tree::countNodesHelper(TreeNode* node) {
    if (node == nullptr) return 0;
    
    int count = 1; // Count current node
    TreeNode* current = node->firstChild;
    while (current != nullptr) {
        count += countNodesHelper(current);
        current = current->nextSibling;
    }
    
    return count;
}

int Tree::getNodeCount() {
    return countNodesHelper(root);
}

int Tree::getHeightHelper(TreeNode* node) {
    if (node == nullptr) return -1;
    return node->getHeight();
}

int Tree::getTreeHeight() {
    return getHeightHelper(root);
}

int Tree::getNodeDepth(const std::string& nodeID) {
    TreeNode* node = findNode(nodeID);
    if (node == nullptr) return -1;
    return node->getDepth();
}

void Tree::displayHelper(TreeNode* node, int level, const std::string& prefix) {
    if (node == nullptr) return;
    
    // Print current node
    for (int i = 0; i < level; i++) {
        cout << "  ";
    }
    cout << prefix << node->nodeID << " (" << node->name << ")" << endl;
    
    // Print children recursively
    TreeNode* current = node->firstChild;
    while (current != nullptr) {
        string childPrefix = "+-- ";
        TreeNode* next = current->nextSibling;
        if (next == nullptr) {
            childPrefix = "+-- ";
        }
        displayHelper(current, level + 1, childPrefix);
        current = next;
    }
}

void Tree::display() {
    if (root == nullptr) {
        cout << "Tree is empty." << endl;
        return;
    }
    cout << "Tree Structure:" << endl;
    displayHelper(root, 0, "");
}

bool Tree::isEmpty() {
    return root == nullptr;
}

void Tree::deleteSubtree(TreeNode* node) {
    if (node == nullptr) return;
    
    // Delete all children first
    TreeNode* current = node->firstChild;
    while (current != nullptr) {
        TreeNode* next = current->nextSibling;
        deleteSubtree(current);
        current = next;
    }
    
    // Clear firstChild before deleting node to prevent destructor from trying to delete already-deleted children
    node->firstChild = nullptr;
    
    // Delete the node itself
    delete node;
}

void Tree::clear() {
    if (root != nullptr) {
        deleteSubtree(root);
        root = nullptr;
    }
}

void Tree::getLeafNodes(TreeNode** leafArray, int& count) {
    count = 0;
    if (root == nullptr) return;
    
    // First pass: count leaf nodes
    int index = 0;
    collectLeafNodesHelper(root, nullptr, index);
    count = index;
    
    // Second pass: collect leaf nodes if array is provided
    if (leafArray != nullptr) {
        index = 0;
        collectLeafNodesHelper(root, leafArray, index);
    }
}

bool Tree::isAncestor(const std::string& ancestorID, const std::string& descendantID) {
    TreeNode* descendant = findNode(descendantID);
    if (descendant == nullptr) return false;
    
    TreeNode* current = descendant->parent;
    while (current != nullptr) {
        if (current->nodeID == ancestorID) {
            return true;
        }
        current = current->parent;
    }
    
    return false;
}

std::string* Tree::getPathToNode(const std::string& nodeID, int& pathLength) {
    TreeNode* node = findNode(nodeID);
    if (node == nullptr) {
        pathLength = 0;
        return nullptr;
    }
    
    // Calculate path length (depth + 1)
    pathLength = node->getDepth() + 1;
    
    // Allocate array for path
    std::string* path = new std::string[pathLength];
    
    // Build path from node to root (reverse order)
    TreeNode* current = node;
    for (int i = pathLength - 1; i >= 0; i--) {
        path[i] = current->nodeID;
        current = current->parent;
    }
    
    return path;
}

