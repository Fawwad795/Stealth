#ifndef TOKEN_STR_CPP
#define TOKEN_STR_CPP

#include <cmath>
#include <iostream>
#include <iomanip>
#include <set>
#include <vector>
#include <string>
#include <cassert>
#include "token_str.h"
using namespace std;

TokenStr::TokenStr()
{
    _val = "";
    set_token_type(TOKEN_STR);
}
TokenStr::TokenStr(string val) : Token()
{
    _val = val;
    set_token_type(TOKEN_STR);
}
// TokenStr(const string& val) : Token()
// {
//     _val = val;
//     set_token_type(TOKEN_STR);
// }
string TokenStr::get_val()
{
    const bool debug = false;
    if(debug)
        cout<<"Entered virtual get_val() in TokenStr()\n";
    return _val;
}
void TokenStr::print_value()
{
    cout<<_val;
}

#endif //TOKEN_STR_