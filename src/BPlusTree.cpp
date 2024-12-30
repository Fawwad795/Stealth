#include "BPlusTree.h"

void BPlusTree::createNewRoot() {
    //creating a new page for the root
    PagePtr newPage = bufferManager.getPage(fileManager.allocateNewPage());
    newPage->convertToIndexPage(true);  // Start as leaf node
    rootPageID = newPage->getID();
}

bool BPlusTree::insert(int key, const RecordPtr& recordPtr) {
    // First check if tree is empty and needs initialization
    if (rootPageID == -1) {
        createNewRoot();
    }

    // Get the root node
    BPlusTreeNode* root = fetchNode(rootPageID);

    // Check if root is full and needs splitting
    if (root->isFull()) {
        // Create new root page
        PagePtr newRootPage = bufferManager.getPage(fileManager.allocateNewPage());
        newRootPage->convertToIndexPage(false);  // Make it an internal node
        BPlusTreeNode* newRoot = new BPlusTreeNode(newRootPage);

        // Make old root the first child of new root
        newRoot->addChild(rootPageID);

        // Split the old root
        splitChild(newRoot, 0);

        // Update root page ID
        rootPageID = newRootPage->getID();
        root = newRoot;
    }

    // Perform the actual insertion
    bool success = insertInternal(root, key, recordPtr);

    // Update statistics only if insertion was successful
    if (success) {
        updateStatistics();

        // If we created a new root earlier, we need to update its statistics
        if (root != nullptr) {
            // Update node-specific statistics like key count and fill factor
            root->getPage()->calculateChecksum();
        }
    }

    // Clean up allocated memory
    delete root;

    return success;
}

bool BPlusTree::remove(int key) {
    // Check if tree is empty
    if (rootPageID == -1) {
        return false;
    }

    // Get the root node
    BPlusTreeNode* root = fetchNode(rootPageID);

    // Perform the removal operation
    bool success = removeInternal(root, key);

    if (success) {
        // Update statistics after successful removal
        updateStatistics();

        // Check if root only has one child after removal
        if (!root->isLeaf() && root->getKeyCount() == 0) {
            // Root is empty, make its only child the new root
            int newRootPageID = root->getChildPageID(0);
            rootPageID = newRootPageID;

            // Free the old root page
            bufferManager.unpinPage(root->getPage()->getID());
            fileManager.freePage(root->getPage()->getID());
        }

        // Update node-specific statistics
        if (root != nullptr) {
            root->getPage()->calculateChecksum();
        }

        // If root has changed, we need to ensure leaf links are still valid
        BPlusTreeNode* newRoot = fetchNode(rootPageID);
        if (newRoot->isLeaf()) {
            newRoot->ensureLeafLinksValid();
        }
        delete newRoot;
    }

    // Clean up allocated memory
    delete root;

    return success;
}

bool BPlusTree::insertInternal(BPlusTreeNode* node, int key, const RecordPtr& recordPtr) {
    // If this is a leaf node, insert here
    if (node->isLeaf()) {
        return node->insertKey(key, recordPtr);
    }

    // Find the child node where key should go
    int childIndex = node->findChildIndex(key);
    BPlusTreeNode* childNode = fetchNode(node->getChildPageID(childIndex));

    // If child is full, split it
    if (childNode->isFull()) {
        splitChild(node, childIndex);

        // After split, determine which child to follow
        if (key > node->getKeyAt(childIndex)) {
            childNode = fetchNode(node->getChildPageID(childIndex + 1));
        }
    }

    // Recursively insert into child
    return insertInternal(childNode, key, recordPtr);
}

void BPlusTree::splitChild(BPlusTreeNode* parent, int childIndex) {
    // Get the child that needs splitting
    BPlusTreeNode* child = fetchNode(parent->getChildPageID(childIndex));

    // Create new node for split
    PagePtr newPage = bufferManager.getPage(fileManager.allocateNewPage());
    newPage->convertToIndexPage(child->isLeaf());
    BPlusTreeNode* newNode = new BPlusTreeNode(newPage);

    // Split the child
    int midKey = child->splitNode(*newNode);

    // Insert the middle key and new child pointer into parent
    parent->insertKeyAndChild(midKey, newPage->getID(), childIndex);
}

RecordPtr BPlusTree::find(int key) const {
    BPlusTreeNode* node = fetchNode(rootPageID);
    return findInternal(node, key);
}

RecordPtr BPlusTree::findInternal(BPlusTreeNode* node, int key) const {
    if (node->isLeaf()) {
        return node->findKey(key);
    }

    // Find appropriate child and recurse
    int childIndex = node->findChildIndex(key);
    return findInternal(fetchNode(node->getChildPageID(childIndex)), key);
}

