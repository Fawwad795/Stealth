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

    //a getter method for record ID
    int getID() const;

    vector<string> getAttributes();

    //a serialization function to convert attributes to a single string
    string serialize() const;

    //a deserialization function to get back attributes in the form of a single record from a serialized string
    static Record deserialize(int id, const string& serializedRecord);

private:
    int id;     //unique id of the record
    vector<string> attributes;      //record attributes
};

#endif //RECORD_H
