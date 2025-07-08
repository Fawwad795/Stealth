#ifndef LEFTPAREN_CPP
#define LEFTPAREN_CPP

#include <cmath>
#include <iostream>
#include <iomanip>
#include <set>
#include <vector>
#include <string>
#include <cassert>
#include "leftparen.h"

using namespace std;

LParen::LParen()
{
    set_token_type(LPAREN);
    _stub = "(";
}
void LParen::print_value() 
{
    cout<<_stub;
}

#endif //LEFTPAREN_H