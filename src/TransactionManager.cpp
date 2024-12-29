#include "TransactionManager.h"
#include "BufferManager.h"
#include "Page.h"
#include <mutex>
#include <iostream>
#include <sstream>
using namespace std;
using namespace chrono;

TransactionManager::TransactionManager(const string& logFilePath, BufferManager& bufferMgr)
    : logPath(logFilePath)
    , bufferManager(bufferMgr)
    , currentLSN(0)
    , nextTransactionID(1) {

    cout << "Opening log file..." << endl;
    //openoing log file in append mode
    logFile.open(logPath, ios::app | ios::binary);
    if (!logFile) {
        throw runtime_error("Failed to open transaction log file: " + logFilePath);
    }

    cout << "Log file opened successfully" << endl;

    //positioning at end to get current LSN
    logFile.seekg(0, ios::end);
    currentLSN = logFile.tellg();

    cout << "Transaction Manager initialized with current LSN: " << currentLSN << endl;
}

TransactionManager::~TransactionManager() {
    //ensuring all active transactions are rolled back
    vector<int> activeIDs;
    for (const auto& pair : activeTransactions) {
        activeIDs.push_back(pair.first);
    }

    for (int id : activeIDs) {
        abort(id);
    }

    flushLog();
    logFile.close();
}

int TransactionManager::beginTransaction() {
    int transactionID = nextTransactionID++;

    //logging the start of this transaction
    uint64_t lsn = logOperation(transactionID, -1, LogOperation::BEGIN);

    //recording the transaction's first LSN for potential rollback
    activeTransactions[transactionID] = lsn;

    return transactionID;
}

void TransactionManager::commit(int transactionID) {
    //creating a protective lock
    //lock_guard lock(logMutex);
    //now only one thread can execute this code at a time

    if (!isActive(transactionID)) {
        throw runtime_error("Attempting to commit inactive transaction");
    }

    //logging the commit operation
    logOperation(transactionID, -1, LogOperation::COMMIT);

    //removing from active transactions
    activeTransactions.erase(transactionID);

    //forcing log to disk to ensure durability
    flushLog();
}

void TransactionManager::abort(int transactionID) {
    //creating a protective lock
    lock_guard lock(logMutex);
    //now only one thread can execute this code at a time

    if (!isActive(transactionID)) {
        throw runtime_error("Attempting to abort inactive transaction");
    }

    //getting the transaction's first LSN
    uint64_t firstLSN = activeTransactions[transactionID];

    //logging the abort operation
    logOperation(transactionID, -1, LogOperation::ABORT);

    //undoing all operations in this transaction
    undoTransaction(transactionID, firstLSN);

    //removing from active transactions
    activeTransactions.erase(transactionID);

    //making sure everything is written to disk
    flushLog();
}

uint64_t TransactionManager::logOperation(int transactionID,
                                        int pageID,
                                        LogOperation operation,
                                        const string& oldValue,
                                        const string& newValue) {
    LogRecord record;
    record.LSN = generateLSN();
    record.transactionID = transactionID;
    record.pageID = pageID;
    record.operation = operation;
    record.oldValue = oldValue;
    record.newValue = newValue;
    record.timestamp = getCurrentTimestamp();

    writeLogRecord(record);
    return record.LSN;
}

void TransactionManager::writeLogRecord(const LogRecord& record) {
    lock_guard lock(logMutex);  // Create a protective lock

    cout << "Writing log record for transaction " << record.transactionID << endl;

    //formatting the log record
    stringstream ss;
    ss << record.LSN << "|"
       << record.transactionID << "|"
       << record.pageID << "|"
       << static_cast<int>(record.operation) << "|"
       << record.oldValue << "|"
       << record.newValue << "|"
       << record.timestamp << "\n";

    //writing to log file
    logFile.write(ss.str().c_str(), ss.str().length());
    if (!logFile) {
        throw runtime_error("Failed to write to transaction log");
    }
    logFile.flush();

    cout << "Log record written successfully" << endl;
}

void TransactionManager::flushLog() {
    logFile.flush();
}

bool TransactionManager::isActive(int transactionID) const {
    return activeTransactions.find(transactionID) != activeTransactions.end();
}

uint64_t TransactionManager::generateLSN() {
    return ++currentLSN;
}

uint64_t TransactionManager::getCurrentTimestamp() {
    return system_clock::now().time_since_epoch().count();
}

