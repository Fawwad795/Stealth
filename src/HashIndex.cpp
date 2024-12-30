#include "HashIndex.h"
#include "Page.h"
#include <algorithm>
#include <cassert>

HashIndex::HashIndex(BufferManager& bufMgr, FileManager& fileMgr, size_t initialBuckets)
    : bufferManager(bufMgr)
    , fileManager(fileMgr)
    , buckets(initialBuckets)
    , entryCount(0) {

    // Initialize buckets
    for (auto& bucket : buckets) {
        bucket.pageID = fileManager.allocateNewPage();
        bucket.entryCount = 0;
        bucket.overflowCount = 0;
    }
}

bool HashIndex::insert(const std::string& key, const RecordPtr& recordPtr) {
    size_t hash = hashFunction(key);
    size_t bucketIndex = getBucketIndex(hash);
    Bucket& bucket = buckets[bucketIndex];

    // Get the bucket's page
    PagePtr page = bufferManager.getPage(bucket.pageID);

    // Create new hash entry
    HashEntry entry{key, recordPtr, hash};

    // Try to insert into main bucket page
    if (page->getFreeSpace() >= sizeof(HashEntry)) {
        // Store entry in page
        page->addRecord(Record(-1, {key,
                                   std::to_string(recordPtr.pageID),
                                   std::to_string(recordPtr.slotNumber),
                                   std::to_string(hash)}));
        bucket.entryCount++;
        entryCount++;

        // Check if we need to resize
        if (shouldGrow()) {
            resize(buckets.size() * 2);
        }

        return true;
    }

    // Main bucket is full, handle overflow
    if (bucket.overflowCount == 0) {
        // Create first overflow page
        int overflowPageID = fileManager.allocateNewPage();
        PagePtr overflowPage = bufferManager.getPage(overflowPageID);

        // Link overflow page
        page->getHeader().nextPage = overflowPageID;
        bucket.overflowCount++;

        // Store entry in overflow page
        return overflowPage->addRecord(Record(-1, {key,
                                                  std::to_string(recordPtr.pageID),
                                                  std::to_string(recordPtr.slotNumber),
                                                  std::to_string(hash)}));
    }

    // Find overflow page with space
    int currentPageID = page->getHeader().nextPage;
    while (currentPageID != -1) {
        PagePtr currentPage = bufferManager.getPage(currentPageID);

        if (currentPage->getFreeSpace() >= sizeof(HashEntry)) {
            // Store entry in this overflow page
            return currentPage->addRecord(Record(-1, {key,
                                                    std::to_string(recordPtr.pageID),
                                                    std::to_string(recordPtr.slotNumber),
                                                    std::to_string(hash)}));
        }

        currentPageID = currentPage->getHeader().nextPage;
    }

    // All overflow pages are full, create new one
    int newOverflowPageID = fileManager.allocateNewPage();
    PagePtr lastPage = bufferManager.getPage(currentPageID);
    lastPage->getHeader().nextPage = newOverflowPageID;
    bucket.overflowCount++;

    // Store entry in new overflow page
    PagePtr newOverflowPage = bufferManager.getPage(newOverflowPageID);
    return newOverflowPage->addRecord(Record(-1, {key,
                                                 std::to_string(recordPtr.pageID),
                                                 std::to_string(recordPtr.slotNumber),
                                                 std::to_string(hash)}));
}

std::vector<RecordPtr> HashIndex::find(const std::string& key) {
    std::vector<RecordPtr> results;
    size_t hash = hashFunction(key);
    size_t bucketIndex = getBucketIndex(hash);
    Bucket& bucket = buckets[bucketIndex];

    // Search main bucket page
    PagePtr page = bufferManager.getPage(bucket.pageID);
    searchPage(page, key, hash, results);

    // Search overflow pages
    int currentPageID = page->getHeader().nextPage;
    while (currentPageID != -1) {
        PagePtr currentPage = bufferManager.getPage(currentPageID);
        searchPage(currentPage, key, hash, results);
        currentPageID = currentPage->getHeader().nextPage;
    }

    return results;
}