vector<RecordPtr> BPlusTree::rangeSearch(int startKey, int endKey) const {
    vector<RecordPtr> results;
    BPlusTreeNode* node = fetchNode(rootPageID);

    // Find leaf node containing startKey
    while (!node->isLeaf()) {
        int childIndex = node->findChildIndex(startKey);
        node = fetchNode(node->getChildPageID(childIndex));
    }

    // Traverse leaf nodes
    while (node != nullptr) {
        // Validate leaf chain before using it
        node->ensureLeafLinksValid();

        // Add qualifying records from current node
        node->getKeysInRange(startKey, endKey, results);

        // Move to next leaf if it exists
        int nextPageID = node->getPage()->getNextLeafPage();
        if (nextPageID == -1) break;

        delete node;  // Clean up current node
        node = fetchNode(nextPageID);

        // Break if we've passed the end key
        if (node->getKeyAt(0) > endKey) break;
    }

    delete node;  // Clean up final node
    return results;
}

BPlusTreeNode* BPlusTree::fetchNode(int pageID) const {
    try {
        // Get the page from buffer manager
        PagePtr page = bufferManager.getPage(pageID);

        // If this is a new page, we need to initialize it
        if (!page->isIndexPage()) {
            page->convertToIndexPage();  // This will set up the page for index use
        }

        // Pin the page while we're using it
        bufferManager.pinPage(pageID);

        // Create a new B+ tree node wrapping this page
        return new BPlusTreeNode(page);

    } catch (const exception& e) {
        // If something goes wrong during fetch, clean up
        bufferManager.unpinPage(pageID);  // Make sure to unpin if we failed
        throw runtime_error("Failed to fetch node: " + string(e.what()));
    }
}

bool BPlusTree::removeInternal(BPlusTreeNode* node, int key) {
    int keyIndex = node->findKeyPosition(key);

    if (node->isLeaf()) {
        // If this is a leaf node, simply remove the key if it exists
        if (keyIndex < node->getKeyCount() && node->getKeyAt(keyIndex) == key) {
            node->removeKey(key);

            // Check if node has underflowed
            if (node->isUnderflow() && node->getPage()->getID() != rootPageID) {
                handleLeafUnderflow(node);
            }
            return true;
        }
        return false;  // Key not found
    }

    // Internal node case
    int childIndex = node->findChildIndex(key);
    BPlusTreeNode* childNode = fetchNode(node->getChildPageID(childIndex));

    // Ensure child has enough keys before descending
    if (childNode->getKeyCount() == BPlusTreeNode::MIN_KEYS) {
        handleInternalNodeUnderflow(node, childIndex);
        // After redistribution/merge, we need to update childNode
        childNode = fetchNode(node->getChildPageID(node->findChildIndex(key)));
    }

    return removeInternal(childNode, key);
}

void BPlusTree::handleLeafUnderflow(BPlusTreeNode* node) {
    BPlusTreeNode* parent = fetchNode(node->getParentPageID());
    int nodeIndex = parent->findChildIndex(node->getKeyAt(0));

    // Try to borrow from left sibling
    if (nodeIndex > 0) {
        BPlusTreeNode* leftSibling = fetchNode(parent->getChildPageID(nodeIndex - 1));
        if (leftSibling->getKeyCount() > BPlusTreeNode::MIN_KEYS) {
            // Borrow from left sibling
            borrowFromLeftLeaf(node, leftSibling, parent, nodeIndex);
            return;
        }
    }

    // Try to borrow from right sibling
    if (nodeIndex < parent->getChildCount() - 1) {
        BPlusTreeNode* rightSibling = fetchNode(parent->getChildPageID(nodeIndex + 1));
        if (rightSibling->getKeyCount() > BPlusTreeNode::MIN_KEYS) {
            // Borrow from right sibling
            borrowFromRightLeaf(node, rightSibling, parent, nodeIndex);
            return;
        }
    }

    // If we can't borrow, we need to merge
    if (nodeIndex > 0) {
        // Merge with left sibling
        BPlusTreeNode* leftSibling = fetchNode(parent->getChildPageID(nodeIndex - 1));
        mergeLeafNodes(leftSibling, node, parent, nodeIndex);
    } else {
        // Merge with right sibling
        BPlusTreeNode* rightSibling = fetchNode(parent->getChildPageID(nodeIndex + 1));
        mergeLeafNodes(node, rightSibling, parent, nodeIndex);
    }
}

void BPlusTree::borrowFromLeftLeaf(BPlusTreeNode* node, BPlusTreeNode* leftSibling,
                                 BPlusTreeNode* parent, int nodeIndex) {
    // Get the last key and record from left sibling
    int key = leftSibling->getKeyAt(leftSibling->getKeyCount() - 1);
    RecordPtr recordPtr = leftSibling->getRecordAt(leftSibling->getKeyCount() - 1);

    // Remove from left sibling
    leftSibling->removeKey(key);

    // Insert into current node
    node->insertKey(key, recordPtr);

    // Update parent's separator key
    parent->updateKeyAt(nodeIndex, node->getKeyAt(0));
}

