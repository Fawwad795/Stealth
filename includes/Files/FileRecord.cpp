#ifndef FILE_RECORD_CPP
#define FILE_RECORD_CPP

#include <cmath>
#include <iostream>
#include <iomanip>
#include <set>
#include <vector>
#include <string>
#include <cassert>
#include "FileRecord.h"
using namespace std;

FileRecord::FileRecord() {
    //initializing record number to -1
    recno = -1;

    //initializing all fields to null
    for(int i = 0; i < ROW; i++) {
        _record[i][0] = '\0';
    }
}

FileRecord::FileRecord(vector<string> strings) {
    //initializing all fields to null
    for(int i = 0; i < ROW; i++) {
        _record[i][0] = '\0';
    }

    //copying strings from vector to record fields
    for(int i = 0; i < strings.size(); i++) {
        strncpy(_record[i], strings[i].c_str(), MAX);
    }
}

long FileRecord::write(fstream& outs) {
    //positioning file pointer at end
    outs.seekg(0, outs.end);

    //getting current position for record number calculation
    long pos = outs.tellp();

    //writing each field to file
    for(int i = 0; i < ROW; i++) {
        outs.write(_record[i], sizeof(_record)/ROW);
    }

    //returning record number
    return pos/sizeof(_record);
}

long FileRecord::read(fstream& ins, long recno) {
    //calculating file position for record
    long pos = recno * sizeof(_record);

    //positioning file pointer
    ins.seekg(pos, ios_base::beg);

    //reading record fields
    int total_gcount = 0;
    for(int i = 0; i < ROW; i++) {
        //reading field
        ins.read(_record[i], sizeof(_record)/ROW);

        //ensuring null termination
        _record[i][ins.gcount()] = '\0';

        //tracking total bytes read
        total_gcount += ins.gcount();
    }

    //returning total bytes read
    return total_gcount;
}

ostream& operator<<(ostream& outs, const FileRecord& r) {
    //outputting each field with fixed width
    for(int i = 0; i < r.ROW; i++) {
        outs<<setw(20)<<r._record[i];
    }
    outs<<"\n";
    return outs;
}

#endif //FILE_RECORD_