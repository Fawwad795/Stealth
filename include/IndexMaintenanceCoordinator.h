#ifndef INDEX_MAINTENANCE_COORDINATOR_H
#define INDEX_MAINTENANCE_COORDINATOR_H

#include "BPlusTree.h"
#include "HashIndex.h"
#include "BufferManager.h"
#include "FileManager.h"
#include "PriorityQueue.h"
#include <unordered_map>
#include <chrono>
using namespace std;

class IndexMaintenanceCoordinator {
public:
    // Constructor takes references to all necessary components
    IndexMaintenanceCoordinator(BufferManager& bufMgr, FileManager& fileMgr);

    // Core maintenance operations
    void scheduleIndexMaintenance(const string& indexName);
    void performScheduledMaintenance();
    void cancelMaintenance(const string& indexName);

    // Health monitoring
    struct IndexHealth {
        double fragmentationLevel;
        size_t deleteOperations;
        double averageAccessTime;
        chrono::system_clock::time_point lastMaintenance;
        bool needsMaintenance;
    };

    // Status reporting
    IndexHealth checkIndexHealth(const string& indexName);
    bool isMaintenanceNeeded(const string& indexName);

private:
    // Maintenance scheduling
    struct MaintenanceTask {
        string indexName;
        double priority;
        chrono::system_clock::time_point scheduledTime;
    };

    // Custom comparator for MaintenanceTask
    struct MaintenanceTaskComparator {
        bool operator()(const MaintenanceTask& a, const MaintenanceTask& b) const {
            return a.priority < b.priority;  // For max heap based on priority
        }
    };

    // Member variables
    BufferManager& bufferManager;
    FileManager& fileManager;
    PriorityQueue<MaintenanceTask, MaintenanceTaskComparator> maintenanceQueue;
    unordered_map<string, IndexHealth> healthMetrics;

    // Internal helper methods
    double calculateMaintenancePriority(const IndexHealth& health);
    void performMaintenance(const string& indexName);
    void updateHealthMetrics(const string& indexName);

    // Add these new members
    unordered_map<string, shared_ptr<BPlusTree>> bPlusTrees;
    unordered_map<string, shared_ptr<HashIndex>> hashIndexes;
    unordered_map<string, size_t> deleteOperationCounts;
    unordered_map<string, size_t> accessCounts;

    // Add these helper methods
    size_t getDeleteOperationCount(const string& indexName) const;
    size_t getAccessCount(const string& indexName) const;

    // Add method to get index pages
    vector<int> getIndexPages(const string& indexName) const;
    static void collectIndexPages(BPlusTreeNode* node, vector<int>& pages);
};

#endif // INDEX_MAINTENANCE_COORDINATOR_H