#ifndef UTILITIES_H
#define UTILITIES_H

#include <cmath>
#include <iostream>
#include <iomanip>
#include <set>
#include <vector>
#include <string>
#include <cassert>
#include <fstream>
#include "../table/typedefs.h"
using namespace std;

bool file_exists(const char filename[]);                            //checking if file exists at specified path
void open_fileRW(fstream& f, const char filename[]) throw(char*);   //opening file in read/write mode with error handling
void open_fileW(fstream& f, const char filename[]);                 //opening file in write mode with error handling
void init_write_to_file_txt(const string& _filename, const vector<string>& data_arr);   //writing initial data to text file
void init_create_file(fstream& f, const char filename[]);           //creating empty file with specified name
vectorstr read_from_file_txt(const string& _filename);              //reading data from text file into string vector
void write_to_file_bin(const string& _filename, const vector<string>& data_arr);        //appending data to binary file
void write_to_file_txt(const string& _filename, const vector<string>& data_arr);        //writing data to text file (overwriting)
void write_to_file_txt_app(const string& _filename, const vector<string>& data_arr);    //appending data to text file


#endif // ZAC_UTILITIES_