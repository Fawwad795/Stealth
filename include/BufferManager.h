#ifndef BUFFERMANAGER_H
#define BUFFERMANAGER_H

#include <unordered_map>
#include <memory>
#include "Page.h"
#include "DoublyLinkedList.h"
using namespace std;
using PagePtr = shared_ptr<Page>;

class BufferManager {
public:
    //constructor and destructor
    BufferManager();
    ~BufferManager();

    //**Member functions to interact with the allocated memory/buffer pool/cache
    void loadPage(int pageID);      //to load a page into the buffer pool
    void flushPage(int pageID);     //to write a page back to the disk
    PagePtr getPage(int pageID);    //to retrieve a page from the buffer pool

    //methods to support indexing
    bool isPinned(int pageID) const;     //checks if a page is in use and shouldn't be evicted
    void pinPage(int pageID);            //manages page pinning for critical operations
    void unpinPage(int pageID);          //manages page pinning for critical operations
    bool hasSpaceForNewPage() const;     //helps prevent buffer overflow

private:
    //data structure to represent the buffer pool - in which pointers to page objects will be saved
    unordered_map<int, PagePtr> bufferPool;     //using an unordered hashmap
    static const int MAX_BUFFER_SIZE = 100;     //maximum size of the buffer
    DoublyLinkedList<int> pageUsageOrder;       //doubly linked list to track page access order - Least Recently Used algorithm


    //helper function to handle page eviction when the buffer is full
    void evictPage();
    unordered_map<int, int> pinCount;   //tracks number of pins per page
};

#endif //BUFFERMANAGER_H
