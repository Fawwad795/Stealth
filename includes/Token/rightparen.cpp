#ifndef RIGHTPAREN_CPP
#define RIGHTPAREN_CPP

#include <cmath>
#include <iostream>
#include <iomanip>
#include <set>
#include <vector>
#include <string>
#include <cassert>
#include "rightparen.h"
using namespace std;

RParen::RParen()
{
    set_token_type(RPAREN);
    _stub = ")";
}
void RParen::print_value()
{
    cout<<_stub;
}

#endif //RIGHTPAREN_H