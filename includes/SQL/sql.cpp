#ifndef SQL_CPP
#define SQL_CPP

#include <cmath>
#include <iostream>
#include <iomanip>
#include <set>
#include <vector>
#include <string>
#include <cassert>
#include "sql.h"
using namespace std;

SQL::SQL(){
    error = false;
    sqlTableNamesTxt = "sqlTablesName.txt";
    sqlWriteToFileTxt(sqlTableNamesTxt);
    vectorstr sqlTablesName = read_from_file_txt(sqlTableNamesTxt);

    cout<<"--Tables Stealth DBMS is managing--\n";
    for(int i = 0; i < sqlTablesName.size(); i++) {
        tables[sqlTablesName[i]] = Table(sqlTablesName[i]);
        cout<< i + 1 << ". " << sqlTablesName[i]<<"\n";
    }
    cout<<"--------------------------\n\n";
    //need to build a table of table names to take care in case where DB might not be managing any tables
}

//method that will handle different user commands
Table SQL::command(string command){
    const bool debug = false;
    try {
        error = false;
        Error_Code error_code;
        char cmd_s[300];
        strcpy(cmd_s, command.c_str());
        Parser parser(cmd_s);
        parseTree.clear();
        parseTree = parser.parse_tree();

        //to create/make a table
        if(parseTree["command"][0] == "make" || parseTree["command"][0] == "create")
        {
            //two argument CTOR to create new table
            if(tables.contains(parseTree["table_name"][0]))
            {
                //error handling should go here
                //user trying to create an existing table
                error_code._code = CANNOT_CREATE_PRE_EXISTING_TABLE;
                throw error_code;
                return tables[parseTree["table_name"][0]];

            }
            else
            {
                Table table(parseTree["table_name"][0], parseTree["col"]);
                tables[parseTree["table_name"][0]] = table;
                write_to_file_txt_app(sqlTableNamesTxt, {parseTree["table_name"][0]});
                if(debug)
                    cout<<"Brand New Table created.\n";
                return table;
            }
            //^ delete all txt and bin files than test
        }
        else if(parseTree["command"][0] == "insert")
        {
            //one argument CTOR to open an existing table to insert into
            // Table table(parseTree["table_name"][0]);
            if(!tables.contains(parseTree["table_name"][0]))
            {
                error_code._code = INSERT_NON_EXISTENT;
                throw error_code;
            }
            tables[parseTree["table_name"][0]].insert_into(parseTree["values"]);
            return tables[parseTree["table_name"][0]];
        }
        else if(parseTree["command"][0] == "select")
        {
            //where means there is a condition
            if(!parseTree.contains("table_name"))
            {
                error_code._code = SELECT_EXPECT_TABLE_NAME;
                throw error_code;
            }
            if(!tables.contains(parseTree["table_name"][0]))
            {
                error_code._code = SELECT_NON_EXISTENT;
                throw error_code;
            }
            if(parseTree["fields"][0] == "*" && parseTree.contains("where"))
            {
                //select * from student where lname = Yao
                //select fname, lname from student
                //v show all fields but select with condition
                if(!parseTree.contains("condition"))
                {
                    error_code._code = EXPECT_CONDITION;
                    throw error_code;
                }
                Table result_table = tables[parseTree["table_name"][0]].select(parseTree["condition"]);
                selectRecNos = tables[parseTree["table_name"][0]].select_recnos();
                return result_table;
            }
            else if(parseTree["fields"][0] == "*")
            {
                //show all fields and select all with no condition
                Table result_table = tables[parseTree["table_name"][0]].select_all();
                selectRecNos = tables[parseTree["table_name"][0]].select_recnos();
                return result_table;
            }
            else if(parseTree.contains("where"))
            {
                //want to select fields with condition
                if(!parseTree.contains("condition"))
                {
                    error_code._code = EXPECT_CONDITION;
                    throw error_code;
                }
                Table result_table = tables[parseTree["table_name"][0]].select(parseTree["fields"], parseTree["condition"]);
                selectRecNos = tables[parseTree["table_name"][0]].select_recnos();
                return result_table;
            }
            else
            {
                //want to show specific fields and select all with no condition
                Table result_table = tables[parseTree["table_name"][0]].select_all(parseTree["fields"]);
                selectRecNos = tables[parseTree["table_name"][0]].select_recnos();
                return result_table;
            }
        }
        else if(parseTree["command"][0] == "show tables")
        {
            // cout<<"SQL CTOR tables:\n"<<tables<<"\n";
            return getTableNamesInATable();
        }
        else if(parseTree["command"][0] == "batch")
        {
            batch();
            error = true;
            return Table();
        }
        else if(parseTree["command"][0] == "drop")
        {
            //throw if trying to drop a non existing table
            //throw if trying to drop without giving the name of a table
            //delete from the map
            //actually delete the txt file and bin associated with the table_name
            if(!parseTree.contains("table_name"))
            {
                error_code._code = DROP_EXPECT_TABLENAME;
                throw error_code;
            }
            if(!tables.contains(parseTree["table_name"][0]))
            {
                error_code._code = DROP_NON_EXISTENT;
                throw error_code;
            }
            string removed_table_name = parseTree["table_name"][0];
            // cout<<"Before removing "<<removed_table_name<<" from tables map\n";
            // cout<<tables;
            if(remove((removed_table_name + "_fields.txt").c_str()) != 0)
                cout<<"Could not remove the file: "<<parseTree["table_name"][0] + "_fields.txt\n";
            if(remove((removed_table_name + "_fields.bin").c_str()) != 0)
                cout<<"Could not remove the file: "<<parseTree["table_name"][0] + "_fields.bin\n";
            tables.erase(removed_table_name);
            // cout<<"After removing "<<removed_table_name<<" from tables map\n";
            // cout<<tables;
            vectorstr before_remove_sql_table_names = read_from_file_txt(sqlTableNamesTxt);
            vectorstr sql_table_names;
            for(int i = 0; i < before_remove_sql_table_names.size(); i++)
            {
                if(tables.contains(before_remove_sql_table_names[i]))
                {
                    sql_table_names.push_back(before_remove_sql_table_names[i]);
                    tables[before_remove_sql_table_names[i]] = Table(before_remove_sql_table_names[i]);
                }
            }
            write_to_file_txt(sqlTableNamesTxt, sql_table_names);
            error = true;
            return Table();
        }
        else
        {
            assert(false && "Invalid Command");
        }
    }
    catch(Error_Code error_)
    {
        //this function will add the command to the Error_code obj so that it can use it to
        //display errors according to postgre sql standards
        modifyErrorStringPostgre(error_, command);
        cout<<error_.get_error_string()<<"\n";
        error = true;
        return Table();
    }
    cout<<"Reached the end of command\n";
}

