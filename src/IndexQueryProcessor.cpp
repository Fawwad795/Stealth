#include "IndexQueryProcessor.h"
#include <chrono>
#include <functional>
#include <sstream>
#include <stdexcept>

#include "BulkLoader.h"

using namespace std::chrono;

IndexQueryProcessor::IndexQueryProcessor(BufferManager& bufMgr, FileManager& fileMgr)
    : bufferManager(bufMgr)
    , fileManager(fileMgr)
    , stats{0, 0, 0.0, 0.0} {
    // Initialize B+ tree
    bPlusTree = std::make_unique<BPlusTree>(bufferManager, fileManager);
}

bool IndexQueryProcessor::processInsert(const std::string& key, const Record& record) {
    auto startTime = high_resolution_clock::now();

    try {
        // Convert string key to integer for B+ tree
        int intKey = parseKey(key);

        // Store the record and get its pointer
        RecordPtr recordPtr;
        if (!storeRecord(record, recordPtr)) {
            return false;
        }

        // Insert into B+ tree
        bool success = bPlusTree->insert(intKey, recordPtr);

        if (success) {
            updateIndexHealth(false);  // Track insert operation
        }

        // Update statistics
        auto endTime = high_resolution_clock::now();
        double duration = duration_cast<microseconds>(endTime - startTime).count();
        updateAccessStats(duration);

        return success;
    } catch (const std::exception& e) {
        // Log error and return false
        std::cerr << "Insert failed: " << e.what() << std::endl;
        return false;
    }
}

bool IndexQueryProcessor::processDelete(const std::string& key) {
    auto startTime = high_resolution_clock::now();

    try {
        int intKey = parseKey(key);

        // Find the record first to ensure it exists
        RecordPtr recordPtr = bPlusTree->find(intKey);
        if (!recordPtr.pageID) {
            return false;  // Record not found
        }

        // Remove from B+ tree
        bool success = bPlusTree->remove(intKey);

        if (success) {
            updateIndexHealth(true);  // Track delete operation
        }

        // Update statistics
        auto endTime = high_resolution_clock::now();
        double duration = duration_cast<microseconds>(endTime - startTime).count();
        updateAccessStats(duration);

        return success;
    } catch (const std::exception& e) {
        std::cerr << "Delete failed: " << e.what() << std::endl;
        return false;
    }
}

std::vector<Record> IndexQueryProcessor::processSelect(const std::string& condition) {
    auto startTime = high_resolution_clock::now();
    std::vector<Record> results;

    try {
        std::string operation, value;
        if (!parseCondition(condition, operation, value)) {
            return results;  // Return empty if condition parsing fails
        }

        int intKey = parseKey(value);

        if (operation == "=") {
            // Exact match query
            RecordPtr recordPtr = bPlusTree->find(intKey);
            if (recordPtr.pageID != -1) {
                results.push_back(fetchRecord(recordPtr));
            }
        } else if (operation == ">" || operation == ">=") {
            // Range query from value to maximum
            int maxKey = bPlusTree->findMax();
            auto recordPtrs = bPlusTree->rangeSearch(intKey, maxKey);
            for (const auto& ptr : recordPtrs) {
                results.push_back(fetchRecord(ptr));
            }
        } else if (operation == "<" || operation == "<=") {
            // Range query from minimum to value
            int minKey = bPlusTree->findMin();
            auto recordPtrs = bPlusTree->rangeSearch(minKey, intKey);
            for (const auto& ptr : recordPtrs) {
                results.push_back(fetchRecord(ptr));
            }
        }

        // Update statistics
        auto endTime = high_resolution_clock::now();
        double duration = duration_cast<microseconds>(endTime - startTime).count();
        updateAccessStats(duration);

    } catch (const std::exception& e) {
        std::cerr << "Select failed: " << e.what() << std::endl;
    }

    return results;
}

