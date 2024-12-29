//Here we are going to implement buffer manager functionalities.
#include "BufferManager.h"
#include "Page.h" //for page handling
#include <iostream>
#include <stdexcept>
using namespace std;

//implementing the constructor
BufferManager::BufferManager() {
    cout << "Buffer Manager Initialized." << endl;
}

//implementing the destructor
BufferManager::~BufferManager() {
    //flushing/saving all pages to the disk during destruction of the buffer
    for(auto& [pageID, pagePtr] : bufferPool) {
        flushPage(pageID);
    }

    //clearing all pages/contents from the buffer pool (hash map)
    bufferPool.clear();

    cout << "Buffer Manager destroyed, all pages flushed to the disk." << endl;
}

//implementing the function for loading a page into the memory/buffer pool from the disk for an easy access
void BufferManager::loadPage(int pageID) {
    //if page is not in the buffer, load it from the disk
    if(bufferPool.find(pageID) == bufferPool.end()) {
        cout << "Loading page " << pageID << " into buffer..." << endl;

        //create a new page with the given ID and pass its pointer
        PagePtr page = make_shared<Page>(pageID);

        //put the page into the buffer
        bufferPool[pageID] = page;

        //adding the page to page usage order list to indicate this page was recently accessed.
        //will allow us to implement page replacement policy such as Least Recently Used (LRU) for eviction
        //if buffer space is fully occupied
        pageUsageOrder.insertToHead(pageID);

        //Checking if the buffer size exceeds the maximum possible
        if(bufferPool.size() > MAX_BUFFER_SIZE) {
            evictPage();
        }
    }
    //else if the page exists in the buffer, do nothing
    else {
        cout << "Page " << pageID << " is already in the buffer." << endl;
    }
}



//implementing the function for flushing a page back into the disk when not needed for quick access
void BufferManager::flushPage(int pageID) {
    //return an iterator pointing to the page if that page exists in the buffer
    auto iterator = bufferPool.find(pageID);

    //if the page exists in the buffer, write it onto the disk
    if(iterator != bufferPool.end()) {
        cout << "Flushing page " << pageID << " to disk." << endl;
        //iterator->second->writeToDisk();
    }
    //if the page does not exist in the buffer
    else {
        cerr << "Page " << pageID << " not found in buffer, cannot flush.\n" << endl;
    }
}


//function for retrieving a page from the buffer pool of the buffer manager
PagePtr BufferManager::getPage(int pageID) {
    cout << "Attempting to get page " << pageID << endl;

    auto iterator = bufferPool.find(pageID);
    if (iterator != bufferPool.end()) {
        cout << "Page found in buffer" << endl;
        pageUsageOrder.remove(pageID);
        pageUsageOrder.insertToHead(pageID);
        return iterator->second;
    }

    cout << "Page not in buffer, loading from disk..." << endl;
    loadPage(pageID);
    cout << "Page loaded into buffer" << endl;

    return bufferPool[pageID];
}


//function for evicting a page from the buffer when it gets full
void BufferManager::evictPage() {

    //if the page usage order list is empty, throw an exception.
    if(pageUsageOrder.empty()) {
        throw runtime_error("Eviction failed: Buffer pool is empty.");
    }

    //finding the first unpinned page in Least Recently Used LRU order
    int pageIDToEvict = -1;
    auto current = pageUsageOrder.peekHead();

    while (current != -1) {
        if (!isPinned(current)) {
            pageIDToEvict = current;
            break;
        }

        //moving to the next page in LRU order
        current = pageUsageOrder.getNext(current);
    }

    if (pageIDToEvict) {
        throw runtime_error("All pages pinned, cannot evict");
    }

    cout << "Evicting page " << pageIDToEvict << " from buffer." << endl;

    //removing the page from the LRU list and buffer pool
    pageUsageOrder.remove(pageIDToEvict);
    flushPage(pageIDToEvict);
    //flushing the page to the disk before removing it from the buffer pool
    bufferPool.erase(pageIDToEvict);
    //removing the page from the buffer pool
    bufferPool.erase(pageIDToEvict);
}

//checks if a page is pinned and shouldn't be evicted
bool BufferManager::isPinned(int pageID) const {
    auto it = pinCount.find(pageID);
    return it != pinCount.end() && it->second > 0;
}

//pins a page if not already in the buffer pool
void BufferManager::pinPage(int pageID) {
    // Ensure page is in buffer
    if (bufferPool.find(pageID) == bufferPool.end()) {
        loadPage(pageID);
    }

    // Increment pin count
    pinCount[pageID]++;
}

//unpin a page if operation on a page finishes
void BufferManager::unpinPage(int pageID) {
    auto it = pinCount.find(pageID);
    if (it != pinCount.end() && it->second > 0) {
        it->second--;

        //if pin count reaches 0, page becomes candidate for eviction
        if (it->second == 0) {
            pinCount.erase(it);
        }
    }
}

//function for helping prevent buffer overflowdk
bool BufferManager::hasSpaceForNewPage() const {
    return bufferPool.size() < MAX_BUFFER_SIZE;
}