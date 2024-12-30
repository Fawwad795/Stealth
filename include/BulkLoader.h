#ifndef BULK_LOADER_H
#define BULK_LOADER_H

#include "BPlusTree.h"
#include "Record.h"
#include <vector>

class BulkLoader {
public:
    // Constructor
    BulkLoader(BPlusTree& tree, BufferManager& bufMgr, FileManager& fileMgr);

    // Core operations
    void addEntry(int key, const RecordPtr& recordPtr);
    void buildIndex();
    void clear();

    // Configuration
    void setFillFactor(float factor);
    float getFillFactor() const;

private:
    struct Entry {
        int key;
        RecordPtr recordPtr;
        bool operator<(const Entry& other) const {
            return key < other.key;
        }
    };

    // Internal helper functions
    void createLeafNodes(std::vector<BPlusTreeNode*>& leafNodes);
    void buildInternalNodes(std::vector<BPlusTreeNode*>& currentLevel);
    void cleanup();

    // Member variables
    BPlusTree& tree;
    BufferManager& bufferManager;
    FileManager& fileManager;
    std::vector<Entry> entries;
    float fillFactor;

    static constexpr float DEFAULT_FILL_FACTOR = 0.85f;
};

#endif // BULK_LOADER_H