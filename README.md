# StealthDBMS - A Basic Database Management System

A lightweight, SQL-like database management system implemented in C++ with custom parsing and storage capabilities.

## Features

- 🗃️ **Table Management**: Create, drop, and manage database tables
- 📊 **SQL-like Queries**: SELECT, INSERT, CREATE TABLE, DROP TABLE operations
- 🔍 **WHERE Clauses**: Filter data with conditional statements
- 💾 **Persistent Storage**: Data persists between sessions
- 🔄 **Batch Processing**: Execute multiple commands from files
- 🌲 **B+ Tree Implementation**: Efficient data indexing and retrieval

## Quick Start

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

### Supported Commands

#### Create Table

```sql
CREATE TABLE employees (id, name, salary)
-- OR
make table employees fields id, name, salary
```

#### Insert Data

```sql
INSERT INTO employees VALUES (1, John, 50000)
insert into employees values 2, Jane, 60000
```

#### Query Data

```sql
SELECT * FROM employees
select name, salary from employees
select * from employees where salary > 50000
```

#### Table Management

```sql
show tables          -- List all tables
DROP TABLE employees -- Delete a table
```

#### Other Commands

```sql
batch               -- Execute commands from batch.txt
exit                -- Quit the program
```

### Example Session

```sql
stealth> CREATE TABLE students (name, age, grade)
stealth> INSERT INTO students VALUES (Alice, 20, A)
stealth> INSERT INTO students VALUES (Bob, 22, B)
stealth> SELECT * FROM students
stealth> SELECT name FROM students WHERE age > 20
stealth> show tables
stealth> exit
```

## Testing

Run the included test suites:

```bash
# Build tests
cmake --build . --target basic_test
cmake --build . --target testB

# Run tests
./bin/basic_test
./bin/testB
```
