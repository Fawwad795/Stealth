#ifndef UTILITIES_CPP
#define UTILITIES_CPP

#include <cmath>
#include <iostream>
#include <iomanip>
#include <set>
#include <vector>
#include <string>
#include <cassert>
#include "utilities.h"
using namespace std;

//checking if file exists by attempting to open it in binary read mode
bool file_exists(const char filename[]) {
    fstream ff;
    ff.open (filename,std::fstream::in | std::fstream::binary );
    if (ff.fail()){
        cout<<"file_exists(): File DOES exist: "<<filename<<endl;
        return false;
    }
    ff.close();
    return true;
}

//opening file for both reading and writing, creating it if it doesn't exist
void open_fileRW(fstream& f, const char filename[]) throw(char*) {
    //opening a nonexistent file for in|out|app causes a fail()
    //so, if the file does not exist, create it by openning it for
    if (!file_exists(filename)){
        //create the file
        f.open(filename, std::fstream::out|std::fstream::binary);

        if (f.fail()){
            cout<<"file open (RW) failed: with out|"<<filename<<"]"<<endl;
            throw runtime_error("file RW failed");
        }
    }
    else{
        f.open (filename, std::fstream::in | std::fstream::out| std::fstream::binary );

        if (f.fail()){
            cout<<"file open (RW) failed. ["<<filename<<"]"<<endl;
            throw runtime_error("file failed to open.");
        }
    }
}

//opening file in write-only mode in binary mode
void open_fileW(fstream& f, const char filename[]) {
    f.open (filename, std::fstream::out| std::fstream::binary );

    if (f.fail()){
        cout<<"file open failed: "<<filename<<endl;
        throw runtime_error("file failed to open.");
    }
}

//initializing text file by writing data array with truncation
void init_write_to_file_txt(const string& _filename, const vector<string>& data_arr) {
    ofstream fout;
    fout.open(_filename, ios::trunc);
    if(fout.fail()) {
        cout <<"Fileout Error opening "<<_filename<<"\n";
        exit(1);
    }
    for(int i = 0; i < data_arr.size(); i++)
        fout << data_arr[i] << "\n";
    fout.close();
}

//creating empty file by opening and closing in write mode
void init_create_file(fstream& f, const char filename[]) {
    open_fileW(f, filename);
    f.close();
}

//reading data from text file line by line into a vector
vectorstr read_from_file_txt(const string& _filename) {
    vectorstr field_name_v;
    ifstream fin;
    fin.open(_filename);
    if(fin.fail()) {
        cout <<"Fileout Error opening "<<_filename<<"\n";
        exit(1);
    }

    string read_line;
    while(fin >> read_line) {
        field_name_v.push_back(read_line);
    }

    fin.close();
    return field_name_v;
}

//appending data array to binary file
void write_to_file_bin(const string& _filename, const vector<string>& data_arr) {
    ofstream fout;
    fout.open(_filename, fstream::app);
    if(fout.fail()) {
        cout <<"Fileout Error opening "<<_filename<<"\n";
        exit(1);
    }
    for(int i = 0; i < data_arr.size(); i++) {
        fout << data_arr[i] << "\n";
    }

    fout.close();
}

//writing data array to text file with truncation
void write_to_file_txt(const string& _filename, const vector<string>& data_arr) {
    ofstream fout;
    fout.open(_filename, fstream::trunc);
    if(fout.fail())
    {
        cout <<"Fileout Error opening "<<_filename<<"\n";
        exit(1);
    }
    for(int i = 0; i < data_arr.size(); i++) {
        fout << data_arr[i] << "\n";
    }

    fout.close();
}

//appending data array to text file
void write_to_file_txt_app(const string& _filename, const vector<string>& data_arr) {
    ofstream fout;
    fout.open(_filename, fstream::app);

    if(fout.fail()) {
        cout <<"Fileout Error opening "<<_filename<<"\n";
        exit(1);
    }
    for(int i = 0; i < data_arr.size(); i++) {
        fout << data_arr[i] << "\n";
    }

    fout.close();
}

#endif //UTILITIES_CPP