//declaration file of the Record class
#ifndef PAGE_H
#define PAGE_H

#include <cstdint>
#include <vector>
#include <cstring>
#include <stdexcept>
#include <string>
#include "Record.h" //to include record class

class TransactionManager;

//Class Page that will hold records
class Page {
public:
    //static const int PAGE_SIZE = 4096; //fixed page size: 4 kilobytes
    static const int PAGE_SIZE = 4096;

    //new page type enumeration for different page purposes
    enum class PageType {
        DATA_PAGE,
        INDEX_PAGE,
        OVERFLOW_PAGE,
        FREE_PAGE
    };

    //page header structure - the metadata
    struct PageHeader {
        int pageID;         //existing page ID
        PageType type;      //type of page
        int freeSpace;      //existing free space in the page
        int recordCount;    //existing number of records in in the page
        int nextPage;       //for linked list of pages
        int prevPage;       //for doubly linked list
        uint32_t checksum;  //for integrity verification
        uint64_t LSN;       //log sequence number
        bool isLeaf;        //field for index pages
        int nextLeafPage;   //for leaf node linking
        int prevLeafPage;    //for bidirectional traversal
    };

    //struct for storing the metadata for each record
    struct RecordMetadata {
        int id;
        mutable int offset;             //offset of a certain record in the 'data' array
        int length;             //length of the record in bytes
    };

    //constructor to initialize a page object, default type is data page
    Page(int id, PageType type = PageType::DATA_PAGE);

    //core methods
    bool addRecord(const Record& record);           //function to add a record to the page
    Record getRecord(int index) const;              //function to retrieve a record from the page by its index
    bool removeRecord(int index);                   //function to remove a record from the page by its index
    int getFreeSpace() const;                       //getter function for bytes of free space available in the page
    int getRecordCount() const;                     //getter function for the number of records in the page
    void defragment();                              //function to defragment the page time to time
    int getID() const;                              //method for getting the page ID

    //methods for integrity and maintenance
    bool isCorrupted() const;                       //method to determine whether the page is corrupted or not
    void calculateChecksum();                       //method to calculate checksum for the page
    float getFragmentationRatio() const;            //method for getting fragmentation ratio
    void compactify();                              //enhanced version of defragmentation

    // Add transaction-related methods
    bool addRecordWithLogging(const Record& record, TransactionManager& txnManager, int txnID);
    bool removeRecordWithLogging(int index, TransactionManager& txnManager, int txnID);
    bool updateRecordWithLogging(int index, const Record& newRecord, TransactionManager& txnManager, int txnID);

    //methods for index support
    bool isLeafPage() const;                        //to determine whether a page is an index page
    bool isIndexPage() const;                       //to determine whether a page is an index page
    void convertToIndexPage(bool isLeaf = false);   //method for converting a page into an index page
    int getNextLeafPage() const;                    //for getting the next leaf page
    void setNextLeafPage(int pageID);               //for setting the next page page into a leaf

private:
    PageHeader header;                              //metadata structure for the page
    char data[PAGE_SIZE - sizeof(PageHeader)];      //raw data storage for records, remaining data for actual data
    vector<RecordMetadata> metadata;                //vector for storing metadata for each record
    vector<pair<int, int>> freeList;  //tracks free space in the data array

    //private helper methods
    void updateChecksum();
    bool validateChecksum() const;

    //new helper methods for index management
    void initializeIndexStructures();
    void validateIndexOperation() const;
};

#endif //PAGE_H