bool IndexQueryProcessor::processUpdate(const std::string& key, const Record& newRecord) {
    auto startTime = high_resolution_clock::now();

    try {
        int intKey = parseKey(key);

        // Find existing record
        RecordPtr oldRecordPtr = bPlusTree->find(intKey);
        if (oldRecordPtr.pageID == -1) {
            return false;  // Record not found
        }

        // Store new record
        RecordPtr newRecordPtr;
        if (!storeRecord(newRecord, newRecordPtr)) {
            return false;
        }

        // Update B+ tree to point to new record
        bool success = bPlusTree->remove(intKey) &&
                      bPlusTree->insert(intKey, newRecordPtr);

        // Update statistics
        auto endTime = high_resolution_clock::now();
        double duration = duration_cast<microseconds>(endTime - startTime).count();
        updateAccessStats(duration);

        return success;
    } catch (const std::exception& e) {
        std::cerr << "Update failed: " << e.what() << std::endl;
        return false;
    }
}

std::vector<Record> IndexQueryProcessor::processRangeSelect(
    const std::string& startKey, const std::string& endKey) {
    auto startTime = high_resolution_clock::now();
    std::vector<Record> results;

    try {
        int start = parseKey(startKey);
        int end = parseKey(endKey);

        auto recordPtrs = bPlusTree->rangeSearch(start, end);
        for (const auto& ptr : recordPtrs) {
            results.push_back(fetchRecord(ptr));
        }

        // Update statistics
        auto endTime = high_resolution_clock::now();
        double duration = duration_cast<microseconds>(endTime - startTime).count();
        updateAccessStats(duration);

    } catch (const std::exception& e) {
        std::cerr << "Range select failed: " << e.what() << std::endl;
    }

    return results;
}

int IndexQueryProcessor::parseKey(const std::string& key) const {
    try {
        return std::stoi(key);
    } catch (const std::exception& e) {
        throw std::invalid_argument("Invalid key format: " + key);
    }
}

bool IndexQueryProcessor::parseCondition(const std::string& condition,
                                      std::string& operation,
                                      std::string& value) const {
    // Simple condition parser for demonstration
    // Expects format: "key <op> value" where <op> is =, <, >, <=, or >=
    std::istringstream iss(condition);
    std::string field;
    iss >> field >> operation >> value;

    // Validate operation
    if (operation != "=" && operation != "<" && operation != ">" &&
        operation != "<=" && operation != ">=") {
        return false;
    }

    return true;
}

Record IndexQueryProcessor::fetchRecord(const RecordPtr& ptr) const {
    // Get page from buffer manager
    PagePtr page = bufferManager.getPage(ptr.pageID);
    return page->getRecord(ptr.slotNumber);
}

bool IndexQueryProcessor::storeRecord(const Record& record, RecordPtr& outPtr) {
    // For simplicity, we'll store in a new page
    // In a real implementation, you'd want to find a page with enough space
    int newPageID = fileManager.allocateNewPage();
    PagePtr page = bufferManager.getPage(newPageID);

    if (page->addRecord(record)) {
        outPtr = RecordPtr(newPageID, page->getRecordCount() - 1);
        return true;
    }
    return false;
}

void IndexQueryProcessor::updateAccessStats(double accessTime) {
    stats.totalQueries++;
    stats.totalAccessTime += accessTime;
    stats.avgAccessTime = stats.totalAccessTime / stats.totalQueries;
}

size_t IndexQueryProcessor::getRecordCount() const {
    return stats.totalQueries;  // This is a simplification
}

double IndexQueryProcessor::getAverageAccessTime() const {
    return stats.avgAccessTime;
}

void IndexQueryProcessor::optimizeIndex() {
    // Implement index optimization logic
    // This could include rebuilding the tree if it's too fragmented
    if (needsRebuild()) {
        rebuildIndex();
    }
}

