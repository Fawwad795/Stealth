#include "Page.h"
#include <algorithm>
#include <cstring>
#include <numeric>
#include "TransactionManager.h"
using namespace std;

//constructor to initialize a page object
Page::Page(int id, PageType type) {
    //initializing the page header
    header.pageID = id;
    header.type = type;
    header.freeSpace = PAGE_SIZE - sizeof(PageHeader);
    header.recordCount = 0;
    header.nextPage = -1;                                   //-1 indicates no next page
    header.prevPage = -1;                                   //-1 indicates no previous page
    header.LSN = 0;                                         //initializing Log Sequence Number

    //clearing the data area
    memset(data, 0, PAGE_SIZE - sizeof(PageHeader));

    //calculating initial checksum
    calculateChecksum();
}

// In Page.cpp - Enhanced addRecord method
bool Page::addRecord(const Record& record) {
    string serializedRecord = record.serialize();
    int recordSize = serializedRecord.size();

    // First, let's check if we need to defragment
    if (header.freeSpace >= recordSize + sizeof(RecordMetadata)) {
        float fragRatio = getFragmentationRatio();
        if (fragRatio > 0.3) { // If more than 30% fragmented
            compactify();      // Try to defragment first
        }
    }

    // Now try to find a suitable free block as before
    for (auto iter = freeList.begin(); iter != freeList.end(); ++iter) {
        if (iter->second >= recordSize + sizeof(RecordMetadata)) {
            int offset = iter->first;

            // Before writing, verify page integrity
            if (isCorrupted()) {
                throw runtime_error("Page corruption detected before write!");
            }

            // Copy record into the free block
            memcpy(data + offset, serializedRecord.c_str(), recordSize);

            // Update metadata and header
            metadata.push_back({record.getID(), offset, recordSize});
            header.recordCount++;

            // Update free block list
            if (iter->second == recordSize) {
                freeList.erase(iter);
            } else {
                iter->first += recordSize;
                iter->second -= recordSize;
            }

            // Update header free space and checksum
            header.freeSpace -= (recordSize + sizeof(RecordMetadata));
            calculateChecksum();

            return true;
        }
    }

    // If no suitable block found, try at the end of used space
    int currentEndOffset = PAGE_SIZE - sizeof(PageHeader) - header.freeSpace;
    if (header.freeSpace >= recordSize + sizeof(RecordMetadata)) {
        memcpy(data + currentEndOffset, serializedRecord.c_str(), recordSize);
        metadata.push_back({record.getID(), currentEndOffset, recordSize});
        header.recordCount++;
        header.freeSpace -= (recordSize + sizeof(RecordMetadata));
        calculateChecksum();
        return true;
    }

    return false;
}

Record Page::getRecord(int index) const {
    if(index < 0 || index >= header.recordCount) {
        throw out_of_range("Invalid record Index");
    }

    //retrieve metadata for the record using the index
    const RecordMetadata& meta = metadata[index];
    string serializedRecord(data + meta.offset, meta.length);

    //return the deserialized record
    return Record::deserialize(meta.id, serializedRecord);
}

bool Page::removeRecord(int index) {
    if(index < 0 || index >= header.recordCount) {
        return false; //invalid index
    }

    //retrieve metadata for the record to be removed
    const RecordMetadata& meta = metadata[index];

    //add the freed space to the free list
    freeList.push_back({meta.offset, meta.length});

    //remove the metadata for this record, decrement the record count for this page
    metadata.erase(metadata.begin() + index);
    header.recordCount--;

    return true;    //successfully removed the record
}

//getter function for bytes of free space available in the page
int Page::getFreeSpace() const {
    return header.freeSpace;
}

//getter function for the number of records in the page
int Page::getRecordCount() const {
    return header.recordCount;
}

//function to defragment the page time to time - to compact the free space at the end of the data array
void Page::defragment() {
    //first sort the metadata by the offset
    sort(metadata.begin(), metadata.end(), [](const RecordMetadata& a, const RecordMetadata& b) {
        return a.offset < b.offset;
    });

    //compact the records to remove gaps in the data array
    int currentOffset = 0;
    for(auto& meta: metadata) {
        //move record to the current offset
        if(meta.offset != currentOffset) {
            memmove(data+currentOffset, data + meta.offset, meta.length);
            meta.offset = currentOffset;
        }
        currentOffset += meta.length + sizeof(RecordMetadata);
    }

    //clear the free list since all free space now at the end of the data array
    freeList.clear();

    //computing the amount of free space left now
    header.freeSpace = PAGE_SIZE - currentOffset;
}

//function for getting the Page ID
int Page::getID() const {
    return header.pageID;
}

//checksum calculation - uses simple additive checksum
void Page::calculateChecksum() {
    //setting checksum to 0 temporarily
    header.checksum = 0;

    //calculating checksum over the header (excluding the checksum field) and data
    uint32_t sum = 0;
    const uint8_t* ptr = reinterpret_cast<const uint8_t*>(&header);

    //summing the header bytes (excluding the checksum field)
    for (size_t i = 0; i < offsetof(PageHeader, checksum); i++) {
        sum += ptr[i];
    }

    //suming the data bytes
    for(size_t i = 0; i < PAGE_SIZE - sizeof(PageHeader); i++) {
        sum += static_cast<uint8_t>(data[i]);
    }

    header.checksum = sum;
}

//function for integrity verification
bool Page::isCorrupted() const {
    //creating a copy of the page to check checksum
    Page tempPage = *this;
    uint32_t storedChecksum = header.checksum;

    //calculating the checksum on the copy
    tempPage.calculateChecksum();

    return storedChecksum != tempPage.header.checksum;
}