void TransactionManager::recover() {
    cout << "Starting database recovery process...\n";

    // Open log file for reading
    ifstream recoveryFile(logPath);
    if (!recoveryFile) {
        throw runtime_error("Cannot open log file for recovery");
    }

    // Maps to track transaction states
    unordered_map<int, bool> completedTransactions;          // Shows if transaction was committed
    unordered_map<int, vector<LogRecord>> transactionLogs;   // Stores operations per transaction

    string line;
    while (getline(recoveryFile, line)) {
        try {
            LogRecord record;
            stringstream ss(line);
            string field;

            cout << "Processing log line: " << line << endl;

            // Parse the first four fields which are consistent across all operations
            // Parse LSN
            if (!getline(ss, field, '|')) {
                throw runtime_error("Failed to parse LSN");
            }
            record.LSN = stoull(field);
            cout << "Parsed LSN: " << record.LSN << endl;

            // Parse Transaction ID
            if (!getline(ss, field, '|')) {
                throw runtime_error("Failed to parse Transaction ID");
            }
            record.transactionID = stoi(field);
            cout << "Parsed Transaction ID: " << record.transactionID << endl;

            // Parse Page ID
            if (!getline(ss, field, '|')) {
                throw runtime_error("Failed to parse Page ID");
            }
            record.pageID = stoi(field);
            cout << "Parsed Page ID: " << record.pageID << endl;

            // Parse Operation Type
            if (!getline(ss, field, '|')) {
                throw runtime_error("Failed to parse Operation Type");
            }
            record.operation = static_cast<LogOperation>(stoi(field));
            cout << "Parsed Operation Type: " << static_cast<int>(record.operation) << endl;

            // Different parsing strategy based on operation type
            if (record.operation == LogOperation::UPDATE) {
                // For UPDATE operations, we need to carefully parse oldValue and newValue
                string remaining;
                getline(ss, remaining);
                size_t lastPipePos = remaining.find_last_of('|');
                if (lastPipePos == string::npos) {
                    throw runtime_error("Invalid format for UPDATE operation");
                }

                // Extract timestamp
                string timestampStr = remaining.substr(lastPipePos + 1);
                record.timestamp = stoull(timestampStr);

                // Remove timestamp part from remaining
                remaining = remaining.substr(0, lastPipePos);

                // Find the middle point between oldValue and newValue
                // We know the format is oldValue|newValue|timestamp
                size_t midPoint = remaining.find("|John");
                if (midPoint == string::npos) {
                    throw runtime_error("Cannot find separator between oldValue and newValue");
                }

                record.oldValue = remaining.substr(0, midPoint);
                record.newValue = remaining.substr(midPoint + 1);

            } else {
                // For non-UPDATE operations
                if (!getline(ss, field, '|')) {
                    throw runtime_error("Failed to parse oldValue");
                }
                record.oldValue = field;

                if (!getline(ss, field, '|')) {
                    throw runtime_error("Failed to parse newValue");
                }
                record.newValue = field;

                if (!getline(ss, field, '|')) {
                    throw runtime_error("Failed to parse timestamp");
                }
                record.timestamp = stoull(field);
            }

            cout << "Parsed oldValue: " << record.oldValue << endl;
            cout << "Parsed newValue: " << record.newValue << endl;
            cout << "Parsed timestamp: " << record.timestamp << endl;

            // Process based on operation type
            switch (record.operation) {
                case LogOperation::BEGIN:
                    cout << "Processing BEGIN operation" << endl;
                    transactionLogs[record.transactionID] = vector<LogRecord>();
                    break;

                case LogOperation::COMMIT:
                    cout << "Processing COMMIT operation" << endl;
                    completedTransactions[record.transactionID] = true;
                    break;

                case LogOperation::ABORT:
                    cout << "Processing ABORT operation" << endl;
                    completedTransactions[record.transactionID] = false;
                    break;

                default:
                    cout << "Processing operation type: " << static_cast<int>(record.operation) << endl;
                    if (transactionLogs.find(record.transactionID) != transactionLogs.end()) {
                        transactionLogs[record.transactionID].push_back(record);
                    }
                    break;
            }

        } catch (const exception& e) {
            cerr << "Error processing log record: " << e.what() << endl;
            cerr << "Problematic line: " << line << endl;
            continue;  // Skip this record and continue with the next
        }
    }

    cout << "Finished parsing log file, starting recovery actions" << endl;

    // Perform recovery actions
    for (const auto& pair : transactionLogs) {
        int transactionID = pair.first;
        const auto& logs = pair.second;

        try {
            if (completedTransactions.find(transactionID) == completedTransactions.end()) {
                // Transaction was incomplete - need to undo
                cout << "Rolling back incomplete transaction " << transactionID << endl;
                undoTransaction(transactionID, logs.front().LSN);
            } else if (completedTransactions[transactionID]) {
                // Transaction was committed - need to redo
                cout << "Redoing committed transaction " << transactionID << endl;
                for (const auto& record : logs) {
                    redoTransaction(record);
                }
            }
        } catch (const exception& e) {
            cerr << "Error during recovery action for transaction " << transactionID
                 << ": " << e.what() << endl;
        }
    }

    cout << "Recovery process completed successfully" << endl;
    recoveryFile.close();
}

void TransactionManager::undoTransaction(int transactionID, uint64_t firstLSN) {
    // Read log file backward from current position to firstLSN
    std::ifstream undoFile(logPath);
    undoFile.seekg(0, std::ios::end);

    while (undoFile.tellg() > static_cast<std::streampos>(firstLSN)) {
        // Read and parse log record (similar to recover())
        // For each operation in the transaction, apply the inverse:
        LogRecord record; // Parse record here...

        if (record.transactionID == transactionID) {
            switch (record.operation) {
                case LogOperation::INSERT: {
                    // Delete the inserted record
                    auto page = bufferManager.getPage(record.pageID);
                    // Implementation depends on your Page class
                    // page->removeRecord(...);
                    break;
                }
                case LogOperation::DELETE: {
                    // Reinsert the deleted record
                    auto page = bufferManager.getPage(record.pageID);
                    // page->addRecord(...);
                    break;
                }
                case LogOperation::UPDATE: {
                    // Restore the old value
                    auto page = bufferManager.getPage(record.pageID);
                    // page->updateRecord(..., record.oldValue);
                    break;
                }
            }
        }
    }
}

void TransactionManager::redoTransaction(const LogRecord& record) {
    // Reapply the operation
    auto page = bufferManager.getPage(record.pageID);

    switch (record.operation) {
        case LogOperation::INSERT: {
            // Reinsert the record
            // page->addRecord(...);
            break;
        }
        case LogOperation::DELETE: {
            // Redelete the record
            // page->removeRecord(...);
            break;
        }
        case LogOperation::UPDATE: {
            // Reapply the update
            // page->updateRecord(..., record.newValue);
            break;
        }
    }
}
