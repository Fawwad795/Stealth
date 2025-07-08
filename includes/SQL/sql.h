#ifndef SQL_H
#define SQL_H

#include <cmath>
#include <iostream>
#include <iomanip>
#include <set>
#include <vector>
#include <string>
#include <cassert>
#include "../Table/table.h"
#include "../Parser/parser.h"
#include "../error_code/error_code.h"
using namespace std;

class SQL{
public:
    SQL();                                      //Initializes the SQL instance.
    Table command(string command);              //Processes SQL-like commands (create, insert, select, drop, etc.) and returns a Table object.
    vectorlong selectRecordNos();               //Retrieves record numbers resulting from the last select operation.
    bool errorState(){return error;}            //Checks if an error occurred during the last operation.
    void printTablesNames();                    //Prints the names of all tables managed by the SQL instance.
    void batch();                               //Processes commands in batch mode from a file (batch.txt).

private:
    mmap_ss parseTree;                                  //Parse tree storing the structure of the command.
    vectorlong selectRecNos;                            //A vector storing record numbers selected in the last query.
    Map<string, Table> tables;                          //A map linking table names to Table objects.
    string sqlTableNamesTxt;                            //File name storing the list of table names.
    bool error;                                         //A flag indicating the error state of the last command.
    void sqlWriteToFileTxt(string filename);            //Ensures that the table names file exists and initializes it if necessary.
    Table getTableNamesInATable();                      //Generates a Table object listing all managed table names.
    void modifyErrorStringPostgre(Error_Code& error_, string& command);      //Modifies error messages to align with PostgreSQL standards.
};


#endif //SQL_H