void HashIndex::searchPage(PagePtr page, const std::string& key, size_t hash,
                         std::vector<RecordPtr>& results) {
    // Search all records in page
    for (int i = 0; i < page->getRecordCount(); i++) {
        Record record = page->getRecord(i);
        auto attributes = record.getAttributes();

        // Check if hash and key match
        if (std::stoull(attributes[3]) == hash && attributes[0] == key) {
            results.push_back(RecordPtr(
                std::stoi(attributes[1]),    // pageID
                std::stoi(attributes[2])     // slotNumber
            ));
        }
    }
}

bool HashIndex::remove(const std::string& key, const RecordPtr& recordPtr) {
    size_t hash = hashFunction(key);
    size_t bucketIndex = getBucketIndex(hash);
    Bucket& bucket = buckets[bucketIndex];

    // Search in main bucket page
    PagePtr page = bufferManager.getPage(bucket.pageID);
    if (removeFromPage(page, key, hash, recordPtr)) {
        bucket.entryCount--;
        entryCount--;

        // Check if we need to resize
        if (shouldShrink()) {
            resize(buckets.size() / 2);
        }

        return true;
    }

    // Search in overflow pages
    int currentPageID = page->getHeader().nextPage;
    while (currentPageID != -1) {
        PagePtr currentPage = bufferManager.getPage(currentPageID);
        if (removeFromPage(currentPage, key, hash, recordPtr)) {
            bucket.entryCount--;
            entryCount--;
            return true;
        }
        currentPageID = currentPage->getHeader().nextPage;
    }

    return false;
}

bool HashIndex::removeFromPage(PagePtr page, const std::string& key, size_t hash,
                             const RecordPtr& recordPtr) {
    for (int i = 0; i < page->getRecordCount(); i++) {
        Record record = page->getRecord(i);
        auto attributes = record.getAttributes();

        if (std::stoull(attributes[3]) == hash &&
            attributes[0] == key &&
            std::stoi(attributes[1]) == recordPtr.pageID &&
            std::stoi(attributes[2]) == recordPtr.slotNumber) {

            return page->removeRecord(i);
        }
    }
    return false;
}

size_t HashIndex::hashFunction(const std::string& key) const {
    // Using a simple but effective string hashing algorithm (djb2)
    size_t hash = 5381;
    for (char c : key) {
        hash = ((hash << 5) + hash) + c;
    }
    return hash;
}

size_t HashIndex::getBucketIndex(size_t hashValue) const {
    return hashValue % buckets.size();
}

void HashIndex::resize(size_t newBucketCount) {
    // Save old buckets
    std::vector<Bucket> oldBuckets = std::move(buckets);

    // Initialize new buckets
    buckets.resize(newBucketCount);
    for (auto& bucket : buckets) {
        bucket.pageID = fileManager.allocateNewPage();
        bucket.entryCount = 0;
        bucket.overflowCount = 0;
    }

    // Redistribute entries
    for (const auto& oldBucket : oldBuckets) {
        PagePtr page = bufferManager.getPage(oldBucket.pageID);
        redistributeEntriesFromPage(page);

        // Process overflow pages
        int currentPageID = page->getHeader().nextPage;
        while (currentPageID != -1) {
            PagePtr currentPage = bufferManager.getPage(currentPageID);
            redistributeEntriesFromPage(currentPage);

            // Free overflow page
            fileManager.freePage(currentPageID);
            currentPageID = currentPage->getHeader().nextPage;
        }

        // Free main bucket page
        fileManager.freePage(oldBucket.pageID);
    }
}

void HashIndex::redistributeEntriesFromPage(PagePtr page) {
    for (int i = 0; i < page->getRecordCount(); i++) {
        Record record = page->getRecord(i);
        auto attributes = record.getAttributes();

        // Re-insert entry into new bucket structure
        insert(attributes[0],  // key
               RecordPtr(std::stoi(attributes[1]),    // pageID
                        std::stoi(attributes[2])));    // slotNumber
    }
}

bool HashIndex::shouldGrow() const {
    return getLoadFactor() > LOAD_FACTOR_GROW;
}

bool HashIndex::shouldShrink() const {
    return buckets.size() > 16 && getLoadFactor() < LOAD_FACTOR_SHRINK;
}

double HashIndex::getLoadFactor() const {
    return static_cast<double>(entryCount) / buckets.size();
}

size_t HashIndex::getBucketCount() const {
    return buckets.size();
}

size_t HashIndex::getEntryCount() const {
    return entryCount;
}