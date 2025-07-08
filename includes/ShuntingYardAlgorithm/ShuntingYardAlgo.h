#ifndef SHUNTING_YARD_H
#define SHUNTING_YARD_H

//including necessary libraries
#include <cmath>
#include <iostream>
#include <iomanip>
#include <set>
#include <vector>
#include "../Queue/Queue.h"
#include "../Stack/Stack.h"
#include "../Token/token.h"
#include "../Token/leftparen.h"
#include "../Token/operator.h"
#include "../Token/rightparen.h"
#include "../error_code/error_code.h"

using namespace std;

class ShuntingYard
{
public:
    //initializing the default constructor
    ShuntingYard()
    {
        _stub = 0;
        _sql_shunting_yard = false;
        _infix = Queue<Token*> ();
    }

    //initializing the parameterized constructor
    ShuntingYard(const Queue<Token*> &infix)
    {
        _stub = 0;
        _sql_shunting_yard = false;
        _infix = infix;
    }

    //overloading the insertion operator for printing
    friend ostream& operator << (ostream& outs, const ShuntingYard& print_me)
    {
        outs<< print_me._infix;
        return outs;
    }

    //setting the input queue for infix expressions
    void set_input(const Queue<Token*> &infix)
    {
        _stub = 0;
        _postfix = Queue<Token*>();
        _infix = infix;
    }

    //converting infix expressions to postfix
    Queue<Token*> postfix(const Queue<Token*> &infix = Queue<Token*>()) throw(Error_Code)
    {
        Error_Code error_code;
        const bool debug = false;

        //checking if the infix queue is empty
        if(_infix.empty())
        {
            _infix = infix;
        }

        //clearing the postfix queue for new conversion
        _postfix.clear();
        Stack<Token*> operator_stack;
        int relational_count = 0;
        int token_str_count = 0;

        //iterating through the infix tokens
        for(Queue<Token*>::Iterator it = _infix.begin(); it != _infix.end(); ++it)
        {
            switch((*it)->get_type())
            {
            case TOKEN_STR:
                token_str_count++;
                if(token_str_count == 3)
                {
                    if(_sql_shunting_yard)
                    {
                        if(_sql_field_indicies->contains((*it)->get_val()))
                        {
                            if(static_cast<Operator*>(*(operator_stack.begin()))->get_operator_type() == RELATIONAL)
                                error_code._code = EXPECT_LOGICAL;
                            else
                                error_code._code = EXPECT_A_RELATIONAL;
                        }
                        else
                        {
                            error_code._error_token = _postfix.back()->get_val();
                            error_code._code = SYNTAX_ERR_AT_NEAR;
                            error_code._modify_to_postgre = true;
                        }
                    }
                    else
                        error_code._code = INVALID_CONDITION;
                    throw error_code;
                }
                _postfix.push(*it);
                break;

            case OPERATOR:
                if(operator_stack.empty())
                    operator_stack.push(*it);
                else
                {
                    while(!operator_stack.empty()
                        && (*operator_stack.begin())->get_type() != LPAREN
                        && static_cast<Operator*>(*(operator_stack.begin()))->get_precedence() >= static_cast<Operator*>(*it)->get_precedence())
                    {
                        if(static_cast<Operator*>(*(operator_stack.begin()))->get_operator_type() == RELATIONAL &&
                           static_cast<Operator*>(*(operator_stack.begin()))->get_precedence() == static_cast<Operator*>(*it)->get_precedence())
                        {
                            error_code._error_token = (*it)->get_val();
                            error_code._code = INVALID_USAGE_OF_OP;
                            throw error_code;
                        }
                        _postfix.push(operator_stack.pop());
                        token_str_count = 0;
                    }
                    operator_stack.push(*it);
                }
                break;

            case LPAREN:
                token_str_count = 0;
                operator_stack.push(*it);
                break;

            case RPAREN:
                token_str_count = 0;
                if(operator_stack.empty())
                {
                    error_code._code = MISSING_LEFT_PAREN;
                    throw error_code;
                }
                while((*operator_stack.begin())->get_type() != LPAREN)
                {
                    _postfix.push(operator_stack.pop());
                    if(operator_stack.empty())
                    {
                        error_code._code = MISSING_LEFT_PAREN;
                        throw error_code;
                    }
                }
                operator_stack.pop();
                break;

            default:
                break;
            }
        }

        //pushing remaining operators to postfix
        while(!operator_stack.empty())
        {
            _postfix.push(operator_stack.pop());
        }

        //validating parentheses matching
        if(_postfix.back()->get_type() == LPAREN)
        {
            error_code._code = MISSING_RIGHT_PAREN;
            throw error_code;
        }

        return _postfix;
    }

    //setting the infix expression queue
    void infix(const Queue<Token*> &infix)
    {
        _infix = infix;
    }

    //configuring SQL-specific shunting yard settings
    void set_sql_shuting_yard(bool sql_shunting_yard, const map_sl* sql_field_indicies){
        _sql_shunting_yard = true;
        _sql_field_indicies = sql_field_indicies;
    }

    //retrieving SQL-specific shunting yard status
    bool get_sql_shunting_yard(){return _sql_shunting_yard;}

private:
    int _stub; //maintaining a placeholder variable
    Queue<Token*> _infix; //storing infix tokens
    Queue<Token*> _postfix; //storing postfix tokens
    bool _sql_shunting_yard; //tracking SQL-specific settings
    const map_sl* _sql_field_indicies; //referencing SQL field indices
};

#endif
