// IndexSelector.cpp
#include "IndexSelector.h"
#include <cmath>
#include <regex>

IndexSelector::IndexSelector(std::shared_ptr<BPlusTree> bTree,
                           std::shared_ptr<HashIndex> hashIdx)
    : bPlusTree(std::move(bTree))
    , hashIndex(std::move(hashIdx)) {
}

IndexSelector::QueryType IndexSelector::analyzeQueryType(
    const std::string& condition) {

    // Look for patterns in the condition to determine query type
    if (condition.find("BETWEEN") != std::string::npos) {
        return QueryType::RANGE_QUERY;
    }

    if (condition.find("LIKE") != std::string::npos) {
        // Check if it's a prefix match (ends with %)
        if (condition.find('%') == condition.length() - 1) {
            return QueryType::PREFIX_MATCH;
        }
        return QueryType::GENERAL_SCAN;
    }

    // Check for equality comparison
    if (condition.find('=') != std::string::npos) {
        return QueryType::EXACT_MATCH;
    }

    // Check for range operators
    if (condition.find('>') != std::string::npos ||
        condition.find('<') != std::string::npos) {
        return QueryType::RANGE_QUERY;
    }

    return QueryType::GENERAL_SCAN;
}

IndexSelector::QueryCharacteristics IndexSelector::analyzeQuery(
    const std::string& condition) {

    QueryCharacteristics characteristics;
    characteristics.type = analyzeQueryType(condition);

    // Analyze the condition to estimate selectivity and result size
    IndexStats btreeStats = getBTreeStats();
    double totalRecords = static_cast<double>(btreeStats.totalRecords);

    switch (characteristics.type) {
        case QueryType::EXACT_MATCH:
            // Assume relatively high selectivity for exact matches
            characteristics.selectivity = 1.0 / std::sqrt(totalRecords);
            characteristics.needsSorting = false;
            break;

        case QueryType::RANGE_QUERY:
            // Estimate range selectivity based on condition
            characteristics.selectivity = 0.2;  // Example assumption
            characteristics.needsSorting = true;
            break;

        case QueryType::PREFIX_MATCH:
            // Prefix matches typically return more records than exact matches
            characteristics.selectivity = 0.1;
            characteristics.needsSorting = true;
            break;

        case QueryType::GENERAL_SCAN:
            characteristics.selectivity = 1.0;
            characteristics.needsSorting = true;
            break;
    }

    characteristics.expectedResults =
        static_cast<size_t>(totalRecords * characteristics.selectivity);

    return characteristics;
}

IndexSelector::IndexType IndexSelector::selectBestIndex(
    const QueryCharacteristics& queryChar) {

    // Get cost estimates for each index type
    double btreeCost = estimateBTreeCost(queryChar);
    double hashCost = estimateHashCost(queryChar);
    double fullScanCost = estimateFullScanCost();

    // Choose the lowest cost option
    double minCost = std::min({btreeCost, hashCost, fullScanCost});

    // Consider additional factors beyond just cost
    if (queryChar.needsSorting &&
        btreeCost <= hashCost * 1.2) {  // Allow 20% overhead for sorted results
        return IndexType::BTREE;
    }

    if (minCost == hashCost && queryChar.type == QueryType::EXACT_MATCH) {
        return IndexType::HASH;
    }

    if (minCost == btreeCost) {
        return IndexType::BTREE;
    }

    return IndexType::NONE;  // Fall back to full scan
}

double IndexSelector::estimateQueryCost(IndexType indexType,
                                      const QueryCharacteristics& queryChar,
                                      const std::string& indexName) {
    SimpleStats stats = collectSimpleStats(indexName);

    switch (queryChar.type) {
        case QueryType::EXACT_MATCH:
            if (indexType == IndexType::HASH) {
                // Hash index: constant time lookup
                return 1.0;
            } else if (indexType == IndexType::BTREE) {
                // B+ Tree: cost is tree height
                return stats.height;
            }
        break;

        case QueryType::RANGE_QUERY:
            if (indexType == IndexType::BTREE) {
                // Cost = tree traversal + estimated number of pages to scan
                double estimatedPages = queryChar.expectedResults / stats.avgRecordsPerPage;
                return stats.height + estimatedPages;
            }
        // Hash index can't handle range queries efficiently
        return std::numeric_limits<double>::max();

        case QueryType::GENERAL_SCAN:
            // Full table scan cost
                return stats.totalRecords / stats.avgRecordsPerPage;
    }

    return std::numeric_limits<double>::max();
}

