// BPlusTreeNode.cpp
#include "BPlusTreeNode.h"
#include <algorithm>

BPlusTreeNode::BPlusTreeNode(PagePtr page) : page(page) {
    loadFromPage();
}

BPlusTreeNode::~BPlusTreeNode() {
    saveToPage();
}

void BPlusTreeNode::loadFromPage() {
    // Clear existing caches
    keys.clear();
    records.clear();
    children.clear();
    
    // Load index entries from page
    vector<Page::IndexEntry> entries = page->getIndexEntries();
    for (const auto& entry : entries) {
        keys.push_back(entry.key);
        if (page->isLeafPage()) {
            records.push_back(entry.recordPtr);
        } else {
            children.push_back(entry.childPageID);
        }
    }
}

void BPlusTreeNode::saveToPage() {
    // Clear existing entries
    page->clearIndexEntries();
    
    // Save cached data back to page
    for (size_t i = 0; i < keys.size(); i++) {
        if (page->isLeafPage()) {
            page->addIndexEntry(keys[i], records[i]);
        } else {
            page->addIndexEntry(keys[i], children[i]);
        }
    }
}

bool BPlusTreeNode::insertKey(int key, const RecordPtr& recordPtr) {
    if (isFull()) {
        return false;
    }
    
    int position = findKeyPosition(key);
    insertAt(position, key, recordPtr);
    return true;
}

void BPlusTreeNode::insertAt(int position, int key, const RecordPtr& recordPtr) {
    keys.insert(keys.begin() + position, key);
    if (page->isLeafPage()) {
        records.insert(records.begin() + position, recordPtr);
    }
}

RecordPtr BPlusTreeNode::findKey(int key) const {
    int position = findKeyPosition(key);
    if (position < keys.size() && keys[position] == key) {
        return records[position];
    }
    return RecordPtr(); // Return invalid record pointer if not found
}

int BPlusTreeNode::findKeyPosition(int key) const {
    return lower_bound(keys.begin(), keys.end(), key) - keys.begin();
}

int BPlusTreeNode::findChildIndex(int key) const {
    if (keys.empty()) return 0;
    return upper_bound(keys.begin(), keys.end(), key) - keys.begin();
}

bool BPlusTreeNode::isLeaf() const {
    return page->isLeafPage();
}

bool BPlusTreeNode::isFull() const {
    return keys.size() >= MAX_KEYS;
}

bool BPlusTreeNode::isUnderflow() const {
    return keys.size() < MIN_KEYS;
}

int BPlusTreeNode::splitNode(BPlusTreeNode& newNode) {
    int midPoint = keys.size() / 2;
    
    // Move half the keys and associated data to the new node
    for (size_t i = midPoint; i < keys.size(); i++) {
        if (isLeaf()) {
            newNode.insertKey(keys[i], records[i]);
        } else {
            newNode.insertKeyAndChild(keys[i], children[i + 1], newNode.getKeyCount());
        }
    }
    
    // Update this node's data
    if (!isLeaf()) {
        int midKey = keys[midPoint - 1];
        keys.resize(midPoint - 1);
        children.resize(midPoint);
        return midKey;
    } else {
        keys.resize(midPoint);
        records.resize(midPoint);
        return keys[midPoint];
    }
}

void BPlusTreeNode::insertKeyAndChild(int key, int childPageID, int index) {
    keys.insert(keys.begin() + index, key);
    children.insert(children.begin() + index + 1, childPageID);
}

int BPlusTreeNode::getKeyCount() const {
    return keys.size();
}

int BPlusTreeNode::getChildCount() const {
    return children.size();
}

int BPlusTreeNode::getKeyAt(int index) const {
    return keys[index];
}

void BPlusTreeNode::updateKeyAt(int index, int newKey) {
    keys[index] = newKey;
}

RecordPtr BPlusTreeNode::getRecordAt(int index) const {
    return records[index];
}

void BPlusTreeNode::getKeysInRange(int startKey, int endKey, 
                                 vector<RecordPtr>& results) const {
    for (size_t i = 0; i < keys.size(); i++) {
        if (keys[i] >= startKey && keys[i] <= endKey) {
            results.push_back(records[i]);
        }
        if (keys[i] > endKey) break;
    }
}

int BPlusTreeNode::getChildPageID(int index) const {
    return children[index];
}

void BPlusTreeNode::addChild(int childPageID) {
    children.push_back(childPageID);
}

void BPlusTreeNode::removeChild(int index) {
    children.erase(children.begin() + index);
    if (index < keys.size()) {
        keys.erase(keys.begin() + index);
    }
}

BPlusTreeNode* BPlusTreeNode::getChild(int index) const {
    // Check if this is a leaf node
    if (isLeaf()) {
        throw std::runtime_error("Cannot get child of leaf node");
    }

    // Check if index is valid
    if (index < 0 || index >= children.size()) {
        throw std::out_of_range("Child index out of range");
    }

    // Create and return a new node for the child
    // Note: The caller is responsible for deleting this node when done
    BufferManager& bufMgr = page->getBufferManager();
    PagePtr childPage = bufMgr.getPage(children[index]);
    return new BPlusTreeNode(childPage);
}

void BPlusTreeNode::ensureLeafLinksValid() {
    // Only leaf nodes need validation
    if (!isLeaf()) {
        return;
    }

    // Get the next leaf page ID from our current page
    int nextPageID = page->getNextLeafPage();

    // If there is no next page (-1), nothing to validate
    if (nextPageID == -1) {
        return;
    }

    try {
        // Try to fetch the next page
        PagePtr nextPage = page->getBufferManager().getPage(nextPageID);

        // Check if the next page is actually a leaf page
        if (!nextPage->isLeafPage()) {
            // If it's not a leaf page, we have an invalid link
            // Fix it by setting the next leaf pointer to -1
            page->setNextLeafPage(-1);

            // Log this issue for debugging purposes
            std::cerr << "Fixed invalid leaf link in page "
                     << page->getID()
                     << " pointing to non-leaf page "
                     << nextPageID << std::endl;
        }

    } catch (const std::exception& e) {
        // If we can't access the next page (it might have been deleted)
        // we need to fix our link
        page->setNextLeafPage(-1);

        std::cerr << "Removed invalid next-leaf reference in page "
                  << page->getID()
                  << ": " << e.what() << std::endl;
    }
}