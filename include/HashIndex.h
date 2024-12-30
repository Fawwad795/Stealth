#ifndef HASH_INDEX_H
#define HASH_INDEX_H

#include <string>
#include <vector>
#include <list>
#include "Record.h"
#include "BufferManager.h"
#include "FileManager.h"

class HashIndex {
public:
    // Constructor specifies initial number of buckets
    HashIndex(BufferManager& bufMgr, FileManager& fileMgr, size_t initialBuckets = 16);

    // Core operations
    bool insert(const std::string& key, const RecordPtr& recordPtr);
    bool remove(const std::string& key, const RecordPtr& recordPtr);

    bool removeFromPage(PagePtr page, const std::string &key, size_t hash, const RecordPtr &recordPtr);

    std::vector<RecordPtr> find(const std::string& key);

    void searchPage(PagePtr page, const std::string &key, size_t hash, std::vector<RecordPtr> &results);

    // Dynamic resizing support
    void resize(size_t newBucketCount);

    void redistributeEntriesFromPage(PagePtr page);

    // Statistics and monitoring
    double getLoadFactor() const;
    size_t getBucketCount() const;
    size_t getEntryCount() const;

private:
    // Bucket structure for hash table
    struct Bucket {
        int pageID;  // Page where this bucket's entries are stored
        size_t entryCount;
        size_t overflowCount;  // Number of overflow pages
    };

    // Entry structure for hash table
    struct HashEntry {
        std::string key;
        RecordPtr recordPtr;
        size_t hash;  // Store hash value to avoid recomputation
    };

    // Hash function and bucket management
    size_t hashFunction(const std::string& key) const;
    size_t getBucketIndex(size_t hashValue) const;
    Bucket& getBucket(size_t index);

    // Dynamic resizing helpers
    bool shouldGrow() const;
    bool shouldShrink() const;
    void redistributeEntries(size_t newBucketCount);

    // Member variables
    BufferManager& bufferManager;
    FileManager& fileManager;
    std::vector<Bucket> buckets;
    size_t entryCount;

    // Constants for resize thresholds
    static constexpr double LOAD_FACTOR_GROW = 0.75;
    static constexpr double LOAD_FACTOR_SHRINK = 0.25;
};

#endif // HASH_INDEX_H