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
    //constructor and Destructor
    BufferManager();
    ~BufferManager();

    //**Member functions to interact with the allocated memory/buffer pool/cache
    //to load a page into the buffer pool
    void loadPage(int pageID);

    //to write a page back to the disk
    void flushPage(int pageID);

    //to retrieve a page from the buffer pool
    PagePtr getPage(int pageID);

private:
    //data structure to represent the buffer pool - in which pointers to page objects will be saved
    unordered_map<int, PagePtr> bufferPool; //using an unordered hashmap

    //maximum size of the buffer
    static const int MAX_BUFFER_SIZE = 100;

    //doubly linked list to track page access order - Least Recently Used algorithm
    DoublyLinkedList<int> pageUsageOrder;

    //helper function to handle page eviction when the buffer is full
    void evictPage();
};

#endif //BUFFERMANAGER_H