//calculating the fragmentation ratio (0.0 to 1.0)
float Page::getFragmentationRatio() const {
    if(freeList.empty()) {
        return 0.0f;    //no fragmentation
    }

    //calculating the total free space
    int totalFreeSpace = accumulate(freeList.begin(), freeList.end(), 0,
        [](int sum, const auto& block){ return sum + block.second; } );

    //the largest block calculation
    int largestBlock = max_element(freeList.begin(), freeList.end(),
        [](const auto& a, const auto& b) {return a.second < b.second; })->second;

    //retuning the fragm. ratio
    return 1.0f - (static_cast<float>(largestBlock) / totalFreeSpace);
}

//enhanced defragmentation with compaction
void Page::compactify() {
    if (freeList.empty()) {
        return;  //there exists no fragmentation to handle
    }

    //sorting records by offset
    vector<pair<int, RecordMetadata>> sortedRecords;
    for (size_t i = 0; i < metadata.size(); i++) {
        sortedRecords.push_back({metadata[i].offset, metadata[i]});
    }
    sort(sortedRecords.begin(), sortedRecords.end(),
    [](const auto& a, const auto& b) {
        return a.first < b.first;  // Sort based on the int member of the pair
    });

    //creating temporary buffer for compacted data
    char tempBuffer[PAGE_SIZE - sizeof(PageHeader)];
    int currentOffset = 0;

    //moving records to compact positions
    for (const auto& record : sortedRecords) {
        int recordSize = record.second.length;
        //moving record data to new position
        memcpy(tempBuffer + currentOffset,
               data + record.second.offset,
               recordSize);
        //updating metadata
        record.second.offset = currentOffset;
        currentOffset += recordSize;
    }

    //copying compacted data back to page
    memcpy(data, tempBuffer, PAGE_SIZE - sizeof(PageHeader));

    //updating free space tracking
    freeList.clear();
    if (currentOffset < PAGE_SIZE - sizeof(PageHeader)) {
        freeList.push_back({currentOffset,
                           PAGE_SIZE - sizeof(PageHeader) - currentOffset});
    }

    //updating the header
    header.freeSpace = PAGE_SIZE - sizeof(PageHeader) - currentOffset;

    //recalculating the checksum after modification
    calculateChecksum();
}

bool Page::addRecordWithLogging(const Record& record, TransactionManager& txnManager, int transactionID) {
    //logging the operation before performing it
    txnManager.logOperation(transactionID, header.pageID, TransactionManager::LogOperation::INSERT,
                          "", record.serialize());

    //performing the actual operation
    return addRecord(record);
}

bool Page::removeRecordWithLogging(int index, TransactionManager& txnManager, int transactionID) {
    //getting the record's current value for logging
    Record oldRecord = getRecord(index);

    //logging the operation
    txnManager.logOperation(transactionID, header.pageID, TransactionManager::LogOperation::DELETE,
                          oldRecord.serialize(), "");

    //performing the deletion
    return removeRecord(index);
}

bool Page::updateRecordWithLogging(int index, const Record& newRecord, TransactionManager& txnManager, int transactionID) {
    //firstly validating the index
    if (index < 0 || index >= header.recordCount) {
        return false;
    }

    //getting the existing record to store as old value for logging
    Record oldRecord = getRecord(index);
    string oldValue = oldRecord.serialize();
    string newValue = newRecord.serialize();

    //logging the update operation before making changes
    txnManager.logOperation(transactionID, header.pageID, TransactionManager::LogOperation::UPDATE,oldValue, newValue);

    //calculating size difference between old and new records
    const RecordMetadata& oldMeta = metadata[index];
    int newSize = newValue.size();
    int sizeDiff = newSize - oldMeta.length;

    //checking if we have enough space for the new record if it's larger
    if (sizeDiff > header.freeSpace) {
        return false;
    }

    //if new record is larger, we might need to defragment
    if (sizeDiff > 0 && getFragmentationRatio() > 0.3) {
        compactify();
    }

    //updating the record data
    memcpy(data + oldMeta.offset, newValue.c_str(), newSize);

    //updating metadata
    metadata[index].length = newSize;
    header.freeSpace -= sizeDiff;

    //updating checksum since page content has changed
    calculateChecksum();

    return true;
}

bool Page::isLeafPage() const {
    return header.type == PageType::INDEX_PAGE && header.isLeaf;
}

bool Page::isIndexPage() const {
    return header.type == PageType::INDEX_PAGE;
}

void Page::convertToIndexPage(bool isLeaf) {
    //clearing existing data
    memset(data, 0, PAGE_SIZE - sizeof(PageHeader));
    metadata.clear();
    freeList.clear();

    //updating header
    header.type = PageType::INDEX_PAGE;
    header.isLeaf = isLeaf;
    header.recordCount = 0;
    header.freeSpace = PAGE_SIZE - sizeof(PageHeader);

    //initializing index-specific structures
    if (isLeaf) {
        // Reserve space for leaf node metadata (next/prev pointers)
        header.nextLeafPage = -1;
        header.prevLeafPage = -1;
        header.freeSpace -= sizeof(int) * 2;
    }

    calculateChecksum();
}


int Page::getNextLeafPage() const {
    if (!isLeafPage()) {
        throw runtime_error("Not a leaf page");
    }
    return header.nextLeafPage;
}


void Page::setNextLeafPage(int pageID) {
    if (!isLeafPage()) {
        throw runtime_error("Not a leaf page");
    }
    header.nextLeafPage = pageID;
    calculateChecksum();
}








