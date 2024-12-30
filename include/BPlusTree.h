#ifndef BPLUS_TREE_H
#define BPLUS_TREE_H

#include "BPlusTreeNode.h"
#include "BufferManager.h"
#include "FileManager.h"
#include <vector>

#include "IndexStatistics.h"

class BPlusTree {
public:
    // Constructor and Destructor
    BPlusTree(BufferManager& bufferManager, FileManager& fileManager, int rootPageID = -1);
    ~BPlusTree();  // We'll need this to clean up resources

    // Core B+ tree operations that we discussed earlier
    bool insert(int key, const RecordPtr& recordPtr);
    bool remove(int key);
    RecordPtr find(int key) const;
    vector<RecordPtr> rangeSearch(int startKey, int endKey) const;

    // Methods to find minimum and maximum keys in the tree
    int findMin() const;
    int findMax() const;

    BPlusTreeNode* getRoot() const;  // Returns the root node
    void setRoot(int pageID) { rootPageID = pageID; }  // Sets the root page ID

    // Add getter for statistics
    const IndexStatistics& getStatistics() const { return indexStats; }

private:
    // Essential member variables for managing the tree
    BufferManager& bufferManager;    // For managing page loading/saving
    FileManager& fileManager;        // For persistent storage
    int rootPageID;                  // Tracks the current root page

    // Helper method for node creation and access
    void createNewRoot();
    BPlusTreeNode* fetchNode(int pageID) const;

    // Methods for handling tree modifications
    bool insertInternal(BPlusTreeNode* node, int key, const RecordPtr& recordPtr);
    bool removeInternal(BPlusTreeNode* node, int key);
    RecordPtr findInternal(BPlusTreeNode* node, int key) const;

    // Methods for handling node splits and merges
    void splitChild(BPlusTreeNode* parent, int childIndex);
    void mergeChildren(BPlusTreeNode* parent, int leftChildIndex);

    // Methods for handling node underflow during deletion
    void handleLeafUnderflow(BPlusTreeNode* node);
    void handleInternalNodeUnderflow(BPlusTreeNode* parent, int childIndex);

    // Methods for redistributing keys between nodes
    void borrowFromLeftLeaf(BPlusTreeNode* node, BPlusTreeNode* leftSibling, 
                          BPlusTreeNode* parent, int nodeIndex);
    void borrowFromRightLeaf(BPlusTreeNode* node, BPlusTreeNode* rightSibling,
                           BPlusTreeNode* parent, int nodeIndex);
    void borrowFromLeftInternal(BPlusTreeNode* node, BPlusTreeNode* leftSibling,
                              BPlusTreeNode* parent, int nodeIndex);
    void borrowFromRightInternal(BPlusTreeNode* node, BPlusTreeNode* rightSibling,
                               BPlusTreeNode* parent, int nodeIndex);

    // Methods for merging nodes
    void mergeLeafNodes(BPlusTreeNode* leftNode, BPlusTreeNode* rightNode,
                       BPlusTreeNode* parent, int rightNodeIndex);
    void mergeInternalNodes(BPlusTreeNode* leftNode, BPlusTreeNode* rightNode,
                          BPlusTreeNode* parent, int rightNodeIndex);

    IndexStatistics indexStats;  // Add this member
    void updateStatistics();     // Add this method declaration
};

#endif // BPLUS_TREE_H