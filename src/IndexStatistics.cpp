#include "IndexStatistics.h"
#include <cmath>
#include <algorithm>
#include <iostream>
#include <iomanip>

void IndexStatistics::updateStats(BPlusTree& tree) {
    // Reset all statistics before recalculating
    totalKeys = 0;
    uniqueKeys = 0;
    treeHeight = 0;
    pageCount = 0;
    averageFillFactor = 0.0;
    histogram.clear();
    keyStatsCache.clear();

    // Initialize histogram buckets before collecting statistics
    initializeHistogram(tree);

    // Traverse the tree to collect statistics
    collectTreeStatistics(tree);

    // Calculate derived statistics after collection
    calculateDerivedStats();
}

void IndexStatistics::initializeHistogram(BPlusTree& tree) {
    // Find the minimum and maximum keys in the tree
    int minKey = tree.findMin();
    int maxKey = tree.findMax();

    // Calculate the size of each bucket
    double bucketSize = (maxKey - minKey + 1.0) / HISTOGRAM_BUCKETS;

    // Initialize all histogram buckets
    histogram.resize(HISTOGRAM_BUCKETS);
    for (size_t i = 0; i < HISTOGRAM_BUCKETS; i++) {
        histogram[i].minKey = minKey + static_cast<int>(i * bucketSize);
        histogram[i].maxKey = minKey + static_cast<int>((i + 1) * bucketSize) - 1;
        histogram[i].count = 0;
    }
}

void IndexStatistics::collectTreeStatistics(BPlusTree& tree) {
    BPlusTreeNode* root = tree.getRoot();
    if (root != nullptr) {
        traverseTree(root);
    }
}

void IndexStatistics::traverseTree(BPlusTreeNode* node) {
    if (node == nullptr) return;

    pageCount++;

    if (node->isLeaf()) {
        // Process leaf node statistics
        processLeafStatistics(node);
    } else {
        // Process internal node and recurse to children
        treeHeight++;
        for (int i = 0; i < node->getChildCount(); i++) {
            traverseTree(node->getChild(i));
        }
    }
}

void IndexStatistics::processLeafStatistics(BPlusTreeNode* leaf) {
    // Update counts and histogram for each key in the leaf
    for (int i = 0; i < leaf->getKeyCount(); i++) {
        int key = leaf->getKeyAt(i);
        totalKeys++;
        updateHistogramForKey(key);
    }

    // Update fill factor statistics
    double fillFactor = static_cast<double>(leaf->getKeyCount()) /
                       BPlusTreeNode::MAX_KEYS;
    averageFillFactor = (averageFillFactor * (pageCount - 1) + fillFactor) /
                       pageCount;
}

void IndexStatistics::updateHistogramForKey(int key) {
    size_t bucketIndex = findHistogramBucket(key);
    if (bucketIndex < histogram.size()) {
        histogram[bucketIndex].count++;
    }
}

size_t IndexStatistics::findHistogramBucket(int key) const {
    // Binary search to find the appropriate bucket
    size_t left = 0;
    size_t right = histogram.size() - 1;

    while (left <= right) {
        size_t mid = (left + right) / 2;
        if (key >= histogram[mid].minKey && key <= histogram[mid].maxKey) {
            return mid;
        }
        if (key < histogram[mid].minKey) {
            right = mid - 1;
        } else {
            left = mid + 1;
        }
    }

    return histogram.size(); // Key out of range
}

void IndexStatistics::calculateDerivedStats() {
    // Calculate unique keys from histogram
    uniqueKeys = 0;
    for (const auto& bucket : histogram) {
        if (bucket.count > 0) {
            uniqueKeys++;
        }
    }
}

IndexStatistics::KeyStats IndexStatistics::getKeyStats(int key) {
    // Check if stats are already cached
    auto it = keyStatsCache.find(key);
    if (it != keyStatsCache.end()) {
        return it->second;
    }

    // Calculate statistics for this key
    KeyStats stats;
    size_t bucketIndex = findHistogramBucket(key);

    if (bucketIndex < histogram.size()) {
        stats.frequency = histogram[bucketIndex].count;
        stats.selectivity = static_cast<double>(stats.frequency) / totalKeys;

        // Calculate distinct values in the bucket
        int bucketRange = histogram[bucketIndex].maxKey -
                         histogram[bucketIndex].minKey + 1;
        stats.distinctValues = std::min(static_cast<size_t>(bucketRange),
                                      stats.frequency);

        // Calculate average range between values
        stats.avgRange = bucketRange / static_cast<double>(stats.distinctValues);
    } else {
        // Key is out of range, return zero statistics
        stats = {0, 0.0, 0, 0.0};
    }

    // Cache the calculated statistics
    keyStatsCache[key] = stats;
    return stats;
}

double IndexStatistics::estimateQueryCost(int startKey, int endKey) {
    // Estimate number of pages that need to be accessed
    double numPages = estimatePageAccess(startKey, endKey);

    // Cost model: Tree traversal + leaf page accesses
    return treeHeight + numPages;
}

double IndexStatistics::estimatePageAccess(int startKey, int endKey) const {
    size_t startBucket = findHistogramBucket(startKey);
    size_t endBucket = findHistogramBucket(endKey);

    size_t estimatedKeys = 0;
    for (size_t i = startBucket; i <= endBucket && i < histogram.size(); i++) {
        estimatedKeys += histogram[i].count;
    }

    // Convert estimated keys to estimated pages using average fill factor
    return std::ceil(static_cast<double>(estimatedKeys) /
                    (BPlusTreeNode::MAX_KEYS * averageFillFactor));
}

void IndexStatistics::printHistogram() const {
    std::cout << "\nIndex Histogram:\n";
    std::cout << std::setw(15) << "Range" << std::setw(10) << "Count" << "\n";
    std::cout << std::string(25, '-') << "\n";

    for (const auto& bucket : histogram) {
        if (bucket.count > 0) {
            std::cout << std::setw(7) << bucket.minKey << "-"
                     << std::setw(7) << bucket.maxKey
                     << std::setw(10) << bucket.count << "\n";
        }
    }
}

// Accessor methods
size_t IndexStatistics::getTotalKeys() const { return totalKeys; }
size_t IndexStatistics::getUniqueKeys() const { return uniqueKeys; }
size_t IndexStatistics::getTreeHeight() const { return treeHeight; }
double IndexStatistics::getAverageFillFactor() const { return averageFillFactor; }
size_t IndexStatistics::getHistogramBucketCount() const { return HISTOGRAM_BUCKETS; }