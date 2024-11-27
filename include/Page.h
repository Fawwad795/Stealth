//declaration file of the Record class
#ifndef PAGE_H
#define PAGE_H

#include <vector>
#include <cstring> //for memcpy function
#include <stdexcept>
#include <string>
#include "Record.h" //to include record class

//Class Page that will hold records
class Page {
public:
    static const int PAGE_SIZE = 4096; //fixed page size: 4 kilobytes
    //static const int PAGE_SIZE = 90;

    //constructor to initialize a page object
    Page(int id);

    //function to add a record to the page
    bool addRecord(const Record& record);

    //function to retrieve a record from the page by its index
    Record getRecord(int index) const;

    //function to remove a record from the page by its index
    bool removeRecord(int index);

    //getter function for bytes of free space available in the page
    int getFreeSpace() const;

    //getter function for the number of records in the page
    int getRecordCount() const;

    //function to defragment the page time to time
    void defragment();

private:
    char data[PAGE_SIZE];       //raw data storage for records
    int pageID;                 //unique ID for the page
    int freeSpace;              //track available free space
    int recordCount;            //keep track of the number of records present in this page

    //struct for storing the metadata for each record
    struct RecordMetadata {
        int id;
        int offset;             //offset of a certain record in the 'data' array
        int length;             //length of the record in bytes
    };

    vector<RecordMetadata> metadata;  //vector for storing metadata for each record
    vector<pair<int, int>> freeList;  //tracks free space in the data array
};

#endif //PAGE_H