void BPlusTree::borrowFromRightLeaf(BPlusTreeNode* node, BPlusTreeNode* rightSibling,
                                  BPlusTreeNode* parent, int nodeIndex) {
    // Get the first key and record from right sibling
    int key = rightSibling->getKeyAt(0);
    RecordPtr recordPtr = rightSibling->getRecordAt(0);

    // Remove from right sibling
    rightSibling->removeKey(key);

    // Insert into current node
    node->insertKey(key, recordPtr);

    // Update parent's separator key
    parent->updateKeyAt(nodeIndex + 1, rightSibling->getKeyAt(0));
}

void BPlusTree::mergeLeafNodes(BPlusTreeNode* leftNode, BPlusTreeNode* rightNode,
                              BPlusTreeNode* parent, int rightNodeIndex) {
    // Move all keys from right node to left node
    for (int i = 0; i < rightNode->getKeyCount(); i++) {
        leftNode->insertKey(rightNode->getKeyAt(i), rightNode->getRecordAt(i));
    }

    // Update leaf node links
    leftNode->getPage()->setNextLeafPage(rightNode->getPage()->getNextLeafPage());

    // Remove right node's entry from parent
    parent->removeChild(rightNodeIndex);

    // Free the right node's page
    bufferManager.unpinPage(rightNode->getPage()->getID());
    fileManager.freePage(rightNode->getPage()->getID());
    delete rightNode;
}

void BPlusTree::handleInternalNodeUnderflow(BPlusTreeNode* parent, int childIndex) {
    BPlusTreeNode* child = fetchNode(parent->getChildPageID(childIndex));

    // Try to borrow from left sibling
    if (childIndex > 0) {
        BPlusTreeNode* leftSibling = fetchNode(parent->getChildPageID(childIndex - 1));
        if (leftSibling->getKeyCount() > BPlusTreeNode::MIN_KEYS) {
            borrowFromLeftInternal(child, leftSibling, parent, childIndex);
            return;
        }
    }

    // Try to borrow from right sibling
    if (childIndex < parent->getChildCount() - 1) {
        BPlusTreeNode* rightSibling = fetchNode(parent->getChildPageID(childIndex + 1));
        if (rightSibling->getKeyCount() > BPlusTreeNode::MIN_KEYS) {
            borrowFromRightInternal(child, rightSibling, parent, childIndex);
            return;
        }
    }

    // If we can't borrow, merge nodes
    if (childIndex > 0) {
        // Merge with left sibling
        BPlusTreeNode* leftSibling = fetchNode(parent->getChildPageID(childIndex - 1));
        mergeInternalNodes(leftSibling, child, parent, childIndex);
    } else {
        // Merge with right sibling
        BPlusTreeNode* rightSibling = fetchNode(parent->getChildPageID(childIndex + 1));
        mergeInternalNodes(child, rightSibling, parent, childIndex + 1);
    }
}

BPlusTreeNode* BPlusTree::getRoot() const {
    // If there's no root yet, return nullptr
    if (rootPageID == -1) {
        return nullptr;
    }

    // Use fetchNode to get the root node
    // fetchNode handles buffer management and page pinning
    return fetchNode(rootPageID);
}

int BPlusTree::findMin() const {
    // Check if tree is empty
    if (rootPageID == -1) {
        throw std::runtime_error("Tree is empty");
    }

    BPlusTreeNode* current = fetchNode(rootPageID);

    // Traverse to the leftmost leaf node
    while (!current->isLeaf()) {
        // Get the leftmost child
        int childID = current->getChildPageID(0);
        // Don't forget to delete the current node before moving to child
        delete current;
        current = fetchNode(childID);
    }

    // Get the minimum key from the leftmost leaf
    int minKey = current->getKeyAt(0);
    delete current;

    return minKey;
}

int BPlusTree::findMax() const {
    // Check if tree is empty
    if (rootPageID == -1) {
        throw std::runtime_error("Tree is empty");
    }

    BPlusTreeNode* current = fetchNode(rootPageID);

    // Traverse to the rightmost leaf node
    while (!current->isLeaf()) {
        // Get the rightmost child
        int childID = current->getChildPageID(current->getChildCount() - 1);
        delete current;
        current = fetchNode(childID);
    }

    // Get the maximum key from the rightmost leaf
    int maxKey = current->getKeyAt(current->getKeyCount() - 1);
    delete current;

    return maxKey;
}

void BPlusTree::updateStatistics() {
    // This method encapsulates all statistics updates in one place
    indexStats.updateStats(*this);
}
