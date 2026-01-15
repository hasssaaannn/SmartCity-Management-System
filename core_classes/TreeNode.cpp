#include "TreeNode.h"
using namespace std;

// TreeNode Implementation
TreeNode::TreeNode(const string& id, const string& n, void* d) 
    : nodeID(id), name(n), data(d), parent(nullptr), firstChild(nullptr), 
      nextSibling(nullptr), childCount(0) {
}

TreeNode::~TreeNode() {
    // Recursively delete all children
    if (firstChild == nullptr) return;
    
    // Delete children one by one
    TreeNode* current = firstChild;
    firstChild = nullptr; // Set to nullptr immediately to prevent re-access
    
    while (current != nullptr) {
        TreeNode* next = current->nextSibling;
        
        // Clear parent and sibling references before deleting
        if (current != nullptr) {
            current->parent = nullptr;
            current->nextSibling = nullptr;
            delete current;
        }
        
        current = next;
    }
}

void TreeNode::addChild(TreeNode* child) {
    if (child == nullptr) return;
    
    // Set parent of child
    child->parent = this;
    child->nextSibling = nullptr;
    
    // Add to the end of children list to maintain insertion order
    if (firstChild == nullptr) {
        firstChild = child;
    } else {
        // Find the last child
        TreeNode* current = firstChild;
        while (current->nextSibling != nullptr) {
            current = current->nextSibling;
        }
        current->nextSibling = child;
    }
    childCount++;
}

bool TreeNode::removeChild(const std::string& childID) {
    if (firstChild == nullptr) return false;
    
    // Check if first child is the one to remove
    if (firstChild->nodeID == childID) {
        TreeNode* toRemove = firstChild;
        firstChild = firstChild->nextSibling;
        toRemove->parent = nullptr;
        toRemove->nextSibling = nullptr;
        delete toRemove;
        childCount--;
        return true;
    }
    
    // Search in siblings
    TreeNode* current = firstChild;
    while (current->nextSibling != nullptr) {
        if (current->nextSibling->nodeID == childID) {
            TreeNode* toRemove = current->nextSibling;
            current->nextSibling = toRemove->nextSibling;
            toRemove->parent = nullptr;
            toRemove->nextSibling = nullptr;
            delete toRemove;
            childCount--;
            return true;
        }
        current = current->nextSibling;
    }
    
    return false;
}

TreeNode* TreeNode::findChild(const std::string& childID) {
    TreeNode* current = firstChild;
    while (current != nullptr) {
        if (current->nodeID == childID) {
            return current;
        }
        current = current->nextSibling;
    }
    return nullptr;
}

bool TreeNode::hasChildren() {
    return firstChild != nullptr;
}

bool TreeNode::isLeaf() {
    return firstChild == nullptr;
}

int TreeNode::getDepth() {
    int depth = 0;
    TreeNode* current = parent;
    while (current != nullptr) {
        depth++;
        current = current->parent;
    }
    return depth;
}

int TreeNode::getHeight() {
    if (isLeaf()) {
        return 0;
    }
    
    int maxChildHeight = -1;
    TreeNode* current = firstChild;
    while (current != nullptr) {
        int childHeight = current->getHeight();
        if (childHeight > maxChildHeight) {
            maxChildHeight = childHeight;
        }
        current = current->nextSibling;
    }
    
    return maxChildHeight + 1;
}

