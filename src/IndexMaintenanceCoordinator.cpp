#include "IndexMaintenanceCoordinator.h"
#include <algorithm>

#include "BulkLoader.h"

IndexMaintenanceCoordinator::IndexMaintenanceCoordinator(
    BufferManager& bufMgr, FileManager& fileMgr)
    : bufferManager(bufMgr)
    , fileManager(fileMgr)
    , maintenanceQueue(MaintenanceTaskComparator()) {
}

void IndexMaintenanceCoordinator::scheduleIndexMaintenance(
    const string& indexName) {

    // First update health metrics for this index
    updateHealthMetrics(indexName);

    // Check if maintenance is actually needed
    if (!isMaintenanceNeeded(indexName)) {
        return;
    }

    // Calculate maintenance priority
    double priority = calculateMaintenancePriority(healthMetrics[indexName]);

    // Create and schedule the maintenance task
    MaintenanceTask task{
        indexName,
        priority,
        chrono::system_clock::now()
    };

    maintenanceQueue.push(task);
}

void IndexMaintenanceCoordinator::performScheduledMaintenance() {
    while (!maintenanceQueue.empty()) {
        MaintenanceTask task = maintenanceQueue.top();
        maintenanceQueue.pop();

        // Verify maintenance is still needed
        if (isMaintenanceNeeded(task.indexName)) {
            performMaintenance(task.indexName);

            // Update health metrics after maintenance
            updateHealthMetrics(task.indexName);

            // Update last maintenance timestamp
            healthMetrics[task.indexName].lastMaintenance =
                chrono::system_clock::now();
        }
    }
}

void IndexMaintenanceCoordinator::cancelMaintenance(
    const string& indexName) {

    // Create a new priority queue excluding the cancelled task
    vector<MaintenanceTask> tasks;

    // Get all tasks
    while (!maintenanceQueue.empty()) {
        MaintenanceTask task = maintenanceQueue.top();
        maintenanceQueue.pop();

        if (task.indexName != indexName) {
            tasks.push_back(task);
        }
    }

    // Recreate queue with remaining tasks
    for (const auto& task : tasks) {
        maintenanceQueue.push(task);
    }
}

IndexMaintenanceCoordinator::IndexHealth
IndexMaintenanceCoordinator::checkIndexHealth(const string& indexName) {
    // If we don't have metrics for this index yet, initialize them
    if (healthMetrics.find(indexName) == healthMetrics.end()) {
        updateHealthMetrics(indexName);
    }

    return healthMetrics[indexName];
}

bool IndexMaintenanceCoordinator::isMaintenanceNeeded(
    const string& indexName) {

    const IndexHealth& health = healthMetrics[indexName];

    // Check various health indicators
    bool highFragmentation = health.fragmentationLevel > 0.3;
    bool highDeletes = health.deleteOperations > 1000;  // Example threshold
    bool slowAccess = health.averageAccessTime > 100.0; // Example threshold in ms

    // Check time since last maintenance
    auto now = chrono::system_clock::now();
    auto timeSinceLastMaintenance =
        chrono::duration_cast<chrono::hours>(
            now - health.lastMaintenance).count();
    bool maintenanceDue = timeSinceLastMaintenance > 24; // Daily maintenance check

    return highFragmentation || highDeletes || slowAccess || maintenanceDue;
}

double IndexMaintenanceCoordinator::calculateMaintenancePriority(
    const IndexHealth& health) {

    // Calculate priority based on multiple factors
    double priority = 0.0;

    // Factor in fragmentation level (0-1)
    priority += health.fragmentationLevel * 0.4;  // 40% weight

    // Factor in delete operations (normalized)
    priority += min(1.0, health.deleteOperations / 10000.0) * 0.3;  // 30% weight

    // Factor in access time (normalized)
    priority += min(1.0, health.averageAccessTime / 200.0) * 0.3;  // 30% weight

    return priority;
}

