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
        iterator->second->writeToDisk();
    }
    //if the page does not exist in the buffer
    else {
        cerr << "Page " << pageID << " not found in buffer, cannot flush.\n" << endl;
    }
}



//function for retrieving a page from the buffer pool of the buffer manager
PagePtr BufferManager::getPage(int pageID) {
    auto iterator = bufferPool.find(pageID);

    //if the page is found in the buffer, move it to the back of the page usage order list, to indicate it has been recently accessed
    if(iterator != bufferPool.end()) {
        cout << "Page " << pageID << " found in buffer" << endl;
        pageUsageOrder.remove(pageID);
        pageUsageOrder.insertToHead(pageID);

        //return the page
        return iterator->second;
    }

    //if the page is not found in the buffer, load it from the disk
    cout << "Page " << pageID << " not in buffer. Loading it from disk..." << endl;
    loadPage(pageID);

    //then return the page
    return bufferPool[pageID];
}

//function for evicting a page from the buffer when it gets full
void BufferManager::evictPage() {

    //if the page usage order list is empty, throw an exception.
    if(pageUsageOrder.empty()) {
        throw runtime_error("Eviction failed: Buffer pool is empty.");
    }

    //else if not empty, evict the least recently used page (front of the usage list)
    int pageIDToEvict = pageUsageOrder.front();
    pageUsageOrder.popFront();

    cout << "Evicting page " << pageIDToEvict << " from buffer." << endl;

    //flushing the page to the disk before removing it from the buffer pool
    flushPage(pageIDToEvict);

    //removing the page from the buffer pool
    bufferPool.erase(pageIDToEvict);
}