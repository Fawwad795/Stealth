@echo off
echo Building StealthDBMS...
echo.

REM Change to the project directory
cd /d "%~dp0"

REM Remove old executable if it exists
if exist stealth_dbms.exe (
    echo Removing old executable...
    del stealth_dbms.exe
)

echo Compiling with g++...
g++ -I./includes ^
    main.cpp ^
    includes/SQL/sql.cpp ^
    includes/Table/table.cpp ^
    includes/Files/FileRecord.cpp ^
    includes/Files/Utilities.cpp ^
    includes/Parser/parser.cpp ^
    includes/Parser/sql_parser_functions.cpp ^
    includes/Parser/parser_state_machine_functions.cpp ^
    includes/SortingAlgorithms/SortAlgorithms.cpp ^
    includes/Stub/stub.cpp ^
    includes/Token/*.cpp ^
    includes/Tokenizer/*.cpp ^
    -o stealth_dbms.exe

if %ERRORLEVEL% EQU 0 (
    echo.
    echo ================================
    echo BUILD SUCCESSFUL!
    echo ================================
    echo Executable created: stealth_dbms.exe
    echo.
    echo To run the program, type:
    echo   .\stealth_dbms.exe
    echo.
) else (
    echo.
    echo ================================
    echo BUILD FAILED!
    echo ================================
    echo Please check the error messages above.
    echo.
)

pause
