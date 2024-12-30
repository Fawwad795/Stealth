#ifndef INDEX_QUERY_PROCESSOR_H
#define INDEX_QUERY_PROCESSOR_H

#include <string>
#include <vector>
#include "BPlusTree.h"
#include "Record.h"
#include "BufferManager.h"
#include "FileManager.h"

// This class provides a high-level interface for database operations,
// translating query-like commands into B+ tree operations
class IndexQueryProcessor {
public:
    // Constructor takes references to required managers
    IndexQueryProcessor(BufferManager& bufferMgr, FileManager& fileMgr);

    // Core query operations
    bool processInsert(const std::string& key, const Record& record);
    bool processDelete(const std::string& key);
    std::vector<Record> processSelect(const std::string& condition);
    bool processUpdate(const std::string& key, const Record& newRecord);

    // Range query support
    std::vector<Record> processRangeSelect(const std::string& startKey,
                                         const std::string& endKey);

    // Index optimization and maintenance
    void optimizeIndex();
    bool rebuildIndex();

    // Statistics and monitoring
    size_t getRecordCount() const;
    double getAverageAccessTime() const;

private:
    // Internal helper methods for query parsing and execution
    int parseKey(const std::string& key) const;
    bool parseCondition(const std::string& condition,
                       std::string& operation,
                       std::string& value) const;

    // Helper methods for record retrieval and manipulation
    Record fetchRecord(const RecordPtr& ptr) const;
    bool storeRecord(const Record& record, RecordPtr& outPtr);

    // Performance monitoring
    void updateAccessStats(double accessTime);

    // Member variables
    BufferManager& bufferManager;
    FileManager& fileManager;
    std::unique_ptr<BPlusTree> bPlusTree;

    // Statistics tracking
    struct QueryStats {
        size_t totalQueries;
        size_t successfulQueries;
        double totalAccessTime;
        double avgAccessTime;
    } stats;

    // Add these new member variables for tracking index health
    struct IndexHealth {
        size_t totalOperations;
        size_t deleteOperations;
        double fragmentationRatio;
        static constexpr double REBUILD_FRAGMENTATION_THRESHOLD = 0.3;
        static constexpr double REBUILD_DELETE_RATIO_THRESHOLD = 0.4;
    } indexHealth;

    // Add the private method declaration
    bool needsRebuild();

    void updateIndexHealth(bool isDelete);
};

#endif // INDEX_QUERY_PROCESSOR_H
