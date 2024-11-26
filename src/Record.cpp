//implementation file of the Record class
#include "Record.h"

//constructor
Record::Record(int id, const vector<string> &attributes)
    : id(id), attributes(attributes) {}

//serializing function that converts attributes to a single string
string Record::serialize() const {
    //creating an output string stream, that will help in appending strings easily, without needing concatenation
    ostringstream outputStringStream;

    //iterate over each attribute the vector
    for(const auto& attr: attributes) {
        outputStringStream << attr << "|";
    }

    //convert the content of the stream into a standard string
    string serializedData = outputStringStream.str();

    //removing the trailing delimiter
    serializedData.pop_back();

    //returning the serialized string
    return serializedData;
}

//deserializing function that converts the serialized data back to a record of attributes
Record Record::deserialize(int id, const string &serializedRecord) {
    vector<string> attributes;  //vector of string to store the attributes
    istringstream inputStringStream(serializedRecord); //input string stream that will read from the serialized data
    string attr;

    //split the serialized data string by the delimiter '|'
    while (getline(inputStringStream, attr, '|')) {
        attributes.push_back(attr); //adding each attribute to the vector
    }

    //create and return a new Record
    return Record(id, attributes);
}



