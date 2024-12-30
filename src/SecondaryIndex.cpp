#include "SecondaryIndex.h"
#include "BulkLoader.h"
#include <functional>

SecondaryIndex::SecondaryIndex(BufferManager& bufMgr, FileManager& fileMgr)
    : bufferManager(bufMgr)
    , fileManager(fileMgr) {
    // Initialize the B+ tree for this secondary index
    bPlusTree = std::make_unique<BPlusTree>(bufferManager, fileManager);
    initializeStats();
}

SecondaryIndex::~SecondaryIndex() {
    // B+ tree will be automatically cleaned up by unique_ptr
}

void SecondaryIndex::insertEntry(const std::string& key, const RecordPtr& recordPtr) {
    int hashedKey = hashKey(key);

    // Check if this key already exists
    auto existingEntries = findEntries(key);
    bool isNewKey = existingEntries.empty();

    // Insert into B+ tree
    bPlusTree->insert(hashedKey, recordPtr);

    // Update statistics
    updateStats(true, isNewKey);
}

std::vector<RecordPtr> SecondaryIndex::findEntries(const std::string& key) {
    int hashedKey = hashKey(key);

    // Use range search to find all entries with this key
    // Since we're using a hash, all entries with the same key will have
    // the same hash value
    return bPlusTree->rangeSearch(hashedKey, hashedKey);
}

void SecondaryIndex::removeEntry(const std::string& key, const RecordPtr& recordPtr) {
    int hashedKey = hashKey(key);

    // First check if this was the last entry for this key
    auto existingEntries = findEntries(key);
    bool isLastEntry = (existingEntries.size() == 1 &&
                       existingEntries[0] == recordPtr);

    // Remove from B+ tree
    bPlusTree->remove(hashedKey);

    // Update statistics
    updateStats(false, isLastEntry);
}

size_t SecondaryIndex::getEntryCount() const {
    return stats.totalEntries;
}

size_t SecondaryIndex::getUniqueKeyCount() const {
    return stats.uniqueKeys;
}

float SecondaryIndex::getAverageEntriesPerKey() const {
    return stats.averageEntriesPerKey;
}

void SecondaryIndex::rebuild() {
    // Create a new B+ tree
    auto newTree = std::make_unique<BPlusTree>(bufferManager, fileManager);

    // Traverse existing tree and copy all entries
    auto root = bPlusTree->getRoot();
    if (root != nullptr) {
        std::vector<std::pair<int, RecordPtr>> entries;
        // Collect all entries from current tree
        collectEntries(root, entries);

        // Sort entries by key
        std::sort(entries.begin(), entries.end());

        // Bulk load into new tree
        BulkLoader loader(*newTree, bufferManager, fileManager);
        for (const auto& entry : entries) {
            loader.addEntry(entry.first, entry.second);
        }
        loader.buildIndex();
    }

    // Replace old tree with new one
    bPlusTree = std::move(newTree);

    // Recalculate statistics
    initializeStats();
}

int SecondaryIndex::hashKey(const std::string& key) const {
    // Using a simple but effective string hashing algorithm (djb2)
    unsigned long hash = 5381;
    for (char c : key) {
        hash = ((hash << 5) + hash) + c;
    }
    return static_cast<int>(hash);
}

void SecondaryIndex::updateStats(bool isInsert, bool isNewKey) {
    if (isInsert) {
        stats.totalEntries++;
        if (isNewKey) {
            stats.uniqueKeys++;
        }
    } else {
        stats.totalEntries--;
        if (isNewKey) {  // isNewKey here means it was the last entry
            stats.uniqueKeys--;
        }
    }

    if (stats.uniqueKeys > 0) {
        stats.averageEntriesPerKey =
            static_cast<float>(stats.totalEntries) / stats.uniqueKeys;
    } else {
        stats.averageEntriesPerKey = 0.0f;
    }
}

void SecondaryIndex::initializeStats() {
    stats = {0, 0, 0, 0.0f};
}


void SecondaryIndex::collectEntries(BPlusTreeNode* node,
                                std::vector<std::pair<int, RecordPtr>>& entries) {
    if (node == nullptr) {
        return;
    }

    if (node->isLeaf()) {
        // For leaf nodes, collect all entries
        for (int i = 0; i < node->getKeyCount(); i++) {
            entries.push_back({
                node->getKeyAt(i),
                node->getRecordAt(i)
            });
        }

        // If there's a next leaf node, process it
        int nextPageID = node->getPage()->getNextLeafPage();
        if (nextPageID != -1) {
            BPlusTreeNode* nextNode = new BPlusTreeNode(
                bufferManager.getPage(nextPageID));
            collectEntries(nextNode, entries);
            delete nextNode;
        }
    } else {
        // For internal nodes, recurse to children
        for (int i = 0; i < node->getChildCount(); i++) {
            BPlusTreeNode* childNode = new BPlusTreeNode(
                bufferManager.getPage(node->getChildPageID(i)));
            collectEntries(childNode, entries);
            delete childNode;
        }
    }
}