# Stealth DBMS - A Basic Database Management System

_A comprehensive SQL-like Database Management System developed as a third semester Data Structures and Algorithms project_

## Project Description

Stealth DBMS is a lightweight, custom-built database management system implemented in C++ that demonstrates the practical application of advanced data structures and algorithms in database design. This project showcases a complete DBMS implementation with SQL-like query processing, efficient data storage, and retrieval mechanisms.

The system features a custom SQL parser, B+ tree indexing for optimized data access, and persistent file-based storage. It supports essential database operations including table creation, data insertion, complex querying with WHERE clauses, and batch processing capabilities. The project emphasizes the implementation of core computer science concepts in a real-world application context.

## ✨ Features

- **DDL Commands**: Create, drop, and manage database tables with custom field definitions
- **DML Commands**: Full support for SELECT, INSERT, CREATE TABLE, DROP TABLE operations
- **Logical Operations in WHERE Clause**: Complex conditional filtering with logical operators (AND, OR, NOT)
- **Relational Operations in WHERE Clause**: Support for =, >, <, >=, <=, != comparisons
- **B+ Tree Implementation**: Efficient data indexing and retrieval for optimal performance
- **Expression Evaluation**: Shunting Yard Algorithm and Reverse Polish Notation for query parsing
- **Custom Parser**: Hand-built SQL parser with state machine implementation
- **Error Handling**: Comprehensive error detection and reporting system
- **Interactive CLI**: User-friendly command-line interface with prompt system

## 📸 Screenshots

The following screenshots demonstrate the Stealth DBMS in action:

![Screenshot 1](screenshots/Screenshot%202025-07-08%20145516.png)
_Building the project_

![Screenshot 2](screenshots/Screenshot%202025-07-08%20145646.png)
_Make Table Command_

![Screenshot 3](screenshots/Screenshot%202025-07-08%20145752.png)
_Insert Into Command_

![Screenshot 4](screenshots/Screenshot%202025-07-08%20145845.png)
_Select Command_

![Screenshot 5](screenshots/Screenshot%202025-07-08%20145934.png)
_Select Command with limitied fields_

![Screenshot 6](screenshots/Screenshot%202025-07-08%20150058.png)
_Where Command_

## DSA Concepts Used and Implemented

This project demonstrates the practical implementation of numerous Data Structures and Algorithms concepts:

### **Data Structures**

- **B+ Trees**: Core indexing mechanism for efficient data storage and retrieval
  - Self-balancing tree structure
  - Optimized for range queries and sequential access
  - Leaf nodes linked for efficient traversal
- **Maps and Multimaps**: Key-value storage using B+ tree implementation
  - Custom template-based implementation
  - Support for duplicate keys in multimaps
- **Doubly Linked Lists**: Used in various components for bidirectional traversal
  - Custom node implementation with previous and next pointers
- **Stacks**: Implementation for expression evaluation and parsing
  - Template-based stack with iterator support
- **Queues**: Token queue management during parsing
  - FIFO operations for token processing
- **Vectors**: Dynamic arrays for flexible data storage
  - Enhanced array functions for optimized operations

### **Algorithms**

- **Shunting Yard Algorithm**: Converting infix expressions to postfix notation
  - Handles operator precedence and associativity
  - Manages parentheses and complex expressions
- **Reverse Polish Notation (RPN)**: Expression evaluation
  - Stack-based evaluation of postfix expressions
  - Efficient computation of boolean and arithmetic expressions
- **State Machine**: SQL parsing and tokenization
  - Finite state automaton for syntax analysis
  - Token recognition and classification
- **Sorting Algorithms**: Data organization and retrieval
  - Custom sorting implementations for query results
- **Search Algorithms**: Data lookup and filtering
  - Efficient searching in B+ tree structures
  - Pattern matching for WHERE clause evaluation

## 🚀 Installation & Usage Instructions

### Prerequisites

- **Windows**: MinGW-w64 or Visual Studio with C++ support
- **Linux/macOS**: GCC compiler
- **IDE** (Optional): CLion, Visual Studio Code, or Visual Studio

### Installation & Running

#### Option 1: Quick Build (Windows)

```bash
# Clone the repository
git clone <your-repo-url>
cd StealthDBMS

# Build and run (Windows)
.\build.bat

# Run the database
.\stealth_dbms.exe
```

#### Option 2: Cross-Platform Build

```bash
# Clone the repository
git clone <your-repo-url>
cd StealthDBMS

# Make build script executable (Linux/macOS)
chmod +x build.sh

# Build the project
./build.sh        # Linux/macOS
# OR
.\build.bat       # Windows

# Run the database
./stealth_dbms    # Linux/macOS
# OR
.\stealth_dbms.exe # Windows
```

#### Option 3: Using CMake

```bash
mkdir build
cd build
cmake ..
cmake --build .

# Run the executable
./bin/main        # Linux/macOS
# OR
.\bin\main.exe    # Windows
```

## Usage

Once you run the program, you'll see:

```
Stealth - A Basic Database Management System
Type 'exit' to quit the program

stealth>
```
