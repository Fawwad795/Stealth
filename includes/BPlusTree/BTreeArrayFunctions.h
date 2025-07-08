#ifndef BTREE_ARRAY_FUNCTIONS_H
#define BTREE_ARRAY_FUNCTIONS_H

#include <cmath>
#include <iostream>
#include <iomanip>
#include <set>
#include <vector>
#include <string>
#include <cassert>
#include "../EnhancedArrayFunctions/EnhancedArrayFunctions.h"
#include "Pair.h"
#include "MPair.h"
using namespace std;

//returning larger of two items for array comparisons
template <class T>
T maximal(const T& a, const T& b) {
    return a > b? a:b;
}

//swapping two items in array operations
template <class T>
void swap(T& a, T& b) {
    T temp = a;
    a = b;
    b = temp;
}

//finding index of the largest item in array for array sorting
template <class T>
int index_of_maximal(T data[ ], int n) {
    int max_index = 0;
    T max = data[0];
    for(int i = 1; i < n; i++)
    {
        if(data[i] > max)
        {
            max = data[i];
            max_index = i;
        }
    }
    return max_index;
}

//inserting entry into sorted array while maintaining order
template <class T>
void ordered_insert(T data[ ], int& n, T entry)
{
    for (int i=0; i<n; i++) {
        //TODO: I have to implement this after I discuss with Ali
    }
}

//finding first element in array greater than or equal to entry
template <class T>
int first_ge(const T data[ ], int n, const T& entry) {
    for(int i = 0; i < n; i++)
    {
        if(data[i] >= entry)
            return i;
    }
    return n;
}

//checking if entry exists in array and returning its index if found
template <class T>
bool is_present(const T data[ ], int n, const T& entry, int &found_index) {
    for(int i = 0; i < n; i++)
    {
        if(data[i] == entry)
        {
            found_index = i;
            return true;
        }
    }
    found_index = -1;
    return false;
}

//adding entry to end of array and incrementing size
template <class T>
void attach_item(T data[ ], int& n, const T& entry) {
    data[n++] = entry;
}

//inserting entry at specific index and shifting elements right
template <class T>
void insert_item(T data[ ], int i, int& n, T entry) {
    assert(i < n && "i must be less than n\n");
    shift_right(data, n, i);
    data[i] = entry;
}

//removing last element from array and storing it in entry
template <class T>
void detach_item(T data[ ], int& n, T& entry) {
    entry = data[n - 1];
    n--;
}

//deleting item at index i and shifting elements left
template <class T>
void delete_item(T data[ ], int i, int& n, const T& entry) {
    shift_left(data, n, i);
}

//combining two arrays by appending data2 to data1
template <class T>
void merge(T data1[ ], int& n1, T data2[ ], int& n2) {
    for(int i = 0; i < n2; i++)
        attach_item(data1, n1, data2[i]);
}

//splitting array by moving half elements from data1 to data2
template <class T>
void split(T data1[ ], int& n1, T data2[ ], int& n2) {
    n2 = n1/2;
    for(int i = n1 - n2; i < n1; i++)
    {
        data2[i - (n1 - n2)] = data1[i];
    }
    n1 = n1 - n2;
}

//copying array from source to destination array
template <class T>
void copy_array(T dest[], const T src[], int& dest_size, int src_size);

//printing array contents with optional position marker
template <class T>
void print_array(const T data[], int n, int pos = -1);

//checking if item is greater than all elements in array
template <class T>
bool is_gt(const T data[], int n, const T& item) {
    for(int i = 0; i < n; i++)
    {
        if(item <= data[i])
            return false;
    }
    return true;
}

//checking if item is less than or equal to all elements in array
template <class T>
bool is_le(const T data[], int n, const T& item) {
    for(int i = 0; i < n; i++)
    {
        if(item > data[i])
            return false;
    }
    return true;
}

//-------------- Vector Extra operators: ---------------------

//printing vector list to output stream
template <typename T>
ostream& operator <<(ostream& outs, const vector<T>& list);

//appending item to vector using push_back
template <typename T>
vector<T>& operator +=(vector<T>& list, const T& addme);

#endif // BTREE_ARRAY_FUNCTIONS_