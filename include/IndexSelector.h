// IndexSelector.h
#ifndef INDEX_SELECTOR_H
#define INDEX_SELECTOR_H

#include "BPlusTree.h"
#include "HashIndex.h"
#include <memory>
#include <string>

class IndexSelector {
public:
    // Represents different types of queries we can process
    enum class QueryType {
        EXACT_MATCH,     // WHERE column = value
        RANGE_QUERY,     // WHERE column BETWEEN x AND y
        PREFIX_MATCH,    // WHERE column LIKE 'prefix%'
        GENERAL_SCAN     // Full table scan or complex conditions
    };

    // Represents available index types
    enum class IndexType {
        BTREE,
        HASH,
        NONE  // When no index is suitable
    };

    // Represents the characteristics of a query
    struct QueryCharacteristics {
        QueryType type;
        double selectivity;      // Estimated fraction of records to be retrieved
        size_t expectedResults;  // Estimated number of matching records
        bool needsSorting;       // Whether results must be ordered
    };

    struct SimpleStats {
        size_t height;           // Tree height for B+ tree
        size_t totalRecords;     // Total records in index
        double avgRecordsPerPage;// Average records per page

        SimpleStats() : height(0), totalRecords(0), avgRecordsPerPage(0) {}
    };

    // Constructor takes references to our indexes
    IndexSelector(std::shared_ptr<BPlusTree> bTree,
                 std::shared_ptr<HashIndex> hashIdx);

    // Core selection methods
    IndexType selectBestIndex(const QueryCharacteristics& queryChar);
    double estimateQueryCost(IndexType indexType,
                           const QueryCharacteristics& queryChar,
                           const std::string& indexName);

    // Query analysis methods
    QueryType analyzeQueryType(const std::string& condition);
    QueryCharacteristics analyzeQuery(const std::string& condition);

    // Method to register new indexes
    void registerIndex(const std::string& name,
                      std::shared_ptr<BPlusTree> bTree) {
        bPlusTrees[name] = bTree;
    }

    void registerIndex(const std::string& name,
                      std::shared_ptr<HashIndex> hashIdx) {
        hashIndexes[name] = hashIdx;
    }

private:
    std::shared_ptr<BPlusTree> bPlusTree;
    std::shared_ptr<HashIndex> hashIndex;

    std::unordered_map<std::string, std::shared_ptr<BPlusTree>> bPlusTrees;
    std::unordered_map<std::string, std::shared_ptr<HashIndex>> hashIndexes;

    // Cost estimation helpers
    double estimateBTreeCost(const QueryCharacteristics& queryChar);
    double estimateHashCost(const QueryCharacteristics& queryChar);
    double estimateFullScanCost();

    // Index statistics
    struct IndexStats {
        size_t height;           // Explicitly using size_t for height
        double avgFillFactor;
        double avgChainLength;   // For hash index
        size_t totalRecords;
    };

    IndexStats getBTreeStats() const;
    IndexStats getHashStats() const;

    SimpleStats collectSimpleStats(const std::string& indexName);
};

#endif // INDEX_SELECTOR_H