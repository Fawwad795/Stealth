#ifndef PARSER_TYPEDEFS_H
#define PARSER_TYPEDEFS_H

#include <cmath>
#include <iostream>
#include <iomanip>
#include <set>
#include <vector>
#include <string>
#include <cassert>
#include <queue>
#include "../Queue/Queue.h"
#include "../bplustree/map.h"
#include "../bplustree/multimap.h"

using namespace std;

typedef Map<string, string> map_ss;
typedef MMap<string, string> mmap_ss;

typedef Map<string, long> map_sl;
typedef MMap<string, long> mmap_sl;

typedef vector<string> vectorstr;
typedef vector<long> vectorlong;
typedef Queue<string> queuestr;

#endif //TYPEDEFS_