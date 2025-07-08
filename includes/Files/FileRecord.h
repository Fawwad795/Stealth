#ifndef FILE_RECORD_H
#define FILE_RECORD_H

#include <cmath>
#include <iostream>
#include <iomanip>
#include <set>
#include <vector>
#include <string>
#include <cstring>
#include <cassert>
#include <fstream>
#include "../EnhancedArrayFunctions/EnhancedArrayFunctions.h"
using namespace std;

class FileRecord {
public:
    //constructing empty file record or with word
    FileRecord();

    //constructing file record from vector of strings
    FileRecord(vector<string> s);

    //writing record to file and returning record number
    long write(fstream& outs);

    //reading record from file at given record number
    //returns number of bytes read (MAX if successful, 0 if past EOF)
    long read(fstream& ins, long recno);

    //outputting record to stream
    friend ostream& operator<<(ostream& outs, const FileRecord& r);

    //defining record size constants
    static const int MAX = 100;  //maximum characters per field
    static const int ROW = 10;   //maximum number of fields

    //record storage array
    char _record[ROW][MAX+1];    //2D array for storing record data

private:
    //for tracking record number
    int recno;
};

#endif //FILE_RECORD_H