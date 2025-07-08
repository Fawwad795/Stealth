#ifndef PAIR_H
#define PAIR_H

#include <cmath>
#include <iostream>
#include <iomanip>
#include <set>
#include <vector>
#include <string>
#include <cassert>
using namespace std;

//template structure representing a key-value pair for implementing map functionality
template <typename K, typename V>
struct Pair{
    K key;
    V value;

    //creating Pair with optional key and value parameters, defaulting to type defaults
    Pair(const K& k=K(), const V& v=V())
    {
        key = k;
        value = v;
    }

    //outputting Pair in format: <Key: key Value: value>
    friend ostream& operator <<(ostream& outs, const Pair<K, V>& print_me)
    {
        outs<<"<Key: "<<print_me.key<<" Value: "<<print_me.value<<">";
        return outs;
    }

    //comparing Pairs by checking if their keys are equal (for map operations)
    friend bool operator ==(const Pair<K, V>& lhs, const Pair<K, V>& rhs)
    {
        //comparing only keys as per map requirements
        return lhs.key == rhs.key;
    }

    //comparing Pairs by checking if left key is less than right key (for ordering)
    friend bool operator < (const Pair<K, V>& lhs, const Pair<K, V>& rhs)
    {
        return lhs.key < rhs.key;
    }

    //comparing Pairs by checking if left key is greater than right key (for ordering)
    friend bool operator > (const Pair<K, V>& lhs, const Pair<K, V>& rhs)
    {
        return lhs.key > rhs.key;
    }

    //comparing Pairs by checking if left key is less than or equal to right key
    friend bool operator <= (const Pair<K, V>& lhs, const Pair<K, V>& rhs)
    {
        return !(lhs > rhs);
    }

    //comparing Pairs by checking if left key is greater than or equal to right key
    friend bool operator >= (const Pair<K, V>& lhs, const Pair<K, V>& rhs)
    {
        return !(lhs < rhs);
    }

    //combining two Pairs by adding both keys and values together
    friend Pair<K, V> operator + (const Pair<K, V>& lhs, const Pair<K, V>& rhs)
    {
        //combining keys and values using their respective + operators
        return (Pair(lhs.key + rhs.key, lhs.value + rhs.value));
    }
};

#endif //PAIR_H