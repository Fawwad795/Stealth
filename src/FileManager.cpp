#include "FileManager.h"
#include <iostream>
using namespace std;

//constructor of to initialize an object of File Manager, with the database file name
FileManager::FileManager(const string &fileName) : fileName(fileName), metadata({0, -1}) {}

//function to create a new database file
void FileManager::createFile() {
    //check if the file already exists to prevent overwriting it
    ifstream checkFile(fileName, ios::binary);
    if(checkFile.good()) {
        checkFile.close();

        throw runtime_error("File already exists. Use a different name or delete the file manually.");
    }
    checkFile.close();

    //open the file in output mode to create it
    fileStream.open(fileName, ios::out | ios::binary);

    //check if the file was created successfully
    if(!fileStream) {
        throw runtime_error("Failed to create file.");
    }

    //write the metadata to the file in binary format using reinterpret cast
    fileStream.write(reinterpret_cast<char*>(&metadata), sizeof(metadata));
    if(!fileStream) {
        throw runtime_error("Failed to write metadata to file.");
    }

    //reserve a space of 1 MB in the file
    const int reservedSpace = 1024 * 1024;
    fileStream.seekp(reservedSpace - 1);
    fileStream.put('\0');
    if(!fileStream) {
        throw runtime_error("Failed to reserve space in file.");
    }

    cout << "Database file '" << fileName << "' created successfully with metadata and reserved space of 1MB.\n";

    fileStream.close();
}


void FileManager::writePage(const Page &page) {
    //open the file in read-write binary mode
    fileStream.open(fileName, ios::in | ios::out | ios::binary);
    if(!fileStream) {
        throw runtime_error("Failed to open file")
    }
}