vectorlong SQL::selectRecordNos()
{
    return selectRecNos;
}
void SQL::printTablesNames()
{
    vectorstr sql_table_names = read_from_file_txt(sqlTableNamesTxt);
    cout<<"--Tables SQL is managing--\n";
    for(int i = 0; i < sql_table_names.size(); i++)
    {
        cout<<sql_table_names[i]<<"\n";
    }
    cout<<"--------------------------\n\n";
}
//delete this print function later
void SQL::batch()
{
    // vectorstr command_vec = read_from_file_txt("batch.txt");
    const bool debug = false;
    ifstream fin;
    fin.open("batch.txt");
    if(debug)
        cout<<"Loading from batch file\n";
    string command_str;
    int i = 1;
    while(getline(fin, command_str))
    {
        cout<<i<<": "<<command_str<<"\n";
        Table temp = command(command_str);
        if(!errorState())
        {
          cout<<temp;
        }
        i++;
    }
    fin.close();
}

//privates
void SQL::sqlWriteToFileTxt(string filename)
{
    if(!file_exists(filename.c_str()))
    {
        init_write_to_file_txt(filename, {});
    }
}
Table SQL::getTableNamesInATable()
{
    Table sql_tables("sql_tables", {"table_names"});
    sql_tables.set_tablenames_table(true);
    vectorstr sql_table_names = read_from_file_txt(sqlTableNamesTxt);
    for(int i = 0; i < sql_table_names.size(); i++)
    {
        sql_tables.insert_into({sql_table_names[i]});
    }
    return sql_tables;
}
void SQL::modifyErrorStringPostgre(Error_Code& error, string& command)
{
    const bool debug = false;
    if(error._modify_to_postgre)
    {
        error._error_input = command;
        char input_buffer[300];
        strcpy(input_buffer, command.c_str());
        STokenizer stk(input_buffer);
        SToken t;
        stk>>t;
        while(stk.more())
        {
            if(t.token_str() == error._error_token)
                break;

            error._character_count += t.token_str().size();
            t = SToken();
            stk>>t;
        }
    }
    if(debug)
        cout<<"error._character_count: "<<error._character_count<<"\n";
}

#endif // SQL_CPP
