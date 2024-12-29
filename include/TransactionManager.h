#ifndef TRANSACTION_MANAGER_H
#define TRANSACTION_MANAGER_H

#include <fstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <mutex>
#include <chrono>
using namespace std;

class Page;
class BufferManager;

class TransactionManager {
public:
    //this enum defines all possible operations that can be logged
    enum class LogOperation {
        BEGIN,      //marks the start of a transaction
        COMMIT,     //indicates successful completion of a transaction
        ABORT,      //shows that the transaction rollback
        UPDATE,     //records data update
        INSERT,     //record a new data insertion
        DELETE      //records a data removal
    };

    //structure to represent a single log record
    struct LogRecord {
        uint64_t LSN;              //Log Sequence Number - unique identifier for each log entry
        int transactionID;         //unique identifier for the transaction
        int pageID;                //page ID of page in which operation occurred
        LogOperation operation;    //type of operation that was performed
        string oldValue;           //previous value (for undo operations)
        string newValue;           //new value after the operation
        uint64_t timestamp;        //when the operation occurred
    };

    //constructor takes the log file path and buffer manager reference
    TransactionManager(const string& logFilePath, BufferManager& bufferMgr);
    //destructor ensures all logs are written to disk
    ~TransactionManager();

    //core transaction operations
    int beginTransaction();                      //starting a new transaction
    void commit(int transactionID);              //finalizes a transaction
    void abort(int transactionID);               //cancels/rollbacks a transaction

    //function for logging operations in the log
    uint64_t logOperation(int transactionID,
                         int pageID,
                         LogOperation operation,
                         const string& oldValue = "",
                         const string& newValue = "");
    void recover();                             //function for restoring database after crash
    void flushLog();                            //function to force log to disk
    bool isActive(int transactionID) const;     //checking if transaction is active

private:
    string logPath;                             //path to the log file
    fstream logFile;                            //file stream for the log
    BufferManager& bufferManager;               //reference to buffer manager

    mutex logMutex;                             //mutex for thread-safe logging
    uint64_t currentLSN;                        //current Log Sequence Number

    //hashmap for tracking active transactions and their first LSN
    unordered_map<int, uint64_t> activeTransactions;
    int nextTransactionID;                      //counter for transaction IDs

    //private helper methods
    void writeLogRecord(const LogRecord& record);
    uint64_t generateLSN();
    static uint64_t getCurrentTimestamp();
    void undoTransaction(int transactionID, uint64_t firstLSN);
    void redoTransaction(const LogRecord& record);
};

#endif // TRANSACTION_MANAGER_H