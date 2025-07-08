#ifndef MPAIR_H
#define MPAIR_H

#include <cmath>
#include <iostream>
#include <iomanip>
#include <set>
#include <vector>
#include <string>
#include <cassert>
#include "../STLUtilities/vector_utilities.h"
using namespace std;

//template structure representing a key-value pair where the value is a vector
template <typename K, typename V>
struct MPair{
    K key;
    vector<V> value_list;

    //--------------------------------------------------------------------------------
    /*  - with a key and no value: this will create an empty vector
     *  - with a key AND a value: pushes the value into the value_list
     *  _ with a key and a vector of values (to replace value_list) */
    //--------------------------------------------------------------------------------

    //creating empty MPair with optional key, initializing empty value_list
    MPair(const K& k=K()) {
        key = k;
    }

    //creating MPair with key and single value, setting value_list to that value
    MPair(const K& k, const V& v) {
        key = k;
        value_list = v;
    }

    //creating MPair with key and vector of values, copying values into value_list
    MPair(const K& k, const vector<V>& vlist) {
        key = k;
        for(int i = 0; i < vlist.size(); i++)
        {
            value_list.push_back(vlist[i]);
        }
    }

    //outputting MPair in format: key: value_list
    friend ostream& operator <<(ostream& outs, const MPair<K, V>& print_me) {
        outs <<print_me.key<<": "<<setw(10)<< print_me.value_list;
        return outs;
    }

    //comparing MPairs by checking if their keys are equal
    friend bool operator ==(const MPair<K, V>& lhs, const MPair<K, V>& rhs) {
        return lhs.key == rhs.key;
    }

    //comparing MPairs by checking if left key is less than right key
    friend bool operator < (const MPair<K, V>& lhs, const MPair<K, V>& rhs) {
        return lhs.key < rhs.key;
    }

    //comparing MPairs by checking if left is less than or equal to right
    friend bool operator <= (const MPair<K, V>& lhs, const MPair<K, V>& rhs) {
        return !(lhs > rhs);
    }

    //comparing MPairs by checking if left key is greater than right key
    friend bool operator > (const MPair<K, V>& lhs, const MPair<K, V>& rhs) {
        return lhs.key > rhs.key;
    }

    //comparing MPairs by checking if left is greater than or equal to right
    friend bool operator >= (const MPair<K, V>& lhs, const MPair<K, V>& rhs) {
        return !(lhs < rhs);
    }

    //combining two MPairs by appending second value_list to first while keeping first key
    friend MPair<K, V> operator + (const MPair<K, V>& lhs, const MPair<K, V>& rhs) {
        vector<V> added_vector = lhs.value_list;
        for(int i = 0; i < rhs.value_list.size(); i++)
        {
            added_vector.push_back(rhs.value_list[i]);
        }
        return MPair<K, V>(lhs.key, added_vector);
    }
};

#endif //MPAIR_H