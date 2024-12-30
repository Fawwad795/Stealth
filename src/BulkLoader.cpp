#include "BulkLoader.h"
#include <algorithm>

BulkLoader::BulkLoader(BPlusTree& tree, BufferManager& bufMgr, FileManager& fileMgr)
    : tree(tree)
    , bufferManager(bufMgr)
    , fileManager(fileMgr)
    , fillFactor(DEFAULT_FILL_FACTOR) {
}

void BulkLoader::addEntry(int key, const RecordPtr& recordPtr) {
    entries.push_back({key, recordPtr});
}

void BulkLoader::buildIndex() {
    if (entries.empty()) return;

    // Sort entries by key to prepare for bulk loading
    std::sort(entries.begin(), entries.end());

    // Create leaf nodes as the first level of the tree
    std::vector<BPlusTreeNode*> leafNodes;
    createLeafNodes(leafNodes);

    // Build internal nodes bottom-up until we have a single root
    std::vector<BPlusTreeNode*> currentLevel = leafNodes;
    while (currentLevel.size() > 1) {
        buildInternalNodes(currentLevel);
    }

    // Set the root of the tree
    if (!currentLevel.empty()) {
        tree.setRoot(currentLevel[0]->getPage()->getID());
    }

    // Clean up temporary resources
    cleanup();
}

void BulkLoader::createLeafNodes(std::vector<BPlusTreeNode*>& leafNodes) {
    // Calculate how many entries we can put in each leaf node based on fill factor
    int entriesPerLeaf = static_cast<int>(BPlusTreeNode::MAX_KEYS * fillFactor);

    for (size_t i = 0; i < entries.size(); i += entriesPerLeaf) {
        // Create a new leaf node
        PagePtr newPage = bufferManager.getPage(fileManager.allocateNewPage());
        newPage->convertToIndexPage(true);  // true indicates a leaf node
        BPlusTreeNode* leaf = new BPlusTreeNode(newPage);

        // Fill the leaf node with sorted entries
        size_t end = std::min(i + entriesPerLeaf, entries.size());
        for (size_t j = i; j < end; j++) {
            leaf->insertKey(entries[j].key, entries[j].recordPtr);
        }

        // Link leaf nodes together for range queries
        if (!leafNodes.empty()) {
            leafNodes.back()->getPage()->setNextLeafPage(leaf->getPage()->getID());
        }

        leafNodes.push_back(leaf);
    }
}

void BulkLoader::buildInternalNodes(std::vector<BPlusTreeNode*>& currentLevel) {
    std::vector<BPlusTreeNode*> nextLevel;

    // Group nodes into sets that will share a parent
    for (size_t i = 0; i < currentLevel.size(); i += BPlusTreeNode::MAX_KEYS + 1) {
        // Create a new internal node to be the parent
        PagePtr newPage = bufferManager.getPage(fileManager.allocateNewPage());
        newPage->convertToIndexPage(false);  // false indicates an internal node
        BPlusTreeNode* internal = new BPlusTreeNode(newPage);

        // Add the first child without a key (leftmost child)
        size_t end = std::min(i + BPlusTreeNode::MAX_KEYS + 1, currentLevel.size());
        internal->addChild(currentLevel[i]->getPage()->getID());

        // Add remaining children with their keys
        for (size_t j = i + 1; j < end; j++) {
            internal->insertKeyAndChild(
                currentLevel[j]->getKeyAt(0),
                currentLevel[j]->getPage()->getID());
        }

        nextLevel.push_back(internal);
    }

    // Update the current level to be the newly created level
    // First, clean up the old level
    for (auto* node : currentLevel) {
        delete node;
    }
    currentLevel = nextLevel;
}

void BulkLoader::cleanup() {
    // Clear the entries vector to free memory
    entries.clear();
    entries.shrink_to_fit();
}

void BulkLoader::clear() {
    entries.clear();
    entries.shrink_to_fit();
}

void BulkLoader::setFillFactor(float factor) {
    if (factor > 0.0f && factor <= 1.0f) {
        fillFactor = factor;
    }
}

float BulkLoader::getFillFactor() const {
    return fillFactor;
}