#ifndef BPLUSTREE_NODE_H
#define BPLUSTREE_NODE_H

#include "Page.h"
#include <vector>
#include <memory>

class BPlusTreeNode {
public:
    // Constants for B+ tree properties
    static const int MAX_KEYS = (Page::PAGE_SIZE - sizeof(Page::PageHeader)) / (sizeof(int) + sizeof(RecordPtr));
    static const int MIN_KEYS = MAX_KEYS / 2;

    // Constructor and destructor
    explicit BPlusTreeNode(PagePtr page);
    ~BPlusTreeNode();

    // Key management methods
    bool insertKey(int key, const RecordPtr& recordPtr);
    bool removeKey(int key);
    RecordPtr findKey(int key) const;
    int getKeyAt(int index) const;
    void updateKeyAt(int index, int newKey);
    int getKeyCount() const;

    // Child management methods (for internal nodes)
    int getChildPageID(int index) const;
    void addChild(int childPageID);
    void removeChild(int index);
    int getChildCount() const;
    BPlusTreeNode* getChild(int index) const;  // Gets child node at given index
    void insertKeyAndChild(int key, int childPageID, int index);

    // Node type checking
    bool isLeaf() const;
    bool isFull() const;
    bool isUnderflow() const;

    // Node operations for tree maintenance
    int splitNode(BPlusTreeNode& newNode);
    void mergeWith(BPlusTreeNode& sibling);
    int findChildIndex(int key) const;
    int findKeyPosition(int key) const;

    // Accessors
    PagePtr getPage() const { return page; }
    int getParentPageID() const;
    void setParentPageID(int pageID);

    // Range search support
    void getKeysInRange(int startKey, int endKey, std::vector<RecordPtr>& results) const;
    RecordPtr getRecordAt(int index) const;

    void ensureLeafLinksValid();

private:
    PagePtr page;                // The underlying page storing node data
    vector<int> keys;           // Cache of keys from the page
    vector<RecordPtr> records;  // Cache of record pointers (for leaf nodes)
    vector<int> children;       // Cache of child page IDs (for internal nodes)

    // Helper methods
    void loadFromPage();        // Loads cached data from page
    void saveToPage();          // Saves cached data to page
    void insertAt(int position, int key, const RecordPtr& recordPtr);
    void removeAt(int position);
};

#endif // BPLUSTREE_NODE_H