bool IndexQueryProcessor::rebuildIndex() {
    try {
        // Step 1: Create a temporary B+ tree
        auto newTree = std::make_unique<BPlusTree>(bufferManager, fileManager);
        
        // Step 2: Collect all existing records with their keys
        std::vector<std::pair<int, RecordPtr>> entries;
        BPlusTreeNode* root = bPlusTree->getRoot();
        
        if (root) {
            // Lambda to collect entries from the tree
            std::function<void(BPlusTreeNode*)> collectEntries = 
                [&](BPlusTreeNode* node) {
                    if (node->isLeaf()) {
                        // Collect all entries from leaf node
                        for (int i = 0; i < node->getKeyCount(); i++) {
                            entries.push_back({
                                node->getKeyAt(i),
                                node->getRecordAt(i)
                            });
                        }
                    } else {
                        // Process child nodes
                        for (int i = 0; i < node->getChildCount(); i++) {
                            BPlusTreeNode* child = node->getChild(i);
                            collectEntries(child);
                            delete child;  // Clean up after processing
                        }
                    }
                };
            
            // Collect all entries from the existing tree
            collectEntries(root);
        }
        
        // Step 3: Sort entries by key for optimal insertion
        std::sort(entries.begin(), entries.end(),
                 [](const auto& a, const auto& b) {
                     return a.first < b.first;
                 });
        
        // Step 4: Use bulk loading for efficient rebuilding
        BulkLoader loader(*newTree, bufferManager, fileManager);
        
        // Set an optimal fill factor for the new tree
        loader.setFillFactor(0.85);  // 85% fill factor for good performance
        
        // Add all entries to the bulk loader
        for (const auto& entry : entries) {
            loader.addEntry(entry.first, entry.second);
        }
        
        // Build the new index
        loader.buildIndex();
        
        // Step 5: Verify the new tree
        if (entries.size() > 0) {
            // Verify first and last entries exist in new tree
            RecordPtr firstRecord = newTree->find(entries.front().first);
            RecordPtr lastRecord = newTree->find(entries.back().first);
            
            if (firstRecord.pageID == -1 || lastRecord.pageID == -1) {
                throw std::runtime_error("Verification of rebuilt index failed");
            }
        }
        
        // Step 6: Replace the old tree with the new one
        bPlusTree = std::move(newTree);
        
        // Step 7: Reset index health metrics
        indexHealth = {0, 0, 0.0};
        
        return true;
        
    } catch (const std::exception& e) {
        std::cerr << "Index rebuild failed: " << e.what() << std::endl;
        return false;
    }
}

bool IndexQueryProcessor::needsRebuild() {
    // This function evaluates whether the B+ tree index needs rebuilding
    // based on several performance metrics:

    // 1. Check fragmentation level
    double currentFragmentation = 0.0;
    BPlusTreeNode* root = bPlusTree->getRoot();
    if (root) {
        // Calculate average page fill ratio by examining leaf nodes
        size_t totalPages = 0;
        size_t totalFreeSpace = 0;

        // Lambda to process each leaf node
        function<void(BPlusTreeNode*)> calculateFragmentation =
            [&](BPlusTreeNode* node) {
                if (node->isLeaf()) {
                    totalPages++;
                    int IndexEntry;
                    size_t usedSpace = node->getKeyCount() * sizeof(IndexEntry);
                    size_t freeSpace = Page::PAGE_SIZE - usedSpace;
                    totalFreeSpace += freeSpace;
                } else {
                    // Recursively process child nodes
                    for (int i = 0; i < node->getChildCount(); i++) {
                        BPlusTreeNode* child = node->getChild(i);
                        calculateFragmentation(child);
                        delete child;  // Clean up after processing
                    }
                }
            };

        calculateFragmentation(root);

        if (totalPages > 0) {
            currentFragmentation = static_cast<double>(totalFreeSpace) /
                                 (totalPages * Page::PAGE_SIZE);
        }
    }

    // 2. Check delete operation ratio
    double deleteRatio = 0.0;
    if (indexHealth.totalOperations > 0) {
        deleteRatio = static_cast<double>(indexHealth.deleteOperations) /
                     indexHealth.totalOperations;
    }

    // 3. Update fragmentation ratio in index health
    indexHealth.fragmentationRatio = currentFragmentation;

    // Return true if any rebuild condition is met
    return (currentFragmentation > IndexHealth::REBUILD_FRAGMENTATION_THRESHOLD) ||
           (deleteRatio > IndexHealth::REBUILD_DELETE_RATIO_THRESHOLD);
}

void IndexQueryProcessor::updateIndexHealth(bool isDelete) {
    indexHealth.totalOperations++;
    if (isDelete) {
        indexHealth.deleteOperations++;
    }
}
