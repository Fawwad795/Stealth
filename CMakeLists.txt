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
        src/Page.cpp)