void IndexMaintenanceCoordinator::performMaintenance(const string& indexName) {
    int rootPageID = -1;

    try {
        // Acquire necessary locks or ensure exclusive access
        // Get the root page ID from the B+ tree or hash index
        auto btreeIt = bPlusTrees.find(indexName);
        if (btreeIt != bPlusTrees.end() && btreeIt->second->getRoot() != nullptr) {
            rootPageID = btreeIt->second->getRoot()->getPage()->getID();
        }
        if (rootPageID != -1) {
            bufferManager.pinIndexPage(rootPageID);
        }  // Prevent page eviction during maintenance

        // Get current health metrics
        const IndexHealth& health = healthMetrics[indexName];

        // Determine index type and perform appropriate maintenance
        if (health.fragmentationLevel > 0.3) {
            // For B+ tree indexes
            auto btreeIt = bPlusTrees.find(indexName);
            if (btreeIt != bPlusTrees.end()) {
                BPlusTreeNode* root = btreeIt->second->getRoot();
                if (root) {
                    // Rebalance tree if needed
                    if (root->isUnderflow() || root->isFull()) {
                        // Create a bulk loader and rebuild the tree
                        BulkLoader loader(*btreeIt->second, bufferManager, fileManager);
                        vector<pair<int, RecordPtr>> entries;

                        // Collect all entries from the current tree
                        function<void(BPlusTreeNode*)> collectEntries =
                            [&](BPlusTreeNode* node) {
                                if (node->isLeaf()) {
                                    for (int i = 0; i < node->getKeyCount(); i++) {
                                        entries.push_back({
                                            node->getKeyAt(i),
                                            node->getRecordAt(i)
                                        });
                                    }
                                } else {
                                    for (int i = 0; i < node->getChildCount(); i++) {
                                        BPlusTreeNode* child = node->getChild(i);
                                        collectEntries(child);
                                        delete child;
                                    }
                                }
                            };

                        collectEntries(root);

                        // Sort entries by key
                        sort(entries.begin(), entries.end());

                        // Load entries into new tree
                        for (const auto& entry : entries) {
                            loader.addEntry(entry.first, entry.second);
                        }

                        loader.buildIndex();
                    }

                    // Optimize leaf node fill factors
                    function<void(BPlusTreeNode*)> optimizeNode = [&](BPlusTreeNode* node) {
                        if (node->isLeaf()) {
                            // Instead of directly handling underflow, we'll use remove and reinsert
                            // to trigger the tree's internal balancing mechanisms
                            if (node->getKeyCount() < BPlusTreeNode::MIN_KEYS) {
                                vector<pair<int, RecordPtr>> entries;
                                // Collect entries from underflowing node
                                for (int i = 0; i < node->getKeyCount(); i++) {
                                    entries.push_back({
                                        node->getKeyAt(i),
                                        node->getRecordAt(i)
                                    });
                                }

                                // Remove and reinsert entries to trigger rebalancing
                                for (const auto& entry : entries) {
                                    btreeIt->second->remove(entry.first);
                                    btreeIt->second->insert(entry.first, entry.second);
                                }
                            }
                        } else {
                            for (int i = 0; i < node->getChildCount(); i++) {
                                BPlusTreeNode* child = node->getChild(i);
                                optimizeNode(child);
                                delete child;
                            }
                        }
                    };

                    optimizeNode(root);
                }
            }

            // For hash indexes
            auto hashIt = hashIndexes.find(indexName);
            if (hashIt != hashIndexes.end()) {
                double loadFactor = hashIt->second->getLoadFactor();
                if (loadFactor < 0.25 || loadFactor > 0.75) {
                    // Resize hash table to optimal size
                    size_t optimalSize = hashIt->second->getEntryCount() * 2;
                    hashIt->second->resize(optimalSize);
                }
            }
        }

        // Compact pages to reduce fragmentation
        vector<int> indexPages = getIndexPages(indexName);
        for (int pageId : indexPages) {
            PagePtr page = bufferManager.getPage(pageId);
            if (page->getFragmentationRatio() > 0.2) {
                page->compactify();
            }
        }

        // Log maintenance completion and update stats
        cout << "Maintenance completed for index: " << indexName << endl;

    } catch (const exception& e) {
        cerr << "Maintenance failed for index " << indexName
             << ": " << e.what() << endl;
        if (rootPageID != -1) {
            bufferManager.unpinIndexPage(rootPageID);
        }  // Make sure to unpin in case of error
        throw;  // Re-throw the exception after cleanup
    }

    // Always unpin pages after maintenance
    if (rootPageID != -1) {
        bufferManager.unpinIndexPage(rootPageID);
    }
}

void IndexMaintenanceCoordinator::updateHealthMetrics(const string& indexName) {
    IndexHealth& health = healthMetrics[indexName];

    try {
        // Calculate fragmentation level
        double totalFragmentation = 0.0;
        size_t pageCount = 0;

        // Use our own getIndexPages method instead of BufferManager's
        vector<int> indexPages = getIndexPages(indexName);

        // Calculate average fragmentation across all pages
        for (int pageId : indexPages) {
            PagePtr page = bufferManager.getPage(pageId);
            totalFragmentation += page->getFragmentationRatio();
            pageCount++;
        }

        // Calculate average fragmentation level
        health.fragmentationLevel = pageCount > 0 ?
            totalFragmentation / pageCount : 0.0;

        // Update operation statistics
        health.deleteOperations = getDeleteOperationCount(indexName);

        // Calculate average access time
        chrono::nanoseconds totalAccessTime(0);
        size_t accessCount = getAccessCount(indexName);

        if (accessCount > 0) {
            health.averageAccessTime =
                static_cast<double>(totalAccessTime.count()) / accessCount;
        }

        // Check if maintenance is needed
        health.needsMaintenance =
            health.fragmentationLevel > 0.3 ||
            health.deleteOperations > 1000 ||
            health.averageAccessTime > 100.0;

        // Update last check timestamp if it hasn't been set
        if (!health.lastMaintenance.time_since_epoch().count()) {
            health.lastMaintenance = chrono::system_clock::now();
        }

    } catch (const exception& e) {
        cerr << "Failed to update health metrics for index " << indexName
             << ": " << e.what() << endl;

        // Set conservative values if update fails
        health.fragmentationLevel = 1.0;  // Assume worst case
        health.needsMaintenance = true;   // Schedule maintenance to be safe
    }
}
