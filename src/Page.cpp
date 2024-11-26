#include "Page.h"
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

    //Check if there is not enough space to store the record and metadata, then return false.
    if(recordSize + sizeof(RecordMetadata) > freeSpace) {
        return false; //not enough space
    }

    //calculate the offset for the new record
    int offset = 0;
    if (!metadata.empty()) {
        const RecordMetadata& lastMeta = metadata.back();
        offset = lastMeta.offset + lastMeta.length;     //start after the last record
    }

    //copy the serialized data into the data array
    memcpy(data + offset, serializedRecord.c_str(), recordSize);

    //add metadata for the record
    metadata.push_back({offset, recordSize}) //metadata is a vector of type RecordMetaData(int offset, int length)
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
    return Record::deserialize(index, serializedRecord);
}

bool Page::removeRecord(int index) {
    if(index < 0 || index >= recordCount) {
        return false; //invalid index
    }

    //adjust free space
    const RecordMetadata& meta = metadata[index];
    freeSpace += meta.length + sizeof(RecordMetadata);

    //remove the metadata
}

