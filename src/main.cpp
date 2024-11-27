#include <iostream>
#include "Page.h"
#include "Record.h"

using namespace std;

int main() {
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
}
