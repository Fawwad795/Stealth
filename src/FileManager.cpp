#include "FileManager.h"
#include <iostream>
using namespace std;

//constructor of to initialize an object of File Manager, with the database file name
FileManager::FileManager(const string &fileName) : fileName(fileName), metadata({1, -1}) {}

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
    cout << "Metadata written to the file.\n";


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
        throw runtime_error("Failed to open file for writing.");
    }

    //calculating the offset for this page
    int offset = sizeof(metadata) + page.getID() * Page::PAGE_SIZE;

    //seek to the offset and then write the page data
    fileStream.seekp(offset);
    if(!fileStream) {
        throw runtime_error("Failed to seek to the correct offset for writing the page.");
    }

    fileStream.write(reinterpret_cast<const char*>(&page), sizeof(Page));
    if(!fileStream) {
        throw runtime_error("Failed to write page to the file.");
    }

    cout << "Page " << page.getID() << " written to file successfully.\n";

    fileStream.close();
}

//function for reading a specific page from the file given the page ID
Page FileManager::readPage(int pageID) {
    //opening the file in read-only binary mode
    fileStream.open(fileName, ios::in | ios::binary);
    if(!fileStream) {
        throw runtime_error("Failed to open the file for reading.");
    }

    //calculating the offset for the page
    int offset = sizeof(metadata) + pageID * Page::PAGE_SIZE;

    //seek to the offset and read the page data
    fileStream.seekg(offset);
    if(!fileStream) {
        throw runtime_error("Failed to seek to the correct offset for reading the page.");
    }

    Page page(pageID);
    fileStream.read(reinterpret_cast<char*>(&page), sizeof(Page));
    if(!fileStream) {
        throw runtime_error("Failed to read page from the file.");
    }

    cout << "Page " << pageID << " read from file successfully.\n";

    fileStream.close();
    return page;
}

//function for deleting the database file
void FileManager::deleteFile() {
    cout << "Hello1";
    if(remove(fileName.c_str()) != 0) {
        throw runtime_error("Failed to delete the database file.");
    }

    cout << "Database file '" << fileName << "' deleted successfully.\n";
}

//function for writing updated metadata to the file
void FileManager::updateMetaData() {
    //opening the file in read-write binary mode
    fileStream.open(fileName, ios::in | ios::out | ios::binary);
    if(!fileStream) {
        throw runtime_error("Failed to open the file for updating the metadata.");
    }

    //writing the metadata to the beggining of the file
    fileStream.seekp(0);
    fileStream.write(reinterpret_cast<const char*>(&metadata), sizeof(metadata));
    if(!fileStream) {
        throw runtime_error("Failed to write the metadata to the file.");
    }

    cout << "Metadata updated successfully.\n";

    fileStream.close();
}

void FileManager::loadMetadata() {
    //opening the file in read-only binary mode
    fileStream.open(fileName, ios::in | ios::binary);
    if(!fileStream) {
        throw runtime_error("Failed to open the file for reading the metadata");
    }

    //read the metadata from the beginning of the file
    fileStream.seekg(0);
    fileStream.read(reinterpret_cast<char*>(&metadata), sizeof(metadata));
    if(!fileStream) {
        throw runtime_error("Failed to read metadata from the file.");
    }

    cout << "Metadata loaded successfully.\n";

    fileStream.close();
}