double IndexSelector::estimateBTreeCost(const QueryCharacteristics& queryChar) {
    IndexStats stats = getBTreeStats();

    // Base cost is tree height (for initial lookup)
    double cost = stats.height;

    switch (queryChar.type) {
        case QueryType::EXACT_MATCH:
            // Just the cost of traversing the tree
            break;

        case QueryType::RANGE_QUERY:
            // Add cost of scanning leaf nodes in range
            cost += queryChar.expectedResults /
                   (Page::PAGE_SIZE * stats.avgFillFactor);
            break;

        case QueryType::PREFIX_MATCH:
            // Similar to range query but potentially more scattered
            cost += 1.2 * queryChar.expectedResults /
                   (Page::PAGE_SIZE * stats.avgFillFactor);
            break;

        case QueryType::GENERAL_SCAN:
            // Full scan cost
            cost = stats.totalRecords /
                   (Page::PAGE_SIZE * stats.avgFillFactor);
            break;
    }

    return cost;
}

double IndexSelector::estimateHashCost(const QueryCharacteristics& queryChar) {
    IndexStats stats = getHashStats();

    switch (queryChar.type) {
        case QueryType::EXACT_MATCH:
            // Cost is 1 (bucket lookup) plus average chain length
            return 1.0 + stats.avgChainLength;

        case QueryType::RANGE_QUERY:
        case QueryType::PREFIX_MATCH:
        case QueryType::GENERAL_SCAN:
            // Hash index is inefficient for non-exact matches
            return std::numeric_limits<double>::max();
    }

    return std::numeric_limits<double>::max();
}

double IndexSelector::estimateFullScanCost() {
    IndexStats stats = getBTreeStats();
    // Simple estimation based on number of pages to scan
    return stats.totalRecords / (Page::PAGE_SIZE * stats.avgFillFactor);
}

IndexSelector::IndexStats IndexSelector::getBTreeStats() const {
    IndexStats stats;
    if (bPlusTree) {
        BPlusTreeNode* root = bPlusTree->getRoot();
        if (root) {
            stats.height = 1;  // Start with root
            stats.avgFillFactor = 0.0;
            stats.totalRecords = 0;

            // Calculate statistics by traversing the tree
            std::function<void(BPlusTreeNode*, int)> traverse =
                [&](BPlusTreeNode* node, int level) {
                    if (node->isLeaf()) {
                        stats.avgFillFactor +=
                            static_cast<double>(node->getKeyCount()) /
                            BPlusTreeNode::MAX_KEYS;
                        stats.totalRecords += node->getKeyCount();
                    } else {
                        stats.height = std::max(stats.height, static_cast<size_t>(level + 1));
                        for (int i = 0; i < node->getChildCount(); i++) {
                            traverse(node->getChild(i), level + 1);
                        }
                    }
                };

            traverse(root, 1);
        }
    }
    return stats;
}

IndexSelector::IndexStats IndexSelector::getHashStats() const {
    IndexStats stats;
    if (hashIndex) {
        stats.avgChainLength =
            static_cast<double>(hashIndex->getEntryCount()) /
            hashIndex->getBucketCount();
        stats.totalRecords = hashIndex->getEntryCount();
        stats.avgFillFactor = hashIndex->getLoadFactor();
    }
    return stats;
}

IndexSelector::SimpleStats IndexSelector::collectSimpleStats(const std::string& indexName) {
        SimpleStats stats;

        // Check if this is a B+ tree index
        auto btreeIt = bPlusTrees.find(indexName);
        if (btreeIt != bPlusTrees.end() && btreeIt->second) {
            BPlusTreeNode* root = btreeIt->second->getRoot();
            if (root) {
                // Calculate tree height
                stats.height = 1;  // Start with root level
                BPlusTreeNode* current = root;
                while (!current->isLeaf()) {
                    stats.height++;
                    // Get first child to measure height
                    current = current->getChild(0);
                }

                // Calculate total records and records per page
                size_t totalRecords = 0;
                size_t totalPages = 0;

                // Lambda to count records in leaf nodes
                std::function<void(BPlusTreeNode*)> countRecords =
                    [&](BPlusTreeNode* node) {
                        if (node->isLeaf()) {
                            totalRecords += node->getKeyCount();
                            totalPages++;
                        } else {
                            for (int i = 0; i < node->getChildCount(); i++) {
                                BPlusTreeNode* child = node->getChild(i);
                                countRecords(child);
                                delete child;
                            }
                        }
                    };

                countRecords(root);

                stats.totalRecords = totalRecords;
                stats.avgRecordsPerPage = totalPages > 0 ?
                    static_cast<double>(totalRecords) / totalPages : 0;
            }
        }

        // Check if this is a hash index
        auto hashIt = hashIndexes.find(indexName);
        if (hashIt != hashIndexes.end() && hashIt->second) {
            stats.totalRecords = hashIt->second->getEntryCount();
            stats.avgRecordsPerPage = Page::MAX_KEYS / 2.0;  // Estimate average
            stats.height = 1;  // Hash indexes are always height 1
        }

        return stats;
    }