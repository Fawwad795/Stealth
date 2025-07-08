#ifndef OPERATOR_CPP
#define OPERATOR_CPP

#include <cmath>
#include <iostream>
#include <iomanip>
#include <set>
#include <vector>
#include <string>
#include <cassert>
#include "operator.h"

using namespace std;

Operator::Operator() {
    _val = "";
    set_token_type(OPERATOR);
    set_operator_type(TOKEN_END+1);
}

Operator::Operator(string val) : Token() {
    _val = val;
    set_token_type(OPERATOR);
    set_operator_type(TOKEN_END+1);
}

string Operator::get_val()
{
    const bool debug = true;
    if(debug)
        cout<<"Entered virtual get_val in operator\n";
    return _val;
}

void Operator::set_operator_type(const int& op_type)
{
    _op_type = op_type;
}

int Operator::get_operator_type()
{
    return _op_type;
}

int Operator::get_precedence()
{
    return _precedence;
}

void Operator::set_precedence(const int& precedence)
{
    _precedence = precedence;
}

void Operator::print_value()
{
    cout<<_val;
}

#endif //OPERATOR_H