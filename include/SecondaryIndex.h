#ifndef SECONDARY_INDEX_H
#define SECONDARY_INDEX_H

#include "BPlusTree.h"
#include "Record.h"
#include <memory>
#include <string>
#include <vector>

class SecondaryIndex {
public:
    // Constructor and destructor
    SecondaryIndex(BufferManager& bufMgr, FileManager& fileMgr);
    ~SecondaryIndex();

    // Core operations
    void insertEntry(const std::string& key, const RecordPtr& recordPtr);
    std::vector<RecordPtr> findEntries(const std::string& key);
    void removeEntry(const std::string& key, const RecordPtr& recordPtr);

    // Statistics and maintenance
    size_t getEntryCount() const;
    size_t getUniqueKeyCount() const;
    float getAverageEntriesPerKey() const;
    void rebuild(); // For maintenance and optimization

private:
    // Statistics tracking
    struct IndexStats {
        size_t totalEntries;
        size_t uniqueKeys;
        size_t height;
        float averageEntriesPerKey;
    };

    // Internal helper functions
    int hashKey(const std::string& key) const;
    void updateStats(bool isInsert, bool isNewKey);
    void initializeStats();

    // Helper method to collect all entries from the tree
    void collectEntries(BPlusTreeNode* node,
                       std::vector<std::pair<int, RecordPtr>>& entries);

    // Member variables
    std::unique_ptr<BPlusTree> bPlusTree;
    BufferManager& bufferManager;
    FileManager& fileManager;
    IndexStats stats;
};

#endif // SECONDARY_INDEX_H