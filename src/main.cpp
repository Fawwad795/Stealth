#include <iostream>
#include "BufferManager.h"  // Include the BufferManager header
using namespace std;

int main() {
    // Create an instance of BufferManager
    BufferManager bufferManager;

    // Load pages into the buffer
    bufferManager.loadPage(1);
    bufferManager.loadPage(2);
    bufferManager.loadPage(3);

    // Retrieve a page from the buffer
    std::shared_ptr<Page> page = bufferManager.getPage(2);
    std::cout << "Accessed page with ID: " << page->pageID << std::endl;

    // Load another page (eviction may happen here)
    bufferManager.loadPage(4);

    // Retrieve the evicted page again
    bufferManager.getPage(1);

    // Flush pages to disk
    bufferManager.flushPage(2);
    bufferManager.flushPage(3);

    return 0;
}


