#ifndef PAGE_H
#define PAGE_H

// Class Page that will hold records
class Page {
public:
    // Constructor to initialize the Page with a pageID
    Page(int id) {
        pageID = id;
        freeSpace = PAGE_SIZE; // Optionally initialize the data array or other members here
    }

    static const int PAGE_SIZE = 4096;  // 4 KB page size
    char data[PAGE_SIZE];               // Storage for data (records)
    int freeSpace;                      // Tracks free space for adding records
    int pageID;                         // Page ID to uniquely identify the page
};

#endif //PAGE_H