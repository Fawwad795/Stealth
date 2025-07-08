cmake_minimum_required(VERSION 3.5 FATAL_ERROR)

# Project declaration
project(StealthDBMS)

# Set C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Include GNU install directories
include(GNUInstallDirs)

# Compiler-specific settings
if(MSVC)
    add_compile_options(/W4)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/bin)
else()
    add_compile_options(-Wall -Wextra)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
endif()

# Output directories
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR})

# Coverage flags for GCC
if(CMAKE_CXX_COMPILER_ID MATCHES GNU)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fprofile-arcs -ftest-coverage")
endif()

# GoogleTest setup
include(FetchContent)
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/609281088cfefc76f9d0ce82e1ff6c30cc3591e5.zip
)

# Prevent overriding parent project's compiler/linker settings on Windows
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# Enable testing
enable_testing()

# Collect all source files from includes directory
set(INCLUDES_FOLDER includes)
file(GLOB_RECURSE SOURCE_FILES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "${INCLUDES_FOLDER}/*.cpp")

# Main executable
add_executable(main
    main.cpp
    ${SOURCE_FILES}
)

# Test executables
add_executable(basic_test
    _tests/_test_files/basic_test.cpp
    ${SOURCE_FILES}
)

add_executable(testB
    _tests/_test_files/testB.cpp
    ${SOURCE_FILES}
)

# Link GoogleTest to test executables
target_link_libraries(basic_test gtest)
target_link_libraries(testB gtest)

