#ifndef FILEMANAGER_H
#define FILEMANAGER_H

#include <string>
#include <fstream>
#include "Page.h"

class FileManager {
public:
    //struct for storing file metadata
    struct FileMetadata {
        int numTables;      //number of tables in the database
        int freePageStart;  //starting page ID for free pages (-1 if no free pages)
    };
    FileMetadata metadata;  //metadata for the database file

    //constructor
    FileManager(const string& fileName);

    //function to create a new database file
    void createFile();

    //function to write a page to the database file
    void writePage(const Page& page);

    //function to read a page from the database file
    Page readPage(int pageID);

    //function to delete the database file
    void deleteFile();

    //function for updating the file metadata
    void updateMetaData();

    //function for reading the file metadata
    void loadMetadata();


private:
    string fileName;    //name of the database file
    fstream fileStream; //stream for file operations
};

#endif //FILEMANAGER_H
