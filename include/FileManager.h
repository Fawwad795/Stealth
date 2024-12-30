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

    void createFile();                   //function to create a new database file
    void writePage(const Page& page);    //function to write a page to the database file
    Page readPage(int pageID);           //function to read a page from the database file
    void deleteFile();                   //function to delete the database file
    void updateMetaData();               //function for updating the file metadata
    void loadMetadata();                 //function for reading the file metadata
    int allocateNewPage();               //returns new page ID
    void freePage(int pageID);

private:
    string fileName;    //name of the database file
    fstream fileStream; //stream for file operations

    static const int PAGE_HEADER_SIZE = sizeof(FileMetadata);
    int totalPages;                     //tracks total number of pages in file
    vector<int> freePageList;           //maintains list of freed pages for reuse
};

#endif //FILEMANAGER_H
