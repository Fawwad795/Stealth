#ifndef MAP_H
#define MAP_H

#include <cmath>
#include <iostream>
#include <iomanip>
#include <set>
#include <vector>
#include <string>
#include <cassert>
#include "Pair.h"
#include "BPlusTree.h"

using namespace std;

//implementing an ordered map container using BPlusTree as the underlying data structure
template <typename K, typename V>
class Map {
public:
    typedef BPlusTree<Pair<K, V> > map_base;

    //implementing iterator class for traversing map elements
    class Iterator {
    public:
        friend class Map;
        //initializing iterator with null pointer
        Iterator() { }

        //constructing iterator from BPlusTree iterator
        Iterator(typename map_base::Iterator it) {
            _it = it;
        }

        //implementing postfix increment operator
        Iterator operator ++(int unused) {
            return Iterator(_it++);
        }

        //implementing prefix increment operator
        Iterator operator ++() {
            return Iterator(++_it);
        }

        //dereferencing iterator to access pair
        Pair<K, V> operator *() {
            return *_it;
        }

        //comparing iterators for equality
        friend bool operator ==(const Iterator& lhs, const Iterator& rhs) {
            return lhs._it == rhs._it;
        }

        //comparing iterators for inequality
        friend bool operator !=(const Iterator& lhs, const Iterator& rhs) {
            return !(lhs == rhs);
        }

    private:
        typename map_base::Iterator _it;
    };

    //initializing empty map with duplicates disabled
    Map():map(false) { }

    //getting iterator to first element
    Iterator begin() {
        return Iterator(map.begin());
    }

    //getting iterator to position after last element
    Iterator end() {
        return Iterator(map.end());
    }

    //checking if map is empty
    int size() const {
        return map.elements_count();
    }

    //getting number of elements in map
    bool empty() const {
        return !size();
    }

    //accessing value by key with bounds checking
    V& operator[](const K& key) {
        return map.get(key).value;
    }

    //accessing value by key (const version)
    const V& operator[](const K& key) const {
        return map.get(key).value;
    }

    //checking if key exists in map
    bool contains(const K& key) const {
        return map.contains(key);
    }

    //accessing value by key with bounds checking
    V& at(const K& key) {
        return map.get(key).value;
    }

    //accessing value by key with bounds checking (const version)
    const V& at(const K& key) const {
        return map.get(key).value;
    }

    //inserting key-value pair into map
    void insert(const K& k, const V& v) {
        Pair<K, V> p(k, v);
        map.insert(p);
    }

    //removing element by key
    void erase(const K& key) {
        map.remove(key);
    }

    //removing all elements
    void clear() {
        map.clear_tree();
    }

    //retrieving value associated with key
    V get(const K& key) {
        return map.get(key).value;
    }

    //finding element by key and returning iterator
    Iterator find(const K& key) {
        return Iterator(map.find(key));
    }

    //checking if pair exists in map
    bool contains(const Pair<K, V>& target) const {
        return map.contains(target);
    }

    //getting iterator to first element not less than key
    Iterator lower_bound(const K& key) {
        return Iterator(map.lower_bound(key));
    }

    //getting iterator to first element greater than key
    Iterator upper_bound(const K& key) {
        return Iterator(map.upper_bound(key));
    }

    //getting range of elements equal to key
    Iterator equal_range(const K& key) {
        return Iterator();
    }

    //validating map structure
    bool is_valid() {
        return map.is_valid();
    }

    //outputting map contents
    friend ostream& operator<<(ostream& outs, const Map<K, V>& print_me) {
        outs<<print_me.map<<endl;
        return outs;
    }

private:
    int key_count;
    BPlusTree<Pair<K, V> > map;
};

#endif //MAP_H