#ifndef INDEX_STATISTICS_H
#define INDEX_STATISTICS_H

#include "BPlusTree.h"
#include <unordered_map>
#include <vector>

class IndexStatistics {
public:
    struct KeyStats {
        size_t frequency;      // Number of occurrences
        double selectivity;    // Frequency / total records
        size_t distinctValues; // Number of unique values
        double avgRange;       // Average range between values
    };

    // Core operations
    void updateStats(BPlusTree& tree);
    KeyStats getKeyStats(int key);
    double estimateQueryCost(int startKey, int endKey);

    // Accessors
    size_t getTotalKeys() const;
    size_t getUniqueKeys() const;
    size_t getTreeHeight() const;
    double getAverageFillFactor() const;

    // Histogram operations
    void printHistogram() const;
    size_t getHistogramBucketCount() const;

private:
    struct HistogramBucket {
        int minKey;
        int maxKey;
        size_t count;
    };

    // Internal helper functions
    void initializeHistogram(BPlusTree& tree);
    void collectTreeStatistics(BPlusTree& tree);
    void traverseTree(BPlusTreeNode* node);
    void processLeafStatistics(BPlusTreeNode* leaf);
    void updateHistogramForKey(int key);
    size_t findHistogramBucket(int key) const;
    double estimatePageAccess(int startKey, int endKey) const;
    void calculateDerivedStats();

    // Member variables
    size_t totalKeys;
    size_t uniqueKeys;
    size_t treeHeight;
    size_t pageCount;
    double averageFillFactor;
    std::vector<HistogramBucket> histogram;
    std::unordered_map<int, KeyStats> keyStatsCache;

    static const size_t HISTOGRAM_BUCKETS = 100;
};

#endif // INDEX_STATISTICS_H