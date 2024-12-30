//declaration file of the Record class
#ifndef RECORD_H
#define RECORD_H

#include <vector>
#include <string>
#include <sstream> //to serialize and deserialize the record attributes
using namespace std;

class Record {
public:
    //constructor to initialize a record object
    Record(int id, const vector<string>& attributes);

    int getID() const;                      //a getter method for record ID
    vector<string> getAttributes();
    string serialize() const;               //a serialization function to convert attributes to a single string
    static Record deserialize(int id, const string& serializedRecord);          //a deserialization function to get back attributes in the form of a single record from a serialized string

private:
    int id;     //unique id of the record
    vector<string> attributes;      //record attributes
};

//structure to locate records in the database
struct RecordPtr {
    int pageID;      //ID of the page containing the record
    int slotNumber;  //position of record within the page

    RecordPtr(int pid = -1, int slot = -1)
        : pageID(pid), slotNumber(slot) {}

    //'==' operator overloading for comparing RecordPtrs
    bool operator==(const RecordPtr& other) const {
        return pageID == other.pageID && slotNumber == other.slotNumber;
    }
};

#endif //RECORD_H
