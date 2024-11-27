#include "Page.h"
#include "algorithm"
#include "iostream"
using namespace std;

//constructor to initialize a page object
Page::Page(int id) {
    pageID = id;
    freeSpace = PAGE_SIZE;
    recordCount = 0;

    //setting all data in the page to empty
    memset(data, 0, PAGE_SIZE);
}

//function to add a record to the page
bool Page::addRecord(const Record &record) {

    //firstly, serialize the record
    string serializedRecord = record.serialize();

    //secondly, record the size of the string in bytes
    int recordSize = serializedRecord.size();

    //look for a suitable free block in the free list
    for(auto iterator = freeList.begin(); iterator != freeList.end(); ++iterator) {
        //if a suitable free block for the record is found, get the offset for it in the data array

        if(iterator->second >= recordSize + sizeof(RecordMetadata)) {
            int offset = iterator->first;

            //copy the record into the free block
            memcpy(data + offset, serializedRecord.c_str(), recordSize);

            //add metadata for the record in the metadata vector, increment record count in the page
            metadata.push_back({record.getID(), offset, recordSize});
            recordCount++;

            //adjust or remove the free block from the free list
            if(iterator->second == recordSize) {
                freeList.erase(iterator); //exact fit, so remove free block
            }
            else {
                iterator->first += recordSize; //adjust start of the remaining free block
                iterator->second -= recordSize; //reduce block size
            }

            return true; //record successfully added
        }
    }

    //defragment if fragmented space prevents addition
    defragment();

    //Check if there is not enough space to store the record and metadata, then return false.
    if(recordSize + sizeof(RecordMetadata) > freeSpace) {
        return false; //not enough space
    }

    //calculate the offset for the new record
    int offset = PAGE_SIZE - freeSpace;

    //copy the serialized data into the data array
    memcpy(data + offset, serializedRecord.c_str(), recordSize);

    //add metadata for the record
    metadata.push_back({record.getID(), offset, recordSize}); //metadata is a vector of type RecordMetaData(int offset, int length)
    //increment the counter for the number fo records
    recordCount++;

    //compute the free space left now
    freeSpace -= (recordSize + sizeof(RecordMetadata));

    return true; //record successfully added
}

Record Page::getRecord(int index) const {
    if(index < 0 || index >= recordCount) {
        throw out_of_range("Invalid record Index");
    }

    //retrieve metadata for the record using the index
    const RecordMetadata& meta = metadata[index];
    string serializedRecord(data + meta.offset, meta.length);

    //return the deserialized record
    return Record::deserialize(meta.id, serializedRecord);
}

bool Page::removeRecord(int index) {
    if(index < 0 || index >= recordCount) {
        return false; //invalid index
    }

    //retrieve metadata for the record to be removed
    const RecordMetadata& meta = metadata[index];

    //add the freed space to the free list
    freeList.push_back({meta.offset, meta.length});

    //remove the metadata for this record, decrement the record count for this page
    metadata.erase(metadata.begin() + index);
    recordCount--;

    return true;    //successfully removed the record
}

//getter function for bytes of free space available in the page
int Page::getFreeSpace() const {
    return freeSpace;
}

//getter function for the number of records in the page
int Page::getRecordCount() const {
    return recordCount;
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
    freeSpace = PAGE_SIZE - currentOffset;
}





