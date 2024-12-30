cmake_minimum_required(VERSION 3.29)
project(Stealth)

# Set the C++ standard to C++20
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Include directories for header files
include_directories(include)

# Add the source files from the src directory
file(GLOB SOURCES "src/*.cpp")

# Create the executable
add_executable(Stealth ${SOURCES}
        src/BufferManager.cpp
        include/BufferManager.h
        src/DoublyLinkedList.cpp
        include/DoublyLinkedList.h
        include/Page.h
        include/Record.h
        src/Record.cpp
        src/Page.cpp
        include/FileManager.h
        src/FileManager.cpp
        include/TransactionManager.h
        src/TransactionManager.cpp
        include/BPlusTreeNode.h
        include/IndexManager.h
        include/SecondaryIndex.h
        src/BPlusTreeNode.cpp
        include/BPlusTree.h
        src/BPlusTree.cpp
        src/SecondaryIndex.cpp
        include/BulkLoader.h
        include/IndexStatistics.h
        src/BulkLoader.cpp
        src/IndexStatistics.cpp
        include/IndexQueryProcessor.h
        src/IndexQueryProcessor.cpp
        include/HashIndex.h
        src/HashIndex.cpp
        include/IndexSelector.h
        src/IndexSelector.cpp
        include/IndexMaintenanceCoordinator.h
        include/PriorityQueue.h
        src/PriorityQueue.cpp
        src/IndexMaintenanceCoordinator.cpp)
