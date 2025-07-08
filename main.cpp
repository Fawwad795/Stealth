#include <iostream>
#include <iomanip>
#include "includes/SQL/sql.h"

using namespace std;

int main(int argc, char** argv) {
    //initializing the SQL-like stealth database management system
    SQL stealthDBSystem;
    Table resultTable;

    //buffer for handling user input/query
    const int maxCommandLength = 300;
    char commandBuffer[maxCommandLength];
    string commandString;

    cout << "Stealth - A Basic Database Management System\n";
    cout << "Type 'exit' to quit the program\n\n";

    while(true) {
        //displaying command prompt
        cout << "stealth> ";

        //getting user input from the terminal
        getline(cin, commandString);

        //checking for exit command
        if(commandString == "exit") {
            break;
        }

        //converting string to char array for command processing
        strncpy(commandBuffer, commandString.c_str(), maxCommandLength - 1);
        commandBuffer[maxCommandLength - 1] = '\0';  //ensuring null termination

        //executing the command and getting results
        resultTable = stealthDBSystem.command(commandString);

        //displaying results if command was successful
        if(!stealthDBSystem.errorState()) {
            //displaying the resulting table
            cout << resultTable;

            //for non-system tables show record numbers
            if(!resultTable.get_tablenames_table()) {
                cout << "Selected record numbers: " << stealthDBSystem.selectRecordNos() << "\n" << endl;
            }
        }
    }

    cout << "\nThank you for using Stealth - Database Management System\n";
    cout << "Session terminated successfully\n\n";

    return 0;
}