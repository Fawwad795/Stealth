#include <iostream>
#include "Page.h"
#include "Record.h"
#include "FileManager.h"
#include "BufferManager.h"
#include "TransactionManager.h"
using namespace std;

int main() {
    try {
        // Initialize file system first
        cout << "Initializing file system..." << endl;
        FileManager fileMgr("database.dat");
        fileMgr.createFile();

        // Create initial page
        Page initialPage(0);
        fileMgr.writePage(initialPage);
        cout << "Initial page created and written to disk" << endl;

        cout << "Initializing Buffer Manager..." << endl;
        BufferManager bufferMgr;

        cout << "Initializing Transaction Manager..." << endl;
        TransactionManager txnMgr("transaction.log", bufferMgr);

        cout << "Beginning transaction..." << endl;
        int txnID = txnMgr.beginTransaction();
        cout << "Transaction begun with ID: " << txnID << endl;

        cout << "Creating test records..." << endl;
        Record record1(1, {"John", "Doe", "30"});
        Record record2(2, {"Jane", "Smith", "25"});
        cout << "Test records created" << endl;

        cout << "Getting page from buffer..." << endl;
        PagePtr pagePtr = bufferMgr.getPage(0);
        cout << "Page retrieved" << endl;

        cout << "Adding first record..." << endl;
        pagePtr->addRecordWithLogging(record1, txnMgr, txnID);
        cout << "First record added" << endl;

        cout << "Adding second record..." << endl;
        pagePtr->addRecordWithLogging(record2, txnMgr, txnID);
        cout << "Done2" << endl;

        // Test record update
        Record updatedRecord(1, {"John", "Doe", "31"});
        if (pagePtr->updateRecordWithLogging(record1.getID() - 1, updatedRecord, txnMgr, txnID)) {
            cout << "Updated record 1 successfully" << endl;
        }

        // Write page to disk
        fileMgr.writePage(*pagePtr);
        // Commit the transaction
        txnMgr.commit(txnID);
        cout << "Transaction committed successfully" << endl;

        // Test recovery by simulating a crash and recovery
        txnMgr.recover();
        cout << "Recovery test completed" << endl;

    } catch (const exception& e) {
        cerr << "Fatal error: " << e.what() << endl;
        return 1;
    }

    return 0;
}

/*int main() {
    FileManager fileManager("database.dat");

    try {
        // Step 1: Create the file
        fileManager.createFile();

        // Step 2: Write metadata
        fileManager.loadMetadata();

        // Step 3: Write a page
        Page page(0);
        fileManager.writePage(page);

        // Step 4: Read the page back
        Page readPage = fileManager.readPage(0);
        cout << "Read page ID: " << readPage.getID() << endl;

        // Step 5: Update metadata
        fileManager.metadata.numTables = 2;
        fileManager.updateMetaData();

        // Step 6: Delete the file
        fileManager.deleteFile();

    } catch (const exception& e) {
        cerr << e.what() << endl;
    }

    return 0;
}*/


//main testing for adding records to a page, and defragmentation works
/*int main() {
    // Create a new page with ID 1
    Page page(1);

    // Add some records
    Record r1(1, {"Alice", "25", "Engineer"});
    Record r2(2, {"Bob", "30", "Doctor"});
    Record r3(3, {"Charlie", "28", "Teacher"});

    page.addRecord(r1);
    page.addRecord(r2);
    page.addRecord(r3);

    cout << "Records added to the page:" << endl;
    for (int i = 0; i < page.getRecordCount(); ++i) {
        Record record = page.getRecord(i);
        cout << "Record " << record.getID() << ": " << record.serialize() << endl;
    }

    // Remove the second record to create fragmentation
    cout << "\nRemoving record 2..." << endl;
    page.removeRecord(1);

    // Show free space
    cout << "Free space after removing record 2: " << page.getFreeSpace() << " bytes" << endl;

    // Add a new record that fits in total free space but not in the fragmented free block
    Record r4(4, {"Eve", "40", "Scientist"});
    bool added = page.addRecord(r4);

    if (added) {
        cout << "Record 4 added successfully!" << endl;
    } else {
        cout << "Record 4 could not be added without defragmentation!" << endl;
    }

    // Show records after adding record 4
    cout << "\nRecords in the page after adding record 4:" << endl;
    for (int i = 0; i < page.getRecordCount(); ++i) {
        Record record = page.getRecord(i);
        cout << "Record " << record.getID() << ": " << record.serialize() << endl;
    }

    // Demonstrate defragmentation
    cout << "\nManually calling defragmentation..." << endl;
    page.defragment();

    // Add record 4 again (should now succeed after defragmentation)
    added = page.addRecord(r4);
    if (added) {
        cout << "Record 4 added successfully after defragmentation!" << endl;
    }
    else {
        cout << "Not enough space";
    }

    // Show final records
    cout << "\nFinal records in the page:" << endl;
    for (int i = 0; i < page.getRecordCount(); ++i) {
        Record record = page.getRecord(i);
        cout << "Record " << record.getID() << ": " << record.serialize() << endl;
    }

    cout << "Free space: " << page.getFreeSpace() << " bytes" << endl;

    return 0;
